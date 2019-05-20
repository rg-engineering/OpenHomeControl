/***************************************************
  This is a library for the Adafruit 1.8" SPI display.

This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
The 1.8" TFT shield
  ----> https://www.adafruit.com/product/802
The 1.44" TFT breakout
  ----> https://www.adafruit.com/product/2088
as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
/* 
* cpoTFT->cpp
* based on above mentioned sample
* Created: 03.01.2017 17:10:05
* Author: Rene
*/


#ifdef HAS_TFT

#include <Adafruit_GFX.h>    // Core graphics library
#ifdef HAS_TFT_ST7735
	#include <Adafruit_ST7735.h> // Hardware-specific library
#endif
#ifdef HAS_TFT_QDTech
//	#include <Adafruit_QDTech.h> // modifizierte Adafruit-Bibliothek
#endif
#include <SPI.h>
#include "../../board.h"
#include "../USBInterface/Serial.h"

extern int getFreeRam();

#ifdef HAS_SDCARD
	#include <SPI.h>
	#include <SdFat.h>
#endif


//http://www.instructables.com/id/Arduino-TFT-display-of-bitmap-images-from-an-SD-Ca/step7/The-future/




//PIN definition moved to board.h
// For the breakout, you can use any 2 or 3 pinsnClockCenter
// These pins will also work for the 1.8" TFT shield
//#define TFT_CS     10
//#define TFT_RST    9  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
//#define TFT_DC     8
//	#define TFT_SCLK 13   // set these to be whatever pins you like!
//	#define TFT_MOSI 11   // set these to be whatever pins you like!
#include "cTFT.h"

#define CLOCK_CENTER_X  64
#define CLOCK_CENTER_Y 25
#define CLOCK_RADIUS  23

#define DIGITAL_WATCH_X 20
#define DIGITAL_WATCH_Y 5

#define DATE_X 40
#define DATE_Y 40

#define TEMP_Y 70

#ifdef DISPLAY_PRESSURE
#define PRESSURE_X 67
#define PRESSURE_Y 155
#define PRESSURE_WIDHT 5
#define PRESSURE_DIST 1
#define MAXPRESSURE 990
#define MINPRESSURE 900
#define MAXHEIGHT 40
#endif

#define WEATHER_X 10
#define WEATHER_Y 105
#define TEMP_FORECAST_X  70
#define TEMP_FORECAST_Y 100

#define WEATHER_POP_X 110
#define WEATHER_POP_Y 155
#define MAXHEIGHT_POP 50

#define TEMPFORECAST_X 70
#define TEMPFORECAST_Y 115


// default constructor
cTFT::cTFT()
{
#ifdef HAS_TFT_ST7735
	// Option 1 (recommended): must use the hardware SPI pins
	// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
	// an output. This is much faster - also required if you want
	// to use the microSD card (see the image drawing example)
	poTFT = new Adafruit_ST7735(PIN_CS_TFT,  PIN_TFT_DC, PIN_TFT_RST);
	

	// Option 2: use any pins but a little slower!
	//#define TFT_SCLK 13   // set these to be whatever pins you like!
	//#define TFT_MOSI 11   // set these to be whatever pins you like!
	//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
#endif

#ifdef HAS_TFT_QDTech
	poTFT = new Adafruit_QDTech(PIN_CS_TFT,PIN_TFT_DC,PIN_TFT_RST);
#endif

	nHour=0;
	nMinute=0;
	nSeconds=0;
	
	nHour_Old=-1;
	nMinute_Old=-1;
	nSeconds_Old=-1;
	fTemp[0]=-99;
	fTemp[1]=-99;
	nDay=1;
	nMonth=1;
	nYear=2018;
	
	fTemp_Old[0]=-99;
	fTemp_Old[1]=-99;
	nDay_Old=0;
	nMonth_Old=0;
	nYear_Old=0;

#ifdef DISPLAY_PRESSURE
	fPressure[0]=-99;
	fPressure_Old[0]=-99;
	fPressure[1]=-99;
	fPressure_Old[1]=-99;
#endif
#ifdef DISPLAY_HUMIDITY
	fHumidity[0]=-99;
	fHumidity_Old[0]=-99;
	fHumidity[1]=-99;
	fHumidity_Old[1]=-99;
#endif
#ifdef DISPLAY_POP
	nPOP=-99;
	nPOP_Old=-99;
#endif
	
	fForecastTemp=-99;
	fForecastTemp_Old=-99;
		
	nForecastRain=-1;
	nForecastRain_Old=-1;
	
	nWeatherIconId=1;
	nWeatherIconId_Old=0;
} //cTFT

// default destructor
cTFT::~cTFT()
{
} //~cTFT


void cTFT::setup(void) 
{
#ifdef HAS_TFT_ST7735
	//cSerial::print("111");
	// Use this initializer if you're using a 1.8" TFT
	poTFT->initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
	//cSerial::print("222");
	// Use this initializer (uncomment) if you're using a 1.44" TFT
	//poTFT->initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab
	
	poTFT->fillScreen(ST7735_WHITE);

#ifdef USE_OLD_TFT_PCB
	poTFT->setRotation(2); //0,1,2,3
#else
	poTFT->setRotation(0); //0,1,2,3
#endif	
	
	//cSerial::print("333");
#endif
	
	//time = millis() - time;
#ifdef HAS_TFT_QDTech
	poTFT->init();   		// Display-Chip initialisieren

	poTFT->setRotation(0);	// Display steht laut Schaltplan auf dem Kopf, also anpassen

	poTFT->fillScreen(QDTech_WHITE);
#endif
  
	delay(500);
	nTime_Clock = millis();
	//nTime_Temp=millis();
#ifdef HAS_ANALOG_WATCH
	DrawClock();
#endif


}

