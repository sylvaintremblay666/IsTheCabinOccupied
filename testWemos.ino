#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "Adafruit_NeoPixel.h"
#include "testWemos.h"
#include "WiFiManager.h"
#include "WebServer.h"
#include "KeyValueFlash.h"

#define OnBoardLED D2
#define ButtonPin  D5
#define DoorSensorPin D6
#define NeoPixelPin   D7

#define RED    0, 255,  0
#define GREEN 255,  0,  0
#define BLUE   0,  0, 255
#define ORANGE 165, 255, 0

// Comment this line to disable serial debug output
#define __DEBUG__

// We use a NeoPixel (RGB led)
Adafruit_NeoPixel neoPixel = Adafruit_NeoPixel(1, NeoPixelPin, NEO_RGB + NEO_KHZ800);

// To store the state of the Sensor
bool sensorState;

WebServer *webServer;

// Slack
String slackWebHookToken = "";

//The setup function is called once at startup of the sketch
void setup()
{
    // Initialize serial console output
	Serial.begin(115200);

	// Initialize reed sensor pin and the sensor state
	pinMode(DoorSensorPin,INPUT);
	sensorState = isTriggered();

	// Initialize the onboard led
	pinMode(OnBoardLED, OUTPUT);
	digitalWrite(OnBoardLED, LOW);

	// Initialize the NeoPixel
	pinMode(NeoPixelPin, OUTPUT);
	neoPixel.setBrightness(50);
	neoPixel.begin();
	neoPixel.setPixelColor(0, RED);
	neoPixel.show();


	// Initialize the button pin
	pinMode(ButtonPin, INPUT);

	KeyValueFlash *config = new KeyValueFlash();
	String lastIP = config->getValue("lastIP");
	slackWebHookToken = config->getValue("slack_token");
	delete config;

    WiFiManager wifiManager;

    if (isButtonPressed()) {
    	wifiManager.setLastIP(lastIP);
    	neoPixel.setPixelColor(0,  ORANGE);
    	neoPixel.show();
    	wifiManager.startConfigPortal("SensorConfig");
    } else {
    	wifiManager.autoConnect("SensorConfig");
    }

	webServer = new WebServer();

	digitalWrite(OnBoardLED, HIGH);
	neoPixel.setPixelColor(0,  BLUE);
	neoPixel.show();

	// Print local IP address
	debug("");
	debug("WiFi connected.");
	debug("IP address: " + WiFi.localIP().toString());

	// Save IP address to flash memory
	config = new KeyValueFlash();
	config->setValue("lastIP", WiFi.localIP().toString());
	delete config;

	// Send WiFi infos to slack
	sendToSlack("Sensor connected to WiFi SSID: " + WiFi.SSID());
	sendToSlack("IP address: " + WiFi.localIP().toString());

	neoPixel.setPixelColor(0,  GREEN);
	neoPixel.show();

	// Register the WebServer endpoints and their callbacks
	webServer->setDefaultPageTitle("CabinSensor");
	webServer->registerEndpoint("GET /", "root page", rootCallback);
	webServer->registerEndpoint("GET /cabinStatus", "Get the status of the cabin (Occupied/Vacant)", cabinStatusCallback);
	webServer->registerEndpoint("GET /flash/read", "Get raw content of the config file from the flash", readFlashCallback);
	//webServer->registerEndpoint("GET /flash/write", "Write the query string as-is to the config file on the flash", writeFlashCallback);
	webServer->registerEndpoint("GET /flash/clear", "Clears the config file", clearConfigFileCallback);

	webServer->registerEndpoint("GET /config/get/{}", "Get an element from the config", getConfigKeyCallback);
	webServer->registerEndpoint("GET /config/set/{}", "Set an element in the config, queryString is the value", setConfigKeyCallback);
	webServer->registerEndpoint("GET /config/delete/{}", "Delete an element from the config", deleteConfigKeyCallback);


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

bool rootCallback(WebServer *ws, WiFiClient *client, String queryString, String restArg1) {
	KeyValueFlash config;
	String tdStyle = "style=\"border: 1px solid black;padding-right: 10px;padding-left: 10px\"";;

	ws->sendDefaultRootPage(queryString, false);

    client->println("<H2>Configuration parameters</H2>");
    client->println("<table style=\"border: 1px solid black;\">");

    KeyValueFlash::Pair *params = config.getConfig();
	client->println("<tr>");
	client->println("<th " + tdStyle + ">Key</th>");
	client->println("<th " + tdStyle + ">Value</th>");
	client->println("</tr>");
    for (short i = 0; i < config.getNbElements(); i++) {
    	client->println("<tr>");
    	client->println("<td " + tdStyle + ">" + params[i].k + "</td>");
    	client->println("<td " + tdStyle + ">" + params[i].v + "</td>");
    	client->println("</tr>");
    }

    ws->sendWebPageFootAndCloseBody();

	return true;
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

bool deleteConfigKeyCallback(WebServer *ws, WiFiClient *client, String queryString, String key) {
	KeyValueFlash config;
	ws->send200();

    config.deleteKey(key);
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
	int proxSensor = digitalRead(DoorSensorPin);
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

	if (slackWebHookToken.equals("")) {
		Serial.println("ERROR: Slack token not configured (slack_token configuration parameter)");
	} else {
		String msg = "{\"text\":\"" + s + "\"}";
		sendSslPOSTnoCertCheck("hooks.slack.com", String("/services/" + slackWebHookToken), msg);
	}
}

void debug(String msg) {
#ifdef __DEBUG__
	Serial.println(msg);
#endif // __DEBUG__
}
