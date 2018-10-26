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
	typedef bool (*CallbackFct)(void*, WiFiClient*);

	WebServer();
	virtual ~WebServer();

	void checkForClientAndProcessRequest(void);

	void send200();
	void send404();
	void sendWebPageHead();
	void sendWebPageFoot();

	bool processRequest();

	bool registerCallback(String, CallbackFct);

private:
	typedef struct {
			CallbackFct fct;
			String contextPath;
		} Callback;

	short maxCallbacks = 5;
	short nbCallbacks = 0;

	String header;
	WiFiServer *server;
	WiFiClient client;

    Callback* callbacks;

    void debug(String);
};

#endif /* WEBSERVER_H_ */
