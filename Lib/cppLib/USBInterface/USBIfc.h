/* 
* cUSBIfc.h
*
* Created: 05.10.2015 19:50:37
* Author: Rene
*/


#ifndef __CUSBIFC_H__
#define __CUSBIFC_H__


#include "../CC1101/CC1101_Main.h"
#include "../CC1101/CC1101_RFSensorPacket.h"
#include "../BH1750/BH1750.h"
#include "../FerrarisCounter/FerrarisCounter.h"
#include "../BMP180/BMP180.h"
#include "../DHT22/DHT22.h"
#include "../OneWireDevices/OneWireDevices.h"
#include "../BME280/BME280.h"
#include "../Ventus/VentusW132.h"
#include "../Ventus/AnalyseProtokoll.h"
#include "../Ventus/Analyse2.h"

#ifdef HAS_TFT_ST7735
#include "../TFT/cTFT.h"
#endif

#define MAX_IN_BUFFER 60

class cUSBIfc
{
//variables
public:
protected:
private:
	bool bEnabled;
	char inData[MAX_IN_BUFFER];
	int inDataCnt;
	
#ifdef HasCC1101
	cCC1101_Main *poCC1101;
#endif
	//int nLED;
//functions
public:
	cUSBIfc();
	~cUSBIfc();
	
	void Init();

	void Do();
	
	static bool bPacketAvailable;
	static bool bTimer;
	bool bSensorMode;
	int nOldIRQCounter;
protected: //RF
	void RF_GetChipData();
	void RF_Init();
	void RF_SendData();
	void RF_SendTestData();
	void RF_SendRawData(int start);
	void RF_ReceiveData();
	void RF_ReadRSSI();
	void RF_ReadLQI();
	void StartSensorMode();
	void ConfigureTimer();
	void SendDataByTimer();
	void enterSleep();
	

protected:
private:
	cUSBIfc( const cUSBIfc &c );
	cUSBIfc& operator=( const cUSBIfc &c );
	
	void ResetData();
	void AttachIRQ();
	void DetachIRQ();
	
	int nCtr;
	
	bool bToggle;
	
#ifdef HasCC1101
	cCC1101_RFSensorPacket oReceivePacket;
	cCC1101_RFSensorPacket oSendPacket;
	//int nNextData; //just to know what to send next
	
	int nPacketsGood;
	int nPacketsCRSFailure;
	int nPacketsReceived;
#endif
	
private:
#ifdef HasBH1750
	cBH1750 *poBH1750;
#endif
private:
	cFerrarisCounter *poFerrarisCounter0;
	cFerrarisCounter *poFerrarisCounter1;

private:
#ifdef HasBMP180
	cBMP180 *poBMP180;
	double fTemp1;
#endif

	
private:
#ifdef HasDHT22
	cDHT22 *poDHT22;
	void GetHumidity();
	float fTemp2;
	float fHumidity;
#endif



private:
	cBME280 *poBME280;


#ifdef HasOneWireDevices
	cOneWireDevices *poOneWireDevices;
#endif

	char nMode4PrintOut;
	
#ifdef HAS_TFT_ST7735
	//cTFT *poDisplay;
#endif

#ifdef HasVentusW132
	cVentusW132 *poVentusW132;
	//CAnalyseProtokoll *poAnalyseProtokoll;
#endif

}; //cUSBIfc



#endif //__CUSBIFC_H__
