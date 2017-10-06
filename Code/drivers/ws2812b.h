/**
 *
 * @brief WS2812B driver - neopixels
 * @file ws2812b.h
 * @author hamster
 *
 *  Adapted from https://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/
 *
 *  Bit bang the LED driver, yo!
 *
 */

#ifndef DRIVERS_WS2812B_H_
#define DRIVERS_WS2812B_H_

void ws2812bSendBit(bool bitVal);
void ws2812bSendByte(unsigned char byte);
void ws2812bInit(void);
void ws2812bSetPixel(unsigned char red, unsigned char green, unsigned char blue);


#endif /* DRIVERS_WS2812B_H_ */
