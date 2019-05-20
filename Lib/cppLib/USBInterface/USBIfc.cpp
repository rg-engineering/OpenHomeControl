/* 
* cUSBIfc.cpp
*
* Created: 05.10.2015 19:50:37
* Author: Rene
*/

#include <avr/power.h>
#include <avr/sleep.h>
#include "../../board.h"
#include "../help/text_de.h"
#include "USBIfc.h"
#include "../Debug/Debug.h"




bool cUSBIfc::bPacketAvailable=false;
bool cUSBIfc::bTimer=false;

int nIRQCnt=0;

#define MAX_IRQ_FOR_SLEEP 50

ISR(TIMER1_OVF_vect)
{
	
	nIRQCnt++;
	
	if (nIRQCnt>MAX_IRQ_FOR_SLEEP) 
	{
		// set the flag. 
		if(cUSBIfc::bTimer == false)
		{
			cUSBIfc::bTimer = true;
		}
		
	}

		
	
}

	
// Handle interrupt from CC1101 
void cc1101signalsInterrupt(void)
{
	// set the flag that a package is available
	cUSBIfc::bPacketAvailable = true;
}

// default constructor
cUSBIfc::cUSBIfc()
{
	
	//poDebug->Log(cDebug::LOG_D,"constr. entry");
		
	bEnabled=true;
	nOldIRQCounter=0;
	bSensorMode=false;
	//nNextData=0;
#ifdef HasCC1101
	nPacketsGood=0;
	nPacketsCRSFailure=0;
	nPacketsReceived=0;
#endif
	bPacketAvailable=false;
	bToggle=false;
#ifdef HasBMP180
	fTemp1=-99;
#endif
#ifdef HasDHT22
	fTemp2=-99;
	fHumidity=0.0;
#endif

#ifdef IsZentrale
	nMode4PrintOut=0;
	cSerial::println("raw mode");
#else
	nMode4PrintOut=1;
	cSerial::println("telegram mode");
#endif		
	ResetData();
	
#ifdef HasCC1101
	poCC1101=NULL;
	poCC1101=new cCC1101_Main();
#endif

	//pinMode(PIN_LED,OUTPUT);
	//nLED=1;
	//digitalWrite(PIN_LED, HIGH);

#ifdef HasBH1750
	poBH1750=NULL;
//on Leonardo and on SmartMeter it's not connected yet...
	poBH1750=new cBH1750(0);
#endif
	poFerrarisCounter0=NULL;
	poFerrarisCounter1=NULL;
#ifdef HasFerrarisCounter
	cDebug::Log(cDebug::LOG_I,"with FerrarisCounter");
	poFerrarisCounter0= new cFerrarisCounter(0);
#ifndef HasCC1101
	poFerrarisCounter1= new cFerrarisCounter(1);
#endif
#endif
	
#ifdef HasBMP180
	poBMP180=NULL;
	poBMP180=new cBMP180();
#endif
	poBME280=NULL;
#ifdef HasBME280
	poBME280=new cBME280();
#endif
	
#ifdef HasDHT22
	poDHT22=NULL;
	poDHT22=new cDHT22(PIN_DHT22);
#endif	


//do never use both together
#ifdef HasOneWireDevices
	poOneWireDevices=NULL;
	poOneWireDevices=new cOneWireDevices(PIN_ONEWIRE);
	poOneWireDevices->SearchDevices();


	poOneWireDevices->ReadDevices();

	int nSize=6;
	int nId[nSize];
	poOneWireDevices->GetID(nId,nSize);

	for (int n=0; n<nSize;n++)
	{
		cSerial::print(String(nId[n],HEX));
	}
	cSerial::println();

#endif



#ifdef HAS_TFT_ST7735
//	poDisplay=NULL;
//	poDisplay=new cTFT();
//	poDisplay->setup();
//	poDisplay->loop();
#endif

#ifdef HasVentusW132
	poVentusW132=new cVentusW132();
	//poAnalyseProtokoll = new CAnalyseProtokoll();
	//poAnalyseProtokoll->setup();
#endif

	//poDebug->Log(cDebug::LOG_D,"constr. exit");
		

} //cUSBIfc

