#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2018-10-29 13:59:18

#include "Arduino.h"
#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "Adafruit_NeoPixel.h"
#include "testWemos.h"
#include "WiFiManager.h"
#include "WebServer.h"
#include "KeyValueFlash.h"

void setup() ;
void loop() ;
bool rootCallback(WebServer *ws, WiFiClient *client, String queryString, String restArg1) ;
bool cabinStatusCallback(WebServer *ws, WiFiClient *client, String queryString, String restArg1) ;
bool readFlashCallback(WebServer *ws, WiFiClient *client, String queryString, String restArg1) ;
bool writeFlashCallback(WebServer *ws, WiFiClient *client, String queryString, String restArg1) ;
bool getConfigKeyCallback(WebServer *ws, WiFiClient *client, String queryString, String configKey) ;
bool setConfigKeyCallback(WebServer *ws, WiFiClient *client, String queryString, String key) ;
bool deleteConfigKeyCallback(WebServer *ws, WiFiClient *client, String queryString, String key) ;
bool clearConfigFileCallback(WebServer *ws, WiFiClient *client, String queryString, String restArg1) ;
bool resetWiFiCallback(WebServer *ws, WiFiClient *client, String queryString, String restArg1) ;
bool isTriggered(void) ;
bool isButtonPressed(void) ;
void sendSslPOSTnoCertCheck(String host, String url, String msg);
void sendToSlack(String s) ;
void debug(String msg) ;

#include "testWemos.ino"


#endif
