/*
 * KeyValueFlash.cpp
 *
 *  Created on: Oct 27, 2018
 *      Author: stremblay
 */
#include "KeyValueFlash.h"

KeyValueFlash::KeyValueFlash() {
	Serial.println("KeyValueFlash: initializing and reading config file");
	SPIFFS.begin();

	readConfigFile();
}

KeyValueFlash::KeyValueFlash(String configName) {
	configFileName = configName;
	KeyValueFlash();
}

KeyValueFlash::~KeyValueFlash() {
}

String KeyValueFlash::getRawContent() {
	Serial.println("KeyValueFlash: Reading from config");
	readConfigFile();
	return configFileContent;
}

void KeyValueFlash::writeRawContent(String rawContent) {
	Serial.println("KeyValueFlash: Writing to config");
	File configFile = SPIFFS.open(configFileName, "w+");
	if (!configFile) {
	    Serial.println("KeyValueFlash: file open failed (when writing)");
	    return;
	}

	configFile.print(rawContent + ';');
	configFile.close();
}

String KeyValueFlash::getValue(String key) {
	String rawConfig = configFileContent;

	Serial.println("KeyValueFlash: Searching for key: \"" + key + "\"");

	while(rawConfig != "") {
		String nextKey = rawConfig.substring(0, rawConfig.indexOf('='));
		String nextVal = rawConfig.substring(rawConfig.indexOf('=') + 1, rawConfig.indexOf(','));

		if(nextKey.equals(key)) {
			Serial.println("Found! Value: " + nextVal);
			return nextVal;
		}

		rawConfig = rawConfig.substring(rawConfig.indexOf(',') + 1, rawConfig.length());
	}
	Serial.println("Not found.");

	return "";
}

void KeyValueFlash::setValue(String key, String val) {
	File configFile = SPIFFS.open(configFileName, "w+");

    String rawConfig = configFileContent;

    Serial.println(String("Saving key / val: " + key + " / " + val));
	while(rawConfig != "") {
		String nextKey = rawConfig.substring(0, rawConfig.indexOf('='));
		String nextVal = rawConfig.substring(rawConfig.indexOf('=') + 1, rawConfig.indexOf(','));

		if(!nextKey.equals(key)) {
			configFile.print(nextKey + "=" + nextVal + ",");
		}

		rawConfig = rawConfig.substring(rawConfig.indexOf(',') + 1, rawConfig.length());
	}

	configFile.println(key + "=" + val + ",;");

	configFile.close();
	readConfigFile();
}

void KeyValueFlash::readConfigFile() {
	File configFile = SPIFFS.open(configFileName, "r");
	if (!configFile) {
	    Serial.println("KeyValueFlash: file open failed (when trying to read)");
	    configFileContent = ";";
	    return;
	}

	configFileContent = configFile.readStringUntil(';');
    Serial.println("Content:");
    Serial.println(configFileContent);

	configFile.close();
}

void KeyValueFlash::clearConfigFile() {
	Serial.println("remove rc:" + SPIFFS.remove(configFileName));
	File configFile = SPIFFS.open(configFileName, "w+");
	if (!configFile) {
		Serial.println("KeyValueFlash: file open failed (when initializing)");
		return;
	}

	configFile.println("reset=true,;");
	configFile.println("");
	configFile.flush();
	configFile.close();
}
