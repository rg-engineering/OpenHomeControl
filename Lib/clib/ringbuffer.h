#ifndef __ringbuffer_H_
#define __ringbuffer_H_


#include "../board.h"
#include <stdint.h>

#include <avr/interrupt.h>

/*
#ifndef TTY_BUFSIZE
	#define TTY_BUFSIZE           64
#endif

#if TTY_BUFSIZE < 256
typedef struct
{
	uint8_t putoff;
	uint8_t getoff;
	uint8_t nbytes;       // Number of data bytes
	char buf[TTY_BUFSIZE];
} rb_t;
#else
typedef struct
{
	uint16_t putoff;
	uint16_t getoff;
	uint16_t nbytes;       // Number of data bytes
	char buf[TTY_BUFSIZE];
} rb_t;
#endif


void rb_put(rb_t *rb, uint8_t data);
uint8_t rb_get(rb_t *rb);
void rb_reset(rb_t *rb);

#ifdef HAS_ARDUINO_USB
extern rb_t Debug_Tx_Buffer;
#endif
*/
#endif