// default destructor
cUSBIfc::~cUSBIfc()
{
#ifdef HasCC1101
	if (poCC1101!=NULL) delete poCC1101;
#endif
#ifdef HasBH1750
	if (poBH1750!=NULL) delete poBH1750;
#endif
	if (poFerrarisCounter0!=NULL) delete poFerrarisCounter0;
	if (poFerrarisCounter1!=NULL) delete poFerrarisCounter1;
#ifdef HasBMP180
	if (poBMP180!=NULL) delete poBMP180;
#endif
#ifdef HasDHT22
	if (poDHT22!=NULL) delete poDHT22;
#endif

#ifdef HasVentusW132
	//if (poVentusW132!=NULL) delete poVentusW132;
#endif

} //~cUSBIfc

void cUSBIfc::Init()
{
	//cDebug::Log(cDebug::LOG_D,"cUSBIfc init");
	ResetData();
	
	RF_Init();
	
	RF_SendData();
	
#ifndef IsZentrale	
//#ifndef DEBUG
	StartSensorMode();
//#endif
#endif

	//RF_SendTestData();
	
	//RX_Test();
}

#ifndef IsZentrale
void cUSBIfc::Do()
{
	if (bEnabled)
	{
		if (cSerial::available() > 0)
		{
			//cDebug::Log("D","cUSBIfc got byte");
	
			char inByte = cSerial::read();
	
			String sTemp1 = String(inByte,HEX);
			sTemp1+=" ";
			cSerial::print(sTemp1);
	
			if (inByte==0x0D  ) //-> jedes 0x0D bricht empfang ab... z.Bsp. 13.03.
			//wenn enter [\n\r], dann los
			//und so geht terraterm nicht mehr..
			//if (inByte==0x0D && inData[inDataCnt-1]==0x0A)
			{
				//cSerial::println(inData[0]);
				String sTemp="";
				switch (inData[0])
				{
					default:
						sTemp = "unknown command 0x";
						sTemp += String(inData[0],HEX);
						sTemp+=" (length=";
						sTemp += String(inDataCnt,DEC);
						sTemp+="):";
			
						for (int m=0;m<inDataCnt;m++)
						{
							sTemp += " ";
							sTemp += String(inData[m],HEX);
						}
			
						cSerial::println(sTemp);
						break;
					case 'v':
					case 'V': //print version
						sTemp = sVERSION;
						#ifdef ARDUINO_NANO
						sTemp += " for Nano";
						#endif

						#ifdef ARDUINO_LEONARDO
						sTemp += " for Leonardo";
						#endif
						cSerial::println(sTemp);
						break;
			//					case 'w': //license data
			//						sTemp = COPYRIGHT;
			//						cSerial::println(sTemp);
			//						break;
			//					case 'c': //redistribute SW details
			//						sTemp = GNU_LICENSE;
			//						cSerial::println(sTemp);
			//						break;
					case 'i':
						Init();
						break;
				#ifdef HasFerrarisCounter
					case 'F':
						switch(inData[1])
						{
							case '1':
							{
								if (poFerrarisCounter0!=NULL)
								{
									int nTemp=poFerrarisCounter0->GetData();
									sTemp="ctr1="+ String(nTemp,DEC);
									cSerial::println(sTemp);
								}
							}
							break;
							case '2':
							{
								if (poFerrarisCounter1!=NULL)
								{
									int nTemp=poFerrarisCounter1->GetData();
									sTemp="ctr2="+ String(nTemp,DEC);
									cSerial::println(sTemp);
								}
							}
							break;
				
							case 'r':
								if (poFerrarisCounter0!=NULL)
								{
									poFerrarisCounter0->Reset();
								}
								if (poFerrarisCounter1!=NULL)
								{
									poFerrarisCounter1->Reset();
								}
							break;
							default:
							{
								if (poFerrarisCounter0!=NULL)
								{
									int nTemp=poFerrarisCounter0->GetData();
									sTemp="ctr0="+ String(nTemp,DEC);
								}
								if (poFerrarisCounter1!=NULL)
								{
									int nTemp=poFerrarisCounter1->GetData();
									sTemp=sTemp + " ctr1="+ String(nTemp,DEC);
								}
								cSerial::println(sTemp);
								//cSerial::println(sTemp);
							}
							break;
						}
					break;
				#endif
				#ifdef HAS_INTERTECHNO
					#ifdef HasCC1101
					case 'I':
						DetachIRQ(); //IRQ handeled in cc1101.cpp
						if (poCC1101!=NULL)
						{
							poCC1101->setITRegs();
						}
					break;
					#endif
				#endif
					case 'a': //start sensor mode
						StartSensorMode();
					break;
			
				#ifdef HasCC1101
					case 'm':
						switch(inData[1])
						{
							default:
							case 'd': //default
								poCC1101->setDefaultRegs();
								//poCC1101->disableAddressCheck();
								//poCC1101->setTxPowerAmp(PA_LongDistance);
								//poCC1101->setTxPowerAmp(PA_LowPower);
							break;
							case 'c': //cul
								//poCC1101->setCULRegs();
								//poCC1101->disableAddressCheck();
								//poCC1101->setTxPowerAmp(PA_LongDistance);
								//poCC1101->setTxPowerAmp(PA_LowPower);
							break;
							case 'r': //raw data for print out
								nMode4PrintOut=0;
								cSerial::println("raw mode");
							break;
							case 'i': //interprete data for print out
								nMode4PrintOut=1;
								cSerial::println("telegram mode");
							break;
						}
					break;

					if (poCC1101!=NULL)
					{
						poCC1101->reset();
					}
				break;
			#endif
			#ifdef HasCC1101
				case 'C':
					if (poCC1101!=NULL)
					{
						poCC1101->ReadAllRegister();
					}
				break;
			#endif
			#ifdef HasCC1101
				case 'g':
					RF_GetChipData();
					break;
			#endif
				case 't':
				break;
			#ifdef HasCC1101
				case 's':
				if (inDataCnt>5)  //must be raw data
				{
					RF_SendRawData(1);
				}
				else
				{
					RF_SendTestData();
					//RF_SendData();
				}
				break;
			#endif
			#ifdef HAS_INTERTECHNO
				case 's':
				//to do...
			break;
			#endif




			}
			ResetData();
		}
		else
		{
			if (inDataCnt<(MAX_IN_BUFFER-1))
			{
				//einfach nur merken...
				inData[inDataCnt]=inByte;
				inDataCnt++;
			}
			else {
				ResetData();
			}
		}
	}
		
	}
	
	
#ifdef HasCC1101
	RF_ReceiveData();
	SendDataByTimer();
#endif
#ifdef HasFerrarisCounter
	if (poFerrarisCounter0!=NULL)
	{
		poFerrarisCounter0->Check();
	}
	if (poFerrarisCounter1!=NULL)
	{
		poFerrarisCounter1->Check();
	}
#endif

#ifdef HasVentusW132
	if (poVentusW132!=NULL) {
		poVentusW132->Do();
	}
//if (poAnalyseProtokoll!=NULL)
//{
//	poAnalyseProtokoll->loop();
//}
#endif
//RX_WirelessSensors();	
	
}

