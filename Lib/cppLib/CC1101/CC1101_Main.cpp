/* 
* cCC1101_Main.cpp
*
* Created: 19.02.2017 15:41:19
* Author: Rene
*/

#include "../Debug/Debug.h"
#include "CC1101_Main.h"
#include <SPI.h>

#ifdef HasCC1101

/**
* Macros
*/
// Select (SPI) CC1101
//#define cc1101_Select()  bitClear(PORT_SPI_SS, BIT_SPI_SS)
#define cc1101_Select()  digitalWrite(PIN_CS_CC1101,LOW)

// Deselect (SPI) CC1101
//#define cc1101_Deselect()  bitSet(PORT_SPI_SS, BIT_SPI_SS)
#define cc1101_Deselect()  digitalWrite(PIN_CS_CC1101,HIGH)

// Wait until SPI MISO line goes low
//#define wait_Miso()  while(bitRead(PORT_SPI_MISO, BIT_SPI_MISO))
#define wait_Miso() while(digitalRead(PIN_MISO))

// Get GDO0 pin state
//#define getGDO0state()  bitRead(PORT_GDO0, BIT_GDO0)
#define getGDO0state() digitalRead(PIN_GDO0)

// Wait until GDO0 line goes high
#define wait_GDO0_high()  while(!getGDO0state())
// Wait until GDO0 line goes low
#define wait_GDO0_low()  while(getGDO0state())

#define cc1101_Set_Out digitalWrite(PIN_GDO0,HIGH)
#define cc1101_Clear_Out digitalWrite(PIN_GDO0,LOW)

#if defined (SPI_HAS_TRANSACTION)
static SPISettings mySPISettings;
#endif


// default constructor
cCC1101_Main::cCC1101_Main()
{
	//paTableByte = PA_LowPower;            // Priority = Low power
	paTableByte = PA_LongDistance;
} //cCC1101_Main

// default destructor
cCC1101_Main::~cCC1101_Main()
{
} //~cCC1101_Main


/**
 * wakeUp
 * 
 * Wake up CC1101 from Power Down state
 */
void cCC1101_Main::wakeUp(void)
{
  cc1101_Select();                      // Select CC1101
  wait_Miso();                          // Wait until MISO goes low
  cc1101_Deselect();                    // Deselect CC1101
}
	
/**
 * writeReg
 * 
 * Write single register into the CC1101 IC via SPI
 * 
 * 'regAddr'	Register address
 * 'value'	Value to be writen
 */
void cCC1101_Main::writeReg(byte regAddr, byte value) 
{
#if defined (SPI_HAS_TRANSACTION)
	SPI.beginTransaction(mySPISettings);
#endif
	cc1101_Select();                      // Select CC1101
	wait_Miso();                          // Wait until MISO goes low
	SPI.transfer(regAddr);
	SPI.transfer(value);
  //spi.send(regAddr);                    // Send register address
  //spi.send(value);                      // Send value
	cc1101_Deselect();                    // Deselect CC1101
 #if defined (SPI_HAS_TRANSACTION)
	SPI.endTransaction();
 #endif
 
	if (regAddr != CC1101_TXFIFO)
	{
		byte nReg=readReg(regAddr,CC1101_CONFIG_REGISTER);
	
		if (nReg!=value)
		{
		/*
		cSerial::println();
		cSerial::print("error ");
		cSerial::print(nReg);
		cSerial::print(" != ");
		cSerial::print(value);
		cSerial::print(" addr: ");
		*/
		
			String sText = "error 0x" + String(value,HEX) + " != 0x"  + String(nReg,HEX) + " on addr "  + String(regAddr,DEC) + " / 0x"  + String(regAddr,HEX);
			cSerial::println(sText);
		}
	}
}

/**
 * writeBurstReg
 * 
 * Write multiple registers into the CC1101 IC via SPI
 * 
 * 'regAddr'	Register address
 * 'buffer'	Data to be writen
 * 'len'	Data length
 */
void cCC1101_Main::writeBurstReg(byte regAddr, byte* buffer, byte len)
{
  byte addr, i;

#if defined (SPI_HAS_TRANSACTION)
	SPI.beginTransaction(mySPISettings);
#endif
  addr = regAddr | WRITE_BURST;         // Enable burst transfer
  cc1101_Select();                      // Select CC1101
  wait_Miso();                          // Wait until MISO goes low
  SPI.transfer(addr);
  //spi.send(addr);                       // Send register address
  
  for(i=0 ; i<len ; i++)
	SPI.transfer(buffer[i]);
    //spi.send(buffer[i]);                // Send value

  cc1101_Deselect();                    // Deselect CC1101  
 #if defined (SPI_HAS_TRANSACTION)
	SPI.endTransaction();
 #endif
}

