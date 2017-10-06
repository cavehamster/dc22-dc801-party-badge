/**
 *
 * @brief Common functions
 * @file common.h
 * @author hamster
 *
 * Common stuff that didn't fit anywhere else
 *
 */

#ifndef DRIVERS_COMMON_H_
#define DRIVERS_COMMON_H_

#define INITIAL_CRC_SEED 0xFFFF;

#include "lpc_types.h"

typedef struct {
	uint32_t Seconds;
	uint32_t Milliseconds;
} TIME;

extern volatile TIME sysTime;

uint16_t crc_16_ccitt(uint16_t crc, uint8_t * data_in, uint16_t data_len);
unsigned char reverseByte(unsigned char x);
void delay_ms(uint32_t ms);
void getTimeStamp(TIME *time);
void timeElapsedSince(TIME *since, TIME *elapsed);
bool timeIsTimeout(TIME *time, uint32_t timeout);


#endif /* DRIVERS_COMMON_H_ */
