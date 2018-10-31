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

#define BLACK    0,   0,   0
#define RED    255,   0,   0
#define GREEN    0, 255,   0
#define BLUE     0,   0, 255
#define ORANGE 255, 165,   0
#define BROWN  163,  76,   0

// Comment this line to disable serial debug output
#define __DEBUG__

// We use a NeoPixel (RGB led)
Adafruit_NeoPixel *neoPixel;

// State holders
bool isDoorClosedLastState = false;
bool isLedSleeping         = false;

// The WebServer
WebServer *webServer;

// Internal counters
unsigned long lastMillisDoorLatch       = millis();
unsigned long lastMillisWiFiConfigReset = millis();
unsigned long lastMillisLedSleep        = millis();

// Config variables
String slackWebHookToken = ""; // Slack

unsigned long doorLatchDelayMS      = 1500;
unsigned long wifiCfgResetMS        = 10000;
unsigned long timeBeforeLedSleepMS  = 10000;

//The setup function is called once at startup of the sketch
void setup()
{
    // Initialize serial console output
	Serial.begin(115200);

	// Initialize reed sensor pin and the sensor state
	pinMode(DoorSensorPin,INPUT);
	isDoorClosedLastState = isDoorSensorTriggered();

	// Initialize the onboard led
	pinMode(OnBoardLED, OUTPUT);
	digitalWrite(OnBoardLED, LOW);

	// Initialize the NeoPixel
	pinMode(NeoPixelPin, OUTPUT);
	neoPixel = new Adafruit_NeoPixel(1, NeoPixelPin, NEO_RGB + NEO_KHZ800);
	setPixelColor(RED);

	// Initialize the button pin
	pinMode(ButtonPin, INPUT);

	KeyValueFlash *config = new KeyValueFlash();
	String lastIP = config->getValue("lastIP");
	slackWebHookToken = config->getValue("slack_token");
	delete config;

	loadConfig();

    WiFiManager wifiManager;

    if (isButtonPressed()) {
    	setPixelColor(ORANGE);
    	wifiManager.setLastIP(lastIP);
    	wifiManager.startConfigPortal("DoorSensorConfig");
    } else {
    	wifiManager.autoConnect("DoorSensorConfig");
    }

	webServer = new WebServer();

	digitalWrite(OnBoardLED, HIGH);
	setPixelColor(BLUE);

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

	setPixelColor(GREEN);

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
	unsigned long currentMillis = millis();
	bool isDoorClosed = isDoorSensorTriggered();

	if (isButtonPressed()) {
		if (currentMillis - lastMillisWiFiConfigReset > wifiCfgResetMS) {
			Serial.println("Hard reset of WiFi config!");

			for(short i = 0; i < 20; i++) {
				setPixelColor(RED);
				delay(50);
				setPixelColor(BLUE);
				delay(50);
			}
			setPixelColor(ORANGE);
			WiFiManager wifiManager;
			wifiManager.resetSettings();
			delay(1000);
			ESP.restart();
		}

		lastMillisLedSleep = currentMillis;
		isLedSleeping = false;
		isDoorClosed ? setPixelColor(BROWN) : setPixelColor(GREEN);

	} else {
		if ((!isLedSleeping) && (currentMillis - lastMillisLedSleep > timeBeforeLedSleepMS)) {
			isLedSleeping = true;
			isDoorClosed ? fadeDownBrown() : fadeDownGreen();
		}

		lastMillisWiFiConfigReset = currentMillis;
	}

	if (isDoorClosedLastState != isDoorClosed) {
		if (currentMillis - lastMillisDoorLatch > doorLatchDelayMS) {
			lastMillisDoorLatch = currentMillis;
			isDoorClosedLastState = isDoorClosed;
			if (isDoorClosed) {
				sendToSlack("Occupied");
				if (!isLedSleeping) setPixelColor(BROWN);
			} else {
				sendToSlack("Empty");
				if (!isLedSleeping) setPixelColor(GREEN);
			}
		}
	} else {
		lastMillisDoorLatch = currentMillis;
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
    client->println("</table>");

    client->println("<H2>Door sensor status</H2>");
    if (isDoorSensorTriggered()) {
    	client->println("Door closed");
    } else {
    	client->println("Door open");
    }

    ws->sendWebPageFootAndCloseBody();

	return true;
}

bool cabinStatusCallback(WebServer *ws, WiFiClient *client, String queryString, String restArg1) {
    ws->send200();

    if(isDoorSensorTriggered()){
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

    loadConfig();

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

bool isDoorSensorTriggered(void) {
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

void setPixelColor(short r, short g, short b) {
	neoPixel->setBrightness(50);
	neoPixel->begin();
	neoPixel->setPixelColor(0, g, r, b);
	neoPixel->show();
}

void loadConfig(void) {
	KeyValueFlash config;

	// door_latch_delay_ms
	String doorLatchDelayMSString = config.getValue("door_latch_delay_ms");
	if (!doorLatchDelayMSString.equals("")) {
		if (doorLatchDelayMSString.toInt() >= 0) {
			doorLatchDelayMS = doorLatchDelayMSString.toInt();
		}
	} else {
		config.setValue("door_latch_delay_ms", String(doorLatchDelayMS));
	}

	// time_before_led_sleep_ms
	String timeBeforeLedSleepMSString = config.getValue("time_before_led_sleep_ms");
	if (!timeBeforeLedSleepMSString.equals("")) {
		if (timeBeforeLedSleepMSString.toInt() >= 0) {
			timeBeforeLedSleepMS = timeBeforeLedSleepMSString.toInt();
		}
	} else {
		config.setValue("time_before_led_sleep_ms", String(timeBeforeLedSleepMS));
	}
}

void fadeDownBrown(void) {
	for (short r = 163, g=76; r >= 0; r--, g--) {
		if (g < 0) g = 0;
		setPixelColor(r, g, 0);
		delay(12);
	}
}

void fadeDownGreen() {
	for (int g = 255; g >= 0; g--) {
		setPixelColor(0, g, 0);
		delay(10);
	}
}

void debug(String msg) {
#ifdef __DEBUG__
	Serial.println(msg);
#endif // __DEBUG__
}