/**
 * cmdStrobe
 * 
 * Send command strobe to the CC1101 IC via SPI
 * 
 * 'cmd'	Command strobe
 */     
void cCC1101_Main::cmdStrobe(byte cmd) 
{
#if defined (SPI_HAS_TRANSACTION)
	SPI.beginTransaction(mySPISettings);
#endif
	cc1101_Select();                      // Select CC1101
	wait_Miso();                          // Wait until MISO goes low
	SPI.transfer(cmd);
	//spi.send(cmd);                        // Send strobe command
	cc1101_Deselect();                    // Deselect CC1101
 #if defined (SPI_HAS_TRANSACTION)
	SPI.endTransaction();
 #endif
}

byte cCC1101_Main::cmdStrobeRet(byte cmd)
{
#if defined (SPI_HAS_TRANSACTION)
	SPI.beginTransaction(mySPISettings);
#endif
	byte val;
	cc1101_Select();                      // Select CC1101
	wait_Miso();                          // Wait until MISO goes low
	val=SPI.transfer(cmd);
	//val=spi.send(cmd);                        // Send strobe command
	
	cc1101_Deselect();                    // Deselect CC1101
 #if defined (SPI_HAS_TRANSACTION)
	SPI.endTransaction();
 #endif
	
	return val;
}

/**
 * readReg
 * 
 * Read CC1101 register via SPI
 * 
 * 'regAddr'	Register address
 * 'regType'	Type of register: CC1101_CONFIG_REGISTER or CC1101_STATUS_REGISTER
 * 
 * Return:
 * 	Data byte returned by the CC1101 IC
 */
byte cCC1101_Main::readReg(byte regAddr, byte regType) 
{
  byte addr, val;
#if defined (SPI_HAS_TRANSACTION)
	SPI.beginTransaction(mySPISettings);
#endif
  addr = regAddr | regType;
  cc1101_Select();                      // Select CC1101
  wait_Miso();                          // Wait until MISO goes low
  SPI.transfer(addr);
  val=SPI.transfer(0x00);
  //spi.send(addr);                       // Send register address
  //val = spi.send(0x00);                 // Read result
  cc1101_Deselect();                    // Deselect CC1101
 #if defined (SPI_HAS_TRANSACTION)
	SPI.endTransaction();
 #endif
  return val;
}

/**
 * readBurstReg
 * 
 * Read burst data from CC1101 via SPI
 * 
 * 'buffer'	Buffer where to copy the result to
 * 'regAddr'	Register address
 * 'len'	Data length
 */
void cCC1101_Main::readBurstReg(byte * buffer, byte regAddr, byte len) 
{
  byte addr, i;
#if defined (SPI_HAS_TRANSACTION)
	SPI.beginTransaction(mySPISettings);
#endif  
  addr = regAddr | READ_BURST;
  cc1101_Select();                      // Select CC1101
  wait_Miso();                          // Wait until MISO goes low
  SPI.transfer(addr);
  //spi.send(addr);                       // Send register address
  for(i=0 ; i<len ; i++)
	buffer[i]=SPI.transfer(0x00);
    //buffer[i] = spi.send(0x00);         // Read result byte by byte
  cc1101_Deselect();                    // Deselect CC1101
 #if defined (SPI_HAS_TRANSACTION)
	SPI.endTransaction();
 #endif
}

/**
 * reset
 * 
 * Reset CC1101
 */
void cCC1101_Main::reset(void) 
{
	cc1101_Deselect();                    // Deselect CC1101
	delayMicroseconds(5);
	cc1101_Select();                      // Select CC1101
	delayMicroseconds(10);
	cc1101_Deselect();                    // Deselect CC1101
	delayMicroseconds(41);
	cc1101_Select();                      // Select CC1101
#if defined (SPI_HAS_TRANSACTION)
	SPI.beginTransaction(mySPISettings);
#endif
	wait_Miso();                          // Wait until MISO goes low
	SPI.transfer(CC1101_SRES);
	//spi.send(CC1101_SRES);                // Send reset command strobe
	wait_Miso();                          // Wait until MISO goes low

	cc1101_Deselect();                    // Deselect CC1101
 #if defined (SPI_HAS_TRANSACTION)
	SPI.endTransaction();
 #endif
 
 #if defined UseRF4Ventus 
	setVentusRegs();  
 #else
	setDefaultRegs();                     // Reconfigure CC1101
#endif

	//ReadAllRegister();
}

/**
 * setDefaultRegs
 * 
 * Configure CC1101 registers
 */
