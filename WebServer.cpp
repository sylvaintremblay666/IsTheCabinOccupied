/*
 * WebServer.cpp
 *
 *  Created on: Oct 24, 2018
 *      Author: stremblay
 */

#include "WebServer.h"
#include "WiFiManager.h"

// Comment this line to disable serial debug output
#define __WEBSERVER_DEBUG__

WebServer::WebServer() {
	server = new WiFiServer(80);
	server->begin();

	callbacks = new Callback[maxCallbacks];
}

WebServer::~WebServer() {
	delete server;
	delete [] callbacks;
}

void WebServer::checkForClientAndProcessRequest(void){
	client = server->available();       // Listen for incoming clients

	if (client) {                       // If a new client connects,
		debug("New Client.");           // print a message out in the serial port
		String currentLine = "";        // make a String to hold incoming data from the client
		while (client.connected()) {    // loop while the client's connected
			if (client.available()) {   // if there's bytes to read from the client,
				char c = client.read(); // read a byte, then
				Serial.write(c);        // print it out the serial monitor
				header += c;
				if (c == '\n') {        // if the byte is a newline character
					// if the current line is blank, you got two newline characters in a row.
					// that's the end of the client HTTP request, so send a response:
					if (currentLine.length() == 0) {
						// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
						// and a content-type so the client knows what's coming, then a blank line:

						debug("Header: ");
						debug(header);
						debug("-----------");

						if(!processRequest()){
							debug("processRequest failed, sending 404");
							send404();
						}

						// Break out of the while loop
						break;
					} else { // if you got a newline, then clear currentLine
						currentLine = "";
					}
				} else if (c != '\r') {  // if you got anything else but a carriage return character,
					currentLine += c;    // add it to the end of the currentLine
				}
			}
		}
		// Clear the header variable
		header = "";
		// Close the connection
		client.stop();
		debug("Client disconnected.");
		debug("");
	}
}

/**
 * Used to register an endpoint in the web server along with a callback method to call
 * when the endpoint is invoked.
 *
 * Callback method signature:
 *
 * bool rootCallBack(void *webServer, WiFiClient *client)
 *
 *
 * @param methodAndPath Method and path. ex: "GET /my/endpoint" (case sensitive)
 * @param description   A description for this endpoint
 * @param functionPtr   Pointer to the callback function invoked when the endpoint is accessed
 * @return always true for now, I should remove the return param if I don't find any usage for it
 */
bool WebServer::registerEndpoint(String methodAndPath, String description, CallbackFct fct) {
	if (nbCallbacks == maxCallbacks) {
		Callback* newArray = new Callback[++maxCallbacks];
		for(short i = 0; i < nbCallbacks; i++){
			newArray[i].methodAndPath = callbacks[i].methodAndPath;
			newArray[i].fct = callbacks[i].fct;
			newArray[i].description = callbacks[i].description;
		}
		delete [] callbacks;
		callbacks = newArray;
	}

	callbacks[nbCallbacks].fct = fct;
	callbacks[nbCallbacks].methodAndPath = methodAndPath;
	callbacks[nbCallbacks].description = description;

	nbCallbacks++;

	return true;
}

void WebServer::send200() {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
}

void WebServer::send404() {
    client.println("HTTP/1.1 404 NOT FOUND");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
    sendWebPageHead();
    client.println("----- NOT FOUND -----");
    sendWebPageFoot();
}

void WebServer::sendWebPageHead() {
    // Open the <html> document
	client.println("<!DOCTYPE html><html>");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<link rel=\"icon\" href=\"data:,\">");

    // Open the <body>
    client.println("<body>");

    client.println("<h1>ESP8266 Web Server</h1>");
}

void WebServer::sendWebPageFoot() {
    // Close the <body> and <html> document
	client.println("</body></html>");

    // The HTTP response ends with another blank line
    client.println();
}

bool WebServer::processRequest(){
	for(short i = 0; i < nbCallbacks; i++){
		if (header.indexOf(callbacks[i].methodAndPath + " HTTP") >= 0){
			callbacks[i].fct(this, &client);
			return true;
		}
	}

	return false;
}

void WebServer::sendWiFiInfos() {
	client.println("<H2>WiFi Connection</H2>");
	client.println("<table>");
	client.println("<tr>");
	client.println("<td " + tdStyle + ">WiFi SSID</td>");
	client.println("<td " + tdStyle + ">" + WiFi.SSID() + "</td>");
	client.println("</tr>");
	client.println("<td " + tdStyle + ">IP Address</td>");
	client.println("<td " + tdStyle + ">" + WiFi.localIP().toString() + "</td>");
	client.println("</tr>");
	client.println("</table>");
}

void WebServer::sendEndpointsList() {
	client.println("<H2>Available Endpoints</H2>");
	client.println("<table>");
	for(short i = 0; i < nbCallbacks; i++){
		client.println("<tr>");
		client.println("<td " + tdStyle + ">" + callbacks[i].methodAndPath + "</td>");
		client.println("<td " + tdStyle + ">" + callbacks[i].description + "</td>");
		client.println("</tr>");
	}
	client.println("</table>");
}

void WebServer::debug(String msg){
#ifdef __WEBSERVER_DEBUG__
	Serial.println(msg);
#endif // __WEBSERVER_DEBUG__
}

