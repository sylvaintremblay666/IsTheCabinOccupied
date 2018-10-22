#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "testWemos.h"
#include "WiFiManager.h"

#define LED D8
#define OnBoardLED D2
#define ProxSensor D9

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// To store the state of the LED
String ledState = "On";

// To store the state of the Sensor
bool sensorState;

// Library to connect to WiFi
WiFiManager wifiManager;

// Slack
#define WEBHOOK_HOST "hooks.slack.com"
//#define WEBHOOK_PATH "/services/T0FC7JHLP/BDLBRNSJ2/WlMaQHi3YP0qTxlVshwCSYZf"
#define WEBHOOK_PATH "/services/T0FHQRZT8/BDJC44JQ1/8kdHBtx2GOmvTHVQvkIaqGCl"

// https://hooks.slack.com/services/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

//The setup function is called once at startup of the sketch
void setup()
{
    // Initialize serial console output
	Serial.begin(115200);

	// Initialize proximity sensor pin and the sensor state
	pinMode(ProxSensor,INPUT);
	sensorState = isProximityTriggered();


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

	server.begin();
}

// The loop function is called in an endless loop
void loop()
{
	if (sensorState != isProximityTriggered()) {
		sensorState = !sensorState;
		if(sensorState) {
			sendToSlack("Presence detected");
		} else {
			sendToSlack("No more presence detected");
		}
	}

	WiFiClient client = server.available();   // Listen for incoming clients

	if (client) {                             // If a new client connects,
		Serial.println("New Client.");        // print a message out in the serial port
		String currentLine = "";              // make a String to hold incoming data from the client
		while (client.connected()) {          // loop while the client's connected
			if (client.available()) {         // if there's bytes to read from the client,
				char c = client.read();             // read a byte, then
				Serial.write(c);              // print it out the serial monitor
				header += c;
				if (c == '\n') {              // if the byte is a newline character
					// if the current line is blank, you got two newline characters in a row.
					// that's the end of the client HTTP request, so send a response:
					if (currentLine.length() == 0) {
						// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
						// and a content-type so the client knows what's coming, then a blank line:

						processRequest(&client);

						send200(&client);

						sendWebPage(&client);

						// Break out of the while loop
						break;
					} else { // if you got a newline, then clear currentLine
						currentLine = "";
					}
				} else if (c != '\r') { // if you got anything else but a carriage return character,
					currentLine += c;    // add it to the end of the currentLine
				}
			}
		}
		// Clear the header variable
		header = "";
		// Close the connection
		client.stop();
		Serial.println("Client disconnected.");
		Serial.println("");
	}
}

void send200(WiFiClient* client) {
    client->println("HTTP/1.1 200 OK");
    client->println("Content-type:text/html");
    client->println("Connection: close");
    client->println();
}

void sendWebPage(WiFiClient* client) {
	sendWebPageHead(client);
	sendWebPageContent(client);
	sendWebPageFoot(client);
}

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

void sendWebPageFoot(WiFiClient* client) {
    client->println("</body></html>");

    // The HTTP response ends with another blank line
    client->println();
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

bool isProximityTriggered(void) {
	int proxSensor = digitalRead(ProxSensor);
	return proxSensor == LOW;
}

void processRequest(WiFiClient* client){
	if (header.indexOf("GET /LED/on") >= 0) {
		Serial.println("LED On");
		ledState = "On";
		digitalWrite(LED, HIGH);
	} else if (header.indexOf("GET /LED/off") >= 0) {
		Serial.println("LED Off");
		ledState = "Off";
		digitalWrite(LED, LOW);
	} else if (header.indexOf("GET /ResetWiFi") >= 0) {
		Serial.println("Reset WiFi parameters");
		send200(client);
		wifiManager.resetSettings();
	}
}

void sendToSlack(String s) {
	WiFiClientSecure client;

	String msg = "{\"text\":\"" + s + "\"}";

	  Serial.print("connecting to : '");
	  Serial.print(WEBHOOK_HOST);
	  Serial.println("'");

	  Serial.println("message : ");
	  Serial.println(msg);

	  if (!client.connect(WEBHOOK_HOST, 443)) {
	    Serial.println("connection failed");
	    return;
	  }

	//  if (client.verify(fingerprint, host)) {
	//    Serial.println("certificate matches");
	//  } else {
	//    Serial.println("certificate doesn't match");
	//  }

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
	      return;
	    }
	  }
/*
	  // Read all the lines of the reply from server and print them to Serial
	  while(client.available()){
	    String line = client.readStringUntil('\r');
	    Serial.print(line);
	  }
	  client.stop();*/
}
