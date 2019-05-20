/* 
* cBH1750.h
*
* Created: 14.12.2015 21:20:59
* Author: Rene
*/


#ifndef __CBH1750_H__
#define __CBH1750_H__

#include <Arduino.h>

#ifdef HasBH1750

class cBH1750
{
//variables
public:
protected:
private:
	byte buffer[2]; // Array to store 2 bytes of light value

//functions
public:
	cBH1750(int mode);
	~cBH1750();
	
	void Init(byte I2Caddress);
	int Read(int I2Caddress);
	unsigned int Get(int *error);
	
protected:
private:
	cBH1750( const cBH1750 &c );
	cBH1750& operator=( const cBH1750 &c );

	int nMode;

}; //cBH1750

#endif

#endif //__CBH1750_H__
