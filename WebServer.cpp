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

	callbacksArray = new Callback[maxCallbacks];
}

WebServer::~WebServer() {
	delete server;
	delete [] callbacksArray;
}

void WebServer::checkForClientAndProcessRequest(void){
	client = server->available();       // Listen for incoming clients

	if (client) {                       // If a new client connects,
		client.disableKeepAlive();
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

	}
}

/**
 * Used to register an endpoint in the web server along with a callback method to invoke
 * when the endpoint receive an incoming request. Single level REST argument is supported.
 * To configure an endpoint for REST argument support, use the {} placeholder at the end
 * of your path in the "methodAndPath" argument. Ex.:
 *
 *   registerEndpoint("GET /config/key/{}", "endpoint to get value for a key", myCallbackFct);
 *
 * When the callback method will be invoked, the "restArg1" parameter will contain the value
 * of the url corresponding to the "{}".
 *
 * ------------------------------------
 * Callback method signature:
 *
 * bool rootCallBack(WebServer *ws, WiFiClient *client, String queryString, String restArg1)
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
 * 		String restArg1    : for REST endpoint, the value of the first REST param (only single REST param is currently supported)
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
		Callback* newCallbacksArray = new Callback[++maxCallbacks];
		for(short i = 0; i < nbCallbacks; i++){
			newCallbacksArray[i].methodAndPath = callbacksArray[i].methodAndPath;
			newCallbacksArray[i].methodAndPathToMatch = callbacksArray[i].methodAndPathToMatch;
			newCallbacksArray[i].fct = callbacksArray[i].fct;
			newCallbacksArray[i].description = callbacksArray[i].description;
			newCallbacksArray[i].nbRestArgs = callbacksArray[i].nbRestArgs;
		}
		delete [] callbacksArray;
		callbacksArray = newCallbacksArray;
	}

	callbacksArray[nbCallbacks].fct = fct;
	callbacksArray[nbCallbacks].methodAndPath = methodAndPath;
	callbacksArray[nbCallbacks].description = description;

	// Check if there's an argument in the registered path (REST support)
	short argIndex = methodAndPath.indexOf("/{}");
	if (argIndex > 0) {
		callbacksArray[nbCallbacks].nbRestArgs = 1;
		callbacksArray[nbCallbacks].methodAndPathToMatch = methodAndPath.substring(0, argIndex);
	} else {
		callbacksArray[nbCallbacks].methodAndPathToMatch = methodAndPath;
	}

	nbCallbacks++;

	return true;
}

void WebServer::send200() {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
}

void WebServer::send201() {
    client.println("HTTP/1.1 201 Created");
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
	String urlNoQueryString = fullUrl;
	if(fullUrl.indexOf("?") > 0) {
		queryString = fullUrl.substring(fullUrl.indexOf("?") + 1, fullUrl.indexOf(" HTTP/"));
		urlNoQueryString = fullUrl.substring(0, fullUrl.indexOf("?")) + fullUrl.substring(fullUrl.indexOf(" HTTP/"), fullUrl.length());
	}

	// If there's a REST argument, extract it


	debug("urlNoQueryString : " + urlNoQueryString);
	debug("queryString      : " + queryString);

	for(short i = 0; i < nbCallbacks; i++){
		if (urlNoQueryString.indexOf(callbacksArray[i].methodAndPathToMatch) >= 0) {
			String trimmedUrl = urlNoQueryString;

			// If this endpoint is registered as REST, extract the argument (only a single argument is currently supported)
			String restArg1 = "";
			if (callbacksArray[i].isRest()) {
				short httpIdx = urlNoQueryString.indexOf("HTTP/");
				restArg1 = urlNoQueryString.substring(callbacksArray[i].methodAndPathToMatch.length() + 1, httpIdx - 1);
				debug("REST endpoint detected, argument is: " + restArg1);
				trimmedUrl = callbacksArray[i].methodAndPathToMatch + urlNoQueryString.substring(httpIdx - 1, urlNoQueryString.length());
				debug("trimmedUrl: " + trimmedUrl);
			}

			if (trimmedUrl.indexOf(callbacksArray[i].methodAndPathToMatch + " HTTP/") >= 0) {
				callbacksArray[i].fct(this, &client, queryString, restArg1);
				return true;
			}
		}
	}

	// The endpoint is not registered, if querying for "/", fallback to the default page. Else, return false.
	if (urlNoQueryString.indexOf("GET / HTTP/") >= 0) {
		sendDefaultRootPage(queryString);
		return true;
	}

	return false;
}

void WebServer::setDefaultPageTitle(String newPageTitle) {
	defaultPageTitle = newPageTitle;
}

void WebServer::sendDefaultRootPage(String queryString, bool closeFootAndBody) {
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

	if (closeFootAndBody) {
		sendWebPageFootAndCloseBody();
	}
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
		client.println("<td " + tdStyle + ">" + callbacksArray[i].methodAndPath + "</td>");
		client.println("<td " + tdStyle + ">" + callbacksArray[i].description + "</td>");
		client.println("</tr>");
	}
	client.println("</table>");
}

void WebServer::debug(String msg){
#ifdef __WEBSERVER_DEBUG__
	Serial.println("WebServer: " + msg);
#endif // __WEBSERVER_DEBUG__
}

