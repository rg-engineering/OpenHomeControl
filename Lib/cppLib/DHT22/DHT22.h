/* 
* cDHT22.h
*
* Created: 15.01.2016 20:21:57
* Author: Rene
*/


#ifndef __CDHT22_H__
#define __CDHT22_H__

#ifdef HasDHT22
#define DHT22_ERROR_VALUE -99.5

typedef enum
{
	DHT_ERROR_NONE = 0,
	DHT_BUS_HUNG,
	DHT_ERROR_NOT_PRESENT,
	DHT_ERROR_ACK_TOO_LONG,
	DHT_ERROR_SYNC_TIMEOUT,
	DHT_ERROR_DATA_TIMEOUT,
	DHT_ERROR_CHECKSUM,
//	DHT_ERROR_TOOQUICK
} DHT22_ERROR_t;


class cDHT22
{
//variables
public:
protected:
private:
    unsigned char _bitmask;
    volatile unsigned char *_baseReg;
//    unsigned long _lastReadTime;
    float _lastHumidity;
    float _lastTemperature;
//functions
public:
	cDHT22(char pin);
	~cDHT22();
	
	char Init();
    DHT22_ERROR_t readData(void);
    float getHumidity();
    float getTemperatureC();
//    void clockReset();
	
protected:
private:
	cDHT22( const cDHT22 &c );
	cDHT22& operator=( const cDHT22 &c );

}; //cDHT22

#endif

#endif //__CDHT22_H__