void cTFT::SetTemperature(float temp, int idx)
{
	if (idx==0 || idx == 1)
	{
		fTemp[idx]=temp;
	}
}



void cTFT::SetTime(int hour, int minute, int second)
{
	nHour=hour;
	nMinute=minute;
	nSeconds=second;
}

void cTFT::SetDate(int day, int month, int year)
{
	nDay=day;
	nMonth=month;
	nYear=year;
}

void cTFT::SetPressure(float pressure, int idx)
{
#ifdef DISPLAY_PRESSURE
	if (idx==0 || idx == 1)
	{
		fPressure[idx]=pressure;
	}
#endif
}

void cTFT::SetHumidity(float humidity, int idx)
{
#ifdef DISPLAY_HUMIDITY
	if (idx==0 || idx == 1)
	{
		fHumidity[idx]=humidity;
	}
#endif
}
void cTFT::SetPoP(int pop)
{
#ifdef DISPLAY_POP
	nPOP=pop;
#endif
}

void cTFT::SetWeatherIcon(byte icon_id)
{
	nWeatherIconId=icon_id;
}

void cTFT::SetTemperatureForecast(float temp)
{
	fForecastTemp=temp;
}

void cTFT::SetRain(int rain)
{
	nForecastRain=rain;
}


void cTFT::loopTest()
{
#ifdef HAS_SDCARD
	
#endif
}

void cTFT::loop() 
{
	//einmal pro sekunde
	uint16_t time_temp = millis() - nTime_Clock;

	if (time_temp>1000)
	{
		nTime_Clock=millis();
		DrawClockWatchHand();
	}
}

//============ sonstige Anzeigen =======================================================================


//============ Uhr Funktionen =======================================================================

//analog clock background
void cTFT::DrawClock()
{
	//outer circle
#ifdef HAS_TFT_ST7735
	poTFT->drawCircle(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_RADIUS, ST7735_BLACK);
#endif
#ifdef HAS_TFT_QDTech
	poTFT->drawCircle(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_RADIUS, QDTech_GREEN);
#endif
	//12 / 3 / 6 / 9 hours
	poTFT->setTextWrap(false);
#ifdef HAS_TFT_ST7735
	poTFT->setTextColor(ST7735_BLACK);
#endif
#ifdef HAS_TFT_QDTech
	poTFT->setTextColor(QDTech_GREEN);
#endif
	poTFT->setTextSize(1);
	
	//inner circle
#ifdef HAS_TFT_ST7735
	poTFT->fillCircle(CLOCK_CENTER_X, CLOCK_CENTER_Y, 3, ST7735_BLACK);
#endif
#ifdef HAS_TFT_QDTech
	poTFT->fillCircle(CLOCK_CENTER_X, CLOCK_CENTER_Y, 3, QDTech_GREEN);
#endif
	
	// minutes
	for (int i=0; i<60;i++)
	{
		int nAngle=i*360/60;
		int LineLenght=5;
		
		if((i%5)==0)
		{
			LineLenght=10;
		}
	
		float fAngle=PI*nAngle/180.0;
		float fX1=(CLOCK_RADIUS-LineLenght)*sin(fAngle);
		int nX1=round(fX1);
		float fY1=(CLOCK_RADIUS-LineLenght)*cos(fAngle);
		int nY1=round(fY1);

		float fX2=(CLOCK_RADIUS)*sin(fAngle);
		int nX2=round(fX2);
		float fY2=(CLOCK_RADIUS)*cos(fAngle);
		int nY2=round(fY2);

#ifdef HAS_TFT_ST7735
		poTFT->drawLine(CLOCK_CENTER_X+nX1, CLOCK_CENTER_Y-nY1, CLOCK_CENTER_X+nX2, CLOCK_CENTER_Y-nY2, ST7735_BLACK);
#endif
#ifdef HAS_TFT_QDTech
		poTFT->drawLine(CLOCK_CENTER_X+nX1, CLOCK_CENTER_Y-nY1, CLOCK_CENTER_X+nX2, CLOCK_CENTER_Y-nY2, QDTech_BLACK);
#endif		
	}
	
	

	
}

