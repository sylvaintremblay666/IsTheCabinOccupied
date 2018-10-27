#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2018-10-27 10:11:58

#include "Arduino.h"
#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "testWemos.h"
#include "WiFiManager.h"
#include "WebServer.h"

void setup() ;
void loop() ;
bool rootCallback(WebServer *ws, WiFiClient *client, String queryString) ;
bool cabinStatusCallback(WebServer *ws, WiFiClient *client, String queryString) ;
bool isTriggered(void) ;
void sendSslPOSTnoCertCheck(String host, String url, String msg);
void sendToSlack(String s) ;
void debug(String msg) ;

#include "testWemos.ino"


#endif
