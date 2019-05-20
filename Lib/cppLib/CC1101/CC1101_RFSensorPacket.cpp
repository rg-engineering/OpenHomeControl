/* 
* cCC1101_RFSensorProtocol.cpp
*
* Created: 28.11.2015 19:59:33
* Author: Rene
*/

/*
 Protokollbeschreibung:
 0 Start Byte
 1.-6. Byte Quelle-ID (6Byte)
 7.-12. Byte Ziel-ID (0xFE Broadcast, 0x10 Zentrale) (6Byte)
 13. Byte ModulType (0x01 Sensor, 0x02 Aktor, 0x03 Display 0x10 Zentrale)
 14. Byte Anzahl Datenpunkt (nicht Länge!)

 pro Datenpunkt:
 0. Byte Type (0x01 Temp, 0x02 Feuchte, 0x03 Luftqualität, 0x04 Datum, 0x05 Uhrzeit, 0x06 Helligkeit, 0x07 Batteriezustand, 0x08 Sabotage, 0x09 AirPressur. 0x0A error message, 0x0B WeatherIcon, 0x0C PoP, 0x0D AvgWindSpeed, 0x0E WindGust, 0x0F WindDir, 0x10 Rain Forecast, 0x11 Temp Forecast)
 1. Byte Type / Länge der Daten (0x01 Byte 0x02 int 0x03 float 0x04 string 0x05 date 0x06 time)
 2. Byte Daten
 3. Byte Einheit (0x00 ohne, 0x01 °C, 0x02 %, 0x03 mBar, 0x04 lux, 0x05 m/s, 0x06 deg, 0x07 mm)

 CRC über gesamtes Telegram vom chip selbst

*/

#include "Arduino.h"
#include "../Debug/Debug.h"
#include "../USBInterface/Serial.h"
#include "CC1101_RFSensorPacket.h"

#include <avr/pgmspace.h>

#ifdef HasCC1101

//char p_buffer[80];
//#define P(str) (strcpy_P(p_buffer, PSTR(str)), p_buffer)

typedef union {
	float floatingPoint;
	byte binary[4];
} binaryFloat;



// default constructor
cCC1101_RFSensorPacket::cCC1101_RFSensorPacket()
{
	repetition=6;
	datatype = DATATYPE_UNKNOWN;
	length=0;
	
} 

// default destructor
cCC1101_RFSensorPacket::~cCC1101_RFSensorPacket()
{
} 

void cCC1101_RFSensorPacket::Reset()
{
	if (length>0) //we have already data
	{
		for (int i=0; i<length;i++)
		{
			data[i]=0xFF;
		}
	}
	repetition=6;
	datatype = DATATYPE_UNKNOWN;
	length=0;
}
/*
alte Version
#define IDX_DATENPUNKTE 3
#define IDX_TYPE 2
#define IDX_TARGET 1
#define IDX_SOURCE 0
*/

#define IDX_DATENPUNKTE 14
#define IDX_TYPE 13
#define IDX_TARGET 7
#define IDX_SOURCE 1
#define IDX_START 0


void cCC1101_RFSensorPacket::AddHeader(int *nId, char type)
{
	//cDebug::Log(cDebug::LOG_E,sID);
	data[IDX_START]=0x00;
	
	for (int i=0; i<6;i++)
	{
		data[IDX_SOURCE+i]=nId[i];  //source: meine ID 6Byte
		data[IDX_TARGET+i]=0xFE;
	}
 	/*data[IDX_SOURCE]=nId[0];  //source: meine ID 6Byte
 	data[IDX_SOURCE+1]=nId[1];  //
 	data[IDX_SOURCE+2]=nId[2];  //
 	data[IDX_SOURCE+3]=nId[3];  //
 	data[IDX_SOURCE+4]=nId[4];  //
 	data[IDX_SOURCE+5]=nId[5];  //
	 */

//	String sTemp;
//	sTemp="ID ";
// 	for(int j=0; j<6; j++)
//	{
// 		sTemp += String(nId[j],HEX);
//		sTemp += " ";
//	}
//	cDebug::Log(cDebug::LOG_E,sTemp);


//	data[IDX_SOURCE]=0x0A;  //source meine ID 6Byte
// 	data[IDX_SOURCE+1]=0x0B;  //
// 	data[IDX_SOURCE+2]=0x0C;  //
// 	data[IDX_SOURCE+3]=0x0D;  //
// 	data[IDX_SOURCE+4]=0x0E;  //
// 	data[IDX_SOURCE+5]=0x0F;  //
/*
	data[IDX_TARGET]=0xFE;  //Target: Broadcast 6Byte
	data[IDX_TARGET+1]=0xFE;
	data[IDX_TARGET+2]=0xFE;
	data[IDX_TARGET+3]=0xFE;
	data[IDX_TARGET+4]=0xFE;
	data[IDX_TARGET+5]=0xFE;
	*/
	data[IDX_TYPE]=type;  //Type: Sensor; actor; display
	data[IDX_DATENPUNKTE]=0x00;  //Datenpunkte
	length=IDX_DATENPUNKTE+1;
}