void cCC1101_Main::setDefaultRegs(void) 
{
	//cDebug::Log(cDebug::LOG_I,"set default");
	cSerial::print("set default ");
	
	pinMode(PIN_GDO0, INPUT);
	//GDO2 not used
	
	writeReg(CC1101_IOCFG2,  CC1101_DEFVAL_IOCFG2);
	writeReg(CC1101_IOCFG1,  CC1101_DEFVAL_IOCFG1);
	writeReg(CC1101_IOCFG0,  CC1101_DEFVAL_IOCFG0);
	writeReg(CC1101_FIFOTHR,  CC1101_DEFVAL_FIFOTHR);
	writeReg(CC1101_PKTLEN,  CC1101_DEFVAL_PKTLEN);
	writeReg(CC1101_PKTCTRL1,  CC1101_DEFVAL_PKTCTRL1);
	writeReg(CC1101_PKTCTRL0,  CC1101_DEFVAL_PKTCTRL0);

  // Set default synchronization word
  //setSyncWord(CC1101_DEFVAL_SYNC1, CC1101_DEFVAL_SYNC0, false);
  	writeReg(CC1101_SYNC1, CC1101_DEFVAL_SYNC1);
  	writeReg(CC1101_SYNC0, CC1101_DEFVAL_SYNC0);


  // Set default device address
  //setDevAddress(CC1101_DEFVAL_ADDR, false);
	writeReg(CC1101_ADDR, CC1101_DEFVAL_ADDR);
  // Set default frequency channel
  //setChannel(CC1101_DEFVAL_CHANNR, false);
  writeReg(CC1101_CHANNR,  CC1101_DEFVAL_CHANNR);
  
  writeReg(CC1101_FSCTRL1,  CC1101_DEFVAL_FSCTRL1);
  writeReg(CC1101_FSCTRL0,  CC1101_DEFVAL_FSCTRL0);

  // Set default carrier frequency = 433 MHz
  //setCarrierFreq(CFREQ_433);
  writeReg(CC1101_FREQ2,  CC1101_DEFVAL_FREQ2_433);
  writeReg(CC1101_FREQ1,  CC1101_DEFVAL_FREQ1_433);
  writeReg(CC1101_FREQ0,  CC1101_DEFVAL_FREQ0_433);


  writeReg(CC1101_MDMCFG4,  CC1101_DEFVAL_MDMCFG4);
  writeReg(CC1101_MDMCFG3,  CC1101_DEFVAL_MDMCFG3);
  writeReg(CC1101_MDMCFG2,  CC1101_DEFVAL_MDMCFG2);
  writeReg(CC1101_MDMCFG1,  CC1101_DEFVAL_MDMCFG1);
  writeReg(CC1101_MDMCFG0,  CC1101_DEFVAL_MDMCFG0);
  writeReg(CC1101_DEVIATN,  CC1101_DEFVAL_DEVIATN);
  writeReg(CC1101_MCSM2,  CC1101_DEFVAL_MCSM2);
  writeReg(CC1101_MCSM1,  CC1101_DEFVAL_MCSM1);
  writeReg(CC1101_MCSM0,  CC1101_DEFVAL_MCSM0);
  writeReg(CC1101_FOCCFG,  CC1101_DEFVAL_FOCCFG);
  writeReg(CC1101_BSCFG,  CC1101_DEFVAL_BSCFG);
  writeReg(CC1101_AGCCTRL2,  CC1101_DEFVAL_AGCCTRL2);
  writeReg(CC1101_AGCCTRL1,  CC1101_DEFVAL_AGCCTRL1);
  writeReg(CC1101_AGCCTRL0,  CC1101_DEFVAL_AGCCTRL0);
  writeReg(CC1101_WOREVT1,  CC1101_DEFVAL_WOREVT1);
  writeReg(CC1101_WOREVT0,  CC1101_DEFVAL_WOREVT0);
  writeReg(CC1101_WORCTRL,  CC1101_DEFVAL_WORCTRL);
  writeReg(CC1101_FREND1,  CC1101_DEFVAL_FREND1);
  writeReg(CC1101_FREND0,  CC1101_DEFVAL_FREND0);
  writeReg(CC1101_FSCAL3,  CC1101_DEFVAL_FSCAL3);
  writeReg(CC1101_FSCAL2,  CC1101_DEFVAL_FSCAL2);
  writeReg(CC1101_FSCAL1,  CC1101_DEFVAL_FSCAL1);
  writeReg(CC1101_FSCAL0,  CC1101_DEFVAL_FSCAL0);
  writeReg(CC1101_RCCTRL1,  CC1101_DEFVAL_RCCTRL1);
  writeReg(CC1101_RCCTRL0,  CC1101_DEFVAL_RCCTRL0);
  writeReg(CC1101_FSTEST,  CC1101_DEFVAL_FSTEST);
  writeReg(CC1101_PTEST,  CC1101_DEFVAL_PTEST);
  writeReg(CC1101_AGCTEST,  CC1101_DEFVAL_AGCTEST);
  writeReg(CC1101_TEST2,  CC1101_DEFVAL_TEST2);
  writeReg(CC1101_TEST1,  CC1101_DEFVAL_TEST1);
  writeReg(CC1101_TEST0,  CC1101_DEFVAL_TEST0);
  
  cSerial::println("done");
}

