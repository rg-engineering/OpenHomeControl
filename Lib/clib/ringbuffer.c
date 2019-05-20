#include "ringbuffer.h"
/*
rb_t Debug_Tx_Buffer;

void rb_reset(rb_t *rb)
{
	rb->getoff = rb->putoff = rb->nbytes = 0;
}

void rb_put(rb_t *rb, uint8_t data)
{
	uint8_t sreg;
	sreg = SREG;
	cli();
	if(rb->nbytes >= TTY_BUFSIZE) 
	{
		SREG = sreg;
		return;
	}
	rb->nbytes++;
	rb->buf[rb->putoff++] = data;
	if(rb->putoff == TTY_BUFSIZE)
		rb->putoff = 0;
	SREG = sreg;
}

uint8_t rb_get(rb_t *rb)
{
	uint8_t sreg;
	uint8_t ret;
	sreg = SREG;
	cli();
	if(rb->nbytes == 0) 
	{
		SREG = sreg;
		return 0;
	}
	rb->nbytes--;
	ret = rb->buf[rb->getoff++];
	if(rb->getoff == TTY_BUFSIZE)
		rb->getoff = 0;
	SREG = sreg;
	return ret;
}


*/