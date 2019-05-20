/* 
* cFerrarisCounter.cpp
*
* Created: 19.12.2015 19:53:28
* Author: Rene
*/

#ifdef HasFerrarisCounter


#include "FerrarisCounter.h"
#include "../../board.h"
#include "../Debug/Debug.h"

int cFerrarisCounter::nCounterIRQ0=0;
int cFerrarisCounter::nCounterIRQ1=0;

void cFerrarisSignalsInterrupt0(void)
{
	// set the flag that a package is available
	cFerrarisCounter::nCounterIRQ0++;
}
void cFerrarisSignalsInterrupt1(void)
{
	// set the flag that a package is available
	cFerrarisCounter::nCounterIRQ1++;
}

// default constructor
cFerrarisCounter::cFerrarisCounter(int nIrq)
{

	pinMode(PIN_FerrarisIn1, INPUT);
	pinMode(PIN_FerrarisIn2, INPUT);
			
	nIRQ=nIrq;
	nLastCheckCounter=0;
	nCounter=0;
	if (nIRQ==0)
	{
		nCounterIRQ0=0;
		attachInterrupt(nIRQ, cFerrarisSignalsInterrupt0, FALLING);
	}
	else if (nIRQ==1)
	{
		nCounterIRQ1=0;
		attachInterrupt(nIRQ, cFerrarisSignalsInterrupt1, FALLING);
	}
	else
	{
		cDebug::Log(cDebug::LOG_E,"wrong IRQ");
	}

} //cFerrarisCounter

// default destructor
cFerrarisCounter::~cFerrarisCounter()
{
} //~cFerrarisCounter

int cFerrarisCounter::GetData()
{
	return nCounter;
}

void cFerrarisCounter::Reset()
{
	nCounter=0;
	nLastCheckCounter=0;
}

void cFerrarisCounter::Check()
{
	if (nIRQ==0)
	{
		if (nCounterIRQ0>0)	
		{
			nCounter++;
			nCounterIRQ0=0;
		}
	}
	else if (nIRQ==1)
	{
		if (nCounterIRQ1>0)
		{
			nCounter++;
			nCounterIRQ1=0;
		}
	}	
	if (nCounter>nLastCheckCounter)
	{
		String sTemp;
		sTemp = String(nCounter,DEC);
		cDebug::Log(cDebug::LOG_D,sTemp);
		nLastCheckCounter=nCounter;
	}
}
#endif