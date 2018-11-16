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


#define BLACK    0,   0,   0
#define RED    255,   0,   0
#define GREEN    0, 255,   0
#define BLUE     0,   0, 255
#define ORANGE 255, 165,   0
#define BROWN  163,  76,   0
#define PURPLE 128,   0, 128

void send200(WiFiClient*);
void sendWebPage(WiFiClient*);
void sendWebPageHead(WiFiClient*);
void sendWebPageFoot(WiFiClient*);
void sendWebPageContent(WiFiClient*);

bool rootCallback(WebServer*, WiFiClient*, String, String);

bool cabinStatusCallback(WebServer*, WiFiClient*, String, String);

bool getConfigKeyCallback(WebServer*, WiFiClient*, String, String);
bool deleteConfigKeyCallback(WebServer*, WiFiClient*, String, String);
bool setConfigKeyCallback(WebServer*, WiFiClient*, String, String);

bool resetWiFiCallback(WebServer*, WiFiClient*, String, String);

bool isDoorClosed(void);
bool isButtonPressed(void);

bool isInSleepMode(void);

float getBatteryVoltage(void);

void sendToSlack(String);
void sendSslPOSTnoCertCheck(String, String, String);

void setPixelColor(short r, short g, short b);
void debug(String);
void loadConfig(void);

void fadeDownBrown(void);
void fadeDownGreen(void);

bool shouldSendLowBatAlert(void);
void sendLowBatAlert(void);

#endif /* TESTWEMOS_H_ */
