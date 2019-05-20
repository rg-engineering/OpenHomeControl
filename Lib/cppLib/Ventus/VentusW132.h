/* 
* cVentusW132.h
*
* Created: 03.09.2017 19:19:55
* Author: Rene
*/


#ifndef __VENTUSW132_H__
#define __VENTUSW132_H__


class cVentusW132
{
//variables
public:
protected:
private:

//functions
public:
	cVentusW132();
	~cVentusW132();

	void Do();
	static unsigned int nIRQCnt;
	static unsigned char nRXState;
	static unsigned char nBitCnt;
	static unsigned char nPacketCnt;
	//static unsigned char nHighCnt;
	//static unsigned char nLowCnt;
	
	float GetTemperature();
	int GetHumidity();
	float GetAvgWindSpeed();
	float GetWindGust();
	int GetWindDir();
	
protected:
private:
	cVentusW132( const cVentusW132 &c );
	cVentusW132& operator=( const cVentusW132 &c );

	float fTemperature;
	int nHumidity;
	float fAvgWindSpeed;
	float fWindGust;
	int nWindDir;
	

	void AttachIRQ();
	void DetachIRQ();
	void EvaluatePackets();

	unsigned int nOldIRQCnt;
	unsigned int nOldRXState;
	
	int oldLevel;

}; //cVentusW132

#endif //__VENTUSW132_H__