void cTFT::DrawClockWatchHand()
{
	nSeconds++;
	if (nSeconds>=60)
	{
		nSeconds=0;
		nMinute++;
	}
	if (nMinute>=60)
	{
		nMinute=0;
		nHour++;
	}
	if (nHour>=24)
	{
		nHour=0;
	}
#ifndef HAS_ANALOG_WATCH
// digital watch
	if (nHour_Old!=nHour || nMinute_Old!=nMinute)
	{
		poTFT->setTextColor(ST7735_WHITE);
		poTFT->setCursor(DIGITAL_WATCH_X, DIGITAL_WATCH_Y);
		poTFT->setTextSize(3);
		char sTime[5];
		sprintf(sTime,"%02d:%02d",nHour_Old,nMinute_Old);
		poTFT->println(sTime);

		poTFT->setTextColor(ST7735_RED);
		poTFT->setCursor(DIGITAL_WATCH_X, DIGITAL_WATCH_Y);
		sprintf(sTime,"%02d:%02d",nHour,nMinute);
		poTFT->println(sTime);
		
		nHour_Old=nHour;
		nMinute_Old=nMinute;
	}

#endif
	
#ifdef HAS_ANALOG_WATCH
		//first hide watch hand
		if (nHour_Old>-1 && nMinute_Old>-1 && nSeconds_Old>-1)
		{
			int nAngle=(nHour_Old>12?(nHour-12):nHour_Old)*360/12;
			//Minuten berücksichtigen
			nAngle += nMinute_Old * 360 / 12 / 60;
			
			float fAngle=PI*nAngle/180.0;
			float fX=(CLOCK_RADIUS-20)*sin(fAngle);
			int nX=round(fX);
			float fY=(CLOCK_RADIUS-20)*cos(fAngle);
			int nY=round(fY);
			#ifdef HAS_TFT_ST7735
			poTFT->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_CENTER_X+nX, CLOCK_CENTER_Y-nY, ST7735_WHITE);
			#endif
			#ifdef HAS_TFT_QDTech
			poTFT->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_CENTER_X+nX, CLOCK_CENTER_Y-nY, QDTech_WHITE);
			#endif
			//minute (longer line)
			nAngle=nMinute_Old * 360 / 60;
			//sekunden berücksichtigen
			nAngle += nSeconds_Old * 360 / 60 / 60;
			
			fAngle=PI*nAngle/180.0;
			fX=(CLOCK_RADIUS-5)*sin(fAngle);
			nX=round(fX);
			fY=(CLOCK_RADIUS-5)*cos(fAngle);
			nY=round(fY);
			#ifdef HAS_TFT_ST7735
			poTFT->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_CENTER_X+nX, CLOCK_CENTER_Y-nY, ST7735_WHITE);
			#endif
			#ifdef HAS_TFT_QDTech
			poTFT->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_CENTER_X+nX, CLOCK_CENTER_Y-nY, QDTech_WHITE);
			#endif
			
			nAngle=nSeconds_Old * 360 / 60;
			fAngle=PI*nAngle/180.0;
			fX=(CLOCK_RADIUS-5)*sin(fAngle);
			nX=round(fX);
			fY=(CLOCK_RADIUS-5)*cos(fAngle);
			nY=round(fY);
			#ifdef HAS_TFT_ST7735
			poTFT->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_CENTER_X+nX, CLOCK_CENTER_Y-nY, ST7735_WHITE);
			#endif
			#ifdef HAS_TFT_QDTech
			poTFT->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_CENTER_X+nX, CLOCK_CENTER_Y-nY, QDTech_WHITE);
			#endif
		}
	
		//and watch hand
	//hour (short line)

	int nAngle=(nHour>12?(nHour-12):nHour)*360/12;
	//Minuten berücksichtigen
	nAngle += nMinute * 360 / 12 / 60;
	
	float fAngle=PI*nAngle/180.0;
	float fX=(CLOCK_RADIUS-20)*sin(fAngle);
	int nX=round(fX);
	float fY=(CLOCK_RADIUS-20)*cos(fAngle);
	int nY=round(fY);
#ifdef HAS_TFT_ST7735
	poTFT->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_CENTER_X+nX, CLOCK_CENTER_Y-nY, ST7735_BLACK);
#endif
#ifdef HAS_TFT_QDTech
	poTFT->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_CENTER_X+nX, CLOCK_CENTER_Y-nY, QDTech_BLACK);
#endif
	
	//minute (longer line)
	nAngle=nMinute * 360 / 60;
	//sekunden berücksichtigen
	nAngle += nSeconds * 360 / 60 / 60;
	
	fAngle=PI*nAngle/180.0;
	fX=(CLOCK_RADIUS-5)*sin(fAngle);
	nX=round(fX);
	fY=(CLOCK_RADIUS-5)*cos(fAngle);
	nY=round(fY);
#ifdef HAS_TFT_ST7735
	poTFT->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_CENTER_X+nX, CLOCK_CENTER_Y-nY, ST7735_BLACK);
#endif
#ifdef HAS_TFT_QDTech
	poTFT->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_CENTER_X+nX, CLOCK_CENTER_Y-nY, QDTech_BLACK);
#endif
	
	nAngle=nSeconds * 360 / 60;
	fAngle=PI*nAngle/180.0;
	fX=(CLOCK_RADIUS-5)*sin(fAngle);
	nX=round(fX);
	fY=(CLOCK_RADIUS-5)*cos(fAngle);
	nY=round(fY);
#ifdef HAS_TFT_ST7735
	poTFT->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_CENTER_X+nX, CLOCK_CENTER_Y-nY, ST7735_RED);
#endif
#ifdef HAS_TFT_QDTech
	poTFT->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_CENTER_X+nX, CLOCK_CENTER_Y-nY, QDTech_MAGENTA);
#endif

#endif
			
	//Time in HH:MM
/*	poTFT->setCursor(CLOCK_CENTER_X-CLOCK_RADIUS, CLOCK_CENTER_Y+CLOCK_RADIUS+3*nLineLength);
	poTFT->setTextSize(2);
	char sTime[5];
	sprintf(sTime,"%02d:%02d",nHour,nMinute);
	poTFT->println(sTime);
	*/
	nHour_Old=nHour;
	nMinute_Old=nMinute;
	nSeconds_Old=nSeconds;

//======================== Date Display
	if (nDay_Old!=nDay || nMonth_Old!=nMonth || nYear_Old!=nYear)
	{
		poTFT->setTextColor(ST7735_WHITE);	
		poTFT->setCursor(DATE_X, DATE_Y);
		poTFT->setTextSize(1);
		char sTime[10];
		sprintf(sTime,"%02d.%02d.%04d",nDay_Old,nMonth_Old,nYear_Old);
		poTFT->println(sTime);

		poTFT->setTextColor(ST7735_BLUE);
		poTFT->setCursor(DATE_X, DATE_Y);
		poTFT->setTextSize(1);
		sprintf(sTime,"%02d.%02d.%04d",nDay,nMonth,nYear);
		poTFT->println(sTime);
	
		nDay_Old=nDay;
		nMonth_Old=nMonth;
		nYear_Old=nYear;
	}
// ======================= Temp Display
#ifdef HAS_TFT_ST7735
	poTFT->drawLine(0, TEMP_Y-15, 128, TEMP_Y-15, ST7735_BLACK);
	poTFT->drawLine(64, TEMP_Y-15, 64, TEMP_Y+30, ST7735_BLACK);
	poTFT->drawLine(0, TEMP_Y+30, 128, TEMP_Y+30, ST7735_BLACK);
