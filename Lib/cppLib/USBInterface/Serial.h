/* 
* cSerial.h
*
* Created: 05.10.2015 21:20:08
* Author: Rene
*/


#ifndef __CSERIAL_H__
#define __CSERIAL_H__

#include <Arduino.h>

#ifdef ARDUINO_NANO
#include <SoftwareSerial.h>
#endif


class cSerial
{
//variables
public:
protected:
private:


#ifdef ARDUINO_NANO
	static SoftwareSerial *poSerial;
#endif
	

//functions
public:
	cSerial();
	~cSerial();
	
	static bool isListening();
	static void println();
	static void println(const String &data);
	static void println(char data);
	static void println(unsigned long data);
	static void println(long data);
	static void println(byte data);
	static void println(int data);
			
	static void print(const String &data);
	static void print(byte data);
	
	static void print(byte data,int base);
	
	static int available();
	static int read();
	
protected:
private:


}; //cSerial

#endif //__CSERIAL_H__
