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

#ifndef _CC1101_H
#define _CC1101_H

#include "Arduino.h"
#include "spi.h"
#include "ccpacket.h"



/**
 * Carrier frequencies
 */
enum CFREQ
{
  CFREQ_868 = 0,
  CFREQ_915,
  CFREQ_433,
  CFREQ_LAST
};

/**
 * RF STATES
 */
enum RFSTATE
{
  RFSTATE_IDLE = 0,
  RFSTATE_RX,
  RFSTATE_TX
};


/**
 * Frequency channels
 */
#define NUMBER_OF_FCHANNELS      10

/**
 * Type of transfers
 */
#define WRITE_BURST              0x40
#define READ_SINGLE              0x80
#define READ_BURST               0xC0

/**
 * Type of register
 */
#define CC1101_CONFIG_REGISTER   READ_SINGLE
#define CC1101_STATUS_REGISTER   READ_BURST

/**
 * PATABLE & FIFO's
 */
#define CC1101_PATABLE           0x3E        // PATABLE address
#define CC1101_TXFIFO            0x3F        // TX FIFO address
#define CC1101_RXFIFO            0x3F        // RX FIFO address

/**
 * Command strobes
 */
#define CC1101_SRES              0x30        // Reset CC1101 chip
#define CC1101_SFSTXON           0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1). If in RX (with CCA):
                                             // Go to a wait state where only the synthesizer is running (for quick RX / TX turnaround).
#define CC1101_SXOFF             0x32        // Turn off crystal oscillator
#define CC1101_SCAL              0x33        // Calibrate frequency synthesizer and turn it off. SCAL can be strobed from IDLE mode without
                                             // setting manual calibration mode (MCSM0.FS_AUTOCAL=0)
#define CC1101_SRX               0x34        // Enable RX. Perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1
#define CC1101_STX               0x35        // In IDLE state: Enable TX. Perform calibration first if MCSM0.FS_AUTOCAL=1.
                                             // If in RX state and CCA is enabled: Only go to TX if channel is clear
#define CC1101_SIDLE             0x36        // Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable
#define CC1101_SWOR              0x38        // Start automatic RX polling sequence (Wake-on-Radio) as described in Section 19.5 if
                                             // WORCTRL.RC_PD=0
#define CC1101_SPWD              0x39        // Enter power down mode when CSn goes high
#define CC1101_SFRX              0x3A        // Flush the RX FIFO buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states
#define CC1101_SFTX              0x3B        // Flush the TX FIFO buffer. Only issue SFTX in IDLE or TXFIFO_UNDERFLOW states
#define CC1101_SWORRST           0x3C        // Reset real time clock to Event1 value
#define CC1101_SNOP              0x3D        // No operation. May be used to get access to the chip status byte

/**
 * CC1101 configuration registers
 */