#else 



//this is the iobroker receiver version
void cUSBIfc::Do()
{
	if (bEnabled && cSerial::available() > 0)
	{
		char inByte = cSerial::read();
		
		//String sTemp1 = String(inByte,HEX);
		//sTemp1+=" ";
		//cSerial::print(sTemp1);
		
		if (inDataCnt<(MAX_IN_BUFFER-1))
		{
			//einfach nur merken...
			inData[inDataCnt]=inByte;
			inDataCnt++;
		}
		else {
			ResetData();
		}
	}
	if (inData[0] == 'm' ) {
		if (inDataCnt>1 && inData[1] == 'i') {
			nMode4PrintOut=1;
			cSerial::println("telegram mode");
			ResetData();
		}
		else if (inDataCnt>1 && inData[1] == 'r') {
			nMode4PrintOut=0;
			cSerial::println("raw mode");
			ResetData();
		}
#ifdef HasCC1101
		if (poCC1101!=NULL)
		{
			poCC1101->setDefaultRegs();
			poCC1101->reset();
		}
#endif
			
	}
	else if (inData[0] == 'S') {
			
		//cSerial::println(">");
			
		if (inDataCnt>1) {
			int nMaxData = inData[1]+2;
			//cSerial::println("!");	
			if( inDataCnt >= nMaxData) {
				//cSerial::println(":");
				//cSerial::println("to send " + inDataCnt);
				
				RF_SendRawData(2);
				ResetData();
			}
		}
	}
	else if (inData[0] == 'I') {
		ResetData();
		RF_SendTestData();
		ResetData();
	}
	else if (inData[0] == 'T') {
		ResetData();
			
		inData[0]=0x53;
		inData[1]=0xe;
		inData[2]=0x0;
		inData[3]=0x10;
		inData[4]=0x10;
		inData[5]=0x10;
		inData[6]=0x10;
		inData[7]=0x10;
		inData[8]=0x10;
		inData[9]=0x87;
		inData[10]=0xfb;
		inData[11]=0x30;
		inData[12]=0x3;
		inData[13]=0x8;
		inData[14]=0x0;
		inData[15]=0x10;
		inData[16]=0x0;
		inDataCnt=17;
			
			
		RF_SendRawData(2);
		ResetData();
	}
	else {
		if (inDataCnt>0) {
			ResetData();
		}
	}
		
	
	
	#ifdef HasCC1101
	RF_ReceiveData();
	SendDataByTimer();
	#endif
}

