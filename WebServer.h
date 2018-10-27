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
	typedef bool (*CallbackFct)(WebServer*, WiFiClient*, String);

	WebServer(void);
	virtual ~WebServer(void);

	void checkForClientAndProcessRequest(void);

	/**
	 * Sends a "HTTP/1.1 200 OK" along with the content-type and connection: close
	 */
	void send200(void);


	void send404(void);
	void sendWebPageHead(void);
	void sendWebPageFoot();

	void sendEndpointsList(void);
	void sendWiFiInfos(void);

	bool processRequest(void);


	bool registerEndpoint(String, String, CallbackFct);

private:
	typedef struct {
			CallbackFct fct;
			String methodAndPath;
			String description;
		} Callback;

	short maxCallbacks = 5;
	short nbCallbacks = 0;

	const String tdStyle = "style=\"border: 1px solid black;padding-right: 10px;padding-left: 10px\"";;

	String header;
	WiFiServer *server;
	WiFiClient client;

    Callback* callbacks;

    void debug(String);
};

#endif /* WEBSERVER_H_ */