#define CC1101_IOCFG2            0x00        // GDO2 Output Pin Configuration
#define CC1101_IOCFG1            0x01        // GDO1 Output Pin Configuration
#define CC1101_IOCFG0            0x02        // GDO0 Output Pin Configuration
#define CC1101_FIFOTHR           0x03        // RX FIFO and TX FIFO Thresholds
#define CC1101_SYNC1             0x04        // Sync Word, High Byte
#define CC1101_SYNC0             0x05        // Sync Word, Low Byte
#define CC1101_PKTLEN            0x06        // Packet Length
#define CC1101_PKTCTRL1          0x07        // Packet Automation Control
#define CC1101_PKTCTRL0          0x08        // Packet Automation Control
#define CC1101_ADDR              0x09        // Device Address
#define CC1101_CHANNR            0x0A        // Channel Number
#define CC1101_FSCTRL1           0x0B        // Frequency Synthesizer Control
#define CC1101_FSCTRL0           0x0C        // Frequency Synthesizer Control
#define CC1101_FREQ2             0x0D        // Frequency Control Word, High Byte
#define CC1101_FREQ1             0x0E        // Frequency Control Word, Middle Byte
#define CC1101_FREQ0             0x0F        // Frequency Control Word, Low Byte
#define CC1101_MDMCFG4           0x10        // Modem Configuration
#define CC1101_MDMCFG3           0x11        // Modem Configuration
#define CC1101_MDMCFG2           0x12        // Modem Configuration
#define CC1101_MDMCFG1           0x13        // Modem Configuration
#define CC1101_MDMCFG0           0x14        // Modem Configuration
#define CC1101_DEVIATN           0x15        // Modem Deviation Setting
#define CC1101_MCSM2             0x16        // Main Radio Control State Machine Configuration
#define CC1101_MCSM1             0x17        // Main Radio Control State Machine Configuration
#define CC1101_MCSM0             0x18        // Main Radio Control State Machine Configuration
#define CC1101_FOCCFG            0x19        // Frequency Offset Compensation Configuration
#define CC1101_BSCFG             0x1A        // Bit Synchronization Configuration
#define CC1101_AGCCTRL2          0x1B        // AGC Control
#define CC1101_AGCCTRL1          0x1C        // AGC Control
#define CC1101_AGCCTRL0          0x1D        // AGC Control
#define CC1101_WOREVT1           0x1E        // High Byte Event0 Timeout
#define CC1101_WOREVT0           0x1F        // Low Byte Event0 Timeout
#define CC1101_WORCTRL           0x20        // Wake On Radio Control
#define CC1101_FREND1            0x21        // Front End RX Configuration
#define CC1101_FREND0            0x22        // Front End TX Configuration
#define CC1101_FSCAL3            0x23        // Frequency Synthesizer Calibration
#define CC1101_FSCAL2            0x24        // Frequency Synthesizer Calibration
#define CC1101_FSCAL1            0x25        // Frequency Synthesizer Calibration
#define CC1101_FSCAL0            0x26        // Frequency Synthesizer Calibration
#define CC1101_RCCTRL1           0x27        // RC Oscillator Configuration
#define CC1101_RCCTRL0           0x28        // RC Oscillator Configuration
#define CC1101_FSTEST            0x29        // Frequency Synthesizer Calibration Control
#define CC1101_PTEST             0x2A        // Production Test
#define CC1101_AGCTEST           0x2B        // AGC Test
#define CC1101_TEST2             0x2C        // Various Test Settings
#define CC1101_TEST1             0x2D        // Various Test Settings
#define CC1101_TEST0             0x2E        // Various Test Settings

/**
 * Status registers
 */
#define CC1101_PARTNUM           0x30        // Chip ID
#define CC1101_VERSION           0x31        // Chip ID
#define CC1101_FREQEST           0x32        // Frequency Offset Estimate from Demodulator
#define CC1101_LQI               0x33        // Demodulator Estimate for Link Quality
#define CC1101_RSSI              0x34        // Received Signal Strength Indication
#define CC1101_MARCSTATE         0x35        // Main Radio Control State Machine State
#define CC1101_WORTIME1          0x36        // High Byte of WOR Time
#define CC1101_WORTIME0          0x37        // Low Byte of WOR Time
#define CC1101_PKTSTATUS         0x38        // Current GDOx Status and Packet Status
#define CC1101_VCO_VC_DAC        0x39        // Current Setting from PLL Calibration Module
#define CC1101_TXBYTES           0x3A        // Underflow and Number of Bytes
#define CC1101_RXBYTES           0x3B        // Overflow and Number of Bytes
#define CC1101_RCCTRL1_STATUS    0x3C        // Last RC Oscillator Calibration Result
#define CC1101_RCCTRL0_STATUS    0x3D        // Last RC Oscillator Calibration Result 


//------------------------------------------------------------------------------
// Chip Status Byte
//------------------------------------------------------------------------------

// Bit fields in the chip status byte
#define CC1101_STATUS_CHIP_RDYn_BM             0x80
#define CC1101_STATUS_STATE_BM                 0x70
#define CC1101_STATUS_FIFO_BYTES_AVAILABLE_BM  0x0F

// Chip states
#define CC1101_STATE_IDLE                      0x00
#define CC1101_STATE_RX                        0x10
#define CC1101_STATE_TX                        0x20
#define CC1101_STATE_FSTXON                    0x30
#define CC1101_STATE_CALIBRATE                 0x40
#define CC1101_STATE_SETTLING                  0x50
#define CC1101_STATE_RX_OVERFLOW               0x60
#define CC1101_STATE_TX_UNDERFLOW              0x70