void cCC1101_RFSensorPacket::AddHeader(char type)
{
	data[IDX_START]=0x00;
	for (int i=0;i<6;i++)
	{
		data[IDX_SOURCE+i]=0x00;  //source meine ID 6Byte
		data[IDX_TARGET+i]=0xFE;
	}
	/*
	data[IDX_SOURCE]=0x00;  //source meine ID 6Byte
 	data[IDX_SOURCE+1]=0x00;  //
 	data[IDX_SOURCE+2]=0x00;  //
 	data[IDX_SOURCE+3]=0x00;  //
 	data[IDX_SOURCE+4]=0x00;  //
 	data[IDX_SOURCE+5]=0x00;  //
	 */
	
	
	/*
	data[IDX_TARGET]=0xFE;  //Target: Broadcast 6Byte
	data[IDX_TARGET+1]=0xFE;
	data[IDX_TARGET+2]=0xFE;
	data[IDX_TARGET+3]=0xFE;
	data[IDX_TARGET+4]=0xFE;
	data[IDX_TARGET+5]=0xFE;
	*/
	data[IDX_TYPE]=type; //0x01;  //Type: Sensor,actor;display
	data[IDX_DATENPUNKTE]=0x00;  //Datenpunkte
	length=IDX_DATENPUNKTE+1;
}
void cCC1101_RFSensorPacket::AddTemperature(float fTemperature)
{
	data[IDX_DATENPUNKTE]=data[IDX_DATENPUNKTE]+1;  //Datenpunkte
	data[length]=0x01;  //Temperature
	data[length+1]=0x03; //float
	
	 binaryFloat temp;
	 temp.floatingPoint = fTemperature;

	
	data[length+2]=temp.binary[0];
	data[length+3]=temp.binary[1];
	data[length+4]=temp.binary[2];
	data[length+5]=temp.binary[3];
	
	data[length+6]=0x01; //°C
	
	length+=7;
	
	char outstr[15];
	dtostrf(fTemperature,7, 2, outstr);
	
	String sLog= "Temp " + String(outstr);
	//cDebug::Log(cDebug::LOG_I, sLog);
	cSerial::println(sLog);
	CheckDataLength();
}
void cCC1101_RFSensorPacket::AddPressure(float fAirPressure)
{
	data[IDX_DATENPUNKTE]=data[IDX_DATENPUNKTE]+1;  //Datenpunkte
	data[length]=0x09;  //Pressure
	data[length+1]=0x03; //float
	
	binaryFloat temp;
	temp.floatingPoint = fAirPressure;

	data[length+2]=temp.binary[0];
	data[length+3]=temp.binary[1];
	data[length+4]=temp.binary[2];
	data[length+5]=temp.binary[3];
	
	data[length+6]=0x03; //mBar
	
	length+=7;	
	
	char outstr[15];
	dtostrf(fAirPressure,7, 2, outstr);
		
	String sLog= "Press " + String(outstr);
	//cDebug::Log(cDebug::LOG_I, sLog);
	cSerial::println(sLog);
	CheckDataLength();
}
void cCC1101_RFSensorPacket::AddHumidity(float fHumidity)
{
	data[IDX_DATENPUNKTE]=data[IDX_DATENPUNKTE]+1;  //Datenpunkte
	data[length]=0x02;  //Humidity
	data[length+1]=0x03; //float
	
	binaryFloat temp;
	temp.floatingPoint = fHumidity;

	data[length+2]=temp.binary[0];
	data[length+3]=temp.binary[1];
	data[length+4]=temp.binary[2];
	data[length+5]=temp.binary[3];
	
	data[length+6]=0x02; //%
	
	length+=7;
	char outstr[15];
	dtostrf(fHumidity,7, 2, outstr);
	String sLog= "Hum " + String(outstr);
	//cDebug::Log(cDebug::LOG_I, sLog);
	cSerial::println(sLog);
	CheckDataLength();
}
/*
void cCC1101_RFSensorPacket::AddAirQuality(float fAirQuality)
{
	CheckDataLength();
}
*/


void cCC1101_RFSensorPacket::AddDate()
{
	data[IDX_DATENPUNKTE]=data[IDX_DATENPUNKTE]+1;  //Datenpunkte
	data[length]=0x04;  //Date
	data[length+1]=0x05; //date: 3*int dd.mm.yyyy
	
	//dummy date
	int day = 21;
	int month = 1;
	int year = 2017;

	data[length+2]=highByte(day);
	data[length+3]=lowByte(day);

	data[length+4]=highByte(month);
	data[length+5]=lowByte(month);

	data[length+6]=highByte(year);
	data[length+7]=lowByte(year);
	
	data[length+8]=0x00; //ohne einheit

	length+=9;
	
	String sLog= "Date " + String(day,DEC) + "." + String(month,DEC) + "." + String(year,DEC) ;
	cSerial::println(sLog);	
	
	CheckDataLength();
}


