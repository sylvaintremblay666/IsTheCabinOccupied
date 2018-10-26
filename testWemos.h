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

bool rootCallBack(void*, WiFiClient*);
bool cabinStatusCallBack(void*, WiFiClient*);

bool isTriggered(void);
void processRequest(WiFiClient*);

void sendToSlack(String);
void sendSslPOSTnoCertCheck(String, String, String);

void debug(String);

#endif /* TESTWEMOS_H_ */