/**
 * CC1101 configuration registers - Default values extracted from SmartRF Studio
 *
 * Configuration:
 *
 * Deviation = 20.629883 
 * Base frequency = 867.999939 
 * Carrier frequency = 867.999939 
 * Channel number = 0 
 * Carrier frequency = 867.999939 
 * Modulated = true 
 * Modulation format = GFSK 
 * Manchester enable = false
 * Data whitening = off
 * Sync word qualifier mode = 30/32 sync word bits detected 
 * Preamble count = 4 
 * Channel spacing = 199.951172 
 * Carrier frequency = 867.999939 
 * Data rate = 38.3835 Kbps
 * RX filter BW = 101.562500 
 * Data format = Normal mode 
 * Length config = Variable packet length mode. Packet length configured by the first byte after sync word 
 * CRC enable = true 
 * Packet length = 255 
 * Device address = 1 
 * Address config = Enable address check
 * Append status = Append two status bytes to the payload of the packet. The status bytes contain RSSI and
 * LQI values, as well as CRC OK
 * CRC autoflush = false 
 * PA ramping = false 
 * TX power = 12
 * GDO0 mode = Asserts when sync word has been sent / received, and de-asserts at the end of the packet.
 * In RX, the pin will also de-assert when a packet is discarded due to address or maximum length filtering
 * or when the radio enters RXFIFO_OVERFLOW state. In TX the pin will de-assert if the TX FIFO underflows
 * Settings optimized for low current consumption
 */
//#define CC1101_DEFVAL_IOCFG2     0x29        // GDO2 Output Pin Configuration
#define CC1101_DEFVAL_IOCFG2     0x2E        // GDO2 Output Pin Configuration
#define CC1101_DEFVAL_IOCFG1     0x2E        // GDO1 Output Pin Configuration
#define CC1101_DEFVAL_IOCFG0     0x06        // GDO0 Output Pin Configuration
#define CC1101_DEFVAL_FIFOTHR    0x07        // RX FIFO and TX FIFO Thresholds
#define CC1101_DEFVAL_SYNC1      0xB5        // Synchronization word, high byte
#define CC1101_DEFVAL_SYNC0      0x47        // Synchronization word, low byte
#define CC1101_DEFVAL_PKTLEN     0x3D        // Packet Length
#define CC1101_DEFVAL_PKTCTRL1   0x06        // Packet Automation Control
#define CC1101_DEFVAL_PKTCTRL0   0x05        // Packet Automation Control
#define CC1101_DEFVAL_ADDR       0xFF        // Device Address
#define CC1101_DEFVAL_CHANNR     0x00        // Channel Number
#define CC1101_DEFVAL_FSCTRL1    0x08        // Frequency Synthesizer Control
#define CC1101_DEFVAL_FSCTRL0    0x00        // Frequency Synthesizer Control
// Carrier frequency = 868 MHz
#define CC1101_DEFVAL_FREQ2_868  0x21        // Frequency Control Word, High Byte
#define CC1101_DEFVAL_FREQ1_868  0x62        // Frequency Control Word, Middle Byte
#define CC1101_DEFVAL_FREQ0_868  0x76        // Frequency Control Word, Low Byte
// Carrier frequency = 902 MHz
#define CC1101_DEFVAL_FREQ2_915  0x22        // Frequency Control Word, High Byte
#define CC1101_DEFVAL_FREQ1_915  0xB1        // Frequency Control Word, Middle Byte
#define CC1101_DEFVAL_FREQ0_915  0x3B        // Frequency Control Word, Low Byte
// Carrier frequency = 433 MHz
// #define CC1101_DEFVAL_FREQ2_433	0x10		// 0D FREQ2    *1E    21    433.92 (InterTechno Frequency)
// #define CC1101_DEFVAL_FREQ1_433	0xb0		// 0E FREQ1    *C4    65
// #define CC1101_DEFVAL_FREQ0_433	0x71		// 0F FREQ0    *EC    e8

