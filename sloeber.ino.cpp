#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2018-10-21 21:33:58

#include "Arduino.h"
#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "testWemos.h"
#include "WiFiManager.h"

void setup() ;
void loop() ;
void send200(WiFiClient* client) ;
void sendWebPage(WiFiClient* client) ;
void sendWebPageHead(WiFiClient* client) ;
void sendWebPageFoot(WiFiClient* client) ;
void sendWebPageContent(WiFiClient* client) ;
bool isProximityTriggered(void) ;
void processRequest(WiFiClient* client);
void sendToSlack(String s) ;

#include "testWemos.ino"


#endif
