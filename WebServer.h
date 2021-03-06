/*
 * WebServer.h
 *
 *  Created on: Oct 24, 2018
 *      Author: stremblay
 */

#ifndef WEBSERVER_H_
#define WEBSERVER_H_

#include "Arduino.h"
#include "ESP8266WiFi.h"

class WebServer {
public:
	typedef bool (*CallbackFct)(WebServer*, WiFiClient*, String, String);

	WebServer(void);
	virtual ~WebServer(void);

	void checkForClientAndProcessRequest(void);

	/**
	 * Sends a "HTTP/1.1 200 OK" along with the content-type and connection: close
	 */
	void send200(void);
	void send201(void);


	void send404(void);
	void sendWebPageHeadAndOpenBody();
	void sendWebPageFootAndCloseBody();

	void sendEndpointsList(void);
	void sendWiFiInfos(void);

	void sendDefaultRootPage(String, bool closeFootAndBody = true);

	bool processRequest(void);


	bool registerEndpoint(String, String, CallbackFct);
	void setDefaultPageTitle(String);

private:
	typedef struct {
			CallbackFct fct;
			String methodAndPath;
			String methodAndPathToMatch;
			String description;
			short nbRestArgs = 0;

			bool isRest(void){ return nbRestArgs > 0; }
		} Callback;

	short maxCallbacks = 5;
	short nbCallbacks = 0;

	const String tdStyle = "style=\"border: 1px solid black;padding-right: 10px;padding-left: 10px\"";

	String header;
	WiFiServer *server;
	WiFiClient client;
	String defaultPageTitle = "ESP8266 Web Server";

    Callback* callbacksArray;

    void debug(String);
};

#endif /* WEBSERVER_H_ */