#endif

void cUSBIfc::SendDataByTimer()
{
#ifdef HasCC1101
	if(bSensorMode && bTimer==true)
	{
		//clock_prescale_set(clock_div_1);
		bTimer = false;
		nIRQCnt=0;
		
		RF_SendData();
	
		//clock_prescale_set(clock_div_32);
		/* Re-enter sleep mode. */
		//enterSleep(); //must be used for battery mode but then wake up in Pin Ventus as well
	}
#endif
}

void cUSBIfc::enterSleep()
{
  set_sleep_mode(SLEEP_MODE_IDLE);
  
  sleep_enable();
#ifdef HasCC1101
	poCC1101->setPowerDownState();
 #endif
  /* Disable all of the unused peripherals. This will reduce power
   * consumption further and, more importantly, some of these
   * peripherals may generate interrupts that will wake our Arduino from
   * sleep!
   */
  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer2_disable();
  power_twi_disable();  

  int nCounter=0;

	nCounter=10;



  /* Now enter sleep mode. */
	while (nCounter>0)
	{
		sleep_mode();
		nCounter--;
	}
  
  /* The program will continue from here after the timer timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
 
#ifdef HasCC1101
  poCC1101->wakeUp();
#endif
}

void cUSBIfc::ConfigureTimer()
{
  /*** Configure the timer.***/
  
  /* Normal timer operation.*/
  TCCR1A = 0x00; 
  
  /* Clear the timer counter register.
   * You can pre-load this register with a value in order to 
   * reduce the timeout period, say if you wanted to wake up
   * ever 4.0 seconds exactly.
   */
  TCNT1=0x0000; 
  
  /* Configure the prescaler for 1:1024, giving us a 
   * timeout of 4.09 seconds.
   */
  //TCCR1B = 0x05;
  //clock_prescale_set(clock_div_32);
  
  /* Enable the timer overlow interrupt. */
  TIMSK1=0x01;
}

void cUSBIfc::StartSensorMode()
{
	//cDebug::Log(cDebug::LOG_I,"Sens");

	ConfigureTimer();
	
	bSensorMode=true;
}

void cUSBIfc::ResetData()
{
	//cDebug::Log(cDebug::LOG_D,"Reset data");
	for (int i=0;i<MAX_IN_BUFFER;i++)
	{
		inData[i]=0x00;
	}
	inDataCnt=0;
	cSerial::println("rr");
}



/*
http://labalec.fr/erwan/?p=497
*/

void cUSBIfc::RF_Init()
{
#ifdef HasCC1101
	//cDebug::Log(cDebug::LOG_D,"RF_Init");
	nCtr=0;
	//byte syncWord = 199;

	if (poCC1101!=NULL)
	{
		poCC1101->init();
	
		//poCC1101->setSyncWord(&syncWord, false);
		//poCC1101->setCarrierFreq(CFREQ_433);
		//poCC1101->disableAddressCheck();
		//poCC1101->setTxPowerAmp(PA_LongDistance);
		//poCC1101->setTxPowerAmp(PA_LowPower);
	}

	delay(1000);

	RF_GetChipData();
	
	AttachIRQ();
#endif
}

void cUSBIfc::AttachIRQ()
{
#ifdef HasCC1101
	attachInterrupt(1, cc1101signalsInterrupt, FALLING);
#endif
}

void cUSBIfc::DetachIRQ()
{
#ifdef HasCC1101
	detachInterrupt(1);
#endif

}

void cUSBIfc::RF_GetChipData()
{
#ifdef HasCC1101
/*
	if (poCC1101!=NULL)
	{

		String sTemp;
		sTemp = "PARTNUM 0x";
		byte nTemp = poCC1101->readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER);
		sTemp += String(nTemp,HEX);
		cDebug::Log(cDebug::LOG_D,sTemp);
		
		sTemp = "VERSION 0x";
		nTemp = poCC1101->readReg(CC1101_VERSION, CC1101_STATUS_REGISTER);
		sTemp += String(nTemp,HEX);
		cDebug::Log(cDebug::LOG_D,sTemp);
		
		sTemp = "MARCSTATE 0x";
		nTemp = poCC1101->readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1f;
		sTemp += String(nTemp,HEX);
		cDebug::Log(cDebug::LOG_D,sTemp);
	}
	*/
#endif
}


void cUSBIfc::RF_SendRawData(int start)
{
#ifdef HasCC1101
	if (inDataCnt>=MAX_IN_BUFFER)
	{
		ResetData();
		cSerial::println("too long");
		return;
	}

//#ifdef DEBUG
//	String sTemp= "xx " + String(inDataCnt,DEC)+": ";
//	for(int j=0; j<inDataCnt; j++)
//	{
//		sTemp += String(inData[j],HEX);
//		sTemp += " ";
//	}
//	cSerial::println(sTemp);
//#endif


	

	oSendPacket.Reset();
	oSendPacket.AddRawData(inData,start, inDataCnt);
	


#ifdef DEBUG
	String sTemp;
	sTemp = "X ";
	for(int j=0; j<oSendPacket.length; j++)
	{
		sTemp += String(oSendPacket.data[j],HEX);
		sTemp += " ";
	}
	sTemp += "Y ";
	cSerial::println(sTemp);
#endif



	DetachIRQ();
	
	if(poCC1101->sendData(oSendPacket))
	{
		cSerial::println("ok");
	}
	else
	{
		cSerial::println("failed");
	}
	// Enable wireless reception interrupt
	AttachIRQ();
#else

	String sTemp= "xxx " + String(inDataCnt,DEC)+": ";
	for(int j=0; j<inDataCnt; j++)
	{
		sTemp += String(inData[j],HEX);
		sTemp += " ";
	}
	cSerial::println(sTemp);

#endif
}

void cUSBIfc::RF_SendData() 
{
	//clock_prescale_set(clock_div_2);
#ifdef HasCC1101
	if (poCC1101!=NULL)
	{


	
#ifdef HasOneWireDevices
		poOneWireDevices->ReadDevices();
#endif // HasOneWireDevices

		oSendPacket.Reset();
		
		char nType=0x01;

#ifdef IsZentrale
		nType=0x10;
#endif

#ifdef HasOneWireDevices
		int nSize=6;
		int nId[nSize];
		poOneWireDevices->GetID(nId,nSize);
	
		oSendPacket.AddHeader(nId,nType); //hier muss die ID aus dem DS2401 übergeben werden
		//oSendPacket.AddHeader(0x01);
#else
		oSendPacket.AddHeader(nType);
#endif
#ifdef HasBMP180
	if (poBMP180!=NULL)
	{
		int nDelay = poBMP180->startTemperature();
		if (nDelay>0)
		{
			delay(nDelay);
		
			char cRet = poBMP180->getTemperature(fTemp1);
			
			if (poBME280==NULL)
			{
				if (cRet==1)
					oSendPacket.AddTemperature(fTemp1);
				else
					oSendPacket.AddTemperature(-99.0);
			}
		}
	
		nDelay = poBMP180->startPressure(3);
	
		if (nDelay>0)
		{
			delay(nDelay);
			double fPressure;
		
			char cRet = poBMP180->getPressure(fPressure, fTemp1);
			if (cRet==1)
				oSendPacket.AddPressure(fPressure);
			else
				oSendPacket.AddPressure(-99);
		}
	}
#endif

	if (poBME280!=NULL)
	{
#ifdef HasBME280
		float fTemp=poBME280->readTemperature();
		float fHum=poBME280->readHumidity();
		float fPress=poBME280->readPressure()/100.0;
		oSendPacket.AddTemperature(fTemp);
		oSendPacket.AddPressure(fPress);
		oSendPacket.AddHumidity(fHum);
		//String sTemp = "| " + String(fTemp,2) + "C " + String(fHum,2)+ "% " + String(fPress,2)+ "mBar ";
		//cDebug::Log(cDebug::LOG_D,sTemp);
#endif
	}
#ifdef HasDHT22
	if (poDHT22!=NULL)
	{
		GetHumidity();
		//oSendPacket.AddTemperature(fTemp2);
		oSendPacket.AddHumidity(fHumidity);
	}
#endif
#ifdef HasBH1750
	if (poBH1750!=NULL)
	{
		int error=-1;
		int nTemp=poBH1750->Get(&error);
		
		//String sTemp = "x " + String(nTemp,DEC) + " " + String(error,DEC);
		//cDebug::Log(cDebug::LOG_D,sTemp);
		
		if (error<0)
		{
			oSendPacket.AddBrightness(nTemp);
		}
		else
		{
			oSendPacket.AddError(error);
		}
	}
#endif

#ifdef HasVentusW132
	if (poVentusW132!=NULL)
	{
		int nHum=poVentusW132->GetHumidity();
		float fTemp=poVentusW132->GetTemperature();
		float fAvgWindSpeed = poVentusW132->GetAvgWindSpeed();
		float fWindGust=poVentusW132->GetWindGust();
		int nWindDir = poVentusW132->GetWindDir();
		
		if (nHum>0)
		{
			oSendPacket.AddHumidity((float) nHum);
		}

		if (fTemp>-99)
		{
			oSendPacket.AddTemperature(fTemp);
		}
		if (fAvgWindSpeed>-1)
		{
			oSendPacket.AddAvgWindSpeed(fAvgWindSpeed);
		}
		if (fWindGust>-1)
		{
			oSendPacket.AddWindGust(fWindGust);
		}
		if (nWindDir>-1)
		{
			oSendPacket.AddWindDir(nWindDir);
		}
	}
#endif


/*
		switch (nNextData)
		{
			default:
			case 0:
				nNextData++;
				if (poBMP180!=NULL)
				{
					int nDelay = poBMP180->startTemperature();
					if (nDelay>0)
					{
						delay(nDelay);
						
						char cRet = poBMP180->getTemperature(fTemp1);
						
						if (cRet==1)
							oSendPacket.AddTemperature(fTemp1);
						else
							oSendPacket.AddTemperature(-99.0);
					}
					
					nDelay = poBMP180->startPressure(3);
					
					if (nDelay>0)
					{
						delay(nDelay);
						double fPressure;
						
						char cRet = poBMP180->getPressure(fPressure, fTemp1);
						if (cRet==1)
							oSendPacket.AddPressure(fPressure);
						else
							oSendPacket.AddPressure(-99);
					}
				}
				else
				{
					oSendPacket.AddTemperature(-99);
					oSendPacket.AddPressure(-99);
				}
				break;
			case 1:
				nNextData++;
				if (poDHT22!=NULL)
				{
					GetHumidity();
					oSendPacket.AddTemperature(fTemp2);
					oSendPacket.AddHumidity(fHumidity);
				}
				else
				{
					oSendPacket.AddTemperature(-99);
					oSendPacket.AddHumidity(-99);
				}
				break;
			case 2:
				nNextData=0;
				if (poBH1750!=NULL)
				{
					oSendPacket.AddBrightness(poBH1750->Get());
				}
				else
				{
					oSendPacket.AddBrightness(-99);
				}
				break;
		}
		*/
// 		//CCPACKET data;
// 		oSendPacket.length=10;
// 		oSendPacket.datatype = DATATYPE_UNKNOWN;
// 		oSendPacket.repetition = 6;
// 	
// 		for (int i=0; i<oSendPacket.length;i++)
// 		{
// 			oSendPacket.data[i]=0x0;
// 		}
// 	
// 		oSendPacket.data[0]=5;
// 		oSendPacket.data[1]=nCtr;
// 		oSendPacket.data[2]=0;
// 		oSendPacket.data[3]=1;
// 		oSendPacket.data[4]=0;
// 		//cc1101.flushTxFifo ();
// 		nCtr++;
// 		if (nCtr>0xFE)
// 		{
// 			nCtr=0;
// 		}
	
		//String sTemp = "default mode - packet: len " + String(oSendPacket.length,DEC);
		//cDebug::Log(cDebug::LOG_D,sTemp);
	
// 		sTemp = " data: ";
// 		for(int j=0; j<data.length; j++)
// 		{
// 			sTemp += String(data.data[j],HEX);
// 			sTemp += " ";
// 		}
// 		poDebug->Log("D",sTemp);
// 		poDebug->Log("D",".");
	
	// 	sTemp = "CC1101_MARCSTATE 0x";
	// 	byte nTemp = poCC1101->readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1f;
	// 	sTemp += String(nTemp,HEX);
	// 	poDebug->Log("D",sTemp);
	

	//print telegramm
#ifdef DEBUG
/* 		String sTemp;
 		for(int j=0; j<oSendPacket.length; j++)
 		{
 			sTemp += String(oSendPacket.data[j],HEX);
 			sTemp += " ";
 		}
 		cDebug::Log(cDebug::LOG_E,sTemp);*/
#endif		
 		DetachIRQ();
 	
		if(poCC1101->sendData(oSendPacket))
		{
			//cDebug::Log(cDebug::LOG_E,"ok");
		}
		else
		{
			//cDebug::Log(cDebug::LOG_E,"sent default failed");
		}
 		// Enable wireless reception interrupt
 		AttachIRQ();
	

	//clock_prescale_set(clock_div_1);
	} //if !NULL
#endif
}

void cUSBIfc::RF_SendTestData() 
{
#ifdef HasCC1101
	if (poCC1101!=NULL)
	{
		oSendPacket.Reset();
#ifdef IsZentrale
		int nType=0x10;
#else
		int nType=0x01;
#endif
#ifdef HasOneWireDevices
		int nSize=6;
		int nId[nSize];
		poOneWireDevices->GetID(nId,nSize);

		oSendPacket.AddHeader(nId,nType); //hier muss die ID aus dem DS2401 übergeben werden
//oSendPacket.AddHeader(0x01);
#else
		

		oSendPacket.AddHeader(nType);
#endif

		//oSendPacket.AddDate();
		//oSendPacket.AddTime();

	//print telegramm
#ifdef DEBUG
 		String sTemp;
		//sTemp = "I ";
 		for(int j=0; j<oSendPacket.length; j++)
 		{
 			sTemp += String(oSendPacket.data[j],HEX);
 			sTemp += " ";
 		}
		//sTemp += "J ";
 		cSerial::println(sTemp);
#endif		
 		DetachIRQ();
 	
		if(poCC1101->sendData(oSendPacket))
		{
			//cSerial::println("ok");
		}
		else
		{
			cSerial::println("sent default failed");
		}
 		// Enable wireless reception interrupt
 		AttachIRQ();
	} //if !NULL
#endif
}
// receiving


void cUSBIfc::RF_ReadLQI()
{
#ifdef HasCC1101
	if (poCC1101!=NULL)
	{
		//byte lqi=0;
		//byte value=0;
		//lqi=poCC1101->readReg(CC1101_LQI, CC1101_STATUS_REGISTER);
		//value = 0x3F - (lqi & 0x3F);
	
		//String sTemp = "LQI " + String(value,DEC);
		//cDebug::Log(cDebug::LOG_I,sTemp);
	}
#endif
}

void cUSBIfc::RF_ReadRSSI()
{
#ifdef HasCC1101
	if (poCC1101!=NULL)
	{

		byte rssi=0;
		byte value=0;

		rssi=(poCC1101->readReg(CC1101_RSSI, CC1101_STATUS_REGISTER));

		if (rssi >= 128)
		{
			value = rssi-256;
			value /= 2;
			value -= 74;
		}
		else
		{
			value = rssi/2;
			value -= 74;
		}
		

		//String sTemp="RSSI " + String(value,DEC) + " dBm ";
		//cSerial::print(sTemp);

	}
#endif
}


void  cUSBIfc::RF_ReceiveData()
{
#ifdef HasCC1101
	if (poCC1101!=NULL)
	{

#ifdef HAS_INTERTECHNO
	if (poCC1101->nIRQCounter>nOldIRQCounter)
	{
		//cDebug::Log(cDebug::LOG_I,"IRQ counter " + String(poCC1101->nIRQCounter,DEC));
		nOldIRQCounter=poCC1101->nIRQCounter;
	}
#endif
	if(bPacketAvailable)
	{
		nPacketsReceived++;
		//poDebug->Log(cDebug::LOG_D,"packet received");
		
		oReceivePacket.Reset();
		// Disable wireless reception interrupt
		DetachIRQ();
				
		//RF_ReadRSSI();
		//RF_ReadLQI();
		// clear the flag
		bPacketAvailable = false;

		if( poCC1101->receiveData(&oReceivePacket) > 0)
		{
			if(!oReceivePacket.crc_ok)
			{
				nPacketsCRSFailure++;
				//cDebug::Log(cDebug::LOG_E,"crc not ok");
				cSerial::println("CRC failed " + String(nPacketsCRSFailure,DEC));
			}
			else if(oReceivePacket.length > 0)
			{

				nPacketsGood++;
				//cDebug::Log(cDebug::LOG_E,"|");
				int nId[] ={0,1,2,3,4,5};
				
				RF_ReadRSSI();
				RF_ReadLQI();

				oReceivePacket.InterpretePacket(nMode4PrintOut,nId);
			}
			else
			{
				//cDebug::Log(cDebug::LOG_E,"-");
			}
		}
 		else
 		{
 			//cDebug::Log(cDebug::LOG_E,"+");
 		}

// 		// Disable wireless reception interrupt
// 		DetachIRQ();
// 		
// 		RF_ReadRSSI();
// 		RF_ReadLQI();
// 		// clear the flag
// 		bPacketAvailable = false;
// 
// 		//CCPACKET packet;
// 
// 		if(poCC1101->receiveData(&oReceivePacket) > 0)
// 		{
// 			if(!oReceivePacket.crc_ok)	
// 			{
// 				poDebug->Log(cDebug::LOG_E,"crc not ok");
// 			}
// 
// 			if(oReceivePacket.length > 0)
// 			{
// 				String sTemp = "packet: len " + String(oReceivePacket.length,DEC);
// 				poDebug->Log(cDebug::LOG_D,sTemp);
// 
// 				
// 				sTemp = "data:";
// 				for(int j=0; j<oReceivePacket.length; j++)
// 				{
// 					sTemp += " ";
// 					sTemp += String(oReceivePacket.data[j],HEX);
// 					
// 				}
// 				poDebug->Log(cDebug::LOG_I,sTemp);
// 				poDebug->Log(cDebug::LOG_D,".");
// 			}
// 		}
		// Enable wireless reception interrupt
		AttachIRQ();
	}
	}
#endif
}

#ifdef HasDHT22
void cUSBIfc::GetHumidity()
{

	DHT22_ERROR_t errorCode;
	errorCode = poDHT22->readData();
	fHumidity=0.0;
	fTemp2=-99;
	String sTemp;
	switch(errorCode)
	{
		case DHT_ERROR_NONE:
			fTemp2 = poDHT22->getTemperatureC();
			fHumidity=poDHT22->getHumidity();
			break;
// 		case DHT_ERROR_CHECKSUM:
// 			cDebug::Log(cDebug::LOG_E,"check sum error ");
// 			break;
 		case DHT_BUS_HUNG:
 			//cDebug::Log(cDebug::LOG_E,"BUS Hung ");
 			break;
// 		case DHT_ERROR_NOT_PRESENT:
// 			cDebug::Log(cDebug::LOG_E,"Not Present ");
// 			break;
// 		case DHT_ERROR_ACK_TOO_LONG:
// 			cDebug::Log(cDebug::LOG_E,"ACK time out ");
// 			break;
// 		case DHT_ERROR_SYNC_TIMEOUT:
// 			cDebug::Log(cDebug::LOG_E,"Sync Timeout ");
// 			break;
// 		case DHT_ERROR_DATA_TIMEOUT:
// 			cDebug::Log(cDebug::LOG_E,"Data Timeout ");
// 			break;
// 		case DHT_ERROR_TOOQUICK:
// 			cDebug::Log(cDebug::LOG_E,"Polled to quick ");
// 			break;
		default:
			sTemp="error: " + String(errorCode,DEC);
			//cDebug::Log(cDebug::LOG_E,sTemp);
			break;
	}

}
#endif

/*
mit CUL empfangener Code von der Fernbedienung
i59 A5A69966A995 56 43
i59 A5A69966A996 56 43
i59 A5A69966A995 56 43
i59 A5A69966A996 56 44
i59 A5A69966A995 56 45
*/


/*
I: CC1101: 0d 2e 2d 07 d3 91 3d 04
I: CC1101: 32 00 00 06 00 10 b0 71
I: CC1101: 55 e4 30 23 b9 00 07 00
I: CC1101: 18 14 6c 07 00 90 87 6b
I: CC1101: f8 56 11 ef 2c 15 1f 41
I: CC1101: 00 59 7f 3f 88 31 0b 00

//ohne senden
0D 2E 2D 07 D3 91 3D 04
32 00 00 06 00 10 B0 71
55 E4 30 23 B9 00 07 00
18 14 6C 07 00 90 87 6B
F8 56 11 EF 0C 3C 1F 41
00 59 7F 3F 88 31 0B 00

in it_send
0D 2E 2D 07 D3 91 3D 04
32 00 00 06 00 10 B0 71
55 E4 30 23 B9 00 07 00
18 14 6C 07 00 90 87 6B
F8 56 11 EF 2C 14 1F 41
00 59 7F 3F 88 31 0B 00

*/

