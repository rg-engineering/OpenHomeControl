/* 
* cAnalyse2.cpp
*
* Created: 21.09.2017 19:56:15
* Author: Rene
*/

#include <stdint.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <Arduino.h>
#include "../../board.h"
#include "Analyse2.h"
#include "../Debug/Debug.h"

// default constructor
cAnalyse2::cAnalyse2()
{
} //cAnalyse2

// default destructor
cAnalyse2::~cAnalyse2()
{
} //~cAnalyse2

#define TH_OutUnit 3
#define WIND_OutUnit 3

#define PINH PIN_VENTUS

#define Wind_Display_Unit 0    // 0 for Km/h, 1 for mph, 2 for m/s, 3 for Knots

// --------------------------------------------------------------------------------------
//   BIOS soil moisture sensor settings
// --------------------------------------------------------------------------------------
// ---- Please use the included BIOS_ID_Scanner sketch to find you BIOS Device ID
#define BiosSensor_ID 0   // Device ID in HEX format, or 0 not used

//---------------------------------------------------------
//  Important for Wireless Sensors data receiving - NEVER TOUCH THIS
//---------------------------------------------------------
#if (TH_OutUnit == 3 || WIND_OutUnit == 3 || RAIN_OutUnit == 3 || BiosSensor_ID)
#define   nobits 36
#define   smin   7500
#define   smax   9999
#define   semin  250
#define   semax  750
#define   lmin   1700
#define   lmax   2300
#define   hmin   3550
#define   hmax   4400
#endif

// -------------------------------------------------------------------
//      Data structure for TX units
// -------------------------------------------------------------------
typedef struct
{
	int16_t  TemperaturaExt[2];
	uint16_t HumidadeExt[2];
	uint32_t Wind_average;
	uint32_t Wind_gust;
	uint16_t Wind_dir;
	uint16_t Rainftipshour;
	uint16_t TotalRain_tips;
	uint16_t SolarRad;
	uint16_t UV;
	uint16_t BatVolt;
	int16_t  SysTemp;
	byte     RSfan;
	uint16_t PacketsSent24h;
	uint16_t AQImonitor[5];
} TX_Units;

TX_Units TX_Unit[4];

// -------------------------------------------------------------------
// LOOP data packet length and structure.
// See http://www.davisnet.com/support/weather/download/VantageSerialProtocolDocs_v230.pdf
// The CRC is not included in here but calculated on the fly.
// WARNING: Some of the items below may not be implemented!!!
// -------------------------------------------------------------------

struct __attribute__((packed)) LoopPacket
{
	char loo[3];                // "LOO" at packet start indicates Rev B packet type
	byte barTrend;              // Barometric trend
	byte packetType;            // Packet type, always zero
	uint16_t nextRecord;        // Location in archive memory where next packet will be written
	uint16_t barometer;         // Current barometer in Hg / 1000
	int16_t insideTemperature;  // Inside temperature in tenths of degrees
	byte insideHumidity;        // Inside relative humidity in percent
	int16_t outsideTemperature; // Outside temperature in tenths of degrees

	byte windGust;              // Wind gust in miles per hour
	byte tenMinAvgWindSpeed;    // Average wind speed over last ten minutes

