#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "testWemos.h"
#include "WiFiManager.h"
#include "WebServer.h"

#define LED D8
#define OnBoardLED D2
#define ProxSensor D9

// Comment this line to disable serial debug output
#define __DEBUG__

// To store the state of the LED
String ledState = "On";

// To store the state of the Sensor
bool sensorState;

// Library to connect to WiFi
WiFiManager wifiManager;

WebServer webServer;

// Slack
#define WEBHOOK_HOST "hooks.slack.com"
#define WEBHOOK_PATH "/services/T0FC7JHLP/BDLBRNSJ2/WlMaQHi3YP0qTxlVshwCSYZf" // stremblay
//#define WEBHOOK_PATH "/services/T0FHQRZT8/BDJC44JQ1/8kdHBtx2GOmvTHVQvkIaqGCl" // ingeno


//The setup function is called once at startup of the sketch
void setup()
{
    // Initialize serial console output
	Serial.begin(115200);

	// Initialize reed sensor pin and the sensor state
	pinMode(ProxSensor,INPUT);
	sensorState = isTriggered();

	// Initialize the LED pins
	pinMode(LED, OUTPUT);
	pinMode(OnBoardLED, OUTPUT);
	digitalWrite(LED, LOW);
	digitalWrite(OnBoardLED, LOW);


	wifiManager.autoConnect("CabinSensor");

	digitalWrite(LED, HIGH);
	digitalWrite(OnBoardLED, HIGH);
	ledState = "On";

	// Print local IP address and start web server
	debug("");
	debug("WiFi connected.");
	debug("IP address: " + WiFi.localIP().toString());

	sendToSlack("Sensor connected to WiFi SSID: " + WiFi.SSID());
	sendToSlack("IP address: " + WiFi.localIP().toString());

	webServer.registerEndpoint("GET /", "", rootCallback);
	webServer.registerEndpoint("GET /cabinStatus", "Get the status of the cabin (Occupied/Vacant)", cabinStatusCallback);
}

// The loop function is called in an endless loop
void loop()
{
	if (sensorState != isTriggered()) {
		sensorState = !sensorState;
		if(sensorState) {
			sendToSlack("Occupied");
		} else {
			sendToSlack("Empty");
		}
	}

	webServer.checkForClientAndProcessRequest();

}

bool rootCallback(WebServer *ws, WiFiClient *client, String queryString) {
    ws->send200();

    ws->sendWebPageHead();

    ws->sendWiFiInfos();
	ws->sendEndpointsList();

	client->println("<H2>QueryString</H2>");
	client->println(queryString);

	ws->sendWebPageFoot();

	return true;
}

bool cabinStatusCallback(WebServer *ws, WiFiClient *client, String queryString) {
    ws->send200();

    if(isTriggered()){
    	client->println("Occupied");
    } else {
    	client->println("Vacant");
    }

	return true;
}

bool isTriggered(void) {
	int proxSensor = digitalRead(ProxSensor);
	return proxSensor == LOW;
}

void sendSslPOSTnoCertCheck(String host, String url, String msg){
	WiFiClientSecure client;

	if (!client.connect(host, 443)) {
		debug("connection failed");
		return;
	}

	// This is how to validate the certificate's fingerprint, but the
	// right fingerprint needs to be registered first for this to work.
	/*
	 if (client.verify(fingerprint, host)) {
	 	 debug("certificate matches");
	 	 } else {
	 	 debug("certificate doesn't match");
	 }
	 */

	  debug("requesting URL: '" + url + "'");

	  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
	          "Host: " + host + "\r\n" +
	          "Connection: close\r\n" +
	          "Accept: */*\r\n" +
	          "User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n" +
	          "Content-Type: application/json;charset=utf-8\r\n" +
	          "Content-Length: "+msg.length()+"\r\n" +
	          "\r\n" +
	          msg + "\r\n");

	  debug("request sent");

	  unsigned long timeout = millis();
	  while (client.available() == 0) {
	    if (millis() - timeout > 5000) {
	      debug(">>> Client Timeout !");
	      client.stop();
	    }
	  }

      /*
	  // Read all the lines of the reply from server and print them to Serial
	  while(client.available()){
	    String line = client.readStringUntil('\r');
	    Serial.print(line);
	  }
	  client.stop();
      */
}

void sendToSlack(String s) {
	WiFiClientSecure client;

	String msg = "{\"text\":\"" + s + "\"}";
	sendSslPOSTnoCertCheck(WEBHOOK_HOST, WEBHOOK_PATH, msg);
}

void debug(String msg) {
#ifdef __DEBUG__
	Serial.println(msg);
#endif // __DEBUG__
}
