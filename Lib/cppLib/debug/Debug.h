/*
 * cDebug.h
 *
 * Created: 11.07.2015 13:51:14
 *  Author: Rene
 */ 


#ifndef DEBUG_H_
#define DEBUG_H_

#include <Arduino.h>

#include "../USBInterface/Serial.h"
#include "../../board.h"

extern "C"
{
	#include "../../clib/ringbuffer.h"
}
/*
class cDebug
{
public:
//	cDebug();
	cDebug(String name, bool enabled=true);
	~cDebug();
	
	
enum
{
	LOG_E = 0x01,
	LOG_W = 0x02,
	LOG_I = 0x04,
	LOG_D = 0x08,
};

public:
	static void Log(const String &sLogText);
	static void Log(int nLevel, const String &sLogText);
	static void Log(int nLevel, byte sLogData);
		
	void Task();
	void Begin();
//	void SetName(String name);
//	void SetEnabled(bool enabled);

	void SetLogLevel(int nLevel);

private:
	static String	sName;
	static bool	bLogEnabled;
	static int		nLogLevel;
};
*/


#endif /* DEBUG_H_ */