	uint16_t windDirection;     // Wind direction from 1 to 360 degrees (0 = no wind data)
	byte extraTemperatures[7];  // Temps from seven extra stations in whole degrees F offset by 90
	byte soilTemperatures[4];   // Soil temps from four extra sensors.  Format as above.
	byte leafTemperatures[4];   // Leaf temps from four extra sensors.  Format as above.
	byte outsideHumidity;       // Outside relative humidity in %.
	byte extraHumidities[7];    // Relative humidity in % from seven extra stations.
	uint16_t rainRate;          // Rain rate as number of rain clicks per hour (e.g 256 = 2.56 in/hr)
	byte uV;                    // UV index
	uint16_t solarRadiation;    // Solar radiation in Watts/m^2
	uint16_t stormRain;         // Storm rain stored as hundredths of an inch
	uint16_t startDateOfStorm;  // Bits 15-12 is month, bits 11-7 is day, and bits 6-0 is year offset by 2000
	uint16_t dayRain;           // Rain today sent as number of rain clicks (0.2mm or 0.01in)
	uint16_t monthRain;         // Rain this month sent as number of rain clicks (0.2mm or 0.01in)
	uint16_t yearRain;          // Rain this year sent as number of rain clicks (0.2mm or 0.01in)
	uint16_t dayET;             // ET today sent as thousandths of an inch
	uint16_t monthET;           // ET this month sent as hundredths of an inch
	uint16_t yearET;            // ET this year sent as hundredths of an inch
	byte soilMoistures[4];      // Soil moisture in centibars from up to four soil sensors
	byte leafWetnesses[4];      // A scale number from 0-15. 0 means very dry, 15 very wet. Supports four leaf sensors.
	byte insideAlarms;          // Currently active inside alarms.
	byte rainAlarms;            // Currently active rain alarms.
	uint16_t outsideAlarms;     // Currently active outside alarms.
	byte extraTempHumAlarms[8]; // Currently active temperature / humidity alarms for up to eight stations.
	byte soilLeafAlarms[4];     // Currently active soil and leaf alarms for up to four sensors
	byte transmitterBatteryStatus;  // Transmitter battery status (0 or 1)
	uint16_t consoleBatteryVoltage; // Console voltage as  ((Data * 300)/512)/100.0
	byte forecastIcons;             // Forecast icons
	byte forecastRuleNumber;        // Forecast rule number
	uint16_t timeOfSunrise;         // Sunrise time stored as hour * 100 + min.
	uint16_t timeOfSunset;          // Sunset time stored as hour * 100 + min.
	char lfcr[2];                   // Carriage return / line feed
};

// -------------------------------------------------------------------
// LOOP2 data packet length and structure.
// See http://www.davisnet.com/support/weather/download/VantageSerialProtocolDocs_v230.pdf
// The CRC is not included in here but calculated on the fly.
// WARNING: Some of the items below may not be implemented!!!
// -------------------------------------------------------------------

struct __attribute__((packed)) Loop2Packet
{
	char loo[3];                // "LOO" at packet start indicates Rev B packet type
	byte barTrend;              // Barometric trend
	byte packetType;            // Packet type, always one (LOOP2 packet)
	uint16_t Unused_1;          // Unused field, filled with 0x7FFF
	uint16_t barometer;         // Current barometer in Hg / 1000
	int16_t insideTemperature;  // Inside temperature in tenths of degrees
	byte insideHumidity;        // Inside relative humidity in percent
	int16_t outsideTemperature; // Outside temperature in tenths of degrees
	byte windGust;              // Wind gust in miles per hour
	byte Unused_2;              // Unused field, filled with 0xFF
	uint16_t windDirection;     // Wind direction from 1 to 360 degrees (0 = no wind data)
	uint16_t tenMinAvgWindSpeed;   // Average wind speed over last ten minutes (in 0.1mph resolution.)
	uint16_t twoMinAvgWindSpeed;   // Average wind speed over last two minutes (in 0.1mph resolution.)
	uint16_t tenMinGustAvg;        // Average wind gust over last ten minutes (in 0.1mph resolution.)
	uint16_t tenMinwindDirection;  // Wind Direction for the 10-Min Wind Guest in 1 to 360 degrees (0 = no wind data)
	uint16_t Unused_3;          // Unused field, filled with 0x7FFF
	uint16_t Unused_4;          // Unused field, filled with 0x7FFF
	int16_t dewPoint;           // The value is a signed two byte value in whole degrees F. (255 = dashed data)
	byte Unused_5;              // Unused field, filled with 0xFF
	byte outsideHumidity;       // Outside relative humidity in %.
	byte Unused_6;              // Unused field, filled with 0xFF
	int16_t heatIndex;
	int16_t windChill;
	int16_t thswIndex;
	uint16_t rainRate;          // Rain rate as number of rain clicks per hour (e.g 256 = 2.56 in/hr)
	byte uV;                    // UV index
	uint16_t solarRadiation;    // Solar radiation in Watts/m^2
	uint16_t stormRain;         // Storm rain stored as hundredths of an inch
	uint16_t startDateOfStorm;  // Bits 15-12 is month, bits 11-7 is day, and bits 6-0 is year offset by 2000
	uint16_t dayRain;           // Rain today sent as number of rain clicks (0.2mm or 0.01in)
	uint16_t fifteenMinRain;
	uint16_t lastHourRain;      // This value is sent as number of rain clicks.
	uint16_t dayET;             // ET today sent as thousandths of an inch
	uint16_t last24HoursRain;   // This value is sent as number of rain clicks.
	byte barReduMethod;         // Bar reduction method: 0 - user offset 1- Altimeter Setting 2- NOAA Bar Reduction. For VP2, this will always be 2.
	uint16_t userBarOffset;     // Barometer calibration number in 1000th of an inch
	uint16_t barCalibNumber;    // Calibration offset in 1000th of an inch
	uint16_t barometerRaw;      // In 1000th of an inch
	uint16_t absBarPressure;    // In 1000th of an inch, equals to the raw sensor reading plus user entered offset
	uint16_t altimSetting;      // In 1000th of an inch
	byte Unused_7;              // Unused field, filled with 0xFF
	byte Unused_8;              // Undefined

