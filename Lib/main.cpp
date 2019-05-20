/*
 * GeneralRFSensor.cpp
 *
 * Created: 04.10.2015 14:28:46
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

#include "board.h"
#include "cpplib/help/text_de.h"
#include "cpplib/Debug/Debug.h"
#include "cpplib/USBInterface/USBIfc.h"
#include "cpplib/USBInterface/Serial.h"


extern "C" void __cxa_pure_virtual() { while (1); }

/*
* Libraries
*/
// Arduino Leonardo source files for serial:
#define USB_VID 0x2341
#define USB_PID 0x8036

//#include <abi.cpp>
#include <CDC.cpp>
#include <USBCore.cpp>
//#include <HID.cpp> //moved to libraries
#include <PluggableUSB.cpp>

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
#include <SoftwareSerial.cpp>
#include <Wire.cpp>
#include <OneWire.cpp>
#include <twi.c>

// Restore original warnings configuration
#pragma GCC diagnostic pop

cSerial *poSerial;
//cDebug *poDebug;

cUSBIfc *poUSBIfc;

String sAppVersion = sVERSION;

#ifdef IsZentrale
String sAppName = "RF Receiver";
#elif SmartMeter
String sAppName = "Smart Meter";
#else
String sAppName = "RF Sensor";
#endif


int getFreeRam()
{
	extern int __heap_start, *__brkval;
	int v;

	v = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);

	return v;
}

int main(void)
{
	init();


	
	
#ifdef ARDUINO_LEONARDO
	#if defined(USBCON)
	USBDevice.attach();
	#endif

	Serial.begin(115200);
	
	while (!Serial)
	{
		; // wait for serial port to connect. Needed for Leonardo only
	}
#endif

	poSerial=NULL;
	//poDebug=NULL;
	poUSBIfc=NULL;
	
	poSerial=new cSerial();
	
	//poDebug=new cDebug("Main",true);
	//poDebug->SetLogLevel(cDebug::LOG_E | cDebug::LOG_W | cDebug::LOG_I | cDebug::LOG_D ); //kein debug !!
	//poDebug->Begin();

	//poDebug->Log(cDebug::LOG_I,sAppName);
	cSerial::println("");
	cSerial::println("");
	cSerial::println(sAppName);
	//delay(100);	
	//poDebug->Log(cDebug::LOG_I,sAppVersion);
	cSerial::println(sAppVersion);
	delay(100);	

//	poDebug->Log(cDebug::LOG_I,COPYRIGHT);
	//poDebug->Log(cDebug::LOG_I,"Hier sollte COPYRIGHT stehen...");
	cSerial::println("(c)2017-2019 rg-engineering");
	delay(100);	


#ifdef ARDUINO_NANO
	//poDebug->Log(cDebug::LOG_I,"build for Nano");
	//poDebug->Log(cDebug::LOG_I,"Nano");
	cSerial::println("build for Nano");
#else
	#ifdef ARDUINO_LEONARDO
		//poDebug->Log(cDebug::LOG_I,"build for Leonardo");
		cSerial::println("build for Leonardo");
	#else
		//poDebug->Log(cDebug::LOG_E,"target undefined");	
	#endif
#endif

	//poDebug->Log(cDebug::LOG_I,"RAM " + String (getFreeRam(),DEC));
	poSerial->println("RAM " +  String (getFreeRam(),DEC));
//	poDebug->Log(cDebug::LOG_D,"initializing...");
	//poDebug->Log(cDebug::LOG_D,"init");

	poUSBIfc = new cUSBIfc();
	poUSBIfc->Init();

	//poDebug->Log(cDebug::LOG_I,"RAM " + String (getFreeRam(),DEC));

	poSerial->println("RAM " +  String (getFreeRam(),DEC));
	//poDebug->Log(cDebug::LOG_D,"starting prog loop");
	//poDebug->Log(cDebug::LOG_D,"go");
    while(1)
    {
		//poDebug->Task();
		poUSBIfc->Do();
#ifdef ARDUINO_LEONARDO
		if (serialEventRun) serialEventRun();
#endif
    }
}
