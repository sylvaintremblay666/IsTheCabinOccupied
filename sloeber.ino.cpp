#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2018-10-28 08:37:09

#include "Arduino.h"
#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "testWemos.h"
#include "WiFiManager.h"
#include "WebServer.h"
#include "KeyValueFlash.h"

void setup() ;
void loop() ;
bool cabinStatusCallback(WebServer *ws, WiFiClient *client, String queryString) ;
bool readFlashCallback(WebServer *ws, WiFiClient *client, String queryString) ;
bool writeFlashCallback(WebServer *ws, WiFiClient *client, String queryString) ;
bool getConfigKeyCallback(WebServer *ws, WiFiClient *client, String queryString) ;
bool setConfigKeyCallback(WebServer *ws, WiFiClient *client, String queryString) ;
bool clearConfigFileCallback(WebServer *ws, WiFiClient *client, String queryString) ;
bool isTriggered(void) ;
void sendSslPOSTnoCertCheck(String host, String url, String msg);
void sendToSlack(String s) ;
void debug(String msg) ;

#include "testWemos.ino"


#endif
