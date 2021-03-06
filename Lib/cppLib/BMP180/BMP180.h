/* 
* cBMP180.h
*
* Created: 09.01.2016 21:55:14
* Author: Rene
*/


#ifndef __CBMP180_H__
#define __CBMP180_H__

#include <Arduino.h>

#ifdef HasBMP180

class cBMP180
{
//variables
public:
protected:
private:
	//byte buffer[2]; // Array to store 2 bytes 
	
//functions
public:
	cBMP180();
	~cBMP180();
	
	char Init();

	char startTemperature(void);
	// command BMP180 to start a temperature measurement
	// returns (number of ms to wait) for success, 0 for fail

	char getTemperature(double &T);
	// return temperature measurement from previous startTemperature command
	// places returned value in T variable (deg C)
	// returns 1 for success, 0 for fail

	char startPressure(char oversampling);
	// command BMP180 to start a pressure measurement
	// oversampling: 0 - 3 for oversampling value
	// returns (number of ms to wait) for success, 0 for fail

	char getPressure(double &P, double &T);
	// return absolute pressure measurement from previous startPressure command
	// note: requires previous temperature measurement in variable T
	// places returned value in P variable (mbar)
	// returns 1 for success, 0 for fail

	double sealevel(double P, double A);
	// convert absolute pressure to sea-level pressure (as used in weather data)
	// P: absolute pressure (mbar)
	// A: current altitude (meters)
	// returns sealevel pressure in mbar

	double altitude(double P, double P0);
	// convert absolute pressure to altitude (given baseline pressure; sea-level, runway, etc.)
	// P: absolute pressure (mbar)
	// P0: fixed baseline pressure (mbar)
	// returns signed altitude in meters

	char getError(void);
	// If any library command fails, you can retrieve an extended
	// error code using this command. Errors are from the wire library:
	// 0 = Success
	// 1 = Data too long to fit in transmit buffer
	// 2 = Received NACK on transmit of address
	// 3 = Received NACK on transmit of data
	// 4 = Other error
	
protected:
private:
	cBMP180( const cBMP180 &c );
	cBMP180& operator=( const cBMP180 &c );
	
private:
	
	char readInt(char address, int16_t &value);
	// read an signed int (16 bits) from a BMP180 register
	// address: BMP180 register address
	// value: external signed int for returned value (16 bits)
	// returns 1 for success, 0 for fail, with result in value

	char readUInt(char address, uint16_t &value);
	// read an unsigned int (16 bits) from a BMP180 register
	// address: BMP180 register address
	// value: external unsigned int for returned value (16 bits)
	// returns 1 for success, 0 for fail, with result in value


	char readBytes(unsigned char *values, char length);
	// read a number of bytes from a BMP180 register
	// values: array of char with register address in first location [0]
	// length: number of bytes to read back
	// returns 1 for success, 0 for fail, with read bytes in values[] array

	char writeBytes(unsigned char *values, char length);
	// write a number of bytes to a BMP180 register (and consecutive subsequent registers)
	// values: array of char with register address in first location [0]
	// length: number of bytes to write
	// returns 1 for success, 0 for fail


	int16_t AC1,AC2,AC3,VB1,VB2,MB,MC,MD;
	uint16_t AC4,AC5,AC6;
	double c5,c6,mc,md,x0,x1,x2,y0,y1,y2,p0,p1,p2;
	char _error;
}; //cBMP180

#endif

#endif //__CBMP180_H__
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    