void cCC1101_RFSensorPacket::AddTime()
{
	data[IDX_DATENPUNKTE]=data[IDX_DATENPUNKTE]+1;  //Datenpunkte
	data[length]=0x05;  //Time
	data[length+1]=0x06; //time: 3*int hh::mm::ss

	//dummy time
	unsigned int hour = 14;
	unsigned int minute = 55;
	unsigned int second = 34;

	data[length+2]=highByte(hour);
	data[length+3]=lowByte(hour);

	data[length+4]=highByte(minute);
	data[length+5]=lowByte(minute);

	data[length+6]=highByte(second);
	data[length+7]=lowByte(second);
	
	data[length+8]=0x00; //ohne einheit

	length+=9;	
	
	String sLog= "Time " + String(hour,DEC) + ":" + String(minute,DEC) + ":" + String(second,DEC) ;
	cSerial::println(sLog);
		
	CheckDataLength();
}

void cCC1101_RFSensorPacket::AddBrightness(unsigned int nBrightness)
{
	data[IDX_DATENPUNKTE]=data[IDX_DATENPUNKTE]+1;  //Datenpunkte
	data[length]=0x06;  //Brightness
	data[length+1]=0x02; //int

	data[length+2]=highByte(nBrightness);
	data[length+3]=lowByte(nBrightness);

	data[length+4]=0x04; //lux

	length+=5;	
	
	//	String sLog= "Add Bright " + String(nBrightness,DEC) + " " + String(data[length+2],HEX) + " " +  String(data[length+3],HEX)
	//	String sLog= String(data[length+2],HEX) + " " +  String(data[length+3],HEX);
	char outstr[15];
	dtostrf(nBrightness,7, 2, outstr);
	String sLog= "Bright " + String(outstr);
	//cDebug::Log(cDebug::LOG_I, sLog);
	cSerial::println(sLog);
	CheckDataLength();
}
/*
void cCC1101_RFSensorPacket::AddBatteryState(int nBatState)
{
	CheckDataLength();
}
*/
/*
void cCC1101_RFSensorPacket::AddSabotage(bool bSabotage)
{
	CheckDataLength();
}
*/
/*
void cCC1101_RFSensorPacket::AddError(int nError)
{
	data[IDX_DATENPUNKTE]=data[IDX_DATENPUNKTE]+1;  //Datenpunkte
	data[length]=0x0A;  //error
	data[length+1]=0x02; //int

	data[length+2]=highByte(nError);
	data[length+3]=lowByte(nError);

	data[length+4]=0x04; //ohne einheit

	length+=5;

	
	//	String sLog= "Add Bright " + String(nBrightness,DEC) + " " + String(data[length+2],HEX) + " " +  String(data[length+3],HEX)
	//	String sLog= String(data[length+2],HEX) + " " +  String(data[length+3],HEX);

	String sLog= "error " + String(nError);
	//cDebug::Log(cDebug::LOG_I, sLog);	
	cSerial::println(sLog);
	CheckDataLength();
}
*/

void cCC1101_RFSensorPacket::AddAvgWindSpeed(float avgWindSpeed)
{
	data[IDX_DATENPUNKTE]=data[IDX_DATENPUNKTE]+1;  //Datenpunkte
	data[length]=0x0D;  //AvgWindSpeed xxx
	data[length+1]=0x03; //float
	
	binaryFloat temp;
	temp.floatingPoint = avgWindSpeed;

	data[length+2]=temp.binary[0];
	data[length+3]=temp.binary[1];
	data[length+4]=temp.binary[2];
	data[length+5]=temp.binary[3];
	
	data[length+6]=0x05; //m/s
	
	length+=7;
	char outstr[15];
	dtostrf(avgWindSpeed,7, 2, outstr);
	String sLog= "AvgWindSpeed " + String(outstr);
	//cDebug::Log(cDebug::LOG_I, sLog);
	cSerial::println(sLog);
	CheckDataLength();	
}
void cCC1101_RFSensorPacket::AddWindGust(float windGust)
{
	data[IDX_DATENPUNKTE]=data[IDX_DATENPUNKTE]+1;  //Datenpunkte
	data[length]=0x0E;  //windGust xxx
	data[length+1]=0x03; //float
	
	binaryFloat temp;
	temp.floatingPoint = windGust;

	data[length+2]=temp.binary[0];
	data[length+3]=temp.binary[1];
	data[length+4]=temp.binary[2];
	data[length+5]=temp.binary[3];
	
	data[length+6]=0x05; //m/s
	
	length+=7;
	char outstr[15];
	dtostrf(windGust,7, 2, outstr);
	String sLog= "windGust " + String(outstr);
	//cDebug::Log(cDebug::LOG_I, sLog);
	cSerial::println(sLog);
	CheckDataLength();
}
void cCC1101_RFSensorPacket::AddWindDir(int windDir)
{
	data[IDX_DATENPUNKTE]=data[IDX_DATENPUNKTE]+1;  //Datenpunkte
	data[length]=0x0f;  //winddir
	data[length+1]=0x02; //int

	data[length+2]=highByte(windDir);
	data[length+3]=lowByte(windDir);

	data[length+4]=0x06; //deg

	length+=5;
	
	//	String sLog= "Add Bright " + String(nBrightness,DEC) + " " + String(data[length+2],HEX) + " " +  String(data[length+3],HEX)
	//	String sLog= String(data[length+2],HEX) + " " +  String(data[length+3],HEX);
	char outstr[15];
	dtostrf(windDir,7, 2, outstr);
	String sLog= "WindDir " + String(outstr);
	//cDebug::Log(cDebug::LOG_I, sLog);
	cSerial::println(sLog);
	CheckDataLength();
}

