#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2018-10-26 15:07:02

#include "Arduino.h"
#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "testWemos.h"
#include "WiFiManager.h"
#include "WebServer.h"

void setup() ;
void loop() ;
bool rootCallBack(void *webServer, WiFiClient *client) ;
bool isTriggered(void) ;
void sendSslPOSTnoCertCheck(String url, String msg);
void sendToSlack(String s) ;

#include "testWemos.ino"


#endif
