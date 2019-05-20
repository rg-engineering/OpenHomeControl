/*
 * main.cpp
 *
 * Created: 03.01.2017 21:31:28
 *  Author: Rene
 */ 
#define ARDUINO_MAIN

// Disable some warnings for the Arduino files
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wattributes"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wuninitialized"

#include <Arduino.h>
extern "C"
{
	#include <pins_arduino.h>
}

#include <avr/power.h>
#include <util/delay.h>

#include "..\Lib\board.h"
#include "..\Lib\cpplib\TFT\cTFT.h"
#include "..\Lib\cpplib\CC1101\CC1101_RFSensorPacket.h"
#include "..\Lib\cpplib\CC1101\CC1101_Main.h"
#include "..\Lib\cpplib\USBInterface\Serial.h"
#include "..\Lib\cpplib\OneWireDevices\onewiredevices.h"
#include "..\Lib\cpplib\BME280\BME280.h"

extern "C" void __cxa_pure_virtual() { while (1); }

/*
* Libraries
*/
#ifdef ARDUINO_LEONARDO
// Arduino Leonardo source files for serial:
#define USB_VID 0x2341
#define USB_PID 0x8036
//#include <abi.cpp>
#include <CDC.cpp>
#include <USBCore.cpp>
//#include <HID.cpp> //moved to libraries
#include <PluggableUSB.cpp>
#endif

//#include <HardwareSerial.cpp>
//#include <HardwareSerial0.cpp>
//#include <HardwareSerial1.cpp>
//#include <HardwareSerial2.cpp>
//#include <HardwareSerial3.cpp>


//#include <IPAddress.cpp>

// Other source files, depends on your program which you need
#include <Print.cpp>
#include <New.cpp>
#include <wiring.c>
#include <wiring_digital.c>
//#include <wiring_analog.c> //analog read/write functions
#include <WString.cpp>
//#include <WMath.cpp>
//#include <Stream.cpp>
//#include <Tone.cpp>

// Unused source files:
#include <WInterrupts.c>
//#include <wiring_pulse.c>
//#include <wiring_shift.c>
//#include <IPAddress.cpp>
//#include <Tone.cpp>

//#include <hooks.c>
#include <SPI.cpp>
//#include <SoftwareSerial.cpp>
#include <Wire.cpp>
#include <OneWire.cpp>
#include <twi.c>

// Restore original warnings configuration
#pragma GCC diagnostic pop

cSerial *poSerial;

String sAppVersion = sVERSION;
String sAppName = "RF Disp";


int getFreeRam()
{
	extern int __heap_start, *__brkval;
	int v;

	v = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);

	return v;
}

#ifdef HasBME280
cBME280 *poBME280;
#endif
#ifdef HAS_TFT
cTFT oTFT;
#endif

//============================== One Wire Devices ====================================================
#ifdef HasOneWireDevices
#define ID_SIZE 6
int nId[ID_SIZE];
cOneWireDevices oOneWireDevices(PIN_ONEWIRE);

void OneWire_Init()
{
	//cSerial::println("111");
	oOneWireDevices.SearchDevices();
	//cSerial::println("222");
	oOneWireDevices.ReadDevices();
	//cSerial::println("333");
	oOneWireDevices.GetID(nId,ID_SIZE);

	for (int n=0; n<ID_SIZE;n++)
	{
		cSerial::print(String(nId[n],HEX));
		cSerial::print(" ");
	}
	cSerial::println();

}
#endif


#ifdef HasCC1101
cCC1101_Main oCC1101;
//cCC1101_RFSensorPacket oReceivePacket;
//cCC1101_RFSensorPacket oSendPacket;
cCC1101_RFSensorPacket oPacket;

	
//int nPacketsGood;
int nPacketsCRSFailure=0;
//int nPacketsReceived;
bool bPacketAvailable;

//================================ CC1101 functions ============================================
// we have it here to save memory
// in sensors we have it dynamically in USBIfc.cpp
/* Handle interrupt from CC1101 (INT0) gdo0 on pin2 */

