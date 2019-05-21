/*
 * cDebug.cpp
 *
 * Created: 11.07.2015 13:51:02
 *  Author: Rene
 */ 

#include "Debug.h"

/*
String	cDebug::sName;
bool	cDebug::bLogEnabled;
int		cDebug::nLogLevel;

cDebug::cDebug(String name, bool enabled)
{
	sName=name;
#ifdef DEBUG
	bLogEnabled=enabled;
#else
	bLogEnabled=false;
#endif
	rb_reset(&Debug_Tx_Buffer);
}

void cDebug::Begin()
{
#ifdef DEBUG
	while (!cSerial::isListening())
	{
		; // wait for serial port to connect. Needed for Leonardo only
	}
#endif
}

cDebug::~cDebug()
{

}

void cDebug::Log(const String &sLogText)
{
#ifdef DEBUG
	if (bLogEnabled )
	{
			if (sLogText.length()>50)
			{
				cSerial::print(sLogText.substring(0,50));
				cSerial::print(sLogText.substring(50));
			}
			else
			{
				cSerial::print(sLogText);
			}
	}
#endif
}
*/

//void cDebug::Log(int nLevel, const String &sLogText)
//{
//#ifdef DEBUG
//	if (bLogEnabled )
//	{
/*		String sLogLevelText="";
		if ( (nLevel & LOG_D) && (nLogLevel & LOG_D))
		{
			sLogLevelText = "D: ";
		}
		else if ( (nLevel & LOG_I) && (nLogLevel & LOG_I))
		{
			sLogLevelText = "I: ";
		}
		else if ( (nLevel & LOG_W) && (nLogLevel & LOG_W))
		{
			sLogLevelText = "W: ";
		}
		else if ( (nLevel & LOG_E) && (nLogLevel & LOG_E))
		{
			sLogLevelText = "E: ";
		}
*/
//		if (sLogLevelText.length()>0)
//		{
			
			//cSerial::println(sLogLevelText + sName + ": " +sLogText);

//			if (sLogText.length()>50)
//			{
//				cSerial::print(sLogText.substring(0,50));
//				cSerial::print(sLogText.substring(51));
//				cSerial::println();

//			}
//			else
//			{
//				cSerial::println(sLogText);
//			}
			
//		}
//data from Sensor=3FAF82820000 as broadcast with 1 DP  Bright    1.00 lux
//	}
//#endif
//}
/*
void cDebug::Log(int nLevel, byte sLogData)
{
	if (bLogEnabled )
	{
		String sLogLevelText="";
		if ( (nLevel & LOG_D) && (nLogLevel & LOG_D))
		{
			sLogLevelText = "D: ";
		}
		else if ( (nLevel & LOG_I) && (nLogLevel & LOG_I))
		{
			sLogLevelText = "I: ";
		}
		else if ( (nLevel & LOG_W) && (nLogLevel & LOG_W))
		{
			sLogLevelText = "W: ";
		}
		else if ( (nLevel & LOG_E) && (nLogLevel & LOG_E))
		{
			sLogLevelText = "E: ";
		}
		if (sLogLevelText.length()>0)
		{
			cSerial::print(sLogData);
		}
	}
}


void cDebug::SetLogLevel(int nLevel)
{
#ifdef DEBUG
	// prüfen fehlt noch
	nLogLevel=nLevel;
#endif
}




//logs aus c-Umgebung
void cDebug::Task()
{	
#ifdef DEBUG
	if (Debug_Tx_Buffer.nbytes>0)
	{
		//Serial.println(Debug_Tx_Buffer.nbytes);
		String sText="";
		for (int i=0;i<Debug_Tx_Buffer.nbytes;i++)
		{
			char a = rb_get(&Debug_Tx_Buffer);
			sText+=a;
		}

		cSerial::println(sText);
		rb_reset(&Debug_Tx_Buffer);
	}
#endif
}
*/