#define CC1101_DEFVAL_FREQ2_433  0x10        // Frequency Control Word, High Byte //433MHz
#define CC1101_DEFVAL_FREQ1_433  0xA7        // Frequency Control Word, Middle Byte
#define CC1101_DEFVAL_FREQ0_433  0x62        // Frequency Control Word, Low Byte

#define CC1101_DEFVAL_MDMCFG4    0xCA        // Modem Configuration
#define CC1101_DEFVAL_MDMCFG3    0x83        // Modem Configuration
#define CC1101_DEFVAL_MDMCFG2    0x93        // Modem Configuration
#define CC1101_DEFVAL_MDMCFG1    0x22        // Modem Configuration
#define CC1101_DEFVAL_MDMCFG0    0xF8        // Modem Configuration
#define CC1101_DEFVAL_DEVIATN    0x35        // Modem Deviation Setting
#define CC1101_DEFVAL_MCSM2      0x07        // Main Radio Control State Machine Configuration
//#define CC1101_DEFVAL_MCSM1      0x30        // Main Radio Control State Machine Configuration
#define CC1101_DEFVAL_MCSM1      0x20        // Main Radio Control State Machine Configuration
#define CC1101_DEFVAL_MCSM0      0x18        // Main Radio Control State Machine Configuration
#define CC1101_DEFVAL_FOCCFG     0x16        // Frequency Offset Compensation Configuration
#define CC1101_DEFVAL_BSCFG      0x6C        // Bit Synchronization Configuration
#define CC1101_DEFVAL_AGCCTRL2   0x43        // AGC Control
#define CC1101_DEFVAL_AGCCTRL1   0x40        // AGC Control
#define CC1101_DEFVAL_AGCCTRL0   0x91        // AGC Control
#define CC1101_DEFVAL_WOREVT1    0x87        // High Byte Event0 Timeout
#define CC1101_DEFVAL_WOREVT0    0x6B        // Low Byte Event0 Timeout
#define CC1101_DEFVAL_WORCTRL    0xFB        // Wake On Radio Control
#define CC1101_DEFVAL_FREND1     0x56        // Front End RX Configuration
#define CC1101_DEFVAL_FREND0     0x10        // Front End TX Configuration
#define CC1101_DEFVAL_FSCAL3     0xE9        // Frequency Synthesizer Calibration
#define CC1101_DEFVAL_FSCAL2     0x2A        // Frequency Synthesizer Calibration
#define CC1101_DEFVAL_FSCAL1     0x00        // Frequency Synthesizer Calibration
#define CC1101_DEFVAL_FSCAL0     0x1F        // Frequency Synthesizer Calibration
#define CC1101_DEFVAL_RCCTRL1    0x41        // RC Oscillator Configuration
#define CC1101_DEFVAL_RCCTRL0    0x00        // RC Oscillator Configuration
#define CC1101_DEFVAL_FSTEST     0x59        // Frequency Synthesizer Calibration Control
#define CC1101_DEFVAL_PTEST      0x7F        // Production Test
#define CC1101_DEFVAL_AGCTEST    0x3F        // AGC Test
#define CC1101_DEFVAL_TEST2      0x81        // Various Test Settings
#define CC1101_DEFVAL_TEST1      0x35        // Various Test Settings
#define CC1101_DEFVAL_TEST0      0x09        // Various Test Settings

