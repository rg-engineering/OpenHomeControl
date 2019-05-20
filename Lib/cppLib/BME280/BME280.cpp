/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/
/* 
* BME280.cpp
* above mentioned source changed to work with wire library only
* Created: 03.12.2016 17:41:18
* Author: Rene
*/


#include "BME280.h"
#include <Wire.h> // I2C/TWI
#include "../Debug/Debug.h"

#define BME280_I2CADDRESS 0x76  // I2C address, low on SDO 
//#define BME280_I2CADDRESS 0x77  // I2C address, high on SDO 
/*=========================================================================
REGISTERS
-----------------------------------------------------------------------*/
enum
{
BME280_REGISTER_DIG_T1              = 0x88,
BME280_REGISTER_DIG_T2              = 0x8A,
BME280_REGISTER_DIG_T3              = 0x8C,

BME280_REGISTER_DIG_P1              = 0x8E,
BME280_REGISTER_DIG_P2              = 0x90,
BME280_REGISTER_DIG_P3              = 0x92,
BME280_REGISTER_DIG_P4              = 0x94,
BME280_REGISTER_DIG_P5              = 0x96,
BME280_REGISTER_DIG_P6              = 0x98,
BME280_REGISTER_DIG_P7              = 0x9A,
BME280_REGISTER_DIG_P8              = 0x9C,
BME280_REGISTER_DIG_P9              = 0x9E,

BME280_REGISTER_DIG_H1              = 0xA1,
BME280_REGISTER_DIG_H2              = 0xE1,
BME280_REGISTER_DIG_H3              = 0xE3,
BME280_REGISTER_DIG_H4              = 0xE4,
BME280_REGISTER_DIG_H5              = 0xE5,
BME280_REGISTER_DIG_H6              = 0xE7,

BME280_REGISTER_CHIPID             = 0xD0,
BME280_REGISTER_VERSION            = 0xD1,
BME280_REGISTER_SOFTRESET          = 0xE0,

BME280_REGISTER_CAL26              = 0xE1,  // R calibration stored in 0xE1-0xF0

BME280_REGISTER_CONTROLHUMID       = 0xF2,
BME280_REGISTER_CONTROL            = 0xF4,
BME280_REGISTER_CONFIG             = 0xF5,
BME280_REGISTER_PRESSUREDATA       = 0xF7,
BME280_REGISTER_TEMPDATA           = 0xFA,
BME280_REGISTER_HUMIDDATA          = 0xFD,
};

// default constructor
cBME280::cBME280()
{
	Init();
} //BME280

// default destructor
cBME280::~cBME280()
{
} //~BME280

//Initialize the sensor
char cBME280::Init()
{
	//poDebug->Log(cDebug::LOG_D, "Init");
	// Start up the Arduino's "wire" (I2C) library:
	
	Wire.begin();
	uint8_t ChipID=read8(BME280_REGISTER_CHIPID);
	if (ChipID != 0x60)
	{
		//cDebug::Log(cDebug::LOG_E, "BME280: wrong chip ID " + String(ChipID,HEX));
		return false;
	}
	else
	{
		//cDebug::Log(cDebug::LOG_E, "BME280");
	}

	readCoefficients();

	 //Set before CONTROL_meas (DS 5.4.3)
	write8(BME280_REGISTER_CONTROLHUMID, 0x05); //16x oversampling

	write8(BME280_REGISTER_CONTROL, 0xB7); // 16x ovesampling, normal mode
	return true;
  }
  
  void cBME280::write8(char reg, char value)
  {
	Wire.beginTransmission(BME280_I2CADDRESS);
	Wire.write((uint8_t)reg);
	Wire.write((uint8_t)value);
	Wire.endTransmission();
		  
  }
  
uint8_t cBME280::read8(char reg)
{
	uint8_t value;

    Wire.beginTransmission(BME280_I2CADDRESS);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom(BME280_I2CADDRESS, (byte)1);
    value = Wire.read();

	return value;
}

/**************************************************************************/
/*!
    @brief  Reads a 16 bit value over I2C
*/
/**************************************************************************/
uint16_t cBME280::read16(char reg)
{
	uint16_t value;

    Wire.beginTransmission(BME280_I2CADDRESS);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom(BME280_I2CADDRESS, (byte)2);
    value = (Wire.read() << 8) | Wire.read();
	
	return value;
}

uint16_t cBME280::read16_LE(char reg) 
{
	uint16_t temp = read16(reg);
	return (temp >> 8) | (temp << 8);

}

/**************************************************************************/
/*!
    @brief  Reads a signed 16 bit value over I2C
*/
/**************************************************************************/
int16_t cBME280::readS16(char reg)
{
	return (int16_t)read16(reg);

}

int16_t cBME280::readS16_LE(char reg)
{
	return (int16_t)read16_LE(reg);

}


/**************************************************************************/
/*!
    @brief  Reads a 24 bit value over I2C
*/
/**************************************************************************/

uint32_t cBME280::read24(char reg)
{
	uint32_t value;

    Wire.beginTransmission(BME280_I2CADDRESS);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom(BME280_I2CADDRESS, (byte)3);
    
    value = Wire.read();
    value <<= 8;
    value |= Wire.read();
    value <<= 8;
    value |= Wire.read();



  return value;
}


