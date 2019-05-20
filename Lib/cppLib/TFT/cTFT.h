/* 
* cTFT.h
*
* Created: 03.01.2017 17:10:05
* Author: Rene
*/


#ifndef __CTFT_H__
#define __CTFT_H__

#ifdef HAS_TFT

#ifdef HAS_TFT_ST7735
	#include <Adafruit_ST7735.h> // Hardware-specific library
#endif
#ifdef HAS_TFT_QDTech
	//#include <Adafruit_QDTech.h> // modifizierte Adafruit-Bibliothek
#endif


#ifdef HAS_SDCARD
	#include <SdFat.h>
#endif

// Color definitions
#define	ST7735_DARKBLUE  0x008B
#define	ST7735_AliceBlue  	0xF0F8FF 	  	
#define	ST7735_AntiqueWhite  	0xFAEBD7 	  	
#define	ST7735_Aqua  	0x00FFFF 	  	
#define	ST7735_Aquamarine  	0x7FFFD4 	  	
#define	ST7735_Azure  	0xF0FFFF 	  	
#define	ST7735_Beige  	0xF5F5DC 	  	
#define	ST7735_Bisque  	0xFFE4C4 	  	
#define	ST7735_Black  	0x000000 	  	
#define	ST7735_BlanchedAlmond  	0xFFEBCD 	  	
#define	ST7735_Blue  	0x0000FF 	  	
#define	ST7735_BlueViolet  	0x8A2BE2 	  	
#define	ST7735_Brown  	0xA52A2A 	  	
#define	ST7735_BurlyWood  	0xDEB887 	  	
#define	ST7735_CadetBlue  	0x5F9EA0 	  	
#define	ST7735_Chartreuse  	0x7FFF00 	  	
#define	ST7735_Chocolate  	0xD2691E 	  	
#define	ST7735_Coral  	0xFF7F50 	  	
#define	ST7735_CornflowerBlue  	0x6495ED 	  	
#define	ST7735_Cornsilk  	0xFFF8DC 	  	
#define	ST7735_Crimson  	0xDC143C 	  	
#define	ST7735_Cyan  	0x00FFFF 	  	
#define	ST7735_DarkBlue  	0x00008B 	  	
#define	ST7735_DarkCyan  	0x008B8B 	  	
#define	ST7735_DarkGoldenRod  	0xB8860B 	  	
#define	ST7735_DarkGray  	0xA9A9A9 	  	
#define	ST7735_DarkGrey  	0xA9A9A9 	  	
#define	ST7735_DarkGreen  	0x006400 	  	
#define	ST7735_DarkKhaki  	0xBDB76B 	  	
#define	ST7735_DarkMagenta  	0x8B008B 	  	
#define	ST7735_DarkOliveGreen  	0x556B2F 	  	
#define	ST7735_DarkOrange  	0xFF8C00 	  	
#define	ST7735_DarkOrchid  	0x9932CC 	  	
#define	ST7735_DarkRed  	0x8B0000 	  	
#define	ST7735_DarkSalmon  	0xE9967A 	  	
#define	ST7735_DarkSeaGreen  	0x8FBC8F 	  	
#define	ST7735_DarkSlateBlue  	0x483D8B 	  	
#define	ST7735_DarkSlateGray  	0x2F4F4F 	  	
#define	ST7735_DarkSlateGrey  	0x2F4F4F 	  	
#define	ST7735_DarkTurquoise  	0x00CED1 	  	
#define	ST7735_DarkViolet  	0x9400D3 	  	
#define	ST7735_DeepPink  	0xFF1493 	  	
#define	ST7735_DeepSkyBlue  	0x00BFFF 	  	
#define	ST7735_DimGray  	0x696969 	  	
#define	ST7735_DimGrey  	0x696969 	  	
#define	ST7735_DodgerBlue  	0x1E90FF 	  	
#define	ST7735_FireBrick  	0xB22222 	  	
#define	ST7735_FloralWhite  	0xFFFAF0 	  	
#define	ST7735_ForestGreen  	0x228B22 	  	
#define	ST7735_Fuchsia  	0xFF00FF 	  	
#define	ST7735_Gainsboro  	0xDCDCDC 	  	
#define	ST7735_GhostWhite  	0xF8F8FF 	  	
#define	ST7735_Gold  	0xFFD700 	  	
#define	ST7735_GoldenRod  	0xDAA520 	  	
#define	ST7735_Gray  	0x808080 	  	
#define	ST7735_Grey  	0x808080 	  	
#define	ST7735_Green  	0x008000 	  	
#define	ST7735_GreenYellow  	0xADFF2F 	  	
#define	ST7735_HoneyDew  	0xF0FFF0 	  	
#define	ST7735_HotPink  	0xFF69B4 	  	
#define	ST7735_IndianRed   	0xCD5C5C 	  	
#define	ST7735_Indigo   	0x4B0082 	  	
#define	ST7735_Ivory  	0xFFFFF0 	  	
#define	ST7735_Khaki  	0xF0E68C 	  	
#define	ST7735_Lavender  	0xE6E6FA 	  	
#define	ST7735_LavenderBlush  	0xFFF0F5 	  	
#define	ST7735_LawnGreen  	0x7CFC00 	  	
#define	ST7735_LemonChiffon  	0xFFFACD 	  	
#define	ST7735_LightBlue  	0xADD8E6 	  	
#define	ST7735_LightCoral  	0xF08080 	  	
#define	ST7735_LightCyan  	0xE0FFFF 	  	
#define	ST7735_LightGoldenRodYellow  	0xFAFAD2 	  	
#define	ST7735_LightGray  	0xD3D3D3 	  	
#define	ST7735_LightGrey  	0xD3D3D3 	  	
#define	ST7735_LightGreen  	0x90EE90 	  	
#define	ST7735_LightPink  	0xFFB6C1 	  	
#define	ST7735_LightSalmon  	0xFFA07A 	  	
#define	ST7735_LightSeaGreen  	0x20B2AA 	  	
#define	ST7735_LightSkyBlue  	0x87CEFA 	  	
#define	ST7735_LightSlateGray  	0x778899 	  	
#define	ST7735_LightSlateGrey  	0x778899 	  	
#define	ST7735_LightSteelBlue  	0xB0C4DE 	  	
#define	ST7735_LightYellow  	0xFFFFE0 	  	
#define	ST7735_Lime  	0x00FF00 	  	
#define	ST7735_LimeGreen  	0x32CD32 	  	
#define	ST7735_Linen  	0xFAF0E6 	  	
#define	ST7735_Magenta  	0xFF00FF 	  	
#define	ST7735_Maroon  	0x800000 	  	
#define	ST7735_MediumAquaMarine  	0x66CDAA 	  	
#define	ST7735_MediumBlue  	0x0000CD 	  	
#define	ST7735_MediumOrchid  	0xBA55D3 	  	
#define	ST7735_MediumPurple  	0x9370DB 	  	
#define	ST7735_MediumSeaGreen  	0x3CB371 	  	
#define	ST7735_MediumSlateBlue  	0x7B68EE 	  	
#define	ST7735_MediumSpringGreen  	0x00FA9A 	  	
#define	ST7735_MediumTurquoise  	0x48D1CC 	  	
#define	ST7735_MediumVioletRed  	0xC71585 	  	
#define	ST7735_MidnightBlue  	0x191970 	  	
#define	ST7735_MintCream  	0xF5FFFA 	  	
#define	ST7735_MistyRose  	0xFFE4E1 	  	
#define	ST7735_Moccasin  	0xFFE4B5 	  	
#define	ST7735_NavajoWhite  	0xFFDEAD 	  	
#define	ST7735_Navy  	0x000080 	  	
#define	ST7735_OldLace  	0xFDF5E6 	  	
#define	ST7735_Olive  	0x808000 	  	
#define	ST7735_OliveDrab  	0x6B8E23 	  	
#define	ST7735_Orange  	0xFFA500 	  	
#define	ST7735_OrangeRed  	0xFF4500 	  	
#define	ST7735_Orchid  	0xDA70D6 	  	
#define	ST7735_PaleGoldenRod  	0xEEE8AA 	  	
#define	ST7735_PaleGreen  	0x98FB98 	  	
#define	ST7735_PaleTurquoise  	0xAFEEEE 	  	
#define	ST7735_PaleVioletRed  	0xDB7093 	  	
#define	ST7735_PapayaWhip  	0xFFEFD5 	  	
#define	ST7735_PeachPuff  	0xFFDAB9 	  	
#define	ST7735_Peru  	0xCD853F 	  	
#define	ST7735_Pink  	0xFFC0CB 	  	
#define	ST7735_Plum  	0xDDA0DD 	  	
#define	ST7735_PowderBlue  	0xB0E0E6 	  	
#define	ST7735_Purple  	0x800080 	  	
#define	ST7735_RebeccaPurple  	0x663399 	  	
#define	ST7735_Red  	0xFF0000 	  	
#define	ST7735_RosyBrown  	0xBC8F8F 	  	
#define	ST7735_RoyalBlue  	0x4169E1 	  	
#define	ST7735_SaddleBrown  	0x8B4513 	  	
#define	ST7735_Salmon  	0xFA8072 	  	
#define	ST7735_SandyBrown  	0xF4A460 	  	
#define	ST7735_SeaGreen  	0x2E8B57 	  	
#define	ST7735_SeaShell  	0xFFF5EE 	  	
#define	ST7735_Sienna  	0xA0522D 	  	
#define	ST7735_Silver  	0xC0C0C0 	  	
#define	ST7735_SkyBlue  	0x87CEEB 	  	
#define	ST7735_SlateBlue  	0x6A5ACD 	  	
#define	ST7735_SlateGray  	0x708090 	  	
#define	ST7735_SlateGrey  	0x708090 	  	
#define	ST7735_Snow  	0xFFFAFA 	  	
#define	ST7735_SpringGreen  	0x00FF7F 	  	
#define	ST7735_SteelBlue  	0x4682B4 	  	
#define	ST7735_Tan  	0xD2B48C 	  	
#define	ST7735_Teal  	0x008080 	  	
#define	ST7735_Thistle  	0xD8BFD8 	  	
#define	ST7735_Tomato  	0xFF6347 	  	
#define	ST7735_Turquoise  	0x40E0D0 	  	
#define	ST7735_Violet  	0xEE82EE 	  	
#define	ST7735_Wheat  	0xF5DEB3 	  	
#define	ST7735_White  	0xFFFFFF 	  	
#define	ST7735_WhiteSmoke  	0xF5F5F5 	  	
#define	ST7735_Yellow  	0xFFFF00 	  	
#define	ST7735_YellowGreen  	0x9ACD32

