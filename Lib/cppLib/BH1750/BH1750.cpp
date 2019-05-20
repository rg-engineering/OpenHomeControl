/* 
* cBH1750.cpp
*
* Created: 14.12.2015 21:20:57
* Author: Rene
*/

//based on idea of
//http://arduino-hannover.de/2014/05/19/mein-paket-ein-bh1750fvi-lichtintensitaetssensormodul-gy-30-das-luxmeter/

//http://www.raspberry-pi-geek.de/Magazin/2015/04/Digital-Light-Sensor-BH1750-am-Raspberry-Pi
//http://www.instructables.com/id/BH1750-Digital-Light-Sensor/

#include "BH1750.h"
#include <Wire.h> // I2C/TWI for BH1750 light sensor
#include "../Debug/Debug.h"

#ifdef HasBH1750

const byte BH1750_I2CADDRESS = 0x23;  // I2C address (ADD=L=0100011)
//const int BH1750_I2CADDRESS  = 0x5C; // I2C address (ADD=H=1011100)


// default constructor
cBH1750::cBH1750(int mode)
{	
	nMode=mode;
	//cDebug::Log(cDebug::LOG_E, "mode " + String(nMode));
	// Initialize sensor according the chosen resolution mode
	Init(BH1750_I2CADDRESS);
} //cBH1750

// default destructor
cBH1750::~cBH1750()
{
} //~cBH1750


unsigned int cBH1750::Get(int *error)
{
	unsigned int sensorValue = 0; // Max. sensorValue allowed up to (2^16)-1

	int ret=Read(BH1750_I2CADDRESS);
	if (ret == 2)
	{
		sensorValue = ((buffer[0] << 8) | buffer[1]);       // Combine 2 bytes
		//sensorValue = sensorValue/1.2;
		//cDebug::Log(cDebug::LOG_E, String(buffer[0],HEX) + " " + String(buffer[0],HEX) + " " + String(sensorValue, DEC) );
	}
	else
	{
		*error=111;
		//cDebug::Log(cDebug::LOG_E, "BH1750 " + String(ret));
	}
	return sensorValue;
}


//Initialize the ambient light sensor BH1750
void cBH1750::Init(byte I2Caddress)
{
	//cDebug::Log(cDebug::LOG_D, "Init");
	
	//cDebug::Log(cDebug::LOG_D, "BH1750");
	
	Wire.begin();
	Wire.beginTransmission(I2Caddress);             // I2C address
	
/*	if (nMode==1)
	{
		Wire.write(0x00); //power down
		//Wire.write(0x20);								// Switch to H-Mode 1Lux resolution 120ms; one time
		//Wire.write(0x21);								// Switch to H-Mode2 0,5Lux resolution; one time
		//Wire.write(0x23);								// Switch to L-Mode 4 Lux resolution 16ms; one time
	}
	else */
	{
		Wire.write(0x10);								// Switch to H-Mode 1Lux resolution 120ms^; cont
		//Wire.write(0x11);								// Switch to H-Mode2 0,5Lux resolution; cont
		//Wire.write(0x13);								// Switch to L-Mode 4 Lux resolution 16ms; cont
	}
	Wire.endTransmission();
}
/*****************************************************************************************/
int cBH1750::Read(int I2Caddress)
{
	//cDebug::Log(cDebug::LOG_D, "Read");
	byte byteNumber = 0;
	Wire.beginTransmission(I2Caddress);				// I2C address

/*	if (nMode==1)
	{
		//Wire.write(0x01); //power on
		Wire.write(0x20);								// Switch to H-Mode 1Lux resolution 120ms; one time
		//Wire.write(0x21);								// Switch to H-Mode2 0,5Lux resolution; one time
		//Wire.write(0x23);								// Switch to L-Mode 4 Lux resolution 16ms; one time

		delay(180);

	}*/

	
	Wire.requestFrom(I2Caddress, 2);				// Request from I2Caddress 2 bytes
	while(Wire.available())
	{
		buffer[byteNumber] = Wire.read();			// Read 2 bytes into array
		byteNumber++;
	}
	Wire.endTransmission();
	return byteNumber;
}
/*****************************************************************************************/

#endif