/**************************************************************************/
/*!
    @brief  Reads the factory-set coefficients
*/
/**************************************************************************/
void cBME280::readCoefficients(void)
{
    _bme280_calib.dig_T1 = read16_LE(BME280_REGISTER_DIG_T1);
    _bme280_calib.dig_T2 = readS16_LE(BME280_REGISTER_DIG_T2);
    _bme280_calib.dig_T3 = readS16_LE(BME280_REGISTER_DIG_T3);

    _bme280_calib.dig_P1 = read16_LE(BME280_REGISTER_DIG_P1);
    _bme280_calib.dig_P2 = readS16_LE(BME280_REGISTER_DIG_P2);
    _bme280_calib.dig_P3 = readS16_LE(BME280_REGISTER_DIG_P3);
    _bme280_calib.dig_P4 = readS16_LE(BME280_REGISTER_DIG_P4);
    _bme280_calib.dig_P5 = readS16_LE(BME280_REGISTER_DIG_P5);
    _bme280_calib.dig_P6 = readS16_LE(BME280_REGISTER_DIG_P6);
    _bme280_calib.dig_P7 = readS16_LE(BME280_REGISTER_DIG_P7);
    _bme280_calib.dig_P8 = readS16_LE(BME280_REGISTER_DIG_P8);
    _bme280_calib.dig_P9 = readS16_LE(BME280_REGISTER_DIG_P9);

    _bme280_calib.dig_H1 = read8(BME280_REGISTER_DIG_H1);
    _bme280_calib.dig_H2 = readS16_LE(BME280_REGISTER_DIG_H2);
    _bme280_calib.dig_H3 = read8(BME280_REGISTER_DIG_H3);
    _bme280_calib.dig_H4 = (read8(BME280_REGISTER_DIG_H4) << 4) | (read8(BME280_REGISTER_DIG_H4+1) & 0xF);
    _bme280_calib.dig_H5 = (read8(BME280_REGISTER_DIG_H5+1) << 4) | (read8(BME280_REGISTER_DIG_H5) >> 4);
    _bme280_calib.dig_H6 = (int8_t)read8(BME280_REGISTER_DIG_H6);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
float cBME280::readTemperature(void)
{
  int32_t var1, var2;

  int32_t adc_T = read24(BME280_REGISTER_TEMPDATA);
  adc_T >>= 4;

  var1  = ((((adc_T>>3) - ((int32_t)_bme280_calib.dig_T1 <<1))) *
	   ((int32_t)_bme280_calib.dig_T2)) >> 11;

  var2  = (((((adc_T>>4) - ((int32_t)_bme280_calib.dig_T1)) *
	     ((adc_T>>4) - ((int32_t)_bme280_calib.dig_T1))) >> 12) *
	   ((int32_t)_bme280_calib.dig_T3)) >> 14;

  t_fine = var1 + var2;

  float T  = (t_fine * 5 + 128) >> 8;
  return T/100;
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
float cBME280::readPressure(void) 
{
  int64_t var1, var2, p;

  readTemperature(); // must be done first to get t_fine

  int32_t adc_P = read24(BME280_REGISTER_PRESSUREDATA);
  adc_P >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)_bme280_calib.dig_P6;
  var2 = var2 + ((var1*(int64_t)_bme280_calib.dig_P5)<<17);
  var2 = var2 + (((int64_t)_bme280_calib.dig_P4)<<35);
  var1 = ((var1 * var1 * (int64_t)_bme280_calib.dig_P3)>>8) +
    ((var1 * (int64_t)_bme280_calib.dig_P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)_bme280_calib.dig_P1)>>33;

  if (var1 == 0) {
    return 0;  // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p<<31) - var2)*3125) / var1;
  var1 = (((int64_t)_bme280_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
  var2 = (((int64_t)_bme280_calib.dig_P8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)_bme280_calib.dig_P7)<<4);
  return (float)p/256;
}


/**************************************************************************/
/*!

*/
/**************************************************************************/
float cBME280::readHumidity(void) 
{

  readTemperature(); // must be done first to get t_fine

  int32_t adc_H = read16(BME280_REGISTER_HUMIDDATA);

  int32_t v_x1_u32r;

  v_x1_u32r = (t_fine - ((int32_t)76800));

  v_x1_u32r = (((((adc_H << 14) - (((int32_t)_bme280_calib.dig_H4) << 20) -
		  (((int32_t)_bme280_calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
	       (((((((v_x1_u32r * ((int32_t)_bme280_calib.dig_H6)) >> 10) *
		    (((v_x1_u32r * ((int32_t)_bme280_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
		  ((int32_t)2097152)) * ((int32_t)_bme280_calib.dig_H2) + 8192) >> 14));

  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
			     ((int32_t)_bme280_calib.dig_H1)) >> 4));

  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
  float h = (v_x1_u32r>>12);
  return  h / 1024.0;
}

/**************************************************************************/
/*!
    Calculates the altitude (in meters) from the specified atmospheric
    pressure (in hPa), and sea-level pressure (in hPa).

    @param  seaLevel      Sea-level pressure in hPa
    @param  atmospheric   Atmospheric pressure in hPa
*/
/**************************************************************************/
float cBME280::readAltitude(float seaLevel)
{
  // Equation taken from BMP180 datasheet (page 16):
  //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

  // Note that using the equation from wikipedia can give bad results
  // at high altitude.  See this thread for more information:
  //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064

	float atmospheric = readPressure() / 100.0F;
	return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
}

/**************************************************************************/
/*!
    Calculates the pressure at sea level (in hPa) from the specified altitude 
    (in meters), and atmospheric pressure (in hPa).  
    @param  altitude      Altitude in meters
    @param  atmospheric   Atmospheric pressure in hPa
*/
/**************************************************************************/
float cBME280::seaLevelForAltitude(float altitude, float atmospheric)
{
  // Equation taken from BMP180 datasheet (page 17):
  //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

  // Note that using the equation from wikipedia can give bad results
  // at high altitude.  See this thread for more information:
  //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064
  
	return atmospheric / pow(1.0 - (altitude/44330.0), 5.255);
}
