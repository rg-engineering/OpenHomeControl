/*
 * board_general.h
 *
 * Created: 29.10.2015 20:33:39
 *  Author: Rene
 */ 

#ifndef BOARD_GENERAL_H_
#define BOARD_GENERAL_H_

#define sVERSION "0.3.00"


#ifdef ATMEGA644
#define HAS_ARDUINO_USB
#define TTY_BUFSIZE           64

#ifdef USE_OLD_TFT_PCB
//on old display
#define PIN_CS_CC1101		10		//PIN3 PB2
#define PIN_CS_TFT			9		//PIN2 PB1
#define PIN_CS_SDCARD		4		//PIN6 PD4
#define PIN_CS_DUMMY		12		//PIN5 PB4 needed to tell µControler we are in master mode, better use that pin as CS somehow

#else
// SPI pins (new display v2?)
#define PIN_CS_CC1101		12 		//PIN5 PB4
#define PIN_CS_TFT			9		//PIN2 PB1
#define PIN_CS_SDCARD		4		//PIN6 PD4

#endif

#define PIN_SCK		15				//PIN8 PB7     !!
#define PIN_MISO	14				//PIN7 PB6     !!
#define PIN_MOSI	13				//PIN6 PB5     

//Pins für CC1101
#define PIN_GDO0	3				//PIN17 PD3
#define PIN_GDO2	2				//PIN16 PD2

//#define PIN_LED		9
//#define RECEIVE_IRQ 1				//

//Pins für TFT
#define PIN_TFT_RST    6			//PIN20 PD6 
#define PIN_TFT_DC     5			//PIN19 PD5


#ifdef HasOneWireDevices
#define PIN_ONEWIRE 8				//PIN1 PB0
#endif








#elif ARDUINO_NANO
#define HAS_ARDUINO_USB
#define TTY_BUFSIZE           64

// SPI pins
#define PIN_CS_CC1101		10
#define PIN_CS_TFT			9 
#define PIN_CS_SDCARD		4 

#define PIN_SCK		13
#define PIN_MISO	12
#define PIN_MOSI	11

//Pins für CC1101
#define PIN_GDO0	3	//int1
#define PIN_GDO2	2	//int0
//#define PIN_LED		9
#define RECEIVE_IRQ 1

//Pins für TFT
#define PIN_TFT_RST    5 // 7  // you can also connect this to the Arduino reset; hier nur temporär wegen PCB "328p V1"
// in which case, set this #define pin to 0!
#define PIN_TFT_DC     6


//nano pinout:
//http://www.pighixxx.com/test/wp-content/uploads/2014/11/nano.png

#define PIN_FerrarisIn1 2
#define PIN_FerrarisIn2 3

#ifdef HasOneWireDevices
	#define PIN_DHT22 8   //6 //kein wirklicher OneWire!! am Nano 6 muss wieder zurück; hier nur temporär wegen PCB "328p V1" auf 8
#else
	#define PIN_DHT22 8 //an PCB "Nano V1" ohne HW-Änderung und ohne OneWire
#endif
//#define PIN_DS2401 8 //das ist OneWire
#define PIN_ONEWIRE 7 //7  //8 //am Nano 8 muss wieder zurück; hier nur temporär wegen PCB "328p V1" auf 7
						//7 am Display modifiziert wegen Ventus; sonst 8
						//8 am RF_Sensor

#define PIN_VENTUS  2 //muss 

#elif ARDUINO_LEONARDO
#define HAS_ARDUINO_USB
#define TTY_BUFSIZE           128

// SPI pins
#define PIN_CS_TFT	11
#define PIN_CS_CC1101		10
#define PIN_CS_SDCARD		4
#define PIN_SCK		15
#define PIN_MISO	14
#define PIN_MOSI	16

//Pins für CC101
#define PIN_GDO0	2
#define PIN_GDO2	3

//#define PIN_LED		9
#define RECEIVE_IRQ 1

//Pins für TFT
#define PIN_TFT_RST    9  // you can also connect this to the Arduino reset
// in which case, set this #define pin to 0!
#define PIN_TFT_DC     8
#endif


#ifdef CUL_STICK
#define HAS_ARDUINO_USB
#define TTY_BUFSIZE           128

// SPI pins
#define PIN_CS		10
#define PIN_SCK		15
#define PIN_MISO	14
#define PIN_MOSI	16
#define PIN_GDO0	2
#define PIN_GDO2	3
#define PIN_LED		9
#define RECEIVE_IRQ 1


#define SPI_PORT		PORTB
#define SPI_DDR			DDRB
#define SPI_SS			PB0
#define SPI_MISO		PB3
#define SPI_MOSI		PB2
#define SPI_SCLK		PB1


#  define CC1100_CS_DDR		SPI_DDR
#  define CC1100_CS_PORT        SPI_PORT
#  define CC1100_CS_PIN		SPI_SS
#  define CC1100_OUT_DDR        DDRD
#  define CC1100_OUT_PORT       PORTD
#  define CC1100_OUT_PIN        PD3
#  define CC1100_OUT_IN         PIND
#  define CC1100_IN_DDR		DDRD
#  define CC1100_IN_PORT        PIND
#  define CC1100_IN_PIN         PD2
#  define CC1100_IN_IN          PIND
#  define CC1100_INT		INT2
#  define CC1100_INTVECT        INT2_vect
#  define CC1100_ISC		ISC20
#  define CC1100_EICR           EICRA
#  define LED_DDR               DDRE
#  define LED_PORT              PORTE
#  define LED_PIN               6

#endif

#endif /* BOARD_GENERAL_H_ */