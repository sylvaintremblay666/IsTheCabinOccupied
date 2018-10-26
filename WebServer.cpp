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
	client = server->available();   // Listen for incoming clients

	if (client) {                             // If a new client connects,
		debug("New Client.");        // print a message out in the serial port
		String currentLine = "";              // make a String to hold incoming data from the client
		while (client.connected()) {          // loop while the client's connected
			if (client.available()) {         // if there's bytes to read from the client,
				char c = client.read();             // read a byte, then
				Serial.write(c);              // print it out the serial monitor
				header += c;
				if (c == '\n') {              // if the byte is a newline character
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
				} else if (c != '\r') { // if you got anything else but a carriage return character,
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

bool WebServer::registerCallback(String contextPath, CallbackFct fct) {
	if (nbCallbacks == maxCallbacks) {
		Callback* newArray = new Callback[++maxCallbacks];
		for(short i = 0; i < nbCallbacks; i++){
			newArray[i].contextPath = callbacks[i].contextPath;
			newArray[i].fct = callbacks[i].fct;
		}
		delete [] callbacks;
		callbacks = newArray;
	}

	callbacks[nbCallbacks].fct = fct;
	callbacks[nbCallbacks].contextPath = contextPath;

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
    // Display the HTML web page
    client.println("<!DOCTYPE html><html>");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<link rel=\"icon\" href=\"data:,\">");

    // Web Page Heading
    client.println("<body><h1>ESP8266 Web Server</h1>");
}

void WebServer::sendWebPageFoot() {
    client.println("</body></html>");

    // The HTTP response ends with another blank line
    client.println();
}

bool WebServer::processRequest(){
	for(short i = 0; i < nbCallbacks; i++){
		if (header.indexOf(callbacks[i].contextPath + " HTTP") >= 0){
			callbacks[i].fct(this, &client);
			return true;
		}
	}

	return false;
}

void WebServer::debug(String msg){
#ifdef __WEBSERVER_DEBUG__
	Serial.println(msg);
#endif // __WEBSERVER_DEBUG__
}

