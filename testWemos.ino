#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "testWemos.h"
#include "WiFiManager.h"
#include "WebServer.h"
#include "KeyValueFlash.h"

#define LED D8
#define OnBoardLED D2
#define ProxSensor D9
#define ButtonPin D5

// Comment this line to disable serial debug output
#define __DEBUG__

// To store the state of the LED
String ledState = "On";

// To store the state of the Sensor
bool sensorState;


WebServer *webServer;

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

	// Initialize the button pin
	pinMode(ButtonPin, INPUT);

	KeyValueFlash *config = new KeyValueFlash();
	String lastIP = config->getValue("lastIP");
	delete config;

    WiFiManager wifiManager;

    if (isButtonPressed()) {
    	wifiManager.setLastIP(lastIP);
    	wifiManager.startConfigPortal("SensorConfig");
    } else {
    	wifiManager.autoConnect("SensorConfig");
    }

	webServer = new WebServer();
//	wifiManager.startConfigPortal("ToTo");

//	WiFi.begin("666", "shantimongrosminetfou");

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	digitalWrite(LED, HIGH);
	digitalWrite(OnBoardLED, HIGH);
	ledState = "On";

	// Print local IP address and start web server
	debug("");
	debug("WiFi connected.");
	debug("IP address: " + WiFi.localIP().toString());

	config = new KeyValueFlash();
	config->setValue("lastIP", WiFi.localIP().toString());
	delete config;

	sendToSlack("Sensor connected to WiFi SSID: " + WiFi.SSID());
	sendToSlack("IP address: " + WiFi.localIP().toString());

	// Register the WebServer endpoints and their callbacks
	webServer->setDefaultPageTitle("CabinSensor");
	webServer->registerEndpoint("GET /cabinStatus", "Get the status of the cabin (Occupied/Vacant)", cabinStatusCallback);
	webServer->registerEndpoint("GET /flash/read", "Get raw content of the config file from the flash", readFlashCallback);
	//webServer->registerEndpoint("GET /flash/write", "Write the query string as-is to the config file on the flash", writeFlashCallback);
	webServer->registerEndpoint("GET /flash/clear", "Clears the config file", clearConfigFileCallback);

	webServer->registerEndpoint("GET /config/get/{}", "Get an element from the config", getConfigKeyCallback);
	webServer->registerEndpoint("GET /config/set/{}", "Set an element in the config, queryString is the value", setConfigKeyCallback);

	webServer->registerEndpoint("GET /wifi/reset", "Reset the WiFi configuration", resetWiFiCallback);
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

	webServer->checkForClientAndProcessRequest();

}

bool cabinStatusCallback(WebServer *ws, WiFiClient *client, String queryString, String restArg1) {
    ws->send200();

    if(isTriggered()){
    	client->println("Occupied");
    } else {
    	client->println("Vacant");
    }

	return true;
}

bool readFlashCallback(WebServer *ws, WiFiClient *client, String queryString, String restArg1) {
	KeyValueFlash config;
	ws->send200();

    client->println("<H2>Config file raw content</H2>");
    client->println(config.getRawContent());

	return true;
}

bool writeFlashCallback(WebServer *ws, WiFiClient *client, String queryString, String restArg1) {
	KeyValueFlash config;

	ws->send200();

    config.writeRawContent(queryString);
    client->println("Success");

	return true;
}

bool getConfigKeyCallback(WebServer *ws, WiFiClient *client, String queryString, String configKey) {
	KeyValueFlash config;
	String value = config.getValue(configKey);
    if (!value.equals("")) {
    	ws->send200();
    	client->println(value);
    } else {
    	ws->send404();
    }

	return true;
}

bool setConfigKeyCallback(WebServer *ws, WiFiClient *client, String queryString, String key) {
	KeyValueFlash config;
	ws->send201();

    config.setValue(key, queryString);
    client->println("Success");

	return true;
}

bool clearConfigFileCallback(WebServer *ws, WiFiClient *client, String queryString, String restArg1) {
	KeyValueFlash config;
	ws->send200();

    config.clearConfigFile();
    client->println("Success");

	return true;
}

bool resetWiFiCallback(WebServer *ws, WiFiClient *client, String queryString, String restArg1) {
    ws->send200();

    WiFiManager wifiManager;
    wifiManager.resetSettings();

	return true;
}

bool isTriggered(void) {
	int proxSensor = digitalRead(ProxSensor);
	return proxSensor == LOW;
}

bool isButtonPressed(void) {
	return (digitalRead(ButtonPin) == HIGH);
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

      /* I don't care about the reply here, but wanted to keep the lines...

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
