/* 
* cFerrarisCounter.h
*
* Created: 19.12.2015 19:53:28
* Author: Rene
*/


#ifndef __CFERRARISCOUNTER_H__
#define __CFERRARISCOUNTER_H__





class cFerrarisCounter
{
//variables
public:
	static int nCounterIRQ0;
	static int nCounterIRQ1;
protected:
	int nIRQ;
private:
	int nCounter;
	int nLastCheckCounter;

//functions
public:
	cFerrarisCounter(int nIrq);
	~cFerrarisCounter();
	
	int GetData(); //just returns the counter
	void Reset(); //reset counter
	void Check();
	
protected:

private:
	cFerrarisCounter( const cFerrarisCounter &c );
	cFerrarisCounter& operator=( const cFerrarisCounter &c );

}; //cFerrarisCounter



#endif //__CFERRARISCOUNTER_H__
