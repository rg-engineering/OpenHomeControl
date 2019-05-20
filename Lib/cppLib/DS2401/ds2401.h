/* 
* DS2401.h
*
* Created: 02.06.2016 21:15:07
* Author: Rene
*/


#ifndef __DS2401_H__
#define __DS2401_H__

#include "OneWire.h"

#define MAX_DS2401 5

struct DS2401
{
	unsigned char data[12];
	unsigned char addr[8];
	unsigned int id;
	unsigned char SerialNumber[8]; //64bit
};


class cDS2401_List : public OneWire
{

  unsigned long last_convert;
  DS2401 ds2401[MAX_DS2401];

//variables
public:
protected:
private:

//functions
public:
	int count;     // number of available sensors. May need to adjust MAX_DS2401 (see above)

	cDS2401_List(unsigned char pin);// searches for available DS2401 sensors
  
	void update(); // calls read() and convert(), but only if the last conversion
	// has finished. You can call update() as often as you like 

	void read(unsigned char no); // read single sensor
	void read(); // read all sensor

  unsigned int get_id(unsigned char i); // get ID of i-th

protected:
private:


}; //DS2401

#endif //__DS2401_H__