void cc1101signalsInterrupt_lokal(void)
{
	// set the flag that a package is available
	bPacketAvailable = true;
}

void AttachIRQ()
{
	//cSerial::println("attach IRQ");
	#ifdef ARDUINO_NANO
	attachInterrupt(1, cc1101signalsInterrupt_lokal, FALLING);
	#endif
	#ifdef ARDUINO_LEONARDO
	attachInterrupt(1, cc1101signalsInterrupt_lokal, FALLING);
	#endif
}
void DetachIRQ()
{

	#ifdef ARDUINO_NANO
	detachInterrupt(1);
	#endif
	#ifdef ARDUINO_LEONARDO
	detachInterrupt(1);
	#endif


}

/*
void RF_GetChipData()
{

		String sTemp;
		sTemp = "PARTNUM 0x";
		byte nTemp = oCC1101.readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER);
		sTemp += String(nTemp,HEX);
		Serial.println(sTemp);
		//cDebug::Log(cDebug::LOG_D,sTemp);
		
		sTemp = "VERSION 0x";
		nTemp = oCC1101.readReg(CC1101_VERSION, CC1101_STATUS_REGISTER);
		sTemp += String(nTemp,HEX);
		Serial.println(sTemp);
		//cDebug::Log(cDebug::LOG_D,sTemp);
		
		sTemp = "MARCSTATE 0x";
		nTemp = oCC1101.readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1f;
		sTemp += String(nTemp,HEX);
		Serial.println(sTemp);
		//cDebug::Log(cDebug::LOG_D,sTemp);
}
*/
void RF_Init()
{
	oCC1101.init();
	//RF_GetChipData();
	AttachIRQ();
}

void RF_ReadLQI()
{
#ifdef HasCC1101
	
	
		//byte lqi=0;
		//byte value=0;
		//lqi=poCC1101->readReg(CC1101_LQI, CC1101_STATUS_REGISTER);
		//value = 0x3F - (lqi & 0x3F);
		
		//String sTemp = "LQI " + String(value,DEC);
		//cDebug::Log(cDebug::LOG_I,sTemp);
	
#endif
}

void RF_ReadRSSI()
{
#ifdef HasCC1101
	
	

		byte rssi=0;
		byte value=0;

		rssi=(oCC1101.readReg(CC1101_RSSI, CC1101_STATUS_REGISTER));

		if (rssi >= 128)
		{
			value = rssi-256;
			value /= 2;
			value -= 74;
		}
		else
		{
			value = rssi/2;
			value -= 74;
		}
		

		String sTemp="RSSI " + String(value,DEC) + " dBm ";
		cSerial::print(sTemp);

	
#endif
}



void RF_SendData()
{
	oPacket.Reset();
#ifndef HasOneWireDevices
	int nId[] ={0,1,2,3,4,5};
#endif
	oPacket.AddHeader(nId,0x03);

	//print telegramm
#ifdef DEBUG
/*
	String sTemp;
	sTemp = "I ";
	for(int j=0; j<oPacket.length; j++)
	{
		sTemp += String(oPacket.data[j],HEX);
		sTemp += " ";
	}
	sTemp += "J ";
	cSerial::println(sTemp);
*/
#endif
	DetachIRQ();
	
	if(oCC1101.sendData(oPacket))
	{
		//cSerial::println("ok");
	}
	else
	{
		cSerial::println("failed");
	}
	// Enable wireless reception interrupt
	AttachIRQ();
}

