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
	KeyValueFlash();
	KeyValueFlash(String);
	virtual ~KeyValueFlash();

	String getValue(String);
	void setValue(String, String);

	String getRawContent();
	void writeRawContent(String);

	void clearConfigFile(void);

private:
	String configFileName = "default";
	String configFileContent = ";";

	void readConfigFile(void);
};

#endif /* KEYVALUEFLASH_H_ */