	byte next10MinWindSpGP;
	byte next15MinWindSpGP;
	byte nextHourlyWindSpGP;
	byte nextDailyWindSpGP;
	byte nextMinRainGP;
	byte nextRainStromGP;
	byte indexMinInHour;
	byte nextMontlyRain;
	byte nextYearlyRain;
	byte nextSeasonalRain;
	uint16_t Unused_9;           // Unused field, filled with 0x7FFF
	uint16_t Unused_10;          // Unused field, filled with 0x7FFF
	uint16_t Unused_11;          // Unused field, filled with 0x7FFF
	uint16_t Unused_12;          // Unused field, filled with 0x7FFF
	uint16_t Unused_13;          // Unused field, filled with 0x7FFF
	uint16_t Unused_14;          // Unused field, filled with 0x7FFF
	//byte lineFeed;
	//byte CarriageReturn;
	char lfcr[2];                   // Carriage return / line feed
};


volatile uint32_t now_millis;
uint32_t Wireless_Data;
uint16_t Auriol_rainftipshour;
LoopPacket loopData;
Loop2Packet loop2Data;  // LOOP2
int16_t Output_T_Int, Output_T_Out;
int16_t Output_H_Int, Output_H_Out;
uint16_t Dew_Point_F;
byte MainSensorsRX    = 0;
float Output_WSpeed, Output_WGust, Output_RToday;
const float mph_To_WindDisplayUnit[4] = {1.609344, 1.0, 0.44704, 0.868976};
const float ms_To_WindDisplayUnit[4]  = {3.6, 2.23694, 1.0, 1.94384};
byte Wireless_SensorID      = 0;

#if (TH_OutUnit == 3 || WIND_OutUnit == 3 || RAIN_OutUnit == 3 || BiosSensor_ID != 0)

void RX_Test()
{
	for (int i=0; i<32;i++)
	{
		bitWrite(Wireless_Data, 32 - i, 1);
		
		cSerial::println(String(Wireless_Data,BIN));
	}
}


