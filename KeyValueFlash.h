/*
 * KeyValueFlash.h
 *
 *  Created on: Oct 27, 2018
 *      Author: stremblay
 */

#ifndef KEYVALUEFLASH_H_
#define KEYVALUEFLASH_H_

#include "Arduino.h"
#include "FS.h"

class KeyValueFlash {
public:
	typedef struct {
		String k;
		String v;
	} Pair;

	KeyValueFlash();
	KeyValueFlash(String);
	virtual ~KeyValueFlash();

	String getValue(String);
	void setValue(String, String);

	short getNbElements(void);
	Pair* getConfig(void);

	String getRawContent();
	void writeRawContent(String);

	void clearConfigFile(void);

private:
	String configFileName = "default";
	String configFileContent = ";";

	Pair *configElements = 0;
	short nbElements = 0;

	void readConfigFile(void);
	void parseConfigFile(void);
};

#endif /* KEYVALUEFLASH_H_ */
