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

bool rootCallback(WebServer*, WiFiClient*, String, String);

bool cabinStatusCallback(WebServer*, WiFiClient*, String, String);
bool readFlashCallback(WebServer*, WiFiClient*, String, String);
bool writeFlashCallback(WebServer*, WiFiClient*, String, String);

bool clearConfigFileCallback(WebServer*, WiFiClient*, String, String);
bool getConfigKeyCallback(WebServer*, WiFiClient*, String, String);
bool deleteConfigKeyCallback(WebServer*, WiFiClient*, String, String);
bool setConfigKeyCallback(WebServer*, WiFiClient*, String, String);

bool resetWiFiCallback(WebServer*, WiFiClient*, String, String);

bool isTriggered(void);
bool isButtonPressed(void);

void sendToSlack(String);
void sendSslPOSTnoCertCheck(String, String, String);

void debug(String);

#endif /* TESTWEMOS_H_ */