//test only
//#define CC1101_ITVAL_IOCFG2     0x2E        // GDO2 Output Pin Configuration
//#define CC1101_ITVAL_IOCFG1     0x2E        // GDO1 Output Pin Configuration
//#define CC1101_ITVAL_IOCFG0     0x06        // GDO0 Output Pin Configuration
/*
#define CC1101_ITVAL_IOCFG2		0x0D		// 00 IOCFG2   *29   *0B    GDO2 as serial output
#define CC1101_ITVAL_IOCFG1		0x2E		// 01 IOCFG1    2E    2E    Tri-State
#define CC1101_ITVAL_IOCFG0		0x2D		// 02 IOCFG0   *3F   *0C    GDO0 for input
#define CC1101_ITVAL_FIFOTHR	0x07		// 03 FIFOTHR   07   *47
#define CC1101_ITVAL_SYNC1		0xD3		// 04 SYNC1     D3    D3
#define CC1101_ITVAL_SYNC0		0x91		// 05 SYNC0     91    91
#define CC1101_ITVAL_PKTLEN		0x3D		// 06 PKTLEN   *FF    3D
#define CC1101_ITVAL_PKTCTRL1	0x04		// 07 PKTCTRL1  04    04
#define CC1101_ITVAL_PKTCTRL0	0x32		// 08 PKTCTRL0 *45    32
#define CC1101_ITVAL_ADDR		0x00		// 09 ADDR      00    00
#define CC1101_ITVAL_CHANNR		0x00		// 0A CHANNR    00    00
#define CC1101_ITVAL_FSCTRL1	0x06		// 0B FSCTRL1  *0F    06    152kHz IF Frquency
#define CC1101_ITVAL_FSCTRL0    0x00		// 0C FSCTRL0   00    00
#define CC1101_ITVAL_FREQ2_433	0x10		// 0D FREQ2    *1E    21    433.92 (InterTechno Frequency)
#define CC1101_ITVAL_FREQ1_433	0xb0		// 0E FREQ1    *C4    65
#define CC1101_ITVAL_FREQ0_433	0x71		// 0F FREQ0    *EC    e8
#define CC1101_ITVAL_MDMCFG4	0x55		// 10 MDMCFG4  *8C    55    bWidth 325kHz
#define CC1101_ITVAL_MDMCFG3	0xe4		// 11 MDMCFG3  *22   *43    Drate:1500 ((256+228)*2^5)*26000000/2^28
#define CC1101_ITVAL_MDMCFG2	0x30		// 12 MDMCFG2  *02   *B0    Modulation: ASK
#define CC1101_ITVAL_MDMCFG1	0x23		// 13 MDMCFG1  *22    23
#define CC1101_ITVAL_MDMCFG0	0xb9		// 14 MDMCFG0  *F8    b9    ChannelSpace: 350kHz
#define CC1101_ITVAL_DEVIATN	0x00		// 15 DEVIATN  *47    00
#define CC1101_ITVAL_MCSM2		0x07		// 16 MCSM2     07    07
#define CC1101_ITVAL_MCSM1		0x00		// 17 MCSM1     30    30
#define CC1101_ITVAL_MCSM0		0x18		// 18 MCSM0    *04    18    Calibration: RX/TX->IDLE
#define CC1101_ITVAL_FOCCFG		0x14		// 19 FOCCFG   *36    14
#define CC1101_ITVAL_BSCFG		0x6C		// 1A BSCFG     6C    6C
#define CC1101_ITVAL_AGCCTRL2   0x07		// 1B AGCCTRL2 *03   *03    42 dB instead of 33dB
#define CC1101_ITVAL_AGCCTRL1   0x00		// 1C AGCCTRL1 *40   *40
#define CC1101_ITVAL_AGCCTRL0   0x90		// 1D AGCCTRL0 *91   *92    4dB decision boundery
#define CC1101_ITVAL_WOREVT1    0x87		// 1E WOREVT1   87    87
#define CC1101_ITVAL_WOREVT0	0x6B		// 1F WOREVT0   6B    6B
#define CC1101_ITVAL_WORCTRL	0xF8		// 20 WORCTRL   F8    F8
#define CC1101_ITVAL_FREND1		0x56		// 21 FREND1    56    56
#define CC1101_ITVAL_FREND0		0x11		// 22 FREND0   *16    17    0x11 for no PA ramping
#define CC1101_ITVAL_FSCAL3		0xEF //0xE9		// 23 FSCAL3   *A9    E9
#define CC1101_ITVAL_FSCAL2		0x2C //0x2A		// 24 FSCAL2   *0A    2A
#define CC1101_ITVAL_FSCAL1		0x14 //0x00		// 25 FSCAL1    20    00
#define CC1101_ITVAL_FSCAL0		0x1F		// 26 FSCAL0    0D    1F
#define CC1101_ITVAL_RCCTRL1	0x41		// 27 RCCTRL1   41    41
#define CC1101_ITVAL_RCCTRL0	0x00		// 28 RCCTRL0   00    00
// ab hier nicht sicher:
#define CC1101_ITVAL_FSTEST     0x59        // Frequency Synthesizer Calibration Control
#define CC1101_ITVAL_PTEST      0x7F        // Production Test
#define CC1101_ITVAL_AGCTEST    0x3F        // AGC Test
#define CC1101_ITVAL_TEST2      0x81        // Various Test Settings
#define CC1101_ITVAL_TEST1      0x35        // Various Test Settings
#define CC1101_ITVAL_TEST0      0x09        // Various Test Settings
*/