#endif

	DrawTemp(0);
	DrawTemp(1);
	DrawTempForecast();

#ifdef DISPLAY_PRESSURE
	//DrawPressure(0);
	DrawPressure(1);
#endif
	
#ifdef DISPLAY_HUMIDITY
	DrawHumidity(0);
	DrawHumidity(1);
#endif

#ifdef DISPLAY_POP
	DrawPoP();
#endif

	DrawRain();
	
//==================== weather icon ==============================
	if (nWeatherIconId!=nWeatherIconId_Old) //test only
	{
		bmpDraw(nWeatherIconId,WEATHER_X,WEATHER_Y);
		
/*	
		poTFT->setTextColor(ST7735_MAGENTA);
		poTFT->setCursor(WEATHER_X,WEATHER_Y );
		poTFT->setTextSize(3);
		poTFT->println(nWeatherIconId);
*/
		nWeatherIconId_Old=nWeatherIconId;
		
	}


//==================== air pressure bars ==============================
/*	if (fPressure[11]!=fPressure_Old[11])
	{
		poTFT->setTextColor(ST7735_WHITE);
		poTFT->setCursor(PRESSURE_X,PRESSURE_Y-40 );
		poTFT->setTextSize(1);
		char sPressure[10];
		char outstr[15];
		dtostrf(fPressure_Old[11],3, 0, outstr);
		sprintf(sPressure,"%s mBar",outstr);
		poTFT->println(sPressure);
	
		poTFT->setTextColor(ST7735_DarkOliveGreen);
		poTFT->setCursor(PRESSURE_X,PRESSURE_Y-40 );
		dtostrf(fPressure[11],3, 0, outstr);
		sprintf(sPressure,"%s mBar",outstr);
		poTFT->println(sPressure);
	}
	
	float pressureMin=9999;
	float pressureMax=0;
	for (int i=0; i<12; i++)
	{
		if (fPressure[i]>pressureMax)
		{
			pressureMax=fPressure[i];
		}
		if (fPressure[i]<pressureMin)
		{
			pressureMin=fPressure[i];
		}
	}
	if (pressureMax - pressureMin < 2.0)
	{
		pressureMax=pressureMin+1.0;
		pressureMin=pressureMax-2.0;
	}

	for (int i=0; i<12; i++)
	{
		//DrawPressure(i,pressureMax,pressureMin);
		fPressure_Old[i]=fPressure[i];
	}

*/



}





#ifdef DISPLAY_HUMIDITY
void cTFT::DrawHumidity(int idx)
{
	int nPosX1=27+64*idx;
	int nPosY=TEMP_Y+20;
	
	if(fHumidity[idx]>-99.9 && fHumidity[idx]!=fHumidity_Old[idx])
	{
		poTFT->setCursor(nPosX1, nPosY);
		poTFT->setTextSize(1);
		char outstr[15];
		
		//first overpaint old value
	#ifdef HAS_TFT_ST7735
		poTFT->setTextColor(ST7735_WHITE);
	#endif
	#ifdef HAS_TFT_QDTech
		poTFT->setTextColor(QDTech_WHITE);
	#endif
		//-22.98
		dtostrf(fHumidity_Old[idx],3, 0, outstr);
		poTFT->print(outstr);
		poTFT->print(" %");
		
		//then paint the new value
		poTFT->setCursor(nPosX1, nPosY);
	#ifdef HAS_TFT_ST7735
		poTFT->setTextColor(ST7735_DARKBLUE);
	#endif
	#ifdef HAS_TFT_QDTech
		poTFT->setTextColor(QDTech_BLUE);
	#endif
		//-22.98
		dtostrf(fHumidity[idx],3, 0, outstr);
		poTFT->print(outstr);
		poTFT->print(" %");
		fHumidity_Old[idx]=fHumidity[idx];
	}
}

#endif

#ifdef DISPLAY_POP
/*
void cTFT::DrawPoP()
{
	int nPosX1=WEATHER_POP_X;
	int nPosY=WEATHER_POP_Y;
	
	if(nPOP>-99 && nPOP!=nPOP_Old)
	{
		poTFT->setCursor(nPosX1, nPosY);
		poTFT->setTextSize(2);
		char outstr[15];
		
		//first overpaint old value
		#ifdef HAS_TFT_ST7735
		poTFT->setTextColor(ST7735_WHITE);
		#endif
		#ifdef HAS_TFT_QDTech
		poTFT->setTextColor(QDTech_WHITE);
		#endif
		//-22.98
		dtostrf(nPOP_Old,3, 0, outstr);
		poTFT->print(outstr);
		poTFT->print(" %");
		
		//then paint the new value
		poTFT->setCursor(nPosX1, nPosY);
		#ifdef HAS_TFT_ST7735
		poTFT->setTextColor(ST7735_DARKBLUE);
		#endif
		#ifdef HAS_TFT_QDTech
		poTFT->setTextColor(QDTech_BLUE);
		#endif
		//-22.98
		dtostrf(nPOP,3, 0, outstr);
		poTFT->print(outstr);
		poTFT->print(" %");
		nPOP_Old=nPOP;
	}
}
*/

void cTFT::DrawPoP()
{
	//int nPosX1=WEATHER_POP_X;
	//int nPosY=WEATHER_POP_Y;
	
	if(nPOP>-99 && nPOP!=nPOP_Old)
	{
		float fPoP= (nPOP_Old-0)*MAXHEIGHT_POP/(100-0);
		int height = (int) fPoP;
		int x=WEATHER_POP_X;
		poTFT->fillRect(x,WEATHER_POP_Y-height,10,height,ST7735_WHITE);

		fPoP= (nPOP-0)*MAXHEIGHT_POP/(100-0);
		height = (int) fPoP;
		poTFT->fillRect(x,WEATHER_POP_Y-height,10,height,ST7735_DARKBLUE);
		
		//cSerial::println("Pop " + String(x,DEC) +" " + String(height));
		nPOP_Old=nPOP;
	}
}

