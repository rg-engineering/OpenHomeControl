/**
 * Copyright (c) 2011 panStamp <contact@panstamp.com>
 * 
 * This file is part of the panStamp project.
 * 
 * panStamp  is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * any later version.
 * 
 * panStamp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with panStamp; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 
 * USA
 * 
 * Author: Daniel Berenguer
 * Creation date: 03/03/2011
 */

//Intertechno-part:
/* 
 * Copyright by O.Droegehorn / 
 *              DHS-Computertechnik GmbH
 * License: GPL v2
*/

//***************************************************************************
// in GeneralRFSensor-Project
// please read license.txt for licensing details
//***************************************************************************
#include "cc1101.h"
//#include "nvolat.h"
//#include "../Debug/Debug.h"
#include "../../board.h"

//we use standard arduino lib
//#include <SPI.h>
#include "spi.h"
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

#ifdef HAS_INTERTECHNO
/* Handle interrupt from CC1101 (INT1) gdo2 on pin3 */
int CC1101::nIRQCounter=0;
void cc1101_IRQ_Intertechno(void)
{
	//to do...
	CC1101::nIRQCounter++;
}
#endif

 /**
  * PATABLE
  */
//const byte paTable[8] = {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60};

/**
 * CC1101
 * 
 * Class constructor
 */
CC1101::CC1101(void)
{
	paTableByte = PA_LowPower;            // Priority = Low power


}

/**
 * wakeUp
 * 
 * Wake up CC1101 from Power Down state
 */