/*
//CUL
#define CC1101_CULVAL_IOCFG2     0x2E        // GDO2 Output Pin Configuration
#define CC1101_CULVAL_IOCFG1     0x2E        // GDO1 Output Pin Configuration
#define CC1101_CULVAL_IOCFG0     0x06        // GDO0 Output Pin Configuration
#define CC1101_CULVAL_FIFOTHR    0x07        // RX FIFO and TX FIFO Thresholds
#define CC1101_CULVAL_SYNC1      0xB5        // Synchronization word, high byte
#define CC1101_CULVAL_SYNC0      0x47        // Synchronization word, low byte
#define CC1101_CULVAL_PKTLEN     0x3D        // Packet Length
#define CC1101_CULVAL_PKTCTRL1   0x06        // Packet Automation Control
#define CC1101_CULVAL_PKTCTRL0   0x05        // Packet Automation Control
#define CC1101_CULVAL_ADDR       0xFF        // Device Address
#define CC1101_CULVAL_CHANNR     0x00        // Channel Number
#define CC1101_CULVAL_FSCTRL1    0x08        // Frequency Synthesizer Control
#define CC1101_CULVAL_FSCTRL0    0x00        // Frequency Synthesizer Control
// Carrier frequency = 868 MHz
#define CC1101_CULVAL_FREQ2_868  0x21        // Frequency Control Word, High Byte
#define CC1101_CULVAL_FREQ1_868  0x62        // Frequency Control Word, Middle Byte
#define CC1101_CULVAL_FREQ0_868  0x76        // Frequency Control Word, Low Byte
// Carrier frequency = 902 MHz
#define CC1101_CULVAL_FREQ2_915  0x22        // Frequency Control Word, High Byte
#define CC1101_CULVAL_FREQ1_915  0xB1        // Frequency Control Word, Middle Byte
#define CC1101_CULVAL_FREQ0_915  0x3B        // Frequency Control Word, Low Byte
// Carrier frequency = 433 MHz
#define CC1101_CULVAL_FREQ2_433  0x10        // Frequency Control Word, High Byte //433MHz
#define CC1101_CULVAL_FREQ1_433  0xB0        // Frequency Control Word, Middle Byte
#define CC1101_CULVAL_FREQ0_433  0x71        // Frequency Control Word, Low Byte
#define CC1101_CULVAL_MDMCFG4    0xCA        // Modem Configuration
#define CC1101_CULVAL_MDMCFG3    0x83        // Modem Configuration
#define CC1101_CULVAL_MDMCFG2    0x93        // Modem Configuration
#define CC1101_CULVAL_MDMCFG1    0x22        // Modem Configuration
#define CC1101_CULVAL_MDMCFG0    0xF8        // Modem Configuration
#define CC1101_CULVAL_DEVIATN    0x35        // Modem Deviation Setting
#define CC1101_CULVAL_MCSM2      0x07        // Main Radio Control State Machine Configuration
#define CC1101_CULVAL_MCSM1      0x20        // Main Radio Control State Machine Configuration
#define CC1101_CULVAL_MCSM0      0x18        // Main Radio Control State Machine Configuration
#define CC1101_CULVAL_FOCCFG     0x16        // Frequency Offset Compensation Configuration
#define CC1101_CULVAL_BSCFG      0x6C        // Bit Synchronization Configuration
#define CC1101_CULVAL_AGCCTRL2   0x43        // AGC Control
#define CC1101_CULVAL_AGCCTRL1   0x40        // AGC Control
#define CC1101_CULVAL_AGCCTRL0   0x91        // AGC Control
#define CC1101_CULVAL_WOREVT1    0x87        // High Byte Event0 Timeout
#define CC1101_CULVAL_WOREVT0    0x6B        // Low Byte Event0 Timeout
#define CC1101_CULVAL_WORCTRL    0xFB        // Wake On Radio Control
#define CC1101_CULVAL_FREND1     0x56        // Front End RX Configuration
#define CC1101_CULVAL_FREND0     0x10        // Front End TX Configuration
#define CC1101_CULVAL_FSCAL3     0xE9        // Frequency Synthesizer Calibration
#define CC1101_CULVAL_FSCAL2     0x2A        // Frequency Synthesizer Calibration
#define CC1101_CULVAL_FSCAL1     0x00        // Frequency Synthesizer Calibration
#define CC1101_CULVAL_FSCAL0     0x1F        // Frequency Synthesizer Calibration
#define CC1101_CULVAL_RCCTRL1    0x41        // RC Oscillator Configuration
#define CC1101_CULVAL_RCCTRL0    0x00        // RC Oscillator Configuration
#define CC1101_CULVAL_FSTEST     0x59        // Frequency Synthesizer Calibration Control
#define CC1101_CULVAL_PTEST      0x7F        // Production Test
#define CC1101_CULVAL_AGCTEST    0x3F        // AGC Test
#define CC1101_CULVAL_TEST2      0x81        // Various Test Settings
#define CC1101_CULVAL_TEST1      0x35        // Various Test Settings
#define CC1101_CULVAL_TEST0      0x09        // Various Test Settings
*/