void cCC1101_RFSensorPacket::InterpretePacket(char mode, int *nId )
{
//	String sLog;


	sLog="";
	char bIsMyTelegram=0; //0= no; 1=broadcast (time and date); 2=exactly for me

	if (mode==1)
	{
#ifdef WithCC1101Logging
		if (length<IDX_DATENPUNKTE+1)
		{
			cSerial::println("not enough data");

			return;
		}
	
		
		//if (data[IDX_TYPE]==0x01 || data[IDX_TYPE]==0x02 || data[IDX_TYPE]==0x03 || data[IDX_TYPE]==0x10)
		//{
			sLog = "got data from ";
			switch(data[IDX_TYPE])
			{
				case 0x01:
					sLog += "Sensor: ";
					break;
				case 0x02:
					sLog += "Actor: ";
					break;
				case 0x03:
					sLog += "Display: ";
					break;
				case 0x10:
					sLog += "Zentrale: ";
					break;
				default:
					{
					char sTemp1[20];
					sprintf(sTemp1,"unknown (%02X): ",data[IDX_TYPE]);
					sLog += String(sTemp1);
					}
					break;
			}
			
			char sTemp[20];
			sprintf(sTemp,"%02X %02X %02X %02X %02X %02X",data[IDX_SOURCE+0],data[IDX_SOURCE+1],data[IDX_SOURCE+2],data[IDX_SOURCE+3],data[IDX_SOURCE+4],data[IDX_SOURCE+5]);
			sLog += String(sTemp);
		//}	
#endif

		if (data[IDX_TARGET]==0xFE)
		{
			bIsMyTelegram=1; //only for time and date
#ifdef WithCC1101Logging
			sLog +=" as broadcast";
#endif
		}
		else if (data[IDX_TARGET]==0x10)
		{
			bIsMyTelegram=0;  //just ignore
#ifdef WithCC1101Logging
			sLog +=" to Zentrale";
#endif
		}
		else
		{
			if (data[IDX_TARGET+0]==nId[0] && data[IDX_TARGET+1]==nId[1] && data[IDX_TARGET+2]==nId[2] && data[IDX_TARGET+3]==nId[3] && data[IDX_TARGET+4]==nId[4] && data[IDX_TARGET+5]==nId[5])
			{
				bIsMyTelegram=2; //take it all;it's only for me
			}
			else {
				bIsMyTelegram=0;  //just ignore
			}
#ifdef WithCC1101Logging
			sLog += " Target=";
			
			char sTemp[20];
			sprintf(sTemp,"%02X %02X %02X %02X %02X %02X",data[IDX_TARGET+0],data[IDX_TARGET+1],data[IDX_TARGET+2],data[IDX_TARGET+3],data[IDX_TARGET+4],data[IDX_TARGET+5]);
			sLog += String(sTemp);

			/*
			for (int i=0;i<6;i++)
			{
				sLog+= String(data[IDX_TARGET+i],HEX);
				sLog+= " ";
			}
			*/
#endif
		}
#ifdef WithCC1101Logging
		sLog += " with " + String(data[IDX_DATENPUNKTE],DEC) + " DP";
		cSerial::println( sLog);
		sLog="";
#endif	
		//data from Sensor=3FAF82820000 as broadcast with 2 P  Temp   30.15 C Press  957.20 mBar
	//got data from Sensor=3FAF82820000 as broadcast wit 2 DP Temp 30.30 C Hum 40.50 %
		int start=IDX_DATENPUNKTE+1;
		
		if (data[IDX_DATENPUNKTE]>0 && data[IDX_DATENPUNKTE]<10)
		{
			for (int i=0; i<data[IDX_DATENPUNKTE];i++)
			{
				start=InterpreteDatapoint(start,bIsMyTelegram);
#ifdef WithCC1101Logging
			//workaround for cut logs
			//if (sLog.length()>40)
			//{
			//	cSerial::println(sLog);
			//	sLog="";
			//}
#endif
			}
		}
		else {
#ifdef WithCC1101Logging
			if (data[IDX_DATENPUNKTE]==0)
			{
				sLog += " w/o ";
			}
			else
			{
				sLog += " too many... ";
			}
#endif		
		}
#ifdef WithCC1101Logging
		cSerial::println( sLog);
		sLog="";
#endif
	}
	else
	{
#ifdef WithCC1101Logging
		//just raw data
 		sLog="I ";
 		for(int j=0; j<length; j++)
 		{
			if (data[j]<10)
			{
				sLog +="0";
			}
 			sLog += String(data[j],HEX);
 			sLog += " ";
 		}
		sLog+="J";
 		//cDebug::Log(cDebug::LOG_E,sTemp);
		 cSerial::println(sLog);
#endif
	}
#ifndef WithCC1101Logging
	sLog = "data with DP=" + String(data[IDX_DATENPUNKTE],DEC);
	//cDebug::Log(cDebug::LOG_E, sLog);
	sLog += " from ";
	switch(data[IDX_TYPE])
	{
		case 0x01:
		sLog += "Sensor";
		break;
		//case 0x02:
		//sLog += "Actor";
		//break;
		case 0x03:
		sLog += "Display";
		break;
		case 0x10:
		sLog += "Zentrale";
		break;
	}
	
	
	cSerial::println(sLog);
#endif
	


// 	int nSource = packet.data[0];
// 	String sDest="";
// 	switch (packet.data[1])
// 	{
// 		case 0xFE:
// 			sDest="broadcast";
// 			break;
// 		case 0x10:
// 			sDest="central";
// 			break;
// 		default:
// 			int nDest=packet.data[1];
// 			sDest="0x"+String(nDest,HEX);
// 			break;
// 	}
// 
// 	String sModuleType="";
// 	switch(packet.data[2])
// 	{
// 		case 0x01:
// 			sModuleType="Sensor";
// 			break;
// 		case 0x02:
// 			sModuleType="Actor";
// 			break;
// 		case 0x10:
// 			sModuleType="Central";
// 			break;			
// 		default:
// 			sModuleType="unknown";
// 			break;
// 	}
// 
// 	String sTemp;
// 	sTemp="from 0x" + String(nSource,HEX) + " to " + sDest + " as " + sModuleType;
// 	poDebug->Log(cDebug::LOG_I, sTemp);
// 
// 	int nDataPointCount=packet.data[3];
// 	int nIndex=4;
// 	for (int i=0; i<nDataPointCount;i++)
// 	{
// 	}
	
	 
}