void cCC1101_Main::setVentusRegs(void)
{
	//cDebug::Log(cDebug::LOG_I,"set default");
	cSerial::print("set Ventus ");
	
	pinMode(PIN_GDO0, INPUT);
	//GDO2 not used
	
	writeReg(CC1101_IOCFG2,  CC1101_VENTUSVAL_IOCFG2);
	writeReg(CC1101_IOCFG1,  CC1101_VENTUSVAL_IOCFG1);
	writeReg(CC1101_IOCFG0,  CC1101_VENTUSVAL_IOCFG0);
	writeReg(CC1101_FIFOTHR,  CC1101_VENTUSVAL_FIFOTHR);
	writeReg(CC1101_PKTLEN,  CC1101_VENTUSVAL_PKTLEN);
	writeReg(CC1101_PKTCTRL1,  CC1101_VENTUSVAL_PKTCTRL1);
	writeReg(CC1101_PKTCTRL0,  CC1101_VENTUSVAL_PKTCTRL0);

	// Set default synchronization word
	//setSyncWord(CC1101_VENTUSVAL_SYNC1, CC1101_VENTUSVAL_SYNC0, false);
	writeReg(CC1101_SYNC1, CC1101_VENTUSVAL_SYNC1);
	writeReg(CC1101_SYNC0, CC1101_VENTUSVAL_SYNC0);


	// Set default device address
	//setDevAddress(CC1101_VENTUSVAL_ADDR, false);
	writeReg(CC1101_ADDR, CC1101_VENTUSVAL_ADDR);
	// Set default frequency channel
	//setChannel(CC1101_VENTUSVAL_CHANNR, false);
	writeReg(CC1101_CHANNR,  CC1101_VENTUSVAL_CHANNR);
	
	writeReg(CC1101_FSCTRL1,  CC1101_VENTUSVAL_FSCTRL1);
	writeReg(CC1101_FSCTRL0,  CC1101_VENTUSVAL_FSCTRL0);

	// Set default carrier frequency = 433 MHz
	//setCarrierFreq(CFREQ_433);
	writeReg(CC1101_FREQ2,  CC1101_VENTUSVAL_FREQ2_433);
	writeReg(CC1101_FREQ1,  CC1101_VENTUSVAL_FREQ1_433);
	writeReg(CC1101_FREQ0,  CC1101_VENTUSVAL_FREQ0_433);


	writeReg(CC1101_MDMCFG4,  CC1101_VENTUSVAL_MDMCFG4);
	writeReg(CC1101_MDMCFG3,  CC1101_VENTUSVAL_MDMCFG3);
	writeReg(CC1101_MDMCFG2,  CC1101_VENTUSVAL_MDMCFG2);
	writeReg(CC1101_MDMCFG1,  CC1101_VENTUSVAL_MDMCFG1);
	writeReg(CC1101_MDMCFG0,  CC1101_VENTUSVAL_MDMCFG0);
	writeReg(CC1101_DEVIATN,  CC1101_VENTUSVAL_DEVIATN);
	writeReg(CC1101_MCSM2,  CC1101_VENTUSVAL_MCSM2);
	writeReg(CC1101_MCSM1,  CC1101_VENTUSVAL_MCSM1);
	writeReg(CC1101_MCSM0,  CC1101_VENTUSVAL_MCSM0);
	writeReg(CC1101_FOCCFG,  CC1101_VENTUSVAL_FOCCFG);
	writeReg(CC1101_BSCFG,  CC1101_VENTUSVAL_BSCFG);
	writeReg(CC1101_AGCCTRL2,  CC1101_VENTUSVAL_AGCCTRL2);
	writeReg(CC1101_AGCCTRL1,  CC1101_VENTUSVAL_AGCCTRL1);
	writeReg(CC1101_AGCCTRL0,  CC1101_VENTUSVAL_AGCCTRL0);
	writeReg(CC1101_WOREVT1,  CC1101_VENTUSVAL_WOREVT1);
	writeReg(CC1101_WOREVT0,  CC1101_VENTUSVAL_WOREVT0);
	writeReg(CC1101_WORCTRL,  CC1101_VENTUSVAL_WORCTRL);
	writeReg(CC1101_FREND1,  CC1101_VENTUSVAL_FREND1);
	writeReg(CC1101_FREND0,  CC1101_VENTUSVAL_FREND0);
	writeReg(CC1101_FSCAL3,  CC1101_VENTUSVAL_FSCAL3);
	writeReg(CC1101_FSCAL2,  CC1101_VENTUSVAL_FSCAL2);
	writeReg(CC1101_FSCAL1,  CC1101_VENTUSVAL_FSCAL1);
	writeReg(CC1101_FSCAL0,  CC1101_VENTUSVAL_FSCAL0);
	writeReg(CC1101_RCCTRL1,  CC1101_VENTUSVAL_RCCTRL1);
	writeReg(CC1101_RCCTRL0,  CC1101_VENTUSVAL_RCCTRL0);
	writeReg(CC1101_FSTEST,  CC1101_VENTUSVAL_FSTEST);
	writeReg(CC1101_PTEST,  CC1101_VENTUSVAL_PTEST);
	writeReg(CC1101_AGCTEST,  CC1101_VENTUSVAL_AGCTEST);
	writeReg(CC1101_TEST2,  CC1101_VENTUSVAL_TEST2);
	writeReg(CC1101_TEST1,  CC1101_VENTUSVAL_TEST1);
	writeReg(CC1101_TEST0,  CC1101_VENTUSVAL_TEST0);
	
	cSerial::println("done");
}