// -------------------------------------------------------------------
//      Receive Wireless Sensors (Auriol Rain Gauge, Bios Soil Temp / Moisture)
// -------------------------------------------------------------------
void RX_WirelessSensors()
{
	Wireless_Data = 0;
	switch (RCode())
	{
		case 0:
		return;

		#if (RAIN_OutUnit == 3)
		case 1:
		{
			uint16_t AuriolTR_tips = 0;
			static uint16_t last_AuriolTR_tips;
			static uint32_t last_millis_0;

			for (byte i = 0; i < 16 ; i++)
			if (bitRead(Wireless_Data, 23 - i) == 1) AuriolTR_tips += 1 << i;

			uint16_t elapsed = (millis() - last_millis_0) / 1000;

			if (AuriolTR_tips != last_AuriolTR_tips)
			{
				if (elapsed >= 3600) Auriol_rainftipshour = 1;
				else Auriol_rainftipshour = (uint16_t)((((AuriolTR_tips - last_AuriolTR_tips) * 3600) / elapsed ));

				if (last_AuriolTR_tips == 0) Auriol_rainftipshour = 0;   // Prevents high Rainfail rates on restarts
				last_millis_0 = millis();
			}

			if (((AuriolTR_tips == last_AuriolTR_tips) || (last_AuriolTR_tips == 0)) && (elapsed >= 60)) Auriol_rainftipshour = (uint16_t)(Auriol_rainftipshour / 3.0); // Progressive decresse of rainfall when rain stops
			last_AuriolTR_tips = AuriolTR_tips;

			TX_Unit[3].Rainftipshour   = Auriol_rainftipshour;
			TX_Unit[3].TotalRain_tips  = AuriolTR_tips;
			bitWrite(MainSensorsRX, 2, 1);

			#ifdef PRINT_DEBUG_AURIOL
			Serial.print(now_millis); Serial.print(" A TR_tips :"); Serial.println(TX_Unit[3].TotalRain_tips);
			#endif

			#if (Relay_Data == 1)
			sendData(Relay_ID, 133, 250, Auriol_rainftipshour, RainToday_tips, DaysWithoutRain );
			#endif
			break;
		}
		#endif


		#if (TH_OutUnit == 3)
		case 2:
		{
			cSerial::println(String(Wireless_Data,BIN));
			/*
			101110111100000000101001100000
			temp 247 hum 500
			101110111100000000101001100000
			temp 247 hum 500
			101110111100000000101001100000
			temp 247 hum 500
			
			
			*/
			
			int AuriolOutTemp = 0;
			int AuriolOutHum  = 0;
			byte Hum1 = 0;
			byte Hum2 = 0;

			//Serial.print("Wireless_SensorID : "); Serial.println(Wireless_SensorID, HEX);

			for (byte i = 0; i < 12 ; i++)
			if (bitRead(Wireless_Data, 27 - i) == 1) AuriolOutTemp += 1 << i;

			// Calculate negative temperature
			if ((AuriolOutTemp & 0x800) == 0x800) AuriolOutTemp = AuriolOutTemp | 0xF000;

			for (byte i = 0; i < 4 ; i++)
			{
				if (bitRead(Wireless_Data, 11 - i) == 1) Hum1 += 1 << i; // tenths
				if (bitRead(Wireless_Data, 15 - i) == 1) Hum2 += 1 << i; // ones
			}

			AuriolOutHum = ((Hum1 * 10) + Hum2) * 10;

			TX_Unit[3].TemperaturaExt[0] = AuriolOutTemp;
			TX_Unit[3].HumidadeExt[0]    = AuriolOutHum;
			loopData.outsideTemperature = (AuriolOutTemp * 1.8) + 320;    // Convert ºC to ºF
			loopData.outsideHumidity    = (AuriolOutHum + 5) * 0.1;       // Round the reading

			#if (Temp_Display_Unit == 0)
			Output_T_Out = AuriolOutTemp;
			#else
			Output_T_Out = loopData.outsideTemperature;
			#endif
			Output_H_Out = AuriolOutHum;

			float Dew_Point = (Output_T_Out / 10.0) - (( 100 - (Output_H_Out / 10.0)) / 5);  // Simpler calculation good for RH above 50%
			Dew_Point_F = (int)((Dew_Point * 1.8) + 32.5);  // Convert to Fahrenheit (Rounded Integer value)

			bitWrite(MainSensorsRX, 0, 1);
			#if (Relay_Data == 1)
			sendData(Relay_ID, 130, Output_T_Int, Output_H_Int, Output_T_Out, Output_H_Out);
			#endif
			
			cSerial::println("temp " + String(TX_Unit[3].TemperaturaExt[0],DEC) + " hum " +String(TX_Unit[3].HumidadeExt[0] ,DEC));

			break;
		}
		#endif


		#if (WIND_OutUnit == 3)
		case 3:
		{
			cSerial::println(String(Wireless_Data,BIN));
			uint16_t AuriolWindAvg     = 0;

			for (byte i = 0; i < 8 ; i++)
			if (bitRead(Wireless_Data, 15 - i) == 1) AuriolWindAvg += 1 << i;
			TX_Unit[3].Wind_average   = AuriolWindAvg * 200;

			if (bitRead(Wireless_Data, 31) == 0)            // Batterie Status Bit
			{
				loopData.transmitterBatteryStatus = 0;        // Good baterie
				loopData.consoleBatteryVoltage    = 522;
			}
			else
			{
				loopData.transmitterBatteryStatus = 1;        // Weak Batterie < 2.6V
				loopData.consoleBatteryVoltage    = 0;
			}
			
			cSerial::println("wind average " + String(TX_Unit[3].Wind_average,DEC));
			break;
		}

		case 4:
		{
			cSerial::println(String(Wireless_Data,BIN));
			uint16_t AuriolWindDir  = 0;
			uint16_t AuriolWindGust = 0;
			static uint16_t lastgust;

			for (byte i = 0; i < 9 ; i++)
			if (bitRead(Wireless_Data, 21 - i) == 1) AuriolWindDir += 1 << i;
			for (byte i = 0; i < 8 ; i++)
			if (bitRead(Wireless_Data, 12 - i) == 1) AuriolWindGust += 1 << i;

			//Serial.print("AuriolWindGust0 : "); Serial.println((AuriolWindGust / 5.0) * 3.6, 1);

			// Wind gust spike removal
			if (AuriolWindGust >= 250) AuriolWindGust = lastgust;  // Some Auriol wind sensor, when have near strikes tends to report false 51m/s wind gusts
			lastgust = AuriolWindGust;

			TX_Unit[3].Wind_dir  = AuriolWindDir * 10;
			TX_Unit[3].Wind_gust = AuriolWindGust * 200;              // gust in m/s

			#if (Work_Mode == 0)

			#if (VP2_WindRes == 0)
			loopData.windGust            = TX_Unit[3].Wind_gust * 0.002237;                                   // m/s * 1000 converted to mhp
			loop2Data.windGust           = loopData.windGust;

			loopData.tenMinAvgWindSpeed  = round(TX_Unit[3].Wind_average * 0.002237);                        // m/s * 1000 converted to mhp
			loop2Data.tenMinAvgWindSpeed = round(TX_Unit[3].Wind_average * 0.02237);                         // m/s * 1000 converted to (0.1)mhp
			loop2Data.twoMinAvgWindSpeed = loop2Data.tenMinAvgWindSpeed;
			loop2Data.tenMinGustAvg      = loop2Data.tenMinAvgWindSpeed;

			Output_WGust  = loopData.windGust * mph_To_WindDisplayUnit[Wind_Display_Unit];
			Output_WSpeed = loopData.tenMinAvgWindSpeed * mph_To_WindDisplayUnit[Wind_Display_Unit];
			#else
			loopData.windGust            = TX_Unit[3].Wind_gust / (100.0 * VP2_WindRes);                      // value is m/s *10 or *20 depending on VP2_WindRes (Cumulus multiplier 0.224 or 0.448)
			loop2Data.windGust           = loopData.windGust;

			loopData.tenMinAvgWindSpeed = TX_Unit[3].Wind_average / (100.0 * VP2_WindRes);                    // value is m/s *10 or *20 depending on VP2_WindRes (Cumulus multiplier 0.224 or 0.448)
			loop2Data.tenMinAvgWindSpeed = round(TX_Unit[3].Wind_average / (10.0 * VP2_WindRes));
			loop2Data.twoMinAvgWindSpeed = loop2Data.tenMinAvgWindSpeed;
			loop2Data.tenMinGustAvg      = loop2Data.tenMinAvgWindSpeed;

			Output_WGust  = (loopData.windGust / 10.0) * VP2_WindRes * ms_To_WindDisplayUnit[Wind_Display_Unit];
			Output_WSpeed = (loopData.tenMinAvgWindSpeed / 10.0) * VP2_WindRes * ms_To_WindDisplayUnit[Wind_Display_Unit];
			#endif

			loopData.windDirection      = TX_Unit[3].Wind_dir / 10;
			#else
			Output_WSpeed = (TX_Unit[3].Wind_average / 1000.0) * ms_To_WindDisplayUnit[Wind_Display_Unit];  // Wind Speed in m/s * 1000 converted to Km/h
			Output_WGust  = (TX_Unit[3].Wind_gust / 1000.0) * ms_To_WindDisplayUnit[Wind_Display_Unit];     // Wind Gust in m/s * 1000 converted to Km/h
			#endif

			bitWrite(MainSensorsRX, 1, 1);

			#if (Relay_Data == 1)
			sendData(Relay_ID, 132, (long)(Output_WSpeed * 1000), (long)(Output_WGust * 1000), TX_Unit[WIND_OutUnit].Wind_dir * 10, 0);
			#endif
			
			cSerial::println("wind dir " + String(TX_Unit[3].Wind_dir,DEC) + " wind gust " +String(TX_Unit[3].Wind_gust,DEC));
			break;
		}
		#endif

		#if (BiosSensor_ID != 0)
		case 7:
		{
			BIOS_Temp      = 0;
			BIOS_SoilMoist = 0;
			
			for (byte i = 0; i < 12 ; i++)
			if (bitRead(Wireless_Data, 16 + i ) == 1) bitWrite(BIOS_Temp, i, 1);
			
			// Calculate negative temperature
			if ((BIOS_Temp & 0x800) == 0x800) BIOS_Temp = BIOS_Temp | 0xF000;
			
			for (byte i = 0; i < 8 ; i++)
			if (bitRead(Wireless_Data, 8 + i) == 1) bitWrite(BIOS_SoilMoist, i, 1);
			
			#ifdef PRINT_DEBUG_BIOS
			Serial.print(F("Bios Temp  :")); Serial.println(BIOS_Temp / 10.0, 1);
			Serial.print(F("Bios SoilW :")); Serial.println(BIOS_SoilMoist);
			#endif
			
			for (byte i = 0; i < 7; i++)
			{
				if (i < 4)
				{
					if (SoilTemp_Source[i][0] == 7)
					{
						Soil_Data[i][0] = BIOS_Temp;
						loopData.soilTemperatures[i] = (Soil_Data[i][0] * 0.18)  + 122.5;
					}
					if (SoilMoist_Source[i][0] == 7)
					{
						Soil_Data[i][1] = constrain(map(BIOS_SoilMoist, 0, 170, 0, 100), 0, 100);   // Output is volumetric water content (0 to 100%)
						loopData.soilMoistures[i] = Soil_Data[i][1];
					}
				}

				if (ExtraS_Source[i][0] == 7)
				{
					ExtraS_Data[i][0] = BIOS_Temp;  // Temperature Data
					loopData.extraTemperatures [i] = (Soil_Data[i][0] * 0.18)  + 122.5;

					#if (Relay_Data == 1)
					sendData(Relay_ID, 140 + i, ExtraS_Data[i][0], 0, 0, 0);
					#endif
				}
			}
		}
		#endif

	} // End Switch Case
}