void RF_ReceiveData()
{
		if(bPacketAvailable)
		{
			//nPacketsReceived++;
			//cSerial::println("packet received");
			
			oPacket.Reset();
			// Disable wireless reception interrupt
			DetachIRQ();
			
			//RF_ReadRSSI();
			//RF_ReadLQI();
			// clear the flag
			bPacketAvailable = false;

			if( oCC1101.receiveData(&oPacket) > 0)
			{
				if(!oPacket.crc_ok)
				{
					nPacketsCRSFailure++;
					//cDebug::Log(cDebug::LOG_E,"crc not ok");
					cSerial::println("CRC failed " + String(nPacketsCRSFailure,DEC));
				}
				else if(oPacket.length > 0)
				{
#ifdef HAS_TFT
					//nPacketsGood++;
					//cSerial::println("|");
					oPacket.fTemp2Display=-99.9;
#ifdef DISPLAY_PRESSURE
					oPacket.fPressure2Display=-99.9;
#endif
#ifdef DISPLAY_HUMIDITY
					oPacket.fHumidity2Display=-99.9;
#endif
#ifdef DISPLAY_POP
					oPacket.nPOP=-99;
#endif
					oPacket.fTempForecast2Display=-99.9;
					oPacket.nRain2Display=-1;
					oPacket.nHour=-1;
					oPacket.nMinute=-1;
					oPacket.nSecond=-1;
					oPacket.nDay=-1;
					oPacket.nMonth=-1;
					oPacket.nYear=-1;
					oPacket.nWeatherIconId=-1;

#endif //HAS_TFT								
#ifndef HasOneWireDevices
					int nId[] ={0,1,2,3,4,5};
#endif
					RF_ReadRSSI();
					RF_ReadLQI();
					
					oPacket.InterpretePacket(1, nId);

#ifdef HAS_TFT
					if (oPacket.fTemp2Display>-99.9)
					{
						oTFT.SetTemperature(oPacket.fTemp2Display,0);
					}
#ifdef DISPLAY_PRESSURE
					if (oPacket.fPressure2Display>-99.9)
					{
						oTFT.SetPressure(oPacket.fPressure2Display,0);
					}
#endif
#ifdef DISPLAY_HUMIDITY
					if (oPacket.fHumidity2Display>-99.9)
					{
						oTFT.SetHumidity(oPacket.fHumidity2Display,0);
					}
#endif
#ifdef DISPLAY_POP
					if (oPacket.nPOP>-99)
					{
						oTFT.SetPoP(oPacket.nPOP);
					}
#endif
					
					if (oPacket.fTemp2Display>-99)
					{
						oTFT.SetTemperatureForecast(oPacket.fTempForecast2Display);
					}
					
					if (oPacket.nRain2Display>-1)
					{
						oTFT.SetRain(oPacket.nRain2Display);
					}
					
					//if (oPacket.nHour>-1 && oPacket.nMinute>-1 && oPacket.nSecond>-1)
					if ( oPacket.nSecond>-1)
					{
						oTFT.SetTime(oPacket.nHour, oPacket.nMinute, oPacket.nSecond);
					}
					//if (oPacket.nDay>-1 && oPacket.nMonth>-1 && oPacket.nYear>-1)
					if (oPacket.nDay>-1)
					{
						oTFT.SetDate(oPacket.nDay, oPacket.nMonth, oPacket.nYear);
					}
					if (oPacket.nWeatherIconId!=255)
					{
						oTFT.SetWeatherIcon(oPacket.nWeatherIconId);
					}

					//cSerial::println("x");
#endif
				}
				else
				{
					//cSerial::println("-");
				}
			}
			else
			{
				//cSerial::println("+");
			}

			AttachIRQ();
		}
		
}
#endif




//============================== Main loop ====================================================

#define ALIVE_DELAY 60000  //should be 1 hour or more; now test only

//test only...
/*
int main_test(void)
{
	init();
	poSerial=NULL;
	poSerial=new cSerial();

	cSerial::println("Hello world");
	
	unsigned long nDelay=millis()+5000;

	while (1)
	{

		if (millis() > nDelay)
		{
		
			cSerial::println("");
			cSerial::println("");
			cSerial::println(sAppName);
			cSerial::println(sAppVersion);
			cSerial::println("(c)2017");
			nDelay=millis()+5000;
			cSerial::println(nDelay);
		}
		
	}
}
*/

