/**
 *
 * @brief WS2812B driver - neopixels
 * @file ws2812b.c
 * @author hamster
 *
 *  Adapted from https://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/
 *
 *  Bit bang the LED driver, yo!
 *
 */

#include "LPC13Uxx.h"
#include "lpc_types.h"
#include "core/inc/lpc13uxx_gpio.h"
#include "drivers/common.h"
#include "drivers/ws2812b.h"

/**
 * Initialize the GPIO for the ws2812b
 */
void ws2812bInit(void){

	// Set the port as output
	LPC_GPIO->DIR[PORT0] |= (1<<7);
	// Let the ws2812b settle
	delay_ms(10);

}

/**
 * Bit-bang a bit out to the ws2812b using some NOPs to get the right timing
 * This is assuming you're going to compile without optimization, you might break
 * the neopixel interface otherwise.  Play with the timing, it's not rocket surgery.
 * Look at the link at the top for more info on the sensitive parts of the interface
 *
 * @param bitVal The bit to send
 */
void ws2812bSendBit( bool bitVal ){

    if(bitVal){
    	// 1-bit
    	// Must be at least 550ns

    	LPC_GPIO->SET[PORT0] |= (1<<7);

    	// The overhead of the for loop is a lot longer than the nop
    	for(int i = 0; i < 2; i++){
    		__NOP();
    	}

    	LPC_GPIO->CLR[PORT0] |= (1<<7);

    }else{

    	// 0-bit
    	// This bit is the sensitive one, it needs to be at least 200ns but less than 500ns
    	// We disable interrupts so we can be more deterministic

    	__disable_irq();

    	LPC_GPIO->SET[PORT0] |= (1<<7);

    	// Just toggling the GPIO is almost enough, this is to just make sure it is in the window
    	__NOP();
    	__NOP();

    	LPC_GPIO->CLR[PORT0] |= (1<<7);

    	__enable_irq();

    }

}

/**
 * Send a byte to the ws2812b
 *
 * @note The data to send must be bit-wise reversed
 * @param byte The byte to send
 */
void ws2812bSendByte(unsigned char byte){

	unsigned char revByte = reverseByte(byte);

    for(unsigned char bit = 0 ; bit < 8 ; bit++){
    	ws2812bSendBit(revByte & 0x01);
    	revByte >>= 1;
    }

}

/**
 * Set the color of the ws2812b - color is the sum of the RGB sent
 *
 * @param red Red value
 * @param green Green value
 * @param blue Blue value
 */
void ws2812bSetPixel(unsigned char red, unsigned char green, unsigned char blue){

	ws2812bSendByte(green);
	ws2812bSendByte(red);
	ws2812bSendByte(blue);

}