void cTFT::DrawRain()
{
	//int nPosX1=WEATHER_POP_X;
	//int nPosY=WEATHER_POP_Y;
	
	if(nForecastRain>-99 && nForecastRain!=nForecastRain_Old)
	{
		float fPoP= (nForecastRain_Old-0)*MAXHEIGHT_POP/(10-0);
		int height = (int) fPoP;
		int x=WEATHER_POP_X;
		poTFT->fillRect(x,WEATHER_POP_Y-height,10,height,ST7735_WHITE);

		fPoP= (nForecastRain-0)*MAXHEIGHT_POP/(10-0);
		height = (int) fPoP;
		poTFT->fillRect(x,WEATHER_POP_Y-height,10,height,ST7735_DARKBLUE);
		
		//cSerial::println("Pop " + String(x,DEC) +" " + String(height));
		nForecastRain_Old=nForecastRain;
	}
}


#endif

void cTFT::DrawTemp(int idx)
{
	int nPosX1=1+64*idx;
	int nPosY=TEMP_Y;
		
	if(fTemp[idx]>-99.9 && fTemp[idx]!=fTemp_Old[idx])
	{
		poTFT->setCursor(nPosX1, nPosY-10);
		poTFT->setTextSize(1);
		poTFT->setTextColor(ST7735_BLACK);
		if (idx==0)
			poTFT->print("out");
		else
			poTFT->print("in");
			
		poTFT->setCursor(nPosX1, nPosY);
		poTFT->setTextSize(2);
		char outstr[15];
			
		//first overpaint old value
#ifdef HAS_TFT_ST7735
		poTFT->setTextColor(ST7735_WHITE);
#endif
#ifdef HAS_TFT_QDTech
		poTFT->setTextColor(QDTech_WHITE);
#endif
		//-22.98
		dtostrf(fTemp_Old[idx],3, 0, outstr);
		poTFT->print(outstr);
		poTFT->print(" C");
			
		//then paint the new value
		poTFT->setCursor(nPosX1, nPosY);
#ifdef HAS_TFT_ST7735
		if (fTemp[idx]<0.0)
		{
			poTFT->setTextColor(ST7735_DARKBLUE);
		}
		else if (fTemp[idx]<10.0)
		{
			poTFT->setTextColor(ST7735_BLUE);
		}
		else if (fTemp[idx]>25.0)
		{
			poTFT->setTextColor(ST7735_RED);
		}
		else
		{
			poTFT->setTextColor(ST7735_GREEN);
		}
#endif
#ifdef HAS_TFT_QDTech
		poTFT->setTextColor(QDTech_BLUE);
#endif
		//-22.98
		dtostrf(fTemp[idx],3, 0, outstr);
		poTFT->print(outstr);
		poTFT->print(" C");
		fTemp_Old[idx]=fTemp[idx];
	}
}

void cTFT::DrawTempForecast()
{
	int nPosX1=TEMPFORECAST_X;
	int nPosY=TEMPFORECAST_Y;
	
	if(fForecastTemp>-99.9 && fForecastTemp!=fForecastTemp_Old)
	{
		poTFT->setCursor(nPosX1, nPosY-10);
		poTFT->setTextSize(1);
		poTFT->setTextColor(ST7735_BLACK);
		
		poTFT->print("max");
		
		poTFT->setCursor(nPosX1, nPosY);
		poTFT->setTextSize(2);
		char outstr[15];
		
		//first overpaint old value
		#ifdef HAS_TFT_ST7735
		poTFT->setTextColor(ST7735_WHITE);
		#endif
		#ifdef HAS_TFT_QDTech
		poTFT->setTextColor(QDTech_WHITE);
		#endif
		//-22.98
		dtostrf(fForecastTemp_Old,2, 0, outstr);
		poTFT->print(outstr);
		poTFT->print(" C");
		
		//then paint the new value
		poTFT->setCursor(nPosX1, nPosY);
		#ifdef HAS_TFT_ST7735
		if (fForecastTemp<0.0)
		{
			poTFT->setTextColor(ST7735_DARKBLUE);
		}
		else if (fForecastTemp<10.0)
		{
			poTFT->setTextColor(ST7735_BLUE);
		}
		else if (fForecastTemp>25.0)
		{
			poTFT->setTextColor(ST7735_RED);
		}
		else
		{
			poTFT->setTextColor(ST7735_GREEN);
		}
		#endif
		#ifdef HAS_TFT_QDTech
		poTFT->setTextColor(QDTech_BLUE);
		#endif
		//-22.98
		dtostrf(fForecastTemp,2, 0, outstr);
		poTFT->print(outstr);
		poTFT->print(" C");
		fForecastTemp_Old=fForecastTemp;
	}
}


void cTFT::DrawPressure(int idx)
{
#ifdef DISPLAY_PRESSURE
	int nPosX1=1+64*idx;
	int nPosY=TEMP_Y+20;
	
	if(fPressure[idx]>-99.9 && fPressure[idx]!=fPressure_Old[idx])
	{
		poTFT->setCursor(nPosX1, nPosY);
		poTFT->setTextSize(1);
		char outstr[15];
		
		//first overpaint old value
		#ifdef HAS_TFT_ST7735
		poTFT->setTextColor(ST7735_WHITE);
		#endif
		#ifdef HAS_TFT_QDTech
		poTFT->setTextColor(QDTech_WHITE);
		#endif
		//-22.98
		dtostrf(fPressure_Old[idx],3, 0, outstr);
		poTFT->print(outstr);
		poTFT->print(" mBar");
		
		//then paint the new value
		poTFT->setCursor(nPosX1, nPosY);
		#ifdef HAS_TFT_ST7735
		poTFT->setTextColor(ST7735_DARKBLUE);
		#endif
		#ifdef HAS_TFT_QDTech
		poTFT->setTextColor(QDTech_BLUE);
		#endif
		//-22.98
		dtostrf(fPressure[idx],3, 0, outstr);
		poTFT->print(outstr);
		poTFT->print(" mBar");
		fPressure_Old[idx]=fPressure[idx];
	}	

#endif
}