int main(void)
{
	init();
	
#ifdef ARDUINO_LEONARDO
	#if defined(USBCON)
	USBDevice.attach();
	#endif
	oSerial.begin(115200);
	
	//Serial.begin(115200);

	while (!oSerial.available())
	{
		; // wait for serial port to connect. Needed for Leonardo only
	}
#endif	
	poSerial=NULL;
	poSerial=new cSerial();

	cSerial::println("");
	cSerial::println("");
	cSerial::println(sAppName);
	cSerial::println(sAppVersion);
	cSerial::println("(c)2017, 2018");
	delay(100);


		
#ifdef ATMEGA644
	cSerial::println("build for ATMEGA644");
#elif ARDUINO_NANO
	cSerial::println("build for Nano");
#elif ARDUINO_LEONARDO
	cSerial::println("build for Leonardo");
#else
	cSerial::println("target undefined");
#endif

	cSerial::println("RAM " +  String (getFreeRam(),DEC));
#ifdef HasOneWireDevices
	OneWire_Init();
#endif
#ifdef HAS_TFT
	oTFT.setup();
#endif
#ifdef HasCC1101
	RF_Init();
#endif
#ifdef HasBME280
	poBME280 = new cBME280();
	poBME280->Init();
#endif

	cSerial::println("RAM " +  String (getFreeRam(),DEC));
	float fTemp=-99.0;
#ifdef HasOneWireDevices
	oOneWireDevices.ReadDevices();
	//cSerial::println("aaa");
	fTemp=oOneWireDevices.GetTemp();
	cSerial::println("Temp DS " + String(fTemp,DEC));
	
#ifdef HAS_TFT
	oTFT.SetTemperature(fTemp,1);
	//cSerial::println("bbb");
#endif
#endif
#ifdef HasBME280
	//cSerial::println("b1b");
	float fTemp1=poBME280->readTemperature();
	cSerial::println("Temp BME " + String(fTemp1,DEC));
	
	//cSerial::println("ccc");
	float fHum=poBME280->readHumidity();
	float fPress=poBME280->readPressure()/100.0;
#ifdef HAS_TFT
	//cSerial::println("d1d");
	if (fTemp<-98.0 && fTemp1>-99.0) {
		oTFT.SetTemperature(fTemp1,1);
	}
	oTFT.SetHumidity(fHum,1);
	oTFT.SetPressure(fPress,1);
	//cSerial::println("ddd");
#endif
#endif
#ifdef HasCC1101
	cSerial::println("f1f");
	RF_SendData(); //initial I'm here
	cSerial::println("fff");
#endif
	unsigned long nDelay=millis()+ALIVE_DELAY;
	
	cSerial::println("init done");
	while(1)
	{

		if (millis() > nDelay)
		{
#ifdef HasOneWireDevices
			oOneWireDevices.ReadDevices();

			fTemp=oOneWireDevices.GetTemp();
			//cSerial::println("Temp DS " + String(fTemp,DEC));
		#ifdef HAS_TFT
			oTFT.SetTemperature(fTemp,1);
		#endif
#endif
#ifdef HasBME280
			fTemp1=poBME280->readTemperature();
			//cSerial::println("Temp BME " + String(fTemp1,DEC));
			float fHum=poBME280->readHumidity();
			float fPress=poBME280->readPressure()/100.0;
		#ifdef HAS_TFT
			if (fTemp<-98.0 && fTemp1>-99.0) {
				oTFT.SetTemperature(fTemp1,1);
			}
			oTFT.SetHumidity(fHum,1);
			oTFT.SetPressure(fPress,1);
		#endif
#endif
#ifdef HasCC1101
			// send: I'm still there
			RF_SendData();
#endif
			nDelay=millis()+ALIVE_DELAY;
		}
#ifdef HasCC1101
		RF_ReceiveData();
#endif
#ifdef HAS_TFT
		oTFT.loop();
#endif
#ifdef ARDUINO_LEONARDO
		if (serialEventRun) serialEventRun();
#endif
	}
}