void CC1101::wakeUp(void)
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
void CC1101::writeReg(byte regAddr, byte value) 
{
#if defined (SPI_HAS_TRANSACTION)
	//SPI.beginTransaction(mySPISettings);
#endif
	cc1101_Select();                      // Select CC1101
	wait_Miso();                          // Wait until MISO goes low
	//SPI.transfer(regAddr);
	//SPI.transfer(value);
  spi.send(regAddr);                    // Send register address
  spi.send(value);                      // Send value
	cc1101_Deselect();                    // Deselect CC1101
 #if defined (SPI_HAS_TRANSACTION)
	//SPI.endTransaction();
 #endif
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
void CC1101::writeBurstReg(byte regAddr, byte* buffer, byte len)
{
  byte addr, i;

#if defined (SPI_HAS_TRANSACTION)
//SPI.beginTransaction(mySPISettings);
#endif
  addr = regAddr | WRITE_BURST;         // Enable burst transfer
  cc1101_Select();                      // Select CC1101
  wait_Miso();                          // Wait until MISO goes low
  //SPI.transfer(addr);
  spi.send(addr);                       // Send register address
  
  for(i=0 ; i<len ; i++)
	//SPI.transfer(buffer[i]);
    spi.send(buffer[i]);                // Send value

  cc1101_Deselect();                    // Deselect CC1101  
 #if defined (SPI_HAS_TRANSACTION)
 //SPI.endTransaction();
 #endif
}

/**
 * cmdStrobe
 * 
 * Send command strobe to the CC1101 IC via SPI
 * 
 * 'cmd'	Command strobe
 */     
void CC1101::cmdStrobe(byte cmd) 
{
#if defined (SPI_HAS_TRANSACTION)
//SPI.beginTransaction(mySPISettings);
#endif
	cc1101_Select();                      // Select CC1101
	wait_Miso();                          // Wait until MISO goes low
	//SPI.transfer(cmd);
	spi.send(cmd);                        // Send strobe command
	cc1101_Deselect();                    // Deselect CC1101
 #if defined (SPI_HAS_TRANSACTION)
 //SPI.endTransaction();
 #endif
}

byte CC1101::cmdStrobeRet(byte cmd)
{
#if defined (SPI_HAS_TRANSACTION)
//SPI.beginTransaction(mySPISettings);
#endif
	byte val;
	cc1101_Select();                      // Select CC1101
	wait_Miso();                          // Wait until MISO goes low
	//val=SPI.transfer(cmd);
	val=spi.send(cmd);                        // Send strobe command
	
	cc1101_Deselect();                    // Deselect CC1101
 #if defined (SPI_HAS_TRANSACTION)
 //SPI.endTransaction();
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
byte CC1101::readReg(byte regAddr, byte regType) 
{
  byte addr, val;
#if defined (SPI_HAS_TRANSACTION)
//SPI.beginTransaction(mySPISettings);
#endif
  addr = regAddr | regType;
  cc1101_Select();                      // Select CC1101
  wait_Miso();                          // Wait until MISO goes low
  //SPI.transfer(addr);
  //val=SPI.transfer(0x00);
  spi.send(addr);                       // Send register address
  val = spi.send(0x00);                 // Read result
  cc1101_Deselect();                    // Deselect CC1101
 #if defined (SPI_HAS_TRANSACTION)
 //SPI.endTransaction();
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
void CC1101::readBurstReg(byte * buffer, byte regAddr, byte len) 
{
  byte addr, i;
#if defined (SPI_HAS_TRANSACTION)
//SPI.beginTransaction(mySPISettings);
#endif  
  addr = regAddr | READ_BURST;
  cc1101_Select();                      // Select CC1101
  wait_Miso();                          // Wait until MISO goes low
  //SPI.transfer(addr);
  spi.send(addr);                       // Send register address
  for(i=0 ; i<len ; i++)
	//buffer[i]=SPI.transfer(0x00);
    buffer[i] = spi.send(0x00);         // Read result byte by byte
  cc1101_Deselect();                    // Deselect CC1101
 #if defined (SPI_HAS_TRANSACTION)
 //SPI.endTransaction();
 #endif
}

/**
 * reset
 * 
 * Reset CC1101
 */
void CC1101::reset(void) 
{
	cc1101_Deselect();                    // Deselect CC1101
	delayMicroseconds(5);
	cc1101_Select();                      // Select CC1101
	delayMicroseconds(10);
	cc1101_Deselect();                    // Deselect CC1101
	delayMicroseconds(41);
	cc1101_Select();                      // Select CC1101
#if defined (SPI_HAS_TRANSACTION)
//SPI.beginTransaction(mySPISettings);
#endif
	wait_Miso();                          // Wait until MISO goes low
	//SPI.transfer(CC1101_SRES);
	spi.send(CC1101_SRES);                // Send reset command strobe
	wait_Miso();                          // Wait until MISO goes low

	cc1101_Deselect();                    // Deselect CC1101
 #if defined (SPI_HAS_TRANSACTION)
 //SPI.endTransaction();
 #endif
	setDefaultRegs();                     // Reconfigure CC1101
	//setCULRegs();
//  setRegsFromEeprom();                  // Take user settings from EEPROM
	
}

/**
 * setDefaultRegs
 * 
 * Configure CC1101 registers
 */
void CC1101::setDefaultRegs(void) 
{
	//cDebug::Log(cDebug::LOG_I,"set default");
	
	pinMode(PIN_GDO0, INPUT);
	//GDO2 not used
	
	nCurSettings=SETTING_DEFAULT;
	writeReg(CC1101_IOCFG2,  CC1101_DEFVAL_IOCFG2);
	writeReg(CC1101_IOCFG1,  CC1101_DEFVAL_IOCFG1);
	writeReg(CC1101_IOCFG0,  CC1101_DEFVAL_IOCFG0);
	writeReg(CC1101_FIFOTHR,  CC1101_DEFVAL_FIFOTHR);
	writeReg(CC1101_PKTLEN,  CC1101_DEFVAL_PKTLEN);
	writeReg(CC1101_PKTCTRL1,  CC1101_DEFVAL_PKTCTRL1);
	writeReg(CC1101_PKTCTRL0,  CC1101_DEFVAL_PKTCTRL0);

  // Set default synchronization word
  setSyncWord(CC1101_DEFVAL_SYNC1, CC1101_DEFVAL_SYNC0, false);

  // Set default device address
  setDevAddress(CC1101_DEFVAL_ADDR, false);
  // Set default frequency channel
  setChannel(CC1101_DEFVAL_CHANNR, false);
  
  writeReg(CC1101_FSCTRL1,  CC1101_DEFVAL_FSCTRL1);
  writeReg(CC1101_FSCTRL0,  CC1101_DEFVAL_FSCTRL0);

  // Set default carrier frequency = 433 MHz
  setCarrierFreq(CFREQ_433);

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
}

/**
 * setDefaultRegs
 * 
 * Configure CC1101 registers
 */
#ifdef HAS_INTERTECHNO
void CC1101::setITRegs(void) 
{
	//cDebug::Log(cDebug::LOG_I,"set Intertechno");
	nCurSettings=SETTING_INTERTECHNO;
	
	pinMode(PIN_GDO0, OUTPUT); // TX input on CC1101
	pinMode(PIN_GDO2, INPUT); // RX output on CC1101
	
#if F_CPU >= 24000000L
// for the 24 MHz clock for the aventurous ones, trying to overclock
	cDebug::Log(cDebug::LOG_I,"compiled for 24 MHz clock");

#elif F_CPU >= 20000000L
// for the 20 MHz clock on rare Arduino boards
	cDebug::Log(cDebug::LOG_I,"compiled for 20 MHz clock");
	
#elif F_CPU >= 16000000L
// for the 16 MHz clock on most Arduino boards
	cDebug::Log(cDebug::LOG_I,"compiled for 16 MHz clock");

#elif F_CPU >= 12000000L
// for the 12 MHz clock if somebody is working with USB
	cDebug::Log(cDebug::LOG_I,"compiled for 12 MHz clock");
	
#elif F_CPU >= 8000000L
// for the 8 MHz internal clock
	cDebug::Log(cDebug::LOG_I,"compiled for 8 MHz clock");
#else
	cDebug::Log(cDebug::LOG_I,"compiled without clock setting");
#endif
	
	
	cc1101_Deselect();
	delay_us(30);
	cc1101_Select();
	delay_us(30);
	cc1101_Deselect();
	delay_us(45);
	
	cc1101_Select();
	spi.send(CC1101_SRES);
	cc1101_Deselect();
	
	delay_us(100);
	
	
	
/*
	writeReg(CC1101_IOCFG2,  CC1101_ITVAL_IOCFG2);
	writeReg(CC1101_IOCFG1,  CC1101_ITVAL_IOCFG1);
	writeReg(CC1101_IOCFG0,  CC1101_ITVAL_IOCFG0);
	writeReg(CC1101_FIFOTHR,  CC1101_ITVAL_FIFOTHR);
	writeReg(CC1101_PKTLEN,  CC1101_ITVAL_PKTLEN);
	writeReg(CC1101_PKTCTRL1,  CC1101_ITVAL_PKTCTRL1);
	writeReg(CC1101_PKTCTRL0,  CC1101_ITVAL_PKTCTRL0);

	// Set default synchronization word
	setSyncWord(CC1101_ITVAL_SYNC1, CC1101_ITVAL_SYNC0, false);

	// Set default device address
	setDevAddress(CC1101_ITVAL_ADDR, false);
	// Set default frequency channel
	setChannel(CC1101_ITVAL_CHANNR, false);
  
	writeReg(CC1101_FSCTRL1,  CC1101_ITVAL_FSCTRL1);
	writeReg(CC1101_FSCTRL0,  CC1101_ITVAL_FSCTRL0);

	// Set default carrier frequency = 868 MHz
	setCarrierFreq(CFREQ_433);

	writeReg(CC1101_MDMCFG4,  CC1101_ITVAL_MDMCFG4);
	writeReg(CC1101_MDMCFG3,  CC1101_ITVAL_MDMCFG3);
	writeReg(CC1101_MDMCFG2,  CC1101_ITVAL_MDMCFG2);
	writeReg(CC1101_MDMCFG1,  CC1101_ITVAL_MDMCFG1);
	writeReg(CC1101_MDMCFG0,  CC1101_ITVAL_MDMCFG0);
	writeReg(CC1101_DEVIATN,  CC1101_ITVAL_DEVIATN);
	writeReg(CC1101_MCSM2,  CC1101_ITVAL_MCSM2);
	writeReg(CC1101_MCSM1,  CC1101_ITVAL_MCSM1);
	writeReg(CC1101_MCSM0,  CC1101_ITVAL_MCSM0);
	writeReg(CC1101_FOCCFG,  CC1101_ITVAL_FOCCFG);
	writeReg(CC1101_BSCFG,  CC1101_ITVAL_BSCFG);
	writeReg(CC1101_AGCCTRL2,  CC1101_ITVAL_AGCCTRL2);
	writeReg(CC1101_AGCCTRL1,  CC1101_ITVAL_AGCCTRL1);
	writeReg(CC1101_AGCCTRL0,  CC1101_ITVAL_AGCCTRL0);
	writeReg(CC1101_WOREVT1,  CC1101_ITVAL_WOREVT1);
	writeReg(CC1101_WOREVT0,  CC1101_ITVAL_WOREVT0);
	writeReg(CC1101_WORCTRL,  CC1101_ITVAL_WORCTRL);
	writeReg(CC1101_FREND1,  CC1101_ITVAL_FREND1);
	writeReg(CC1101_FREND0,  CC1101_ITVAL_FREND0);
	writeReg(CC1101_FSCAL3,  CC1101_ITVAL_FSCAL3);
	writeReg(CC1101_FSCAL2,  CC1101_ITVAL_FSCAL2);
	writeReg(CC1101_FSCAL1,  CC1101_ITVAL_FSCAL1);
	writeReg(CC1101_FSCAL0,  CC1101_ITVAL_FSCAL0);
	writeReg(CC1101_RCCTRL1,  CC1101_ITVAL_RCCTRL1);
	writeReg(CC1101_RCCTRL0,  CC1101_ITVAL_RCCTRL0);
	writeReg(CC1101_FSTEST,  CC1101_ITVAL_FSTEST);
	writeReg(CC1101_PTEST,  CC1101_ITVAL_PTEST);
	writeReg(CC1101_AGCTEST,  CC1101_ITVAL_AGCTEST);
	writeReg(CC1101_TEST2,  CC1101_ITVAL_TEST2);
	writeReg(CC1101_TEST1,  CC1101_ITVAL_TEST1);
	writeReg(CC1101_TEST0,  CC1101_ITVAL_TEST0);
*/

	writeReg(CC1101_IOCFG2,CC1101_ITVAL_IOCFG2);
	writeReg(CC1101_IOCFG1,CC1101_ITVAL_IOCFG1);
	writeReg(CC1101_IOCFG0,CC1101_ITVAL_IOCFG0);
	writeReg(CC1101_FIFOTHR,CC1101_ITVAL_FIFOTHR);
	writeReg(CC1101_SYNC1,CC1101_ITVAL_SYNC1);
	writeReg(CC1101_SYNC0,CC1101_ITVAL_SYNC0);
	writeReg(CC1101_PKTLEN,CC1101_ITVAL_PKTLEN);
	writeReg(CC1101_PKTCTRL1,CC1101_ITVAL_PKTCTRL1);
	writeReg(CC1101_PKTCTRL0,CC1101_ITVAL_PKTCTRL0);
	writeReg(CC1101_ADDR,CC1101_ITVAL_ADDR);
	writeReg(CC1101_CHANNR,CC1101_ITVAL_CHANNR);		
	writeReg(CC1101_FSCTRL1,CC1101_ITVAL_FSCTRL1);
	writeReg(CC1101_FSCTRL0,CC1101_ITVAL_FSCTRL0);
	writeReg(CC1101_FREQ2,CC1101_ITVAL_FREQ2_433);	
	writeReg(CC1101_FREQ1,CC1101_ITVAL_FREQ1_433);
	writeReg(CC1101_FREQ0,CC1101_ITVAL_FREQ0_433);
	

	writeReg(CC1101_MDMCFG4,CC1101_ITVAL_MDMCFG4);
	writeReg(CC1101_MDMCFG3,CC1101_ITVAL_MDMCFG3);
	writeReg(CC1101_MDMCFG2,CC1101_ITVAL_MDMCFG2);
	writeReg(CC1101_MDMCFG1,CC1101_ITVAL_MDMCFG1);
	writeReg(CC1101_MDMCFG0,CC1101_ITVAL_MDMCFG0);
	writeReg(CC1101_DEVIATN,CC1101_ITVAL_DEVIATN);
	writeReg(CC1101_MCSM2,CC1101_ITVAL_MCSM2);
	writeReg(CC1101_MCSM1,CC1101_ITVAL_MCSM1);
	writeReg(CC1101_MCSM0,CC1101_ITVAL_MCSM0);
	writeReg(CC1101_FOCCFG,CC1101_ITVAL_FOCCFG);
	writeReg(CC1101_BSCFG,CC1101_ITVAL_BSCFG);
	writeReg(CC1101_AGCCTRL2,CC1101_ITVAL_AGCCTRL2);
	writeReg(CC1101_AGCCTRL1,CC1101_ITVAL_AGCCTRL1);
	writeReg(CC1101_AGCCTRL0,CC1101_ITVAL_AGCCTRL0);
	writeReg(CC1101_WOREVT1,CC1101_ITVAL_WOREVT1);
	writeReg(CC1101_WOREVT0,CC1101_ITVAL_WOREVT0);
	writeReg(CC1101_WORCTRL,CC1101_ITVAL_WORCTRL);
	writeReg(CC1101_FREND1,CC1101_ITVAL_FREND1);
	writeReg(CC1101_FREND0,CC1101_ITVAL_FREND0);
	writeReg(CC1101_FSCAL3,CC1101_ITVAL_FSCAL3);
	writeReg(CC1101_FSCAL2,CC1101_ITVAL_FSCAL2);
	writeReg(CC1101_FSCAL1,CC1101_ITVAL_FSCAL1);
	writeReg(CC1101_FSCAL0,CC1101_ITVAL_FSCAL0);
	writeReg(CC1101_RCCTRL1,CC1101_ITVAL_RCCTRL1);
	writeReg(CC1101_RCCTRL0,CC1101_ITVAL_RCCTRL0);
	
	it_interval = 420;
	itv3_start_bit = 235;
	itv3_bit = 275;
	itv3_latch = 2650;
	itv3_low = 1180;
	itv3_sync = 10000;
	
	paTableByte=PA_LowPower;
	writeReg(CC1101_PATABLE, paTableByte);
		
	String sTemp="paTableByte = 0x"+String(paTableByte,HEX);
	cDebug::Log(cDebug::LOG_I,sTemp);
	
	cc1101_Select();
	spi.send(CC1101_SCAL);
	cc1101_Deselect();

	AttachIRQ_IT();		
}
#endif

/*
void CC1101::setCULRegs(void) 
{
	cDebug::Log(cDebug::LOG_I,"set CUL");
	
	pinMode(PIN_GDO0, INPUT);
	//GDO2 not used
	
	nCurSettings=SETTING_CUL;
	writeReg(CC1101_IOCFG2,  CC1101_CULVAL_IOCFG2);
	writeReg(CC1101_IOCFG1,  CC1101_CULVAL_IOCFG1);
	writeReg(CC1101_IOCFG0,  CC1101_CULVAL_IOCFG0);
	writeReg(CC1101_FIFOTHR,  CC1101_CULVAL_FIFOTHR);
	writeReg(CC1101_PKTLEN,  CC1101_CULVAL_PKTLEN);
	writeReg(CC1101_PKTCTRL1,  CC1101_CULVAL_PKTCTRL1);
	writeReg(CC1101_PKTCTRL0,  CC1101_CULVAL_PKTCTRL0);

	// Set default synchronization word
	setSyncWord(CC1101_CULVAL_SYNC1, CC1101_CULVAL_SYNC0, false);

	// Set default device address
	setDevAddress(CC1101_CULVAL_ADDR, false);
	// Set default frequency channel
	setChannel(CC1101_CULVAL_CHANNR, false);
	
	writeReg(CC1101_FSCTRL1,  CC1101_CULVAL_FSCTRL1);
	writeReg(CC1101_FSCTRL0,  CC1101_CULVAL_FSCTRL0);

	// Set default carrier frequency = 433 MHz
	setCarrierFreq(CFREQ_433);

	writeReg(CC1101_MDMCFG4,  CC1101_CULVAL_MDMCFG4);
	writeReg(CC1101_MDMCFG3,  CC1101_CULVAL_MDMCFG3);
	writeReg(CC1101_MDMCFG2,  CC1101_CULVAL_MDMCFG2);
	writeReg(CC1101_MDMCFG1,  CC1101_CULVAL_MDMCFG1);
	writeReg(CC1101_MDMCFG0,  CC1101_CULVAL_MDMCFG0);
	writeReg(CC1101_DEVIATN,  CC1101_CULVAL_DEVIATN);
	writeReg(CC1101_MCSM2,  CC1101_CULVAL_MCSM2);
	writeReg(CC1101_MCSM1,  CC1101_CULVAL_MCSM1);
	writeReg(CC1101_MCSM0,  CC1101_CULVAL_MCSM0);
	writeReg(CC1101_FOCCFG,  CC1101_CULVAL_FOCCFG);
	writeReg(CC1101_BSCFG,  CC1101_CULVAL_BSCFG);
	writeReg(CC1101_AGCCTRL2,  CC1101_CULVAL_AGCCTRL2);
	writeReg(CC1101_AGCCTRL1,  CC1101_CULVAL_AGCCTRL1);
	writeReg(CC1101_AGCCTRL0,  CC1101_CULVAL_AGCCTRL0);
	writeReg(CC1101_WOREVT1,  CC1101_CULVAL_WOREVT1);
	writeReg(CC1101_WOREVT0,  CC1101_CULVAL_WOREVT0);
	writeReg(CC1101_WORCTRL,  CC1101_CULVAL_WORCTRL);
	writeReg(CC1101_FREND1,  CC1101_CULVAL_FREND1);
	writeReg(CC1101_FREND0,  CC1101_CULVAL_FREND0);
	writeReg(CC1101_FSCAL3,  CC1101_CULVAL_FSCAL3);
	writeReg(CC1101_FSCAL2,  CC1101_CULVAL_FSCAL2);
	writeReg(CC1101_FSCAL1,  CC1101_CULVAL_FSCAL1);
	writeReg(CC1101_FSCAL0,  CC1101_CULVAL_FSCAL0);
	writeReg(CC1101_RCCTRL1,  CC1101_CULVAL_RCCTRL1);
	writeReg(CC1101_RCCTRL0,  CC1101_CULVAL_RCCTRL0);
	writeReg(CC1101_FSTEST,  CC1101_CULVAL_FSTEST);
	writeReg(CC1101_PTEST,  CC1101_CULVAL_PTEST);
	writeReg(CC1101_AGCTEST,  CC1101_CULVAL_AGCTEST);
	writeReg(CC1101_TEST2,  CC1101_CULVAL_TEST2);
	writeReg(CC1101_TEST1,  CC1101_CULVAL_TEST1);
	writeReg(CC1101_TEST0,  CC1101_CULVAL_TEST0);

}
*/

/**
 * init
 * 
 * Initialize CC1101
 */
void CC1101::init(void) 
{
	//cDebug::Log(cDebug::LOG_D,"start init");
	spi.init();                           // Initialize SPI interface
	//SPI.begin();
	//mySPISettings = SPISettings(8000000, MSBFIRST, SPI_MODE0);
	
	//spi.setClockDivider(SPI_CLOCK_DIV16);
	//spi.setBitOrder(MSBFIRST);
	pinMode(PIN_GDO0, INPUT);                 // Config GDO0 as input
#ifdef HAS_INTERTECHNO
	DetachIRQ_IT();
#endif
	reset();                              // Reset CC1101

	// Configure PATABLE
	//writeBurstReg(CC1101_PATABLE, (byte*)paTable, 8);
	writeReg(CC1101_PATABLE, paTableByte);
	
	String sTemp="paTableByte = 0x"+String(paTableByte,HEX);
	//cDebug::Log(cDebug::LOG_I,sTemp);
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
void CC1101::setSyncWord(uint8_t syncH, uint8_t syncL, bool save) 
{
//	if ((syncWord[0] != syncH) || (syncWord[1] != syncL))
	{
		//String sData = "sync word 0x" + String(syncH,HEX) + " 0x" + String(syncL,HEX);
		//Serial.print(sData);
		//cDebug::Log(cDebug::LOG_D,sData);
		
		writeReg(CC1101_SYNC1, syncH);
		writeReg(CC1101_SYNC0, syncL);
// 		syncWord[0] = syncH;
// 		syncWord[1] = syncL;
		// Save in EEPROM
		/*if (save)
		{
			EEPROM.write(EEPROM_SYNC_WORD, syncH);
			EEPROM.write(EEPROM_SYNC_WORD + 1, syncL);
		}*/
		
 		//byte regH = readReg(CC1101_SYNC1,CC1101_CONFIG_REGISTER);
 		//byte regL = readReg(CC1101_SYNC0,CC1101_CONFIG_REGISTER);
 		//sData = "  sync word read 0x" + String(regH,HEX) + " 0x" + String(regL,HEX);
		//Serial.println(sData);
 		//cDebug::Log(cDebug::LOG_D,sData);
	}
// 	else
// 	{
// 		cDebug::Log(cDebug::LOG_D,"syncword not set");
// 	}
}

/**
 * setSyncWord (overriding method)
 * 
 * Set synchronization word
 * 
 * 'syncH'	Synchronization word - pointer to 2-byte array
 * 'save' If TRUE, save parameter in EEPROM
 */
void CC1101::setSyncWord(byte *sync, bool save) 
{
  CC1101::setSyncWord(sync[0], sync[1], save);
}

/**
 * setDevAddress
 * 
 * Set device address
 * 
 * 'addr'	Device address
 * 'save' If TRUE, save parameter in EEPROM
 */
void CC1101::setDevAddress(byte addr, bool save) 
{
//	if (devAddress != addr)
	{
		String sData = "dev address set " + String(addr,DEC);
		//cDebug::Log(cDebug::LOG_D,sData);
		writeReg(CC1101_ADDR, addr);
//		devAddress = addr;
		// Save in EEPROM
		/*if (save)
			EEPROM.write(EEPROM_DEVICE_ADDR, addr);  
			*/
		byte reg = readReg(CC1101_ADDR,CC1101_CONFIG_REGISTER);
		sData = "dev address read " + String(reg,DEC);
		//cDebug::Log(cDebug::LOG_D,sData);
  }
}

/**
 * setChannel
 * 
 * Set frequency channel
 * 
 * 'chnl'	Frequency channel
 * 'save' If TRUE, save parameter in EEPROM
 */
void CC1101::setChannel(byte chnl, bool save) 
{
	//if (channel != chnl)
	{
		String sData = "channel set " + String(chnl,DEC);
		//cDebug::Log(cDebug::LOG_D,sData);
		writeReg(CC1101_CHANNR,  chnl);
		//channel = chnl;
		// Save in EEPROM
		/*if (save)
			EEPROM.write(EEPROM_FREQ_CHANNEL, chnl);
			*/
 		byte reg = readReg(CC1101_CHANNR,CC1101_CONFIG_REGISTER);
 		sData = "channel read " + String(reg,DEC);
 		//cDebug::Log(cDebug::LOG_D,sData);
	}
}

/**
 * setCarrierFreq
 * 
 * Set carrier frequency
 * 
 * 'freq'	New carrier frequency
 */
void CC1101::setCarrierFreq(byte freq)
{

	switch(freq)
	{
// 		case CFREQ_915:
// 			cDebug::Log(cDebug::LOG_D,"freq: 915 MHz");
// 			writeReg(CC1101_FREQ2,  CC1101_DEFVAL_FREQ2_915);
// 			writeReg(CC1101_FREQ1,  CC1101_DEFVAL_FREQ1_915);
// 			writeReg(CC1101_FREQ0,  CC1101_DEFVAL_FREQ0_915);
// 			break;
		case CFREQ_433:
			//cDebug::Log(cDebug::LOG_D,"freq: 433,92 MHz");
// 			if (nCurSettings==SETTING_CUL)
// 			{
// 				writeReg(CC1101_FREQ2,  CC1101_CULVAL_FREQ2_433);
// 				writeReg(CC1101_FREQ1,  CC1101_CULVAL_FREQ1_433);
// 				writeReg(CC1101_FREQ0,  CC1101_CULVAL_FREQ0_433);
// 			}
// 			else
			{
				writeReg(CC1101_FREQ2,  CC1101_DEFVAL_FREQ2_433);
				writeReg(CC1101_FREQ1,  CC1101_DEFVAL_FREQ1_433);
				writeReg(CC1101_FREQ0,  CC1101_DEFVAL_FREQ0_433);
						
			}
			break;
		default:
			//cDebug::Log(cDebug::LOG_E,"freq: 433 MHz");
			writeReg(CC1101_FREQ2,  CC1101_DEFVAL_FREQ2_433);
			writeReg(CC1101_FREQ1,  CC1101_DEFVAL_FREQ1_433);
			writeReg(CC1101_FREQ0,  CC1101_DEFVAL_FREQ0_433);
		break;
	}
	
 	byte reg2=readReg(CC1101_FREQ2,CC1101_CONFIG_REGISTER);
 	byte reg1=readReg(CC1101_FREQ1,CC1101_CONFIG_REGISTER);
 	byte reg0=readReg(CC1101_FREQ0,CC1101_CONFIG_REGISTER);	
 	
 	String sData = "freq registers 0x" + String (reg2,HEX) + " 0x"+ String (reg1,HEX) + " 0x"+ String (reg0,HEX);
 	//cDebug::Log(cDebug::LOG_D,sData);
	
	//carrierFreq = freq;  
}

/**
 * setRegsFromEeprom
 * 
 * Set registers from EEPROM
 */
 /*
void CC1101::setRegsFromEeprom(void)
{
	byte bVal;
	byte arrV[2];

	cDebug::Log(cDebug::LOG_D,"set register from eeprom");

	// Read RF channel from EEPROM
	bVal = EEPROM.read(EEPROM_FREQ_CHANNEL);
	// Set RF channel
	if (bVal < NUMBER_OF_FCHANNELS )
	{
		//cDebug::Log(cDebug::LOG_D,"set channel from eeprom");
		setChannel(bVal, false);
	}
	// Read Sync word from EEPROM
	arrV[0] = EEPROM.read(EEPROM_SYNC_WORD);
	arrV[1] = EEPROM.read(EEPROM_SYNC_WORD + 1);
	// Set Sync word. 0x00 and 0xFF values are not allowed
	if (((arrV[0] != 0x00) && (arrV[0] != 0xFF)) || ((arrV[1] != 0x00) && (arrV[1] != 0xFF)))
	{
		//cDebug::Log(cDebug::LOG_D,"set sync word from eeprom");
		setSyncWord(arrV[0], arrV[1], false);
	}
	// Read device address from EEPROM
	bVal = EEPROM.read(EEPROM_DEVICE_ADDR);
	// Set device address
	if (bVal > 0)
	{
		//cDebug::Log(cDebug::LOG_D,"set device address eeprom");
		setDevAddress(bVal, false);
	}
}
*/
/**
 * setPowerDownState
 * 
 * Put CC1101 into power-down state
 */
void CC1101::setPowerDownState() 
{
  // Comming from RX state, we need to enter the IDLE state first
  cmdStrobe(CC1101_SIDLE);
  // Enter Power-down state
  cmdStrobe(CC1101_SPWD);
}


boolean CC1101::sendData(CCPACKET packet)
{

	if (nCurSettings==SETTING_DEFAULT)
	{
		//cDebug::Log(cDebug::LOG_D,"send data default");
		return sendData_default(packet);
	}
#ifdef HAS_INTERTECHNO
	else
	{
		//cDebug::Log(cDebug::LOG_D,"send data IT");
		return sendData_Intertechno(packet);
	}
#endif
	return false;
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
boolean CC1101::sendData_default(CCPACKET packet)
{
	byte marcState;
	bool res = false;
 
// 	String sData = "Data to send: length = " + String(packet.length,DEC);
// 	poDebug->Log(cDebug::LOG_I,sData);
// 	sData="Data to send: Data";
// 	poDebug->Log(sData);
//  	for (int i=0; i<packet.length; i++)
//  	{
//  		poDebug->Log(String (packet.data[i],HEX));
//  	}

 
	// Declare to be in Tx state. This will avoid receiving packets whilst
	// transmitting
	rfState = RFSTATE_TX;

	// Enter RX state
// 	poDebug->Log(cDebug::LOG_D,"Enter RX state");
	setRxState();

	// Check that the RX state has been entered
	while (((marcState = readStatusReg(CC1101_MARCSTATE)) & 0x1F) != 0x0D)
	{
		if (marcState == 0x11)        // RX_OVERFLOW
			flushRxFifo();              // flush receive queue
	}
// 	marcState = readStatusReg(CC1101_MARCSTATE);
// 	String sTemp = "RX state 0x" + String(marcState,HEX);
// 	poDebug->Log(cDebug::LOG_D,sTemp);


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
	wait_GDO0_high();

	// Wait until the end of the packet transmission
	//poDebug->Log("Wait until the end of the packet transmission");
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

	return res;
}

#ifdef HAS_INTERTECHNO
boolean CC1101::sendData_Intertechno(CCPACKET packet)
{
	cDebug::Log(cDebug::LOG_I,"send data in IT mode");
	

  	cmdStrobe(CC1101_SIDLE);
  	cmdStrobe(CC1101_SFRX );
  	cmdStrobe(CC1101_SFTX );
	
	cmdStrobe(CC1101_SIDLE);
	unsigned int cnt = 0xff;
	while(cnt-- && (cmdStrobeRet(CC1101_STX) & CC1101_STATUS_STATE_BM) != CC1101_STATE_TX)
	  delay_us(10);


	int sizeOfPackage = packet.length; // IT-V1 = 14, IT-V3 = 33, IT-V3-Dimm = 37
	int mode = 0; // IT V1

	if (sizeOfPackage == 33 || sizeOfPackage == 37) 
	{
		mode = 1; // IT V3
	}
	for(int i = 0; i < packet.repetition; i++)  
	{
		if (packet.datatype == DATATYPE_IT) 
		{
			if (mode == 1) 
			{
				send_IT_sync_V3();
				send_IT_latch_V3();
			} 
			else 
			{
				send_IT_sync();
			}
		}


		for(int j = 0; j < sizeOfPackage; j++)  
		{
			if(packet.data[j] == 0) 
			{
				if (packet.datatype == DATATYPE_IT) 
				{
					if (mode == 1) 
					{
						send_IT_bit_V3(0);
					} 
					else 
					{
						send_IT_bit(0);
					}
				}
			} 
			else if (packet.data[j] == 1) 
			{
				if (packet.datatype == DATATYPE_IT) 
				{
					if (mode == 1) 
					{
						send_IT_bit_V3(1);
					} 
					else 
					{
						send_IT_bit(1);
					}
				}
			} 
			else if (packet.data[j] == 2) 
			{
				send_IT_bit_V3(2);
			} 
			else 
			{
				if (mode == 1) 
				{
					send_IT_bit_V3(3);
				} 
				else 
				{
					send_IT_bit(2);
				}
			}
		}
	} //Do it n Times

	poDebug->Log(cDebug::LOG_D,"enter Idle state");
	setIdleState();       // Enter IDLE state
	flushTxFifo();        // Flush Tx FIFO

	// Enter back into RX state
	//poDebug->Log(cDebug::LOG_D,"enter RX state");
	setRxState();

	// Declare to be in Rx state
	rfState = RFSTATE_RX;
	//poDebug->Log(cDebug::LOG_D,"transmit done");
	
	String sTemp;
	for(int j = 0; j < sizeOfPackage; j++)  
	{
		if(packet.data[j] == 0) 
		{
			sTemp+='0';
		} 
		else if (packet.data[j] == 1) 
		{
			sTemp+='1';
		} 
		else if (packet.data[j] == 2) 
		{
			sTemp+='2';
		} 
		else 
		{
			if (packet.datatype == DATATYPE_IT) 
			{
				if (mode == 1) 
				{
					sTemp+='D';
				} 
				else 
				{
					sTemp+='F';
				}
			}
		}
	}
	poDebug->Log(cDebug::LOG_D,sTemp);
	
	
	return true;
}
#endif

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
byte CC1101::receiveData(CCPACKET * packet)
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


#ifdef HAS_INTERTECHNO
void CC1101::send_IT_sync()
{
	// Sync-Bit for IT V1 send before package
	cc1101_Set_Out;         // High
	delay_us(it_interval);
	cc1101_Clear_Out;       // Low
	for(int k = 0; k < 31; k++)
	{
		delay_us(it_interval);
	}
}

void CC1101::send_IT_sync_V3()
{
	//int8_t k;
	cc1101_Set_Out;         // High
	delay_us(itv3_start_bit);
	cc1101_Clear_Out;       // Low
	//for(k = 0; k < 40; k++)  {
	delay_us(itv3_sync);
	//}	
}

void CC1101::send_IT_latch_V3()
{
	// int8_t k;
	cc1101_Set_Out;         // High
	delay_us(itv3_bit);
	cc1101_Clear_Out;       // Low
	//for(k = 0; k < 10; k++)  {
	delay_us(itv3_latch);
	//}
}

void CC1101::send_IT_bit_V3(short bit)
{
	if (bit == 1) 
	{
		cc1101_Set_Out;         // High
		delay_us(itv3_bit);
		cc1101_Clear_Out;       // Low
		delay_us(itv3_low);

		cc1101_Set_Out;         // High
		delay_us(itv3_bit);
		cc1101_Clear_Out;       // Low
		delay_us(itv3_bit);
	} 
	else if (bit == 0) 
	{
		cc1101_Set_Out;         // High
		delay_us(itv3_bit);
		cc1101_Clear_Out;       // Low
		delay_us(itv3_bit);

		cc1101_Set_Out;         // High
		delay_us(itv3_bit);
		cc1101_Clear_Out;       // Low
		delay_us(itv3_low);
	} 
	else if (bit == 2) 
	{
		cc1101_Set_Out;         // High
		delay_us(itv3_bit);
		cc1101_Clear_Out;       // Low
		delay_us(itv3_low);

		cc1101_Set_Out;         // High
		delay_us(itv3_bit);
		cc1101_Clear_Out;       // Low
		delay_us(itv3_low);
	} 
	else 
	{
		cc1101_Set_Out;         // High
		delay_us(itv3_bit);
		cc1101_Clear_Out;       // Low
		delay_us(itv3_bit);

		cc1101_Set_Out;         // High
		delay_us(itv3_bit);
		cc1101_Clear_Out;       // Low
		delay_us(itv3_bit);
	}
}

void CC1101::send_IT_bit(short bit)
{
	if (bit == 1) 
	{
		cc1101_Set_Out;         // High
		delay_us(it_interval * 3);
		cc1101_Clear_Out;       // Low
		delay_us(it_interval);

		cc1101_Set_Out;         // High
		delay_us(it_interval * 3);
		cc1101_Clear_Out;       // Low
		delay_us(it_interval);
	} 
	else if (bit == 0) 
	{
		cc1101_Set_Out;         // High
		delay_us(it_interval);
		cc1101_Clear_Out;       // Low
		delay_us(it_interval * 3);

		cc1101_Set_Out;         // High
		delay_us(it_interval);
		cc1101_Clear_Out;       // Low
		delay_us(it_interval * 3);
	} 
	else 
	{
		cc1101_Set_Out;         // High
		delay_us(it_interval);
		cc1101_Clear_Out;       // Low
		delay_us(it_interval * 3);

		cc1101_Set_Out;         // High
		delay_us(it_interval * 3);
		cc1101_Clear_Out;       // Low
		delay_us(it_interval);
	}	
}


void CC1101::delay_us( uint16_t d )
{
	d<<=2;                // 4 cycles/loop, we are running 16MHz
	_delay_loop_2(d);
}
#endif

// void ccTX(void)
// {
// 	uint8_t cnt = 0xff;
// 	EIMSK  &= ~_BV(CC1100_INT);
// 	// Going from RX to TX does not work if there was a reception less than 0.5
// 	// sec ago. Due to CCA? Using IDLE helps to shorten this period(?)
// 	ccStrobe(CC1100_SIDLE);
// 	while(cnt-- &&
// 		(ccStrobe(CC1100_STX) & CC1100_STATUS_STATE_BM) != CC1100_STATE_TX)
// 	my_delay_us(10);
// }


void CC1101::ReadAllRegister()
{	
//	poDebug->Log(cDebug::LOG_I,"IRQ counter " + String(nIRQCounter,DEC));
	
	//dump first 48 register as CUL
	String sTemp="";
	 for(int i = 0; i < 0x30; i++) 
	 {
		 byte nReg;
		 nReg=readReg(i,CC1101_CONFIG_REGISTER);
		 sTemp+=String(nReg,HEX) + " ";
		 if((i&7) == 7)
		 {
			 //cDebug::Log(cDebug::LOG_I,sTemp);
			 sTemp="";
		 }
	 }
	
	
/*	
	byte sRet=0;

	sRet=readReg(CC1101_IOCFG2,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"IOCFG2 " + String(sRet,HEX));
	sRet=readReg(CC1101_IOCFG1,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"IOCFG1 " + String(sRet,HEX));	
	sRet=readReg(CC1101_IOCFG0,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"IOCFG0 " + String(sRet,HEX));
	sRet=readReg(CC1101_FIFOTHR,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"FIFOTHR " + String(sRet,HEX));
	sRet=readReg(CC1101_SYNC1,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"SYNC1 " + String(sRet,HEX));
	sRet=readReg(CC1101_SYNC0,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"SYNC0 " + String(sRet,HEX));
	sRet=readReg(CC1101_PKTLEN,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"PKTLEN " + String(sRet,HEX));
	sRet=readReg(CC1101_PKTCTRL1,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"PKTCTRL1 " + String(sRet,HEX));
	sRet=readReg(CC1101_PKTCTRL0,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"PKTCTRL0 " + String(sRet,HEX));
	sRet=readReg(CC1101_ADDR,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"ADDR " + String(sRet,HEX));
	sRet=readReg(CC1101_CHANNR,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"CHANNR " + String(sRet,HEX));
	sRet=readReg(CC1101_FSCTRL1,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"FSCTRL1 " + String(sRet,HEX));
	sRet=readReg(CC1101_FSCTRL0,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"FSCTRL0 " + String(sRet,HEX));
	sRet=readReg(CC1101_FREQ2,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"FREQ2 " + String(sRet,HEX));
	sRet=readReg(CC1101_FREQ1,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"FREQ1 " + String(sRet,HEX));
	sRet=readReg(CC1101_FREQ0,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"FREQ0 " + String(sRet,HEX));
*/

/*
	sRet=readReg(CC1101_MDMCFG4,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"MDMCFG4 " + String(sRet,HEX));
	sRet=readReg(CC1101_MDMCFG3,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"MDMCFG3 " + String(sRet,HEX));
	sRet=readReg(CC1101_MDMCFG2,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"MDMCFG2 " + String(sRet,HEX));
	sRet=readReg(CC1101_MDMCFG1,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"MDMCFG1 " + String(sRet,HEX));
	sRet=readReg(CC1101_MDMCFG0,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"MDMCFG0 " + String(sRet,HEX));
	sRet=readReg(CC1101_DEVIATN,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"DEVIATN " + String(sRet,HEX));
	sRet=readReg(CC1101_MCSM2,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"MCSM2 " + String(sRet,HEX));
	sRet=readReg(CC1101_MCSM1,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"MCSM1 " + String(sRet,HEX));
	sRet=readReg(CC1101_MCSM0,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"MCSM0 " + String(sRet,HEX));
	sRet=readReg(CC1101_FOCCFG,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"FOCCFG " + String(sRet,HEX));
	sRet=readReg(CC1101_BSCFG,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"BSCFG " + String(sRet,HEX));
	sRet=readReg(CC1101_AGCCTRL2,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"AGCCTRL2 " + String(sRet,HEX));
	sRet=readReg(CC1101_AGCCTRL1,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"AGCCTRL1 " + String(sRet,HEX));
	sRet=readReg(CC1101_AGCCTRL0,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"AGCCTRL0 " + String(sRet,HEX));
	sRet=readReg(CC1101_WOREVT1,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"WOREVT1 " + String(sRet,HEX));
	sRet=readReg(CC1101_WOREVT0,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"WOREVT0 " + String(sRet,HEX));
	sRet=readReg(CC1101_WORCTRL,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"WORCTRL " + String(sRet,HEX));
	sRet=readReg(CC1101_FREND1,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"FREND1 " + String(sRet,HEX));
	sRet=readReg(CC1101_FREND0,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"FREND0 " + String(sRet,HEX));
	sRet=readReg(CC1101_FSCAL3,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"FSCAL3 " + String(sRet,HEX));
	sRet=readReg(CC1101_FSCAL2,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"FSCAL2 " + String(sRet,HEX));
	sRet=readReg(CC1101_FSCAL1,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"FSCAL1 " + String(sRet,HEX));
	sRet=readReg(CC1101_FSCAL0,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"FSCAL0 " + String(sRet,HEX));
	sRet=readReg(CC1101_RCCTRL1,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"RCCTRL1 " + String(sRet,HEX));
	sRet=readReg(CC1101_RCCTRL0,CC1101_CONFIG_REGISTER);
	poDebug->Log(cDebug::LOG_I,"RCCTRL0 " + String(sRet,HEX));
*/


}

#ifdef HAS_INTERTECHNO
void CC1101::AttachIRQ_IT()
{
/*
	#ifdef ARDUINO_NANO
	attachInterrupt(1, cc1101_IRQ_Intertechno, FALLING);
	#endif
	#ifdef ARDUINO_LEONARDO
	attachInterrupt(0, cc1101_IRQ_Intertechno, FALLING);
	#endif
*/
}

void CC1101::DetachIRQ_IT()
{
	/*
	#ifdef ARDUINO_NANO
	detachInterrupt(0);
	#endif
	#ifdef ARDUINO_LEONARDO
	detachInterrupt(1);
	#endif
	*/

}
#endif

/*
CUL dump
0D 2E 2D 07 D3 91 3D 04
32 00 00 06 00 10 B0 71
55 E4 30 23 B9 00 07 00
18 14 6C 07 00 90 87 6B
F8 56 11 EF 0C 3C 1F 41
00 59 7F 3F 88 31 0B 00


Arduino dump
I: CC1101: 0d 2e 2d 07 d3 91 3d 04
I: CC1101: 32 00 00 06 00 10 b0 71
I: CC1101: 55 e4 30 23 b9 00 07 00
I: CC1101: 18 14 6c 07 00 90 87 6b
I: CC1101: f8 56 11 ef 0a 3d 1f 41
I: CC1101: 00 59 7f 3f 88 31 0b 00
*/