// ====================== ab hier Testfunktionen ====================================================
/*void cTFT::testlines(uint16_t color) {
	poTFT->fillScreen(ST7735_BLACK);
	for (int16_t x=0; x < poTFT->width(); x+=6) {
		poTFT->drawLine(0, 0, x, poTFT->height()-1, color);
	}
	for (int16_t y=0; y < poTFT->height(); y+=6) {
		poTFT->drawLine(0, 0, poTFT->width()-1, y, color);
	}

	poTFT->fillScreen(ST7735_BLACK);
	for (int16_t x=0; x < poTFT->width(); x+=6) {
		poTFT->drawLine(poTFT->width()-1, 0, x, poTFT->height()-1, color);
	}
	for (int16_t y=0; y < poTFT->height(); y+=6) {
		poTFT->drawLine(poTFT->width()-1, 0, 0, y, color);
	}

	poTFT->fillScreen(ST7735_BLACK);
	for (int16_t x=0; x < poTFT->width(); x+=6) {
		poTFT->drawLine(0, poTFT->height()-1, x, 0, color);
	}
	for (int16_t y=0; y < poTFT->height(); y+=6) {
		poTFT->drawLine(0, poTFT->height()-1, poTFT->width()-1, y, color);
	}

	poTFT->fillScreen(ST7735_BLACK);
	for (int16_t x=0; x < poTFT->width(); x+=6) {
		poTFT->drawLine(poTFT->width()-1, poTFT->height()-1, x, 0, color);
	}
	for (int16_t y=0; y < poTFT->height(); y+=6) {
		poTFT->drawLine(poTFT->width()-1, poTFT->height()-1, 0, y, color);
	}
}

void cTFT::testdrawtext(char *text, uint16_t color) {
	poTFT->setCursor(0, 0);
	poTFT->setTextColor(color);
	poTFT->setTextWrap(true);
	poTFT->print(text);
}

void cTFT::testfastlines(uint16_t color1, uint16_t color2) {
	poTFT->fillScreen(ST7735_BLACK);
	for (int16_t y=0; y < poTFT->height(); y+=5) {
		poTFT->drawFastHLine(0, y, poTFT->width(), color1);
	}
	for (int16_t x=0; x < poTFT->width(); x+=5) {
		poTFT->drawFastVLine(x, 0, poTFT->height(), color2);
	}
}

void cTFT::testdrawrects(uint16_t color) {
	poTFT->fillScreen(ST7735_BLACK);
	for (int16_t x=0; x < poTFT->width(); x+=6) {
		poTFT->drawRect(poTFT->width()/2 -x/2, poTFT->height()/2 -x/2 , x, x, color);
	}
}

void cTFT::testfillrects(uint16_t color1, uint16_t color2) {
	poTFT->fillScreen(ST7735_BLACK);
	for (int16_t x=poTFT->width()-1; x > 6; x-=6) {
		poTFT->fillRect(poTFT->width()/2 -x/2, poTFT->height()/2 -x/2 , x, x, color1);
		poTFT->drawRect(poTFT->width()/2 -x/2, poTFT->height()/2 -x/2 , x, x, color2);
	}
}

void cTFT::testfillcircles(uint8_t radius, uint16_t color) {
	for (int16_t x=radius; x < poTFT->width(); x+=radius*2) {
		for (int16_t y=radius; y < poTFT->height(); y+=radius*2) {
			poTFT->fillCircle(x, y, radius, color);
		}
	}
}

void cTFT::testdrawcircles(uint8_t radius, uint16_t color) {
	for (int16_t x=0; x < poTFT->width()+radius; x+=radius*2) {
		for (int16_t y=0; y < poTFT->height()+radius; y+=radius*2) {
			poTFT->drawCircle(x, y, radius, color);
		}
	}
}

void cTFT::testtriangles() {
	poTFT->fillScreen(ST7735_BLACK);
	int color = 0xF800;
	int t;
	int w = poTFT->width()/2;
	int x = poTFT->height()-1;
	int y = 0;
	int z = poTFT->width();
	for(t = 0 ; t <= 15; t+=1) {
		poTFT->drawTriangle(w, y, y, x, z, x, color);
		x-=4;
		y+=4;
		z-=4;
		color+=100;
	}
}

void cTFT::testroundrects() {
	poTFT->fillScreen(ST7735_BLACK);
	int color = 100;
	int i;
	int t;
	for(t = 0 ; t <= 4; t+=1) {
		int x = 0;
		int y = 0;
		int w = poTFT->width()-2;
		int h = poTFT->height()-2;
		for(i = 0 ; i <= 16; i+=1) {
			poTFT->drawRoundRect(x, y, w, h, 5, color);
			x+=2;
			y+=3;
			w-=4;
			h-=6;
			color+=1100;
		}
		color+=100;
	}
}

void cTFT::tftPrintTest() {
	poTFT->setTextWrap(false);
	poTFT->fillScreen(ST7735_BLACK);
	poTFT->setCursor(0, 30);
	poTFT->setTextColor(ST7735_RED);
	poTFT->setTextSize(1);
	poTFT->println("Hello World!");
	poTFT->setTextColor(ST7735_YELLOW);
	poTFT->setTextSize(2);
	poTFT->println("Hello World!");
	poTFT->setTextColor(ST7735_GREEN);
	poTFT->setTextSize(3);
	poTFT->println("Hello World!");
	poTFT->setTextColor(ST7735_BLUE);
	poTFT->setTextSize(4);
	poTFT->print(1234.567);
	delay(1500);
	poTFT->setCursor(0, 0);
	poTFT->fillScreen(ST7735_BLACK);
	poTFT->setTextColor(ST7735_WHITE);
	poTFT->setTextSize(0);
	poTFT->println("Hello World!");
	poTFT->setTextSize(1);
	poTFT->setTextColor(ST7735_GREEN);
	poTFT->print(PI, 6);
	poTFT->println(" Want pi?");
	poTFT->println(" ");
	poTFT->print(8675309, HEX); // print 8,675,309 out in HEX!
	poTFT->println(" Print HEX!");
	poTFT->println(" ");
	poTFT->setTextColor(ST7735_WHITE);
	poTFT->println("Sketch has been");
	poTFT->println("running for: ");
	poTFT->setTextColor(ST7735_MAGENTA);
	poTFT->print(millis() / 1000);
	poTFT->setTextColor(ST7735_WHITE);
	poTFT->print(" seconds.");
}

void cTFT::mediabuttons() {
	// play
	poTFT->fillScreen(ST7735_BLACK);
	poTFT->fillRoundRect(25, 10, 78, 60, 8, ST7735_WHITE);
	poTFT->fillTriangle(42, 20, 42, 60, 90, 40, ST7735_RED);
	delay(500);
	// pause
	poTFT->fillRoundRect(25, 90, 78, 60, 8, ST7735_WHITE);
	poTFT->fillRoundRect(39, 98, 20, 45, 5, ST7735_GREEN);
	poTFT->fillRoundRect(69, 98, 20, 45, 5, ST7735_GREEN);
	delay(500);
	// play color
	poTFT->fillTriangle(42, 20, 42, 60, 90, 40, ST7735_BLUE);
	delay(50);
	// pause color
	poTFT->fillRoundRect(39, 98, 20, 45, 5, ST7735_RED);
	poTFT->fillRoundRect(69, 98, 20, 45, 5, ST7735_RED);
	// play color
	poTFT->fillTriangle(42, 20, 42, 60, 90, 40, ST7735_GREEN);
	
}
*/

