/*
 * KeyValueFlash.h
 *
 *  Created on: Nov 12, 2018
 *      Author: stremblay
 */

#ifndef KEYVALUEFLASH_H_
#define KEYVALUEFLASH_H_

#include "Arduino.h"
#include "FS.h"

#define INITIAL_ELEMENTS_ARRAY_SIZE 10
#define MAX_KEY_SIZE 32

class KeyValueFlash {
public:
	KeyValueFlash();
	KeyValueFlash(String configName);
	virtual ~KeyValueFlash();

	typedef struct {
	    char k[MAX_KEY_SIZE];
	    char v[64];
	} Pair;

	void   set(String key, String value);
	String get(String key);
	void   remove(String key);

private:
	String configFolder    = "/default";

	Pair   *elements         = NULL;
	short  nbElements        = 0;
	short  elementsArraySize = INITIAL_ELEMENTS_ARRAY_SIZE;

	void   upsizeElementsArray(short nbNewSlots);

};

#endif /* KEYVALUEFLASH_H_ */
