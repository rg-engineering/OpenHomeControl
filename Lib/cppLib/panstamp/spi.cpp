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

#include "spi.h"
#include "../../board.h"
/**
 * init
 * 
 * SPI initialization
 */

void SPI1::init() 
{
  //digitalWrite(SPI_SS, HIGH);
  digitalWrite(PIN_CS_CC1101, HIGH);
    
  // Configure SPI pins
//   pinMode(SPI_SS, OUTPUT);
//   pinMode(SPI_MOSI, OUTPUT);
//   pinMode(SPI_MISO, INPUT);
//   pinMode(SPI_SCK, OUTPUT);
// 
//   digitalWrite(SPI_SCK, HIGH);
//   digitalWrite(SPI_MOSI, LOW);

  pinMode(PIN_CS_CC1101, OUTPUT);
  pinMode(PIN_MOSI, OUTPUT);
  pinMode(PIN_MISO, INPUT);
  pinMode(PIN_SCK, OUTPUT);

  digitalWrite(PIN_SCK, HIGH);
  digitalWrite(PIN_MOSI, LOW);


  // SPI speed = clk/4
  
  //Bit 7 	Bit 6 	Bit 5 	Bit 4 	Bit 3 	Bit 2 	Bit 1 	Bit 0
  //SPIE    SPE     DORD    MSTR    CPOL    CPHA    SPR1    SPR0
  //siehe http://avrbeginners.net/architecture/spi/spi.html#spsr
  SPCR = _BV(SPE) | _BV(MSTR);
  
  //CUL
  //  SPCR  = _BV(MSTR) | _BV(SPE);
  //  SPSR |= _BV(SPI2X);
}

/**
 * send
 * 
 * Send byte via SPI
 * 
 * 'value'	Value to be sent
 * 
 * Return:
 * 	Response received from SPI slave
 */

byte SPI1::send(byte value) 
{
  SPDR = value;                          // Transfer byte via SPI
  wait_Spi();                            // Wait until SPI operation is terminated

  return SPDR;
}

