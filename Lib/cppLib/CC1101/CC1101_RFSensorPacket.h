/* 
* cCC1101_RFSensorProtocol.h
*
* Created: 28.11.2015 19:59:33
* Author: Rene
*/


#ifndef __CC1101_RFSENSORPACKET_H__
#define __CC1101_RFSENSORPACKET_H__


#include "CC1101_Packet.h"
#ifdef HasCC1101

class cCC1101_RFSensorPacket : public CCPACKET
{
//variables
public:
protected:
private:

//functions
public:
	cCC1101_RFSensorPacket();
	~cCC1101_RFSensorPacket();
	
	void Reset();

	void AddHeader(int *nId, char type);	
	void AddHeader(char type);
	void AddTemperature(float fTemperature);
	void AddHumidity(float fHumidity);
//	void AddAirQuality(float fAirQuality);
	void AddPressure(float fAirPressure);
	void AddDate();
	void AddTime();
	void AddBrightness(unsigned int nBrightness);
//	void AddBatteryState(int nBatState);
//	void AddSabotage(bool bSabotage);
//	void AddError(int nError);
	
	void AddAvgWindSpeed(float avgWindSpeed);
	void AddWindGust(float windGust);
	void AddWindDir(int windDir);
	
	
	void AddRawData(char *indata,int start, int inlength);

	void InterpretePacket(char mode, int *nId);
	
	float fTemp2Display;
#ifdef DISPLAY_PRESSURE
	float fPressure2Display;
#endif
#ifdef DISPLAY_HUMIDITY
	float fHumidity2Display;
#endif	
#ifdef DISPLAY_POP
	int nPOP;
#endif
	float fTempForecast2Display;
	int nRain2Display;

	int nHour;
	int nMinute;
	int nSecond;
	int nDay;
	int nMonth;
	int nYear;
	byte nWeatherIconId;


protected:
private:
	int InterpreteDatapoint(int start, char ismytelegram);
	String sLog;
	
	bool CheckDataLength();


}; //cCC1101_RFSensorPacket

#endif

#endif //__CC1101_RFSENSORPACKET_H__