void cTFT::clearscreen()
{
	poTFT->fillScreen(ST7735_WHITE);
}

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 20

bool cTFT::bmpDraw(int index, uint8_t x, uint8_t y) 
{
	String picturename;
	
	picturename ="/daswetter/galeria1/";
	
	picturename+=String(index);
	picturename+=".bmp";
	
	cSerial::println("File " + picturename);
	
	char buffer[31];
	picturename.toCharArray(buffer,30,0);
	return (bmpDraw(buffer, x, y));
}

bool cTFT::bmpDraw(char *filename, uint8_t x, uint8_t y) 
{
	
	cSerial::println("RAM " +  String (getFreeRam(),DEC));
	
#ifdef HAS_SDCARD
	SdFat sd;

	sd.cardBegin(PIN_CS_SDCARD, SPI_QUARTER_SPEED);
	
	if (!sd.begin(PIN_CS_SDCARD, SPI_QUARTER_SPEED)) 
	{
		cSerial::println("init sd failed");
		return false;
	}

	//sd.ls("/", LS_R);

//====================================
	// open file

	bmpFile=new SdFile(filename, O_READ);

	// check for open error
	if (!bmpFile->isOpen()) 
	{
		poTFT->setTextColor(ST7735_MAGENTA);
		poTFT->setCursor(x,y );
		poTFT->setTextSize(2);
		//to do
		//poTFT->println("e " + filename);

		poTFT->println("sd error");

		
		delete (bmpFile);
		cSerial::println("file not open.");
		return false;
	}
	
	cSerial::print("File ");
	cSerial::print(filename);
	cSerial::print(" size ");
	cSerial::println(bmpFile->fileSize());
	


	int      bmpWidth, bmpHeight;   // W+H in pixels
	int headersize;
	int filesize;
	uint8_t  bmpDepth;              // Bit depth (currently must be 24)
	uint32_t bmpImageoffset;        // Start of image data in file
	uint32_t rowSize;               // Not always = bmpWidth; may have padding
	uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
	uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
	boolean  goodBmp = false;       // Set to true on valid header parse
	boolean  flip    = true;        // BMP is stored bottom-to-top
	int      w, h, row, col;
	uint8_t  r, g, b;
	uint32_t  pos;
	uint32_t  startTime = millis();

	if((x >= poTFT->width()) || (y >= poTFT->height())) 
	{
		cSerial::print("position error ");
		delete (bmpFile);
		return false;
	}
	// Parse BMP header
	if(read16(bmpFile) == 0x4D42) { // BMP signature = 2 Byte
		
		filesize = read32(bmpFile); // file size == 4 Byte
		cSerial::print("File size: ");
		cSerial::println(filesize);
		
		(void)read32(bmpFile); // Read & ignore creator bytes = 4 Bytes
		
		bmpImageoffset = read32(bmpFile); // Start of image data = 4 Byte
		cSerial::print("Image Offset: "); 
		cSerial::println(bmpImageoffset);
		// Read DIB header

		headersize= read32(bmpFile); //header size = 4 byte
		cSerial::print("Header size: ");
		cSerial::println(headersize);
		
		bmpWidth  = read32(bmpFile); // bild breite = 4 byte
		cSerial::print("width ");
		cSerial::println(bmpWidth);
		
		bmpHeight = read32(bmpFile); // bild höhe = 4 Byte
		cSerial::print("height "); 
		cSerial::println(bmpHeight);
	
		if(read16(bmpFile) == 1) { // # planes -- must be '1' = 2 Byte
			bmpDepth = read16(bmpFile); // bits per pixel = 2 Byte
			cSerial::print("Bit Depth: "); 
			cSerial::println(bmpDepth);

			if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

				goodBmp = true; // Supported BMP format -- proceed!
				
				cSerial::println("BMP is good");
				
				// BMP rows are padded (if needed) to 4-byte boundary
				rowSize = (bmpWidth * 3 + 3) & ~3;

				// If bmpHeight is negative, image is in top-down order.
				// This is not canon but has been observed in the wild.
				if(bmpHeight < 0) {
					bmpHeight = -bmpHeight;
					flip      = false;
				}

				// Crop area to be loaded
				w = bmpWidth;
				h = bmpHeight;
				if((x+w-1) >= poTFT->width())  w = poTFT->width()  - x;
				if((y+h-1) >= poTFT->height()) h = poTFT->height() - y;

				// Set TFT address window to clipped image bounds
				
				//old version
				//poTFT->setAddrWindow(x, y, x+w-1, y+h-1);
				
				//new version
				poTFT->startWrite();
				poTFT->setAddrWindow(x, y, w, h);
				poTFT->endWrite();

				cSerial::print("set window "); 
				cSerial::print(x); cSerial::print(" "); 
				cSerial::print(y); cSerial::print(" "); 
				cSerial::print(x+w-1); cSerial::print(" ");
				cSerial::print(y+h-1); cSerial::print(" ");
				cSerial::println();

				for (row=0; row<h; row++) { // For each scanline...

					// Seek to start of scan line.  It might seem labor-
					// intensive to be doing this on every line, but this
					// method covers a lot of gritty details like cropping
					// and scanline padding.  Also, the seek only takes
					// place if the file position actually needs to change
					// (avoids a lot of cluster math in SD library).
					
					if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
						pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
					else     // Bitmap is stored top-to-bottom
						pos = bmpImageoffset + row * rowSize;
				    if(bmpFile->curPosition() != pos) { // Need seek?
						bmpFile->seekSet(pos);
						buffidx = sizeof(sdbuffer); // Force buffer reload
					}
					
					for (col=0; col<w; col++) { // For each pixel...
						// Time to read more pixel data?
						if (buffidx >= sizeof(sdbuffer)) { // Indeed
							bmpFile->read(sdbuffer, sizeof(sdbuffer));
							buffidx = 0; // Set index to beginning
						}

						// Convert pixel from BMP to TFT format, push to display
						b = sdbuffer[buffidx++];
						g = sdbuffer[buffidx++];
						r = sdbuffer[buffidx++];
						
						//cSerial::print("col ");
						//cSerial::print(b,HEX); cSerial::print(" ");
						//cSerial::print(g, HEX); cSerial::print(" ");
						//cSerial::print(r, HEX);
						//cSerial::println();
						
						
						poTFT->pushColor(poTFT->color565(r,g,b));
					} // end pixel
				} // end scanline
				cSerial::print("Loaded in ");
				cSerial::print(millis() - startTime);
				cSerial::println(" ms");
				
				//new version
				poTFT->endWrite();
			} // end goodBmp
		}
	}

	bmpFile->close();
	
	delete (bmpFile);
	if(!goodBmp) cSerial::println("BMP format not recognized.");
	
	cSerial::println("RAM " +  String (getFreeRam(),DEC));
	
	return goodBmp;