int cCC1101_RFSensorPacket::InterpreteDatapoint(int bytenumber, char ismytelegram)
{
    byte type =  data[bytenumber];
    bytenumber++;

#ifdef WithCC1101Logging
	if (ismytelegram)
	{
		sLog += " my ";
	}
	
#endif

    switch (type) {
        case 0x01:
#ifdef WithCC1101Logging
			sLog += " Temperature ";
#endif
            break;
        case 0x02:
#ifdef WithCC1101Logging
			sLog += " Humidity ";
#endif
            break;
        case 0x03:
#ifdef WithCC1101Logging
			sLog += " AirQuality ";
#endif
            break;
        case 0x04:
#ifdef WithCC1101Logging
			sLog += " Date ";
#endif
            break;
        case 0x05:
#ifdef WithCC1101Logging
			sLog += " Time ";
#endif
            break;
        case 0x06:
#ifdef WithCC1101Logging
			sLog += " Brightness ";
#endif
            break;
        case 0x07:
#ifdef WithCC1101Logging
			sLog += " Battery ";
#endif
            break;
        case 0x08:
#ifdef WithCC1101Logging
			sLog += " Sabotage ";
#endif
            break;
        case 0x09:
#ifdef WithCC1101Logging
			sLog += " AirPressur ";
#endif
            break;
        case 0x0A:
#ifdef WithCC1101Logging
			sLog += " Error ";
#endif
            break;
        case 0x0B:
#ifdef WithCC1101Logging
			sLog += " WeatherIcon ";
#endif
            break;
        case 0x0C:
#ifdef WithCC1101Logging
			sLog += " PoP ";
#endif
            break;
        case 0x0D:
#ifdef WithCC1101Logging
			sLog += " AvgWindSpeed ";
#endif
            break;
        case 0x0E:
#ifdef WithCC1101Logging
			sLog += " WindGust ";
#endif
            break;
        case 0x0F:
#ifdef WithCC1101Logging
        sLog += " WindDir ";
#endif
        break;
		case 0x10:
#ifdef WithCC1101Logging
		sLog += " Rain_forecast ";
#endif
		break;
		case 0x11:
#ifdef WithCC1101Logging
		sLog += " Temperature_forecast ";
#endif
		break;
		default:
#ifdef WithCC1101Logging
			sLog += " unknown type ";
			sLog += String(type,HEX);
#endif
			break;
    }

    char datatype =  data[bytenumber];
    bytenumber++;

    switch (datatype) {
        case 0x01: // Byte 
			{
			int nVal=0;
			nVal =  data[bytenumber];
			bytenumber++;
#ifdef WithCC1101Logging	
			char outstr[15];
			dtostrf(nVal,3, 0, outstr);
			sLog += String(outstr);
#endif
			if (ismytelegram==2)
			{
				if (type==0x0B) {
					nWeatherIconId=nVal;
				}
			}
			}

            break;
        case 0x02: // int 

			{
			int nVal=0;
			nVal =  ((data[bytenumber] << 8) | data[bytenumber+1]);
			bytenumber++;
			bytenumber++;
#ifdef WithCC1101Logging
			char outstr[15];
			dtostrf(nVal,3, 0, outstr);
			sLog += String(outstr);
#endif
			if (ismytelegram==2) {
				if (type==0x0C) {
#ifdef DISPLAY_POP
					nPOP=nVal;
#endif
				}
				else if (type == 0x1f) {
					nRain2Display = nVal;
				}
			}
			}
            break;
        case 0x03: // float
			{
			binaryFloat temp;

			temp.binary[0]=data[bytenumber];
			bytenumber++;
			temp.binary[1]=data[bytenumber];
			bytenumber++;
			temp.binary[2]=data[bytenumber];
			bytenumber++;
			temp.binary[3]=data[bytenumber];
			bytenumber++;
#ifdef WithCC1101Logging
			char outstr[15];
			dtostrf(temp.floatingPoint,3, 2, outstr);
			sLog += String(outstr);
#endif	

			if (ismytelegram==2) {
				if (type==0x01)	{
					fTemp2Display=temp.floatingPoint;	
				}
				else if (type == 0x09) {
#ifdef DISPLAY_PRESSURE
					fPressure2Display=temp.floatingPoint;
#endif					
				}
				else if (type == 0x02) {
#ifdef DISPLAY_HUMIDITY
					fHumidity2Display=temp.floatingPoint;
#endif				
				}
				else if (type == 0x11) {
					fTempForecast2Display=temp.floatingPoint;
				}
				
			}
			}
            break;
        case 0x04: // string

            break;

        case 0x05: // date
			{
			int day=((data[bytenumber] << 8) | data[bytenumber+1]);
			bytenumber++;
			bytenumber++;
			int month=((data[bytenumber] << 8) | data[bytenumber+1]);
			bytenumber++;
			bytenumber++;
			int year=((data[bytenumber] << 8) | data[bytenumber+1]);
			bytenumber++;
			bytenumber++;
			
			if (ismytelegram>0){
				nDay=day;
				nMonth=month;
				nYear=year;
			}
			
#ifdef WithCC1101Logging
			sLog += String(day,DEC);
			sLog += ".";
			sLog += String(month,DEC);
			sLog +=".";
			sLog += String(year,DEC);
#endif
			}
            break;
        case 0x06: // time
			{
			int hour=((data[bytenumber] << 8) | data[bytenumber+1]);
			bytenumber++;
			bytenumber++;
			int minute=((data[bytenumber] << 8) | data[bytenumber+1]);
			bytenumber++;
			bytenumber++;
			int second=((data[bytenumber] << 8) | data[bytenumber+1]);
			bytenumber++;
			bytenumber++;

			if (ismytelegram>0){
				nHour=hour;
				nMinute=minute;
				nSecond=second;
			}

#ifdef WithCC1101Logging
			sLog += String(hour,DEC);
			sLog += ":";
			sLog += String(minute,DEC);
			sLog +=":";
			sLog += String(second,DEC);
#endif
			}
            break;
			default:
#ifdef WithCC1101Logging
			sLog += " unknown data type ";
			sLog += String(datatype,HEX);
#endif
			break;
    }

    char dataunit = data[bytenumber];
    bytenumber++;
    //adapter.log.debug("dataunit " + dataunit);
    switch (dataunit) {
        case 0x00:
#ifdef WithCC1101Logging
			//sLog +=  "";//ohne
#endif
            break;
        case 0x01:
#ifdef WithCC1101Logging
			sLog +=  "°C";
#endif
            break;
        case 0x02:
#ifdef WithCC1101Logging
			sLog +=  "%";
#endif
            break;
        case 0x03:
#ifdef WithCC1101Logging
			sLog +=  "mBar";
#endif
            break;
        case 0x04:
#ifdef WithCC1101Logging
			sLog +=  "lux";
#endif
            break;
        case 0x05:
#ifdef WithCC1101Logging
			sLog +=  "m/s";
#endif
            break;
        case 0x06:
#ifdef WithCC1101Logging
			sLog +=  "deg";
#endif
            break;
		case 0x07:
#ifdef WithCC1101Logging
			sLog += "mm";
#endif
			break;
		default:
#ifdef WithCC1101Logging
			sLog += " unknown unit ";
			sLog += String(dataunit,HEX);
#endif
break;		
			break;
    }
    
    return bytenumber;
}

