/* 
* BME280.h
*
* Created: 03.12.2016 17:41:18
* Author: Rene
*/


#ifndef __BME280_H__
#define __BME280_H__

#include <Arduino.h>

typedef struct
{
	uint16_t dig_T1;
	int16_t  dig_T2;
	int16_t  dig_T3;

	uint16_t dig_P1;
	int16_t  dig_P2;
	int16_t  dig_P3;
	int16_t  dig_P4;
	int16_t  dig_P5;
	int16_t  dig_P6;
	int16_t  dig_P7;
	int16_t  dig_P8;
	int16_t  dig_P9;

	uint8_t  dig_H1;
	int16_t  dig_H2;
	uint8_t  dig_H3;
	int16_t  dig_H4;
	int16_t  dig_H5;
	int8_t   dig_H6;
} bme280_calib_data;

class cBME280
{
//variables
public:
protected:
private:

//functions
public:
	cBME280();
	~cBME280();
	
	char Init();
	
	float readTemperature(void);
	float readPressure(void);
	float readHumidity(void);
	float readAltitude(float seaLevel);
	float seaLevelForAltitude(float altitude, float atmospheric);
protected:
private:
	cBME280( const cBME280 &c );
	cBME280& operator=( const cBME280 &c );

   void readCoefficients(void);
   uint8_t spixfer(uint8_t x);

   void      write8(char reg, char value);
   uint8_t   read8(char reg);
   uint16_t  read16(char reg);
   uint32_t  read24(char reg);
   int16_t   readS16(char reg);
   uint16_t  read16_LE(char reg); // little endian
   int16_t   readS16_LE(char reg); // little endian

   int32_t   _sensorID;
   int32_t t_fine;

   bme280_calib_data _bme280_calib;	
	

}; //BME280

#endif //__BME280_H__
