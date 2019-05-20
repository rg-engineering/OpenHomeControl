/* 
* cSerial.cpp
*
* Created: 05.10.2015 21:20:08
* Author: Rene
*/


#include "Serial.h"

#ifdef ARDUINO_NANO
SoftwareSerial *cSerial::poSerial = NULL;
#endif

// default constructor
cSerial::cSerial()
{

#ifdef ARDUINO_NANO
	poSerial= new SoftwareSerial(0,1);

#ifdef IsZentrale
	poSerial->begin(9600);
#else
	poSerial->begin(115200);
#endif	
	//poSerial->begin(57600);
	//poSerial->begin(19200);
#endif

#ifdef ARDUINO_LEONARDO
	Serial.begin(115200);
#endif
	
} //cSerial

// default destructor
cSerial::~cSerial()
{
} //~cSerial


bool cSerial::isListening()
{
#ifdef ARDUINO_NANO
	return poSerial->isListening();
#endif

#ifdef ARDUINO_LEONARDO
	return true;
#endif
}

void cSerial::println()
{
#ifdef ARDUINO_NANO
	poSerial->println();
#endif
#ifdef ARDUINO_LEONARDO
	Serial.println();
#endif
}

void cSerial::println(const String &data)
{
#ifdef ARDUINO_NANO
	poSerial->println(data);
#endif
#ifdef ARDUINO_LEONARDO
	Serial.println(data);
#endif
}

void cSerial::println(char data)
{
#ifdef ARDUINO_NANO
	poSerial->println(data);
#endif
#ifdef ARDUINO_LEONARDO
	Serial.println(data);
#endif
}

void cSerial::println(unsigned long data)
{
#ifdef ARDUINO_NANO
	poSerial->println(data);
#endif
#ifdef ARDUINO_LEONARDO
	Serial.println(data);
#endif
}

void cSerial::println(long data)
{
#ifdef ARDUINO_NANO
	poSerial->println(data);
#endif
#ifdef ARDUINO_LEONARDO
	Serial.println(data);
#endif
}

void cSerial::println(byte data)
{
#ifdef ARDUINO_NANO
	poSerial->println(data);
#endif
#ifdef ARDUINO_LEONARDO
	Serial.println(data);
#endif
}

void cSerial::println(int data)
{
#ifdef ARDUINO_NANO
	poSerial->println(data);
#endif
#ifdef ARDUINO_LEONARDO
	Serial.println(data);
#endif
}
	
	

void cSerial::print(const String &data)
{
#ifdef ARDUINO_NANO
	poSerial->print(data);
#endif
#ifdef ARDUINO_LEONARDO
	Serial.print(data);
#endif
}

void cSerial::print(byte data)
{
#ifdef ARDUINO_NANO
	poSerial->print(data);
#endif
#ifdef ARDUINO_LEONARDO
	Serial.print(data);
#endif
}

void cSerial::print(byte data,int base)
{
#ifdef ARDUINO_NANO
	poSerial->print(data,base);
#endif
#ifdef ARDUINO_LEONARDO
	Serial.print(data,base);
#endif
}


int cSerial::available()
{
#ifdef ARDUINO_NANO
	return poSerial->available();
#endif
#ifdef ARDUINO_LEONARDO
	return Serial.available();
#endif
}

int cSerial::read()
{
#ifdef ARDUINO_NANO	
	return poSerial->read();
#endif
#ifdef ARDUINO_LEONARDO
	return Serial.read();
#endif
}