class cTFT
{
//variables
public:
protected:
private:

//functions
public:
	cTFT();
	~cTFT();
	
	void setup(void);
	void loop();
	void loopTest();
	
	void SetTemperature(float temp, int idx);
	void SetPressure(float pressure, int idx);
	void SetTime(int hour, int minute, int second);
	void SetDate(int day, int month, int year);
	void SetWeatherIcon(byte icon_id);
	void SetHumidity(float humidity, int idx);
	void SetPoP(int pop);
	void SetTemperatureForecast(float temp);
	void SetRain(int rain);
	
	bool bmpDraw(char *filename, uint8_t x, uint8_t y);
	bool bmpDraw(int index, uint8_t x, uint8_t y);
	void clearscreen();

protected:
private:
	cTFT( const cTFT &c );
	cTFT& operator=( const cTFT &c );

#ifdef HAS_TFT_ST7735
	Adafruit_ST7735 *poTFT;
#endif
#ifdef HAS_TFT_QDTech
	Adafruit_QDTech *poTFT;
#endif
/*	
	void testlines(uint16_t color);
	void testdrawtext(char *text, uint16_t color);
	void testfastlines(uint16_t color1, uint16_t color2);
	void testdrawrects(uint16_t color);
	void testfillrects(uint16_t color1, uint16_t color2);
	void testfillcircles(uint8_t radius, uint16_t color);
	void testdrawcircles(uint8_t radius, uint16_t color);
	void testtriangles();
	void testroundrects();
	void tftPrintTest();
	void mediabuttons();
	*/
	void DrawClock();
	void DrawClockWatchHand();
	void DrawTemp(int idx);
	void DrawPressure(int idx);
	void DrawHumidity(int idx);
	void DrawPoP();
	void DrawRain();
	void DrawTempForecast();
	
/*
	uint32_t read32(File f);
	uint16_t read16(File f);
*/
	
	float fTemp[2]; //temperature out to display
	int nHour;
	int nMinute;
	int nSeconds;
	int nDay;
	int nMonth;
	int nYear;
		
	int nHour_Old;
	int nMinute_Old;
	int nSeconds_Old;
	float fTemp_Old[2];
	int nDay_Old;
	int nMonth_Old;
	int nYear_Old;		
	uint16_t nTime_Clock;
	//uint16_t nTime_Temp;

#ifdef DISPLAY_PRESSURE
	float fPressure[2];
	float fPressure_Old[2];
#endif	
#ifdef DISPLAY_HUMIDITY
	float fHumidity[2];
	float fHumidity_Old[2];
#endif
#ifdef DISPLAY_POP
	int nPOP;
	int nPOP_Old;
#endif

	byte nWeatherIconId;
	byte nWeatherIconId_Old;
	
	float fForecastTemp;
	float fForecastTemp_Old;
	
	int nForecastRain;
	int nForecastRain_Old;

#ifdef HAS_SDCARD
	uint16_t read16(SdFile *f);
	uint32_t read32(SdFile *f);
	SdFile *bmpFile;
#endif
	
}; //cTFT

#endif //Has_TFT

#endif //__CTFT_H__
