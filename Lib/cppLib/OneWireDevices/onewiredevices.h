/* 
* OnWireDevices.h
*
* Created: 05.06.2016 21:21:09
* Author: Rene
*/


#ifndef __ONWIREDEVICES_H__
#define __ONWIREDEVICES_H__

#ifdef HasOneWireDevices

#include "OneWire.h"

#define MAX_OneWireDevice 2

struct oOneWireDeviceData
{
	unsigned char data[10];
	unsigned char addr[8];
//	float temp;
	unsigned int id;
	//unsigned int type;== first byte in addr
};

class cOneWireDevices :  public OneWire
{
//variables
public:
protected:
private:

//functions
public:
	cOneWireDevices(unsigned char pin);
	~cOneWireDevices();

	void SearchDevices();
	void ReadDevices();

	void GetID(int *nId, int nSize);
	float GetTemp();

protected:
private:
	cOneWireDevices( const cOneWireDevices &c );
	cOneWireDevices& operator=( const cOneWireDevices &c );
	
	void lookUpSensors();


private:
	int nDeviceCount;
	oOneWireDeviceData OneWireDeviceData[MAX_OneWireDevice];
	float temp;

}; //OnWireDevices

#endif

#endif //__ONEWIREDEVICES_H__
