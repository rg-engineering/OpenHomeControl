/* 
* cAnalyse2.h
*
* Created: 21.09.2017 19:56:15
* Author: Rene
*/


#ifndef __CANALYSE2_H__
#define __CANALYSE2_H__


class cAnalyse2
{
//variables
public:
protected:
private:

//functions
public:
	cAnalyse2();
	~cAnalyse2();
protected:
private:
	cAnalyse2( const cAnalyse2 &c );
	cAnalyse2& operator=( const cAnalyse2 &c );

}; //cAnalyse2

void RX_WirelessSensors();
byte RCode();
void RX_Test();

#endif //__CANALYSE2_H__
