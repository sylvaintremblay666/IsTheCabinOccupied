/*
 * WebServer.cpp
 *
 *  Created on: Oct 24, 2018
 *      Author: stremblay
 */

#include "WebServer.h"
#include "WiFiManager.h"

// Comment this line to disable serial debug output
// #define __WEBSERVER_DEBUG__

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
							debug("processRequest failed (endpoint not registered), sending 404");
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
 * Used to register an endpoint in the web server along with a callback method to invoke
 * when the endpoint receive an incoming request.
 *
 * ------------------------------------
 * Callback method signature:
 *
 * bool rootCallBack(WebServer *ws, WiFiClient *client, String queryString)
 *
 * The callback method is responsible for sending the response code and body. It needs to:
 *
 * - Properly send the response code followed by a blank line. Ex.:
 *		client->println("HTTP/1.1 200 OK");
 *		client->println("Content-type:text/html");
 *		client->println("Connection: close");
 * 		client->println();
 *
 * - Send the body. Ex.:
 *		client->println("<!DOCTYPE html><html>");
 *		client->println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
 *		client->println("<link rel=\"icon\" href=\"data:,\">");
 * 		client->println("<body>");
 * 		client->println("<H1>Hello, World!</H1>");
 * 		client->println("</body></html>);
 *
 * Methods are available in the WebServer* instance received in parameter (*ws) to send generic
 * response codes / html response parts, look around and you'll see.
 *
 * The parameters of the callback are:
 * 		WebServer *ws      : the instance of the WebServer class calling the callback, you can use its methods like "ws->send200()"
 * 		WiFiClient *client : the WiFiClient instance you have to use to send the response back to the client (client.println)
 * 		String queryString : the queryString sent along with the request
 *
 * The return value of the callback function is currently not used.
 * ------------------------------------
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
    sendWebPageHeadAndOpenBody();
    client.println("----- 404 - NOT FOUND -----");
    sendWebPageFootAndCloseBody();
}

void WebServer::sendWebPageHeadAndOpenBody() {
    // Open the <html> document
	client.println("<!DOCTYPE html><html>");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<link rel=\"icon\" href=\"data:,\">");

    // Open the <body>
    client.println("<body>");

    if(defaultPageTitle != ""){
    	client.println("<h1>" + defaultPageTitle + "</h1>");
    }
}

void WebServer::sendWebPageFootAndCloseBody() {
    // Close the <body> and <html> document
	client.println("</body></html>");

    // The HTTP response ends with another blank line
    client.println();
}

bool WebServer::processRequest(){
	debug("Starting to process request");

	// Keep only the first line of the header
	String fullUrl = header.substring(0, header.indexOf('\n'));
	debug("fullUrl: " + fullUrl);

	// If there's a query string, extract it
	String queryString = "";
	String trimmedUrl = fullUrl;
	if(fullUrl.indexOf("?") > 0) {
		queryString = fullUrl.substring(fullUrl.indexOf("?") + 1, fullUrl.indexOf(" HTTP/"));
		trimmedUrl = fullUrl.substring(0, fullUrl.indexOf("?")) + fullUrl.substring(fullUrl.indexOf(" HTTP/"), fullUrl.length());
	}
	debug("trimmedUrl : " + trimmedUrl);
	debug("queryString: " + queryString);

	for(short i = 0; i < nbCallbacks; i++){
		if (trimmedUrl.indexOf(callbacks[i].methodAndPath + " HTTP/") >= 0){
			callbacks[i].fct(this, &client, queryString);
			return true;
		}
	}

	// The endpoint is not registered, if querying for "/", fallback to the default page. Else, return false.
	if (trimmedUrl.indexOf("GET / HTTP/") >= 0) {
		sendDefaultRootPage(queryString);
		return true;
	}

	return false;
}

void WebServer::setDefaultPageTitle(String newPageTitle) {
	defaultPageTitle = newPageTitle;
}

void WebServer::sendDefaultRootPage(String queryString) {
    send200();

    sendWebPageHeadAndOpenBody();

    sendWiFiInfos();
	sendEndpointsList();

	client.println("<H2>QueryString</H2>");
	if (queryString == "") {
		client.println("No query string");
	} else {
		client.println(queryString);
	}

	sendWebPageFootAndCloseBody();
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

