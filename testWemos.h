/*
 * testWemos.h
 *
 *  Created on: Oct 21, 2018
 *      Author: stremblay
 */
#include "ESP8266WiFi.h"
#include "WebServer.h"

#ifndef TESTWEMOS_H_
#define TESTWEMOS_H_

void send200(WiFiClient*);
void sendWebPage(WiFiClient*);
void sendWebPageHead(WiFiClient*);
void sendWebPageFoot(WiFiClient*);
void sendWebPageContent(WiFiClient*);

bool cabinStatusCallback(WebServer*, WiFiClient*, String);
bool readFlashCallback(WebServer*, WiFiClient*, String);
bool writeFlashCallback(WebServer*, WiFiClient*, String);

bool clearConfigFileCallback(WebServer*, WiFiClient*, String);
bool getConfigKeyCallback(WebServer*, WiFiClient*, String);
bool setConfigKeyCallback(WebServer*, WiFiClient*, String);

bool isTriggered(void);
void processRequest(WiFiClient*);

void sendToSlack(String);
void sendSslPOSTnoCertCheck(String, String, String);

void debug(String);

#endif /* TESTWEMOS_H_ */