#endif
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.
#ifdef HAS_SDCARD

uint16_t cTFT::read16(SdFile *f) {
	uint16_t result;
	uint8_t lsb;
	uint8_t msb;

	f->read(&lsb, 1);
	f->read(&msb, 1);

/*
	if (f->read(&lsb, 1)<0)
	{
		cSerial::println("error1");
	}
	if (f->read(&msb, 1)<0)
	{
		cSerial::println("error2");
	}
*/	
	((uint8_t *)&result)[0] = lsb; // LSB
	((uint8_t *)&result)[1] = msb; // MSB

/*	 cSerial::print(lsb,HEX); Serial.print(" ");
	 cSerial::print(msb,HEX); Serial.println(" ");

	 cSerial::print(" cur.Pos "); Serial.println(f->curPosition());
*/

	return result;
}

uint32_t cTFT::read32(SdFile *f) {
	uint32_t result;
	 uint8_t lsb1;
	 uint8_t msb1;
	 uint8_t lsb2;
	 uint8_t msb2;

	 f->read(&lsb1, 1);
	 f->read(&lsb2, 1);
	 f->read(&msb1, 1);
	 f->read(&msb2, 1);

/*
	 if (f->read(&lsb1, 1)<0)
	 {
		 cSerial::println("error3");
	 }
	 if (f->read(&lsb2, 1)<0)
	 {
		 cSerial::println("error4");
	 }
	 if (f->read(&msb1, 1)<0)
	 {
		 cSerial::println("error5");
	 }
	 if (f->read(&msb2, 1)<0)
	 {
		 cSerial::println("error6");
	 }
*/	 
	 ((uint8_t *)&result)[0] = lsb1; // LSB
	 ((uint8_t *)&result)[1] = lsb2; // LSB	 
	 ((uint8_t *)&result)[2] = msb1; // LSB	 
	 ((uint8_t *)&result)[3] = msb2; // LSB
	 
/*	 cSerial::print(lsb1,HEX); Serial.print(" ");
	 cSerial::print(lsb2,HEX); Serial.print(" ");
	 cSerial::print(msb1,HEX); Serial.print(" ");
	 cSerial::print(msb2,HEX); Serial.println(" ");
	 
	 cSerial::print(" cur.Pos "); Serial.println(f->curPosition());
	*/ 
	return result;
}




#endif
#endif