/*
int cCC1101_RFSensorPacket::InterpreteDatapoint(int start, char ismytelegram)
{

	int nRet=start;
	//String sLog;
	switch (data[start])  //get the type of first DP
	{
		case 0x01:
#ifdef WithCC1101Logging
			sLog += " Temp ";
#endif
			if (data[start+1]==0x03) //float
			{
				binaryFloat temp;
				
				temp.binary[0]=data[start+2];
				temp.binary[1]=data[start+3];
				temp.binary[2]=data[start+4];
				temp.binary[3]=data[start+5];
				if (ismytelegram>1)
				{
					fTemp2Display=temp.floatingPoint;				
				}
#ifdef WithCC1101Logging				
				char outstr[15];
				dtostrf(temp.floatingPoint,3, 2, outstr);
				sLog= sLog + String(outstr);
				if (data[start+6]== 0x01) //°C
				{
					sLog += " C";
				}
#endif
				nRet=start+7;
			}
			break;
		case 0x02:
#ifdef WithCC1101Logging
			sLog += " Hum ";
#endif
			if (data[start+1]==0x03) //float
			{

				binaryFloat temp;
				
				temp.binary[0]=data[start+2];
				temp.binary[1]=data[start+3];
				temp.binary[2]=data[start+4];
				temp.binary[3]=data[start+5];
#ifdef DISPLAY_HUMIDITY
				if (ismytelegram>1)
				{
					fHumidity2Display=temp.floatingPoint;
				}
#endif

#ifdef WithCC1101Logging
				char outstr[15];
				dtostrf(temp.floatingPoint,3, 2, outstr);
				sLog= sLog + String(outstr);
				if (data[start+6]== 0x02) //%
				{
					sLog += "%";
				}
#endif
				nRet=start+7;
			}
			break;
		case 0x03:
#ifdef WithCC1101Logging
			sLog += " Qual";
#endif
			break;
		case 0x04:
#ifdef WithCC1101Logging
			sLog += " Date ";
#endif
			if (data[start+1]==0x05) //time = 3*int
			{
				if (ismytelegram>0)
				{
					nDay=((data[start+2] << 8) | data[start+3]);
					nMonth=((data[start+4] << 8) | data[start+5]);
					nYear=((data[start+6] << 8) | data[start+7]);
				}
#ifdef WithCC1101Logging
				sLog += String(nDay,DEC);
				sLog+= ".";
				sLog += String(nMonth,DEC);
				sLog+=".";
				sLog += String(nYear,DEC);
#endif
				nRet=start+9;
			}
			break;
		case 0x05:
#ifdef WithCC1101Logging
			sLog += " time ";
#endif
			if (data[start+1]==0x06) //time = 3*int 
			{
				if (ismytelegram>0)
				{
					nHour=((data[start+2] << 8) | data[start+3]);
					nMinute=((data[start+4] << 8) | data[start+5]);
					nSecond=((data[start+6] << 8) | data[start+7]);
				}
#ifdef WithCC1101Logging
				sLog += String(nHour,DEC);
				sLog+= ".";
				sLog += String(nMinute,DEC);
				sLog+=".";
				sLog += String(nSecond,DEC);
#endif
				nRet=start+9;
			}

			break;
		case 0x06:
#ifdef WithCC1101Logging
			sLog += " Bright ";
#endif
			if (data[start+1]==0x02) //int
			{
#ifdef WithCC1101Logging	
				int nVal=0;
				nVal = ((data[start+2] << 8) | data[start+3]);
			
				char outstr[15];
				dtostrf(nVal,3, 0, outstr);
				sLog += String(outstr);
#endif

			}
#ifdef WithCC1101Logging
			if (data[start+4]== 0x04) //lux
			{
				sLog += " lux";
			}
#endif
			nRet=start+5;
			break;
		case 0x07:
#ifdef WithCC1101Logging
			sLog += " Bat";
#endif
			break;
		case 0x08:
#ifdef WithCC1101Logging
			sLog += " Sab";
#endif
			break;
		case 0x09:
#ifdef WithCC1101Logging
			sLog += " Press ";
#endif
			if (data[start+1]==0x03) //float
			{
				
				binaryFloat temp;
				
				temp.binary[0]=data[start+2];
				temp.binary[1]=data[start+3];
				temp.binary[2]=data[start+4];
				temp.binary[3]=data[start+5];
#ifdef DISPLAY_PRESSURE
				if (ismytelegram>1)
				{
					fPressure2Display=temp.floatingPoint;
				}
#endif				
#ifdef WithCC1101Logging
				char outstr[15];
				dtostrf(temp.floatingPoint,3, 2, outstr);
				sLog= sLog + String(outstr);
				if (data[start+6]== 0x03) //mBar
				{
					sLog += " mBar";
				}
#endif
				nRet=start+7;
			}
			break;
		case 0x0A:
#ifdef WithCC1101Logging
			sLog += " error ";
#endif
			if (data[start+1]==0x02) //int
			{
				int nVal=0;
				nVal = ((data[start+2] << 8) | data[start+3]);
#ifdef WithCC1101Logging
				char outstr[15];
				dtostrf(nVal,3, 0, outstr);
				sLog += String(outstr);
#endif

			}
#ifdef WithCC1101Logging
			if (data[start+4]== 0x04) //kein
			{
				sLog += " ";
			}
#endif
			nRet=start+5;
			break;
		case 0x0B:
#ifdef WithCC1101Logging
		sLog += " WeatherIcon ";
#endif
		if (data[start+1]==0x01) //byte
		{
			int nVal=0;
			nVal = data[start+2] ;
			if (ismytelegram>1)
			{
				nWeatherIconId=nVal;
			}
#ifdef WithCC1101Logging
			char outstr[15];
			dtostrf(nVal,3, 0, outstr);
			sLog += String(outstr);
#endif

		}
#ifdef WithCC1101Logging
		if (data[start+4]== 0x04) //kein
		{
			sLog += " ";
		}
#endif
		nRet=start+4;
		break;
		
	case 0x0C: //PoP
#ifdef WithCC1101Logging
		sLog += " PoP ";
#endif
		if (data[start+1]==0x02) //int
		{
			int nVal=0;
			nVal = ((data[start+2] << 8) | data[start+3]);
#ifdef DISPLAY_POP
			if (ismytelegram>0)
			{
				nPOP=nVal;
			}
#endif
#ifdef WithCC1101Logging
			char outstr[15];
			dtostrf(nVal,3, 0, outstr);
			sLog += String(outstr);
#endif

		}
#ifdef WithCC1101Logging
		if (data[start+4]== 0x02) //%
		{
			sLog += "%";
		}
#endif
		nRet=start+5;
		break;	
		
	case 0x0D:
#ifdef WithCC1101Logging
		{
			sLog += " AvgWindSpeed ";
			
			char outstr[15];
			sLog += " type ";
			dtostrf(data[start+1],3, 0, outstr);
			sLog += String(outstr);
		}
#endif
		break;
	case 0x0E:
#ifdef WithCC1101Logging
		{
			sLog += " WindGust ";
			
			char outstr[15];
			sLog += " type ";
			dtostrf(data[start+1],3, 0, outstr);
			sLog += String(outstr);
		}
#endif

		break;
	case 0x0F:
#ifdef WithCC1101Logging
		{
			sLog += " WindDir ";
			
			char outstr[15];
			sLog += " type ";
			dtostrf(data[start+1],3, 0, outstr);
			sLog += String(outstr);
		}
#endif

		break;
		
			
	default:
#ifdef WithCC1101Logging
		{
			char outstr[15];
			sLog += " unknown type ";
			dtostrf(data[start],3, 0, outstr);
			sLog += String(outstr);
		}
#endif
			break;
	}
	//cDebug::Log(cDebug::LOG_I, sLog);
	return nRet;
}
*/
bool cCC1101_RFSensorPacket::CheckDataLength()
{
	bool bRet=true;
	if (length>CC1101_DATA_LEN)
	{
		//cDebug::Log(cDebug::LOG_E, "too many data " + String(length));
		bRet=false;
	}
	return bRet;
}