/**
 * Macros
 */
// Read CC1101 Config register
#define readConfigReg(regAddr)    readReg(regAddr, CC1101_CONFIG_REGISTER)
// Read CC1101 Status register
#define readStatusReg(regAddr)    readReg(regAddr, CC1101_STATUS_REGISTER)
// Enter Rx state
#define setRxState()              cmdStrobe(CC1101_SRX)
// Enter Tx state
#define setTxState()              cmdStrobe(CC1101_STX)
// Enter IDLE state
#define setIdleState()            cmdStrobe(CC1101_SIDLE)
// Flush Rx FIFO
#define flushRxFifo()             cmdStrobe(CC1101_SFRX)
// Flush Tx FIFO
#define flushTxFifo()             cmdStrobe(CC1101_SFTX)
// Disable address check
#define disableAddressCheck()     writeReg(CC1101_PKTCTRL1, 0x04)
// Enable address check
#define enableAddressCheck()      writeReg(CC1101_PKTCTRL1, 0x06)
// Disable CCA
#define disableCCA()              writeReg(CC1101_MCSM1, 0)
// Enable CCA
#define enableCCA()               writeReg(CC1101_MCSM1, CC1101_DEFVAL_MCSM1)
// Set PATABLE single byte
#define setTxPowerAmp(setting)    paTableByte = setting
// PATABLE values
#define PA_LowPower               0x60
#define PA_LongDistance           0xC0

/**
 * Class: CC1101
 * 
 * Description:
 * CC1101 interface
 */
class CC1101
{
  private:
    /**
     * Atmega's SPI interface
     */
    SPI1 spi; 

    /**
     * writeBurstReg
     * 
     * Write multiple registers into the CC1101 IC via SPI
     * 
     * 'regAddr'	Register address
     * 'buffer'	Data to be writen
     * 'len'	Data length
     */
    //void writeBurstReg(byte regAddr, byte* buffer, byte len);

    /**
     * readBurstReg
     * 
     * Read burst data from CC1101 via SPI
     * 
     * 'buffer'	Buffer where to copy the result to
     * 'regAddr'	Register address
     * 'len'	Data length
     */
    void readBurstReg(byte * buffer, byte regAddr, byte len);



    /**
     * setRegsFromEeprom
     * 
     * Set registers from EEPROM
     */
    //void setRegsFromEeprom(void);

