/* 
* DS2401.cpp
*
* Created: 02.06.2016 21:15:05
* Author: Rene
*/


#include "DS2401.h"

#include "../Debug/Debug.h"


// ------------------------------------------------------------------
// ------------------------------------------------------------------
// ------------------------------------------------------------------
// ------------------------------------------------------------------
cDS2401_List::cDS2401_List(unsigned char p) : OneWire(p)
{
	last_convert=0;
	count=0;
	int max_loops=255;
	while ((count<MAX_DS2401) && (OneWire::search(ds2401[count].addr) && (max_loops--)))
	{
		if ( OneWire::crc8( ds2401[count].addr, 7) != ds2401[count].addr[7])
		{
			cDebug::Log(cDebug::LOG_E,"000");
			continue; //Serial.print("CRC is not valid!\n");
		}
		
		if ( ds2401[count].addr[0] != 0x01)
		{
			cDebug::Log(cDebug::LOG_E,"111");
			continue; //Serial.print("Device is not a DS2401 family device.\n");
		}

		cDebug::Log(cDebug::LOG_E,"222");
		ds2401[count].id=(ds2401[count].addr[1]<<8)+ds2401[count].addr[7];
		count++;
	}
	OneWire::reset_search();

}
// ------------------------------------------------------------------
void cDS2401_List::update()
{
	unsigned long cur=millis(); // 32bit
	unsigned long elapsed=0;
	if (cur<last_convert)
	{
		elapsed=(0xffffffff-last_convert);
		elapsed+=cur;
	}
	else
	elapsed=cur-last_convert;
	
	if (elapsed<1000) return;

	read();
}

// ------------------------------------------------------------------
void cDS2401_List::read()
{
	for (int i=0;i<count;i++) read(i);
}

void cDS2401_List::read(unsigned char no)
{
}

// ------------------------------------------------------------------
unsigned int cDS2401_List::get_id(unsigned char no)
{
	if (no>=count) return 0;
	return ds2401[no].id;
}
// ------------------------------------------------------------------