void cCC1101_RFSensorPacket::AddRawData(char *indata,int start, int inlength)
{
	for (int i=0; i<inlength-start;i++)
	{
		data[i]=indata[i+start];
	}	
	length=inlength-start;
	
	//test only
	//int nId[] ={0,1,2,3,4,5};
	//InterpretePacket(1,nId);
}
/*
 0 3f af 82 18 0 0 fe fe fe fe fe fe 1 4 1 3 16 d b1 41 1 9 3 47 4d 73 44 3 2 3 66 66 de 41 2 6 2 0 3e 4 J
 I 0 ce 12 83 18 0 0 fe fe fe fe fe fe 1 4 1 3 98 db b2 41 1 9 3 b3 85 73 44 3 2 3 66 66 18 42 2 6 2 0 48 4 J
 I 0 3f af 82 18 0 0 fe fe fe fe fe fe 1 4 1 3 3d 27 b1 41 1 9 3 66 52 73 44 3 2 3 9a 99 dd 41 2 6 2 0 3e 4 J
 I 0 ce 12 83 18 0 0 fe fe fe fe fe fe 1 4 1 3 c2 82 b2 41 1 9 3 30 82 73 44 3 2 3 66 66 18 42 2 6 2 0 49 4 J
 I 0 3f af 82 18 0 0 fe fe fe fe fe fe 1 4 1 3 9f 68 b1 41 1 9 3 a4 4f 73 44 3 2 3 cd cc dc 41 2 6 2 0 3e 4 J
 I 0 ce 12 83 18 0 0 fe fe fe fe fe fe 1 4 1 3 36 c2 b2 41 1 9 3 f7 84 73 44 3 2 3 66 66 18 42 2 6 2 0 49 4 J
 I 0 3f af 82 18 0 0 fe fe fe fe fe fe 1 4 1 3 7a 4e b1 41 1 9 3 0 54 73 44 3 2 3 0 0 dc 41 2 6 2 0 3e 4 J
 I 0 ce 12 83 18 0 0 fe fe fe fe fe fe 1 4 1 3 58 e b3 41 1 9 3 66 87 73 44 3 2 3 66 66 18 42 2 6 2 0 49 4 J
 I 0 3f af 82 18 0 0 fe fe fe fe fe fe 1 4 1 3 66 41 b1 41 1 9 3 f0 51 73 44 3 2 3 66 66 e2 41 2 6 2 0 3d 4 J
 I 0 ce 12 83 18 0 0 fe fe fe fe fe fe 1 4 1 3 98 db b2 41 1 9 3 7 84 73 44 3 2 3 33 33 19 42 2 6 2 0 48 4 J
 I 0 3f af 82 18 0 0 fe fe fe fe fe fe 1 4 1 3 8e 5b b1 41 1 9 3 b4 51 73 44 3 2 3 66 66 e6 41 2 6 2 0 3d 4 J
 I 0 ce 12 83 18 0 0 fe fe fe fe fe fe 1 4 1 3 a7 1 b3 41 1 9 3 e3 84 73 44 3 2 3 cd cc 1a 42 2 6 2 0 47 4 J
 */

/*
Zeit/Datum-Telegramm
s 0  0  0  0  0  0  0 fe fe fe fe fe fe 1  2 4 5 0 15 0 1 7 e1 0 5 6 0 e 0 37 0 22 0
  0 10 10 10 10 10 10 fe fe fe fe fe fe 10 2 4 5 0 16 0 0 f e1 0 5 6 0 e 0 37 0 22 0 b5 
*/

#endif