/**
 * init
 * 
 * Initialize CC1101
 */
void cCC1101_Main::init(void) 
{
	cSerial::println("start init");
	//spi.init();                           // Initialize SPI interface
	//cSerial::println("SPI begin");
	
#ifdef ATMEGA644	
	//pinMode(PIN_CS_DUMMY,OUTPUT);
#endif
	pinMode(PIN_CS_CC1101,OUTPUT);
	
	SPI.begin();
	//cSerial::println(" done");
	mySPISettings = SPISettings(8000000, MSBFIRST, SPI_MODE0);
	
	//cSerial::print("set clock");
	SPI.setClockDivider(SPI_CLOCK_DIV16);
	//SPI.setClockDivider(SPI_CLOCK_DIV128);
	
	
	//cSerial::println(" done");
	//cSerial::print("set bit order");
	SPI.setBitOrder(MSBFIRST);
	//cSerial::println(" done");
	pinMode(PIN_GDO0, INPUT);                 // Config GDO0 as input

	//cSerial::print("reset");
	reset();                              // Reset CC1101
	//cSerial::println(" done");

	// Configure PATABLE
	//writeBurstReg(CC1101_PATABLE, (byte*)paTable, 8);
	writeReg(CC1101_PATABLE, paTableByte);
	
	//String sTemp="paTableByte = 0x"+String(paTableByte,HEX);
	//cDebug::Log(cDebug::LOG_I,sTemp);
	//cSerial::println("init done");
}

/**
 * setSyncWord
 * 
 * Set synchronization word
 * 
 * 'syncH'	Synchronization word - High byte
 * 'syncL'	Synchronization word - Low byte
 * 'save' If TRUE, save parameter in EEPROM
 */
/*
void cCC1101_Main::setSyncWord(uint8_t syncH, uint8_t syncL, bool save) 
{
//	if ((syncWord[0] != syncH) || (syncWord[1] != syncL))
//	{
		//String sData = "sync word 0x" + String(syncH,HEX) + " 0x" + String(syncL,HEX);
		//Serial.print(sData);
		//cDebug::Log(cDebug::LOG_D,sData);
		
		writeReg(CC1101_SYNC1, syncH);
		writeReg(CC1101_SYNC0, syncL);
// 		syncWord[0] = syncH;
// 		syncWord[1] = syncL;
		// Save in EEPROM
		//if (save)
		//{
		//	EEPROM.write(EEPROM_SYNC_WORD, syncH);
		//	EEPROM.write(EEPROM_SYNC_WORD + 1, syncL);
		//}
		
 		//byte regH = readReg(CC1101_SYNC1,CC1101_CONFIG_REGISTER);
 		//byte regL = readReg(CC1101_SYNC0,CC1101_CONFIG_REGISTER);
 		//sData = "  sync word read 0x" + String(regH,HEX) + " 0x" + String(regL,HEX);
		//Serial.println(sData);
 		//cDebug::Log(cDebug::LOG_D,sData);
//	}
// 	else
// 	{
// 		cDebug::Log(cDebug::LOG_D,"syncword not set");
// 	}
}
*/
/**
 * setSyncWord (overriding method)
 * 
 * Set synchronization word
 * 
 * 'syncH'	Synchronization word - pointer to 2-byte array
 * 'save' If TRUE, save parameter in EEPROM
 */
/*
void cCC1101_Main::setSyncWord(byte *sync, bool save) 
{
  cCC1101_Main::setSyncWord(sync[0], sync[1], save);
}
*/
/**
 * setDevAddress
 * 
 * Set device address
 * 
 * 'addr'	Device address
 * 'save' If TRUE, save parameter in EEPROM
 */
/*
void cCC1101_Main::setDevAddress(byte addr, bool save) 
{
//	if (devAddress != addr)
//	{
		//String sData = "dev address set " + String(addr,DEC);
		//cDebug::Log(cDebug::LOG_D,sData);
		writeReg(CC1101_ADDR, addr);
//		devAddress = addr;
		// Save in EEPROM
		//if (save)
		//	EEPROM.write(EEPROM_DEVICE_ADDR, addr);  
		//
		//byte reg = readReg(CC1101_ADDR,CC1101_CONFIG_REGISTER);
		//sData = "dev address read " + String(reg,DEC);
		//cDebug::Log(cDebug::LOG_D,sData);
//  }
}
*/
/**
 * setChannel
 * 
 * Set frequency channel
 * 
 * 'chnl'	Frequency channel
 * 'save' If TRUE, save parameter in EEPROM
 */
/*
void cCC1101_Main::setChannel(byte chnl, bool save) 
{
	//if (channel != chnl)
//	{
		//String sData = "channel set " + String(chnl,DEC);
		//cDebug::Log(cDebug::LOG_D,sData);
		writeReg(CC1101_CHANNR,  chnl);
		//channel = chnl;
		// Save in EEPROM
		//if (save)
		//	EEPROM.write(EEPROM_FREQ_CHANNEL, chnl);
		//	
 		//byte reg = readReg(CC1101_CHANNR,CC1101_CONFIG_REGISTER);
 		//sData = "channel read " + String(reg,DEC);
 		//cDebug::Log(cDebug::LOG_D,sData);
//	}
}
*/
/**
 * setCarrierFreq
 * 
 * Set carrier frequency
 * 
 * 'freq'	New carrier frequency
 */
/*
void cCC1101_Main::setCarrierFreq(byte freq)
{

//	switch(freq)
//	{
// 		case CFREQ_915:
// 			cDebug::Log(cDebug::LOG_D,"freq: 915 MHz");
// 			writeReg(CC1101_FREQ2,  CC1101_DEFVAL_FREQ2_915);
// 			writeReg(CC1101_FREQ1,  CC1101_DEFVAL_FREQ1_915);
// 			writeReg(CC1101_FREQ0,  CC1101_DEFVAL_FREQ0_915);
// 			break;
//		case CFREQ_433:
			//cDebug::Log(cDebug::LOG_D,"freq: 433,92 MHz");
// 			if (nCurSettings==SETTING_CUL)
// 			{
// 				writeReg(CC1101_FREQ2,  CC1101_CULVAL_FREQ2_433);
// 				writeReg(CC1101_FREQ1,  CC1101_CULVAL_FREQ1_433);
// 				writeReg(CC1101_FREQ0,  CC1101_CULVAL_FREQ0_433);
// 			}
// 			else
//			{
				writeReg(CC1101_FREQ2,  CC1101_DEFVAL_FREQ2_433);
				writeReg(CC1101_FREQ1,  CC1101_DEFVAL_FREQ1_433);
				writeReg(CC1101_FREQ0,  CC1101_DEFVAL_FREQ0_433);
						
//			}
//			break;
//		default:
			//cDebug::Log(cDebug::LOG_E,"freq: 433 MHz");
//			writeReg(CC1101_FREQ2,  CC1101_DEFVAL_FREQ2_433);
//			writeReg(CC1101_FREQ1,  CC1101_DEFVAL_FREQ1_433);
//			writeReg(CC1101_FREQ0,  CC1101_DEFVAL_FREQ0_433);
//		break;
//	}
	
// 	byte reg2=readReg(CC1101_FREQ2,CC1101_CONFIG_REGISTER);
// 	byte reg1=readReg(CC1101_FREQ1,CC1101_CONFIG_REGISTER);
// 	byte reg0=readReg(CC1101_FREQ0,CC1101_CONFIG_REGISTER);	
 	
// 	String sData = "freq registers 0x" + String (reg2,HEX) + " 0x"+ String (reg1,HEX) + " 0x"+ String (reg0,HEX);
 	//cDebug::Log(cDebug::LOG_D,sData);
	
	//carrierFreq = freq;  
}
*/
/**
 * setPowerDownState
 * 
 * Put CC1101 into power-down state
 */
void cCC1101_Main::setPowerDownState() 
{
  // Comming from RX state, we need to enter the IDLE state first
  cmdStrobe(CC1101_SIDLE);
  // Enter Power-down state
  cmdStrobe(CC1101_SPWD);
}




/**
 * sendData
 * 
 * Send data packet via RF
 * 
 * 'packet'	Packet to be transmitted. First byte is the destination address
 *
 *  Return:
 *    True if the transmission succeeds
 *    False otherwise
 */
boolean cCC1101_Main::sendData(CCPACKET packet)
{
	byte marcState;
	bool res = false;
 
 	//String sData = "send: length = " + String(packet.length,DEC) + " Data ";
 	//cSerial::print(sData);

  	//for (int i=0; i<packet.length; i++)
  	//{
  	//	cSerial::print(String (packet.data[i],HEX));
  	//}
	//cSerial::println();

	//check GDO0
/*	if (getGDO0state())
	{
		cSerial::println("GDO0 is high");
	}
	else
	{
		cSerial::println("GDO0 is low");
	}
	*/
	
 
 
	// Declare to be in Tx state. This will avoid receiving packets whilst
	// transmitting
	rfState = RFSTATE_TX;

	// Enter RX state
// 	poDebug->Log(cDebug::LOG_D,"Enter RX state");
	//cSerial::println("Enter RX state");
	setRxState();
	//cSerial::println(" done");

	// Check that the RX state has been entered
	while (((marcState = readStatusReg(CC1101_MARCSTATE)) & 0x1F) != 0x0D)
	{
		//cSerial::print(marcState);
		if (marcState == 0x11)        // RX_OVERFLOW
			flushRxFifo();              // flush receive queue
	}
// 	marcState = readStatusReg(CC1101_MARCSTATE);
// 	String sTemp = "RX state 0x" + String(marcState,HEX);
// 	poDebug->Log(cDebug::LOG_D,sTemp);
	//cSerial::println("delay");

	//poDebug->Log("delay");
	delayMicroseconds(500);

	// Set data length at the first position of the TX FIFO
	writeReg(CC1101_TXFIFO,  packet.length);
	// Write data into the TX FIFO
	writeBurstReg(CC1101_TXFIFO, packet.data, packet.length);

// 	byte noofbytes = readStatusReg(CC1101_TXBYTES) & 0x7F;
// 	if( noofbytes != 0)
// 	{
// 		String sData = " bytes: " + String(noofbytes,DEC);
// 		poDebug->Log(cDebug::LOG_D,sData);
// 	}

	// CCA enabled: will enter TX state only if the channel is clear
// 	poDebug->Log(cDebug::LOG_D,"enter TX state");
	setTxState();

	// Check that TX state is being entered (state = RXTX_SETTLING)
	marcState = readStatusReg(CC1101_MARCSTATE) & 0x1F;
	if((marcState != 0x13) && (marcState != 0x14) && (marcState != 0x15))
	{
		//cDebug::Log(cDebug::LOG_E,"TX state not entered");
		setIdleState();       // Enter IDLE state
		flushTxFifo();        // Flush Tx FIFO
		setRxState();         // Back to RX state

		// Declare to be in Rx state
		rfState = RFSTATE_RX;
		return false;
	}

	// Wait for the sync word to be transmitted
	//poDebug->Log("Wait for the sync word to be transmitted");
	//cSerial::println("Wait for the sync word to be transmitted");
	
/*	
	if (getGDO0state())
	{
		cSerial::println("a");
	}
	else
	{
		cSerial::println("b");
	}
*/	
	wait_GDO0_high();

	// Wait until the end of the packet transmission
	//poDebug->Log("Wait until the end of the packet transmission");
	//cSerial::println("Wait until the end of the packet transmission");
/*	if (getGDO0state())
	{
		cSerial::println("c");
	}
	else
	{
		cSerial::println("d");
	}
*/
	wait_GDO0_low();

	// Check that the TX FIFO is empty
	if((readStatusReg(CC1101_TXBYTES) & 0x7F) == 0)
		res = true;

//	poDebug->Log(cDebug::LOG_D,"enter Idle state");
	setIdleState();       // Enter IDLE state
	flushTxFifo();        // Flush Tx FIFO

	// Enter back into RX state
//	poDebug->Log(cDebug::LOG_D,"enter RX state");
	setRxState();

	// Declare to be in Rx state
	rfState = RFSTATE_RX;

	//cSerial::println("done");
	return res;
}


/**
 * receiveData
 * 
 * Read data packet from RX FIFO
 *
 * 'packet'	Container for the packet received
 * 
 * Return:
 * 	Amount of bytes received
 */