// --------------------------------------------------------------------------------------
//   Decode data from wireless sensors and return type of data (Temp, Wind, Rain, etc...
// --------------------------------------------------------------------------------------
byte RCode()
{
	//String code = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
	String code = "xxxxxxxx";

	byte lastbit_RX = 7;
	long startMicros = micros(), endMicros;
	static uint32_t last_BIOSreceived;
	
	//cSerial::print(".");
	if (digitalRead(PIN_VENTUS)) return 0;
	//cSerial::println("a");
	while (!digitalRead(PIN_VENTUS))
	{
		if ((micros() - startMicros) > smax)
		return 0;
	}
	
	if ((micros() - startMicros) < smin)
	return 0;
	startMicros = micros();
	while (digitalRead(PIN_VENTUS))
	{
		if ((micros() - startMicros) > semax)
		return 0;
	}
	if ((micros() - startMicros) < semin)
	return 0;
	
	//cSerial::println("b");
	for (byte i = 0; i < nobits; i++)
	{
		startMicros = micros();
		while (!digitalRead(PIN_VENTUS))
		{
			if ((micros() - startMicros) < 1);
		}
		endMicros = micros();
		if (((endMicros - startMicros) > lmin) && ((endMicros - startMicros) < lmax))
		{
			if (i > 7)
			{
				if (lastbit_RX + 1 != i) return 0;
				bitWrite(Wireless_Data, 39 - i, 0);
				lastbit_RX = i;
			}
			else
			{
				code.setCharAt(i, '0');
				bitWrite(Wireless_SensorID, 7 - i, 0);
			}
		}
		else if (((endMicros - startMicros) > hmin) && ((endMicros - startMicros) < hmax))
		{
			if (i > 7)
			{
				if (lastbit_RX + 1 != i) return 0;
				bitWrite(Wireless_Data, 39 - i, 1);
				lastbit_RX = i;
			}
			else
			{
				code.setCharAt(i, '1');
				bitWrite(Wireless_SensorID, 7 - i, 1);
			}
		}
		startMicros = micros();
		while (digitalRead(PIN_VENTUS))
		{
			if ((micros() - startMicros) > semax)
			return 0;
		}
		if ((micros() - startMicros) < semin)
		return 0;
	}

	#if (RAIN_OutUnit == 3)
	if (Wireless_SensorID != BiosSensor_ID && bitRead(Wireless_Data, 30) == 1 && bitRead(Wireless_Data, 29) == 1 && bitRead(Wireless_Data, 28) == 0 && bitRead(Wireless_Data, 27) == 1 && bitRead(Wireless_Data, 26) == 1 && bitRead(Wireless_Data, 25) == 0 && bitRead(Wireless_Data, 24) == 0)
	return 1;  // Rain packet
	#endif

	#if (TH_OutUnit == 3)
	bool AuriolTempPacket = true;
	for (byte j = 0; j < 3; j++)   // Check Auriol Temp Packet validity
	if (code.charAt(j) == 'x')
	{
		AuriolTempPacket = false;  // Return false if invalid Packet
		break;
	}
	if ( (Wireless_SensorID != BiosSensor_ID) && (AuriolTempPacket == true) && ( (bitRead(Wireless_Data, 30) == 0 && bitRead(Wireless_Data, 29) == 0) || (bitRead(Wireless_Data, 30) == 1 && bitRead(Wireless_Data, 29) == 0) || (bitRead(Wireless_Data, 30) == 0 && bitRead(Wireless_Data, 29) == 1) ))
	return 2;  // Temp / Hum packet
	#endif

	#if (WIND_OutUnit == 3)
	if (Wireless_SensorID != BiosSensor_ID && bitRead(Wireless_Data, 30) == 1 && bitRead(Wireless_Data, 29) == 1 && bitRead(Wireless_Data, 27) == 1 && bitRead(Wireless_Data, 26) == 0 && bitRead(Wireless_Data, 25) == 0)
	return 3;  //  Wind Average / Bat state packet
	else if (Wireless_SensorID != BiosSensor_ID && bitRead(Wireless_Data, 27) == 1 && bitRead(Wireless_Data, 26) == 1 && bitRead(Wireless_Data, 24) == 1 && bitRead(Wireless_Data, 23) == 1 && bitRead(Wireless_Data, 22) == 1)
	return 4;  // Wind Dir / Gust packet
	#endif

	#if (BiosSensor_ID != 0)
	if (Wireless_SensorID == BiosSensor_ID && ((now_millis - last_BIOSreceived) > 10000ul))
	{
		last_BIOSreceived = now_millis;
		return 7;  // BIOS Soil Temp / Moisture packet
	}
	#endif

	return 0;
}

#endif
