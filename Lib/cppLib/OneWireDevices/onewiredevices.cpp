/* 
* OnWireDevices.cpp
*
* Created: 05.06.2016 21:21:08
* Author: Rene
*/


#include "OneWireDevices.h"
#include "../Debug/Debug.h"

#ifdef HasOneWireDevices

// default constructor
cOneWireDevices::cOneWireDevices(unsigned char pin) : OneWire(pin)
{
	nDeviceCount=0;
#ifndef ATMEGA644
	lookUpSensors(); //geht am Display nicht???
#endif
} //OnWireDevices

// default destructor
cOneWireDevices::~cOneWireDevices()
{
	
} //~OnWireDevices

void cOneWireDevices::SearchDevices()
{
	int max_loops=255;

	//DeviceDate liste aktualisieren...
	nDeviceCount=0;
	while ( (OneWire::search(OneWireDeviceData[nDeviceCount].addr) && (max_loops--)))
	{
		if ( OneWire::crc8( OneWireDeviceData[nDeviceCount].addr, 7) != OneWireDeviceData[nDeviceCount].addr[7])
		{
			//cDebug::Log(cDebug::LOG_E,"CRC error");
			continue; //Serial.print("CRC is not valid!\n");
		}
			
		String sTemp="";
		switch (OneWireDeviceData[nDeviceCount].addr[0])
		{
			case 0x01:
			{
				sTemp="DS2401";
				//cDebug::Log(cDebug::LOG_E,sTemp);
				cSerial::println(sTemp);
			}
			break;
			case 0x10:
			{
				sTemp="DS18S20";
				//cDebug::Log(cDebug::LOG_E,sTemp);
				cSerial::println(sTemp);
				
				//start convert for next read
				OneWire::reset();
				OneWire::select(OneWireDeviceData[nDeviceCount].addr);
				OneWire::write(0x44,0);
			}
			break;
			default:
			{
				//sTemp="device 0x" + String(OneWireDeviceData[nDeviceCount].addr[0],HEX);
				//cDebug::Log(cDebug::LOG_E,sTemp);
				//cSerial::println(sTemp);
			}
			break;
		}
		
		//just print serial number 
/*		for (int n=1; n<7;n++)
		{
			cSerial::print(" ");
			cSerial::print(String(OneWireDeviceData[nDeviceCount].addr[n],HEX));
			
		}
		cSerial::println();
	*/	
		OneWireDeviceData[nDeviceCount].id=(OneWireDeviceData[nDeviceCount].addr[1]<<8)+OneWireDeviceData[nDeviceCount].addr[7];
		nDeviceCount++;
	}
	
	if (nDeviceCount==0)
	{
		cSerial::println("no device");
	}

	OneWire::reset_search();
}

void cOneWireDevices::ReadDevices()
{
	for (int no=0;no<nDeviceCount;no++)
	{
		int present = OneWire::reset();
		
		if (present)
		{

			
			switch (OneWireDeviceData[no].addr[0])
			{
				case 0x01:
				{
					//OneWire::select(OneWireDeviceData[no].addr);
					//String sTemp;
					//OneWire::write(0x33);         // read ROM
					//unsigned char *d=OneWireDeviceData[no].data;
					
					//d[0] = OneWire::read();
					//sTemp="family 0x"+ String(d[0],HEX) + " serial";
					//for ( int i=0;i<8;i++)  // read all 8 bytes from ROM (1Byte family code; 6 Byte serial number; 1 Byte CRC)
					//{
					//	OneWireDeviceData[no].data[i] = OneWire::read();
					//	//sTemp+=" 0x" + String(d[i],HEX);
					//}
					
					//d[7] = OneWire::read();
					//sTemp+=" CRC 0x"+ String(d[7],HEX);
					
					//cDebug::Log(cDebug::LOG_E,sTemp);
					//cSerial::println(sTemp);
				}
				break;
				case 0x10:
				{
					
					OneWire::reset();
					OneWire::select(OneWireDeviceData[no].addr);
					
					OneWire::write(0xBE);         // read scratchpad
					
					for ( int i=0;i<9;i++)  // read all 9 bytes from scratchpad
					{
						OneWireDeviceData[no].data[i] = OneWire::read();
						// to do print
					}
					
					    if( OneWireDeviceData[no].data[1] == 0 )
					    {
						    temp = (int) OneWireDeviceData[no].data[0] >> 1;
						} 
						else 
						{
						    temp = -1 * (int) (0x100-OneWireDeviceData[no].data[0]) >> 1;
					    }
					    temp -= 0.25;
/*
					    // improve resolution of temperature
					    float hi_precision = (int) OneWireDeviceData[no].data[7] - (int) OneWireDeviceData[no].data[6];
					    hi_precision = hi_precision / (int) OneWireDeviceData[no].data[7];
					    temp += hi_precision;
*/				
						//start convert for next read
						 OneWire::reset();
						 OneWire::select(OneWireDeviceData[no].addr);
						 OneWire::write(0x44,0);

					//just to save memory
				}
				break;
				default:
				{
					//String sTemp="unknown device";
					//cDebug::Log(cDebug::LOG_E,sTemp);
					//cSerial::println(sTemp);
				}
				break;
			}
		}
	}
}

float cOneWireDevices::GetTemp()
{
	return temp;
}

void cOneWireDevices::GetID(int *nId, int nSize)
{
	if (nDeviceCount==0)
	{
		for (int n=0;n<6;n++)
		{
			nId[n]=0;
		}
	
		//String sTemp="no device";
		//cDebug::Log(cDebug::LOG_E,sTemp);	
	}
	else
	{	
		char bFound= 0;
		for (int no=0;no<nDeviceCount;no++)
		{
			
			//das ist ein DS2401; aber was, wenn nur DS18x20 vorhanden?
			if (OneWireDeviceData[no].addr[0]==0x01)
			{
				bFound=1;
				for (int i=0;i<6;i++)
				{
					nId[i]=OneWireDeviceData[no].addr[i+1];
				}
				
				break;
			}
		}
		
		if (!bFound)
		{
			for (int i=0;i<6;i++)
			{
				nId[i]=OneWireDeviceData[0].addr[i+1];
			}
		}
		
	}
	return;
}


void cOneWireDevices::lookUpSensors()
{
	byte address[8];
	int i=0;
	byte ok = 0, tmp = 0;
	
	cSerial::println("--Suche gestartet--");
	while (OneWire::search(address))
	{
		tmp = 0;
		//0x10 = DS18S20
		if (address[0] == 0x10)
		{
			cSerial::print("Device is a DS18S20 : ");
			tmp = 1;
		}
		else
		{
			//0x28 = DS18B20
			if (address[0] == 0x28)
			{
				cSerial::print("Device is a DS18B20 : ");
				tmp = 1;
			}
		}
		//display the address, if tmp is ok
		if (tmp == 1)
		{
			if (OneWire::crc8(address, 7) != address[7])
			{
				cSerial::println("but it doesn't have a valid CRC!");
			}
			else
			{
				//all is ok, display it
				for (i=0;i<8;i++)
				{
					if (address[i] < 9)
					{
						cSerial::print("0");
					}
					cSerial::print("0x");
					cSerial::print(address[i],HEX);
					if (i<7)
					{
						cSerial::print(", ");
					}
				}
				cSerial::println("");
				ok = 1;
			}
		}//end if tmp
	}//end while
	if (ok == 0)
	{
		cSerial::println("Keine Sensoren gefunden");
	}
	cSerial::println("--Suche beendet--");
}

#endif