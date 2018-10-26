#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "testWemos.h"
#include "WiFiManager.h"
#include "WebServer.h"

#define LED D8
#define OnBoardLED D2
#define ProxSensor D9

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
	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	sendToSlack("Sensor connected to WiFi SSID: " + WiFi.SSID());
	sendToSlack("IP address: " + WiFi.localIP().toString());

	webServer.registerCallback("GET /", rootCallBack);
	webServer.registerCallback("GET /1", rootCallBack);
	webServer.registerCallback("GET /2", rootCallBack);
}

// The loop function is called in an endless loop
void loop()
{
	if (sensorState != isTriggered()) {
		sensorState = !sensorState;
		if(sensorState) {
			sendToSlack("Presence detected");
		} else {
			sendToSlack("No more presence detected");
		}
	}

	webServer.checkForClientAndProcessRequest();

}

bool rootCallBack(void *webServer, WiFiClient *client) {
    WebServer *ws = (WebServer*) webServer;

    Serial.println("Into callBack!");

    ws->send200();

    ws->sendWebPageHead();

	client->println("<H2>CallBack!</H2>");

	ws->sendWebPageFoot();

	return true;
}

bool isTriggered(void) {
	int proxSensor = digitalRead(ProxSensor);
	return proxSensor == LOW;
}

void sendSslPOSTnoCertCheck(String url, String msg){
	WiFiClientSecure client;

	if (!client.connect(url, 443)) {
		Serial.println("connection failed");
		return;
	}

	// This is how to validate the certificate's fingerprint, but the
	// right fingerprint needs to be registered first for this to work.
	/*
	 if (client.verify(fingerprint, host)) {
	 	 Serial.println("certificate matches");
	 	 } else {
	 	 Serial.println("certificate doesn't match");
	 }
	 */

	  Serial.print("requesting URL: '");
	  Serial.print(WEBHOOK_PATH);
	  Serial.println("'");

	  client.print(String("POST ") + WEBHOOK_PATH + " HTTP/1.1\r\n" +
	          "Host: " + WEBHOOK_HOST + "\r\n" +
	          "Connection: close\r\n" +
	          "Accept: */*\r\n" +
	          "User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n" +
	          "Content-Type: application/json;charset=utf-8\r\n" +
	          "Content-Length: "+msg.length()+"\r\n" +
	          "\r\n" +
	          msg + "\r\n");

	  Serial.println("request sent");

	  unsigned long timeout = millis();
	  while (client.available() == 0) {
	    if (millis() - timeout > 5000) {
	      Serial.println(">>> Client Timeout !");
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
	sendSslPOSTnoCertCheck(WEBHOOK_HOST, msg);
}

/*
void sendWebPageHead(WiFiClient* client) {
    // Display the HTML web page
    client->println("<!DOCTYPE html><html>");
    client->println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client->println("<link rel=\"icon\" href=\"data:,\">");
    // CSS to style the on/off buttons
    // Feel free to change the background-color and font-size attributes to fit your preferences
    client->println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
    client->println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
    client->println("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
    client->println(".button2 {background-color: #77878A;}</style></head>");

    // Web Page Heading
    client->println("<body><h1>IsTheCabinOccupied? - ESP8266 Web Server</h1>");
}

void sendWebPageContent(WiFiClient* client) {
	client->println("<p>Welcome Here!</p>");

	client->print("<p>Is proximity sensor triggered: ");
	isProximityTriggered() ? client->print("Yes") : client->print("No");
	client->println("</p>");

	if (ledState == "Off") {
		client->println("<p><a href=\"/LED/on\"><button class=\"button\">Turn Led ON</button></a></p>");
	} else {
		client->println("<p><a href=\"/LED/off\"><button class=\"button button2\">Turn Led OFF</button></a></p>");
	}

	client->println("<p><a href=\"/ResetWiFi\"><button class=\"button button2\">Reset WiFi Settings</button></a></p>");
}
*/