  public:
     /**
     * setDefaultRegs
     * 
     * Configure CC1101 registers
     */
    void setDefaultRegs(void);
//	void setCULRegs(void);
    /*
     * RF state
     */
    byte rfState;

    /**
     * Tx Power byte (single PATABLE config)
     */
    byte paTableByte;

    /**
     * Carrier frequency
     */
    //byte carrierFreq;

    /**
     * Frequency channel
     */
    //byte channel;

    /**
     * Synchronization word
     */
    //byte syncWord[2];

    /**
     * Device address
     */
    byte devAddress;

    /**
     * CC1101
     * 
     * Class constructor
     */
    CC1101(void);
	
	void writeBurstReg(byte regAddr, byte* buffer, byte len);

    /**
     * cmdStrobe
     * 
     * Send command strobe to the CC1101 IC via SPI
     * 
     * 'cmd'	Command strobe
     */
    void cmdStrobe(byte cmd);
	byte cmdStrobeRet(byte cmd);
    /**
     * wakeUp
     * 
     * Wake up CC1101 from Power Down state
     */
    void wakeUp(void);

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
    byte readReg(byte regAddr, byte regType);

    /**
     * writeReg
     * 
     * Write single register into the CC1101 IC via SPI
     * 
     * 'regAddr'	Register address
     * 'value'	Value to be writen
     */
    void writeReg(byte regAddr, byte value);

    /**
     * reset
     * 
     * Reset CC1101
     */
    void reset(void);
    
    /**
     * init
     * 
     * Initializa CC1101
     */
    void init(void);

    /**
     * setSyncWord
     * 
     * Set synchronization word
     * 
     * 'syncH'	Synchronization word - High byte
     * 'syncL'	Synchronization word - Low byte
     * 'save' If TRUE, save parameter in EEPROM
     */
    void setSyncWord(uint8_t syncH, uint8_t syncL, bool save=true);

    /**
     * setSyncWord (overriding method)
     * 
     * Set synchronization word
     * 
     * 'syncH'	Synchronization word - pointer to 2-byte array
     * 'save' If TRUE, save parameter in EEPROM
     */
    void setSyncWord(byte *sync, bool save=true);

    /**
     * setDevAddress
     * 
     * Set device address
     * 
     * 'addr'	Device address
     * 'save' If TRUE, save parameter in EEPROM
     */
    void setDevAddress(byte addr, bool save=true);

    /**
     * setCarrierFreq
     * 
     * Set carrier frequency
     * 
     * 'freq'	New carrier frequency
     */
    void setCarrierFreq(byte freq);
    
    /**
     * setChannel
     * 
     * Set frequency channel
     * 
     * 'chnl'	Frequency channel
     * 'save' If TRUE, save parameter in EEPROM
     */
    void setChannel(byte chnl, bool save=true);

    /**
     * setPowerDownState
     * 
     * Put CC1101 into power-down state
     */
    void setPowerDownState();
    
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
    boolean sendData(CCPACKET packet);
    boolean sendData_default(CCPACKET packet);
#ifdef HAS_INTERTECHNO
	boolean sendData_Intertechno(CCPACKET packet);
#endif	
    /**
     * receiveData
     * 
     * Read data packet from RX FIFO
     * 
     * Return:
     * 	Amount of bytes received
     */
    byte receiveData(CCPACKET *packet);
	
	
public:
	void ReadAllRegister();
public:
	enum
	{
		SETTING_DEFAULT = 1,
		SETTING_CUL=2,
#ifdef HAS_INTERTECHNO
		SETTING_INTERTECHNO =3
#endif

	};
	int nCurSettings;

#ifdef HAS_INTERTECHNO
public:
	void setITRegs(void);
	static int nIRQCounter;
private:
	void delay_us( uint16_t d );
	void send_IT_sync();
	void send_IT_sync_V3();
	void send_IT_latch_V3();
	void send_IT_bit_V3(short bit);
	void send_IT_bit(short bit);
	
	void AttachIRQ_IT();
	void DetachIRQ_IT();
	
	int it_interval;
	int itv3_start_bit;
	int itv3_bit;
	int itv3_latch;
	int itv3_low;
	int itv3_sync;
#endif
};

#endif

