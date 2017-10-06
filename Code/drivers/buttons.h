/**
 *
 * @brief Driver for the buttons
 * @file buttons.h
 * @author hamster
 *
 * Supports short presses (with debounce) and long presses
 *
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#define BUTTON_SHORT_PRESS	15		// Consider a short press debounced after this time
#define BUTTON_LONG_PRESS	400		// Consider a button to be a long press after this time

typedef enum{
	BUTTON_A	= (1<<0),
	BUTTON_B	= (1<<1),
	BUTTON_C	= (1<<2),
	BUTTON_D	= (1<<3)
} BUTTON_ID;

typedef struct{
	uint8_t ShortPress	: 4;
	uint8_t LongPress	: 4;
	struct{
		TIME A;
		TIME B;
		TIME C;
		TIME D;
	} ButtonTimer;
	bool Initialized;
} BUTTON_STATE;

bool buttonIsShortPressed(BUTTON_ID button);
bool buttonIsLongPressed(BUTTON_ID button);
void buttonInit(void);
bool buttonIsInitialized(void);
void buttonSample(void);


#endif