byte cCC1101_Main::receiveData(CCPACKET * packet)
{
	byte val;
	byte rxBytes = readStatusReg(CC1101_RXBYTES);

	// Any byte waiting to be read and no overflow?
	if (rxBytes & 0x7F && !(rxBytes & 0x80))
	{
		// Read data length
		packet->length = readConfigReg(CC1101_RXFIFO);
		// If packet is too long
		if (packet->length > CC1101_DATA_LEN)
		{
			//cDebug::Log(cDebug::LOG_E,"E1");
			packet->length = 0;   // Discard packet
		}
		else
		{
			// Read data packet
			readBurstReg(packet->data, CC1101_RXFIFO, packet->length);
			// Read RSSI
			packet->rssi = readConfigReg(CC1101_RXFIFO);
			// Read LQI and CRC_OK
			val = readConfigReg(CC1101_RXFIFO);
			packet->lqi = val & 0x7F;
			packet->crc_ok = bitRead(val, 7);
		}
	}
	else
	{
		if ( (rxBytes & 0x80))
		{
			//cDebug::Log(cDebug::LOG_E,"E2");
		}
		packet->length = 0;
	}

  setIdleState();       // Enter IDLE state
  flushRxFifo();        // Flush Rx FIFO
  //cmdStrobe(CC1101_SCAL);

  // Back to RX state
  setRxState();

  return packet->length;
}

void cCC1101_Main::ReadAllRegister()
{	
//	poDebug->Log(cDebug::LOG_I,"IRQ counter " + String(nIRQCounter,DEC));
	
	//dump first 48 register as CUL
	String sTemp="";
	cSerial::println();
	 for(int i = 0; i < 0x30; i++) 
	 {
		 byte nReg;
		 nReg=readReg(i,CC1101_CONFIG_REGISTER);
		 sTemp+=String(nReg,HEX) + " ";
		 if((i&7) == 7)
		 {
			 //cDebug::Log(cDebug::LOG_I,sTemp);
			 cSerial::println(sTemp);
			 sTemp="";
		 }
	 }
}

#endif


/* original mit atmega 328
start init
SPI begin done
set clock done
set bit order done
reset

2e 2e 6 7 b5 47 3d 6
5 ff 0 8 0 10 a7 62
ca 83 93 22 f8 35 7 20
18 16 6c 43 40 91 87 6b
fb 56 10 e9 2a 0 1f 41
0 59 7f 3f 81 35 9 0
done
init done
Enter RX state done
delay

error 0 != 15 addr: ?
Wait for the sync word to be transmitted
Wait until the end of the packet transmission
crc not ok
data with DP=0
data with DP=2

*/

/* stand mit atmega 644

SPI begin done
set clock done
set bit order done
reset
error 51 != 46 addr:
error 152 != 46 addr:
error 145 != 6 addr:
error 17 != 7 addr:
error 17 != 61 addr:
error 34 != 6 addr:
error 102 != 5 addr:
error 34 != 181 addr:
error 153 != 71 addr:
error 34 != 255 addr:
error 102 != 0 addr:
error 153 != 8 addr:
error 153 != 0 addr:
error 17 != 16 addr:
error 17 != 167 addr:
error 34 != 98 addr:
error 17 != 202 addr:
error 17 != 131 addr:
error 34 != 147 addr:
error 102 != 34 addr:
error 34 != 248 addr:
error 34 != 53 addr:
error 51 != 7 addr:
error 17 != 32 addr:
error 153 != 24 addr:
error 34 != 22 addr:
error 17 != 108 addr:
error 34 != 67 addr:
rror 34 != 64 addr:
error 102 != 145 addr:
error 17 != 135 addr:
error 51 != 107 addr:
error 136 != 251 addr:
error 51 != 86 addr: !
error 17 != 16 addr: "
error 102 != 233 addr: #
error 153 != 42 addr: $
error 153 != 0 addr: %
error 34 != 31 addr: &
error 102 != 65 addr: '
error 17 != 0 addr: (
error 17 != 89 addr: )
error 51 != 127 addr: *
error 17 != 63 addr: +
error 51 != 129 addr: ,
error 34 != 53 addr: -
error 17 != 9 addr: .
88 11 22 99 99 88 11 33
22 98 99 99 99 99 99 99
88 33 22 98 89 33 22 99
88 33 22 99 9

soll
2e 2e 06 07 b5 47 3d 06
05 ff 00 08 00 10 a7 62
ca 83 93 22 f8 35 07 20
18 16 6c 43 40 91 87 6b
fb 56 10 e9 2a 00 1f 41
00 59 7f 3f 81 35 09 00

*/