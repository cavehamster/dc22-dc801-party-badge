/**
 *
 * @brief Driver for the buttons
 * @file buttons.c
 * @author hamster
 *
 * Supports short presses (with debounce) and long presses
 *
 */


#include "LPC13Uxx.h"
#include "lpc_types.h"
#include <string.h>
#include "core/inc/lpc13uxx_gpio.h"
#include "drivers/common.h"
#include "drivers/buttons.h"

static volatile BUTTON_STATE buttonState;

/**
 * Scan the button physical state, debounce and determine long pressed or not
 */
void buttonSample(void){
	
	static uint8_t oldState = 0;

	uint8_t curState = (
			(LPC_GPIO->B1[20] << 0) |
			(LPC_GPIO->B0[1]  << 1) |
			(LPC_GPIO->B1[19] << 2) |
			(LPC_GPIO->B0[16] << 3) );
	
	// Check the current state
	if((curState & BUTTON_A) == BUTTON_A){
		if(!((oldState & BUTTON_A) == BUTTON_A)){
			// First time seeing this button down.
			// Time stamp it and update the old state.
			getTimeStamp((void *)&buttonState.ButtonTimer.A);
			oldState |= BUTTON_A;
		}
	}
	if((curState & BUTTON_B) == BUTTON_B){
		if(!((oldState & BUTTON_B) == BUTTON_B)){
			// First time seeing this button down.
			// Time stamp it and update the old state.
			getTimeStamp((void *)&buttonState.ButtonTimer.B);
			oldState |= BUTTON_B;
		}
	}
	if((curState & BUTTON_C) == BUTTON_C){
		if(!((oldState & BUTTON_C) == BUTTON_C)){
			// First time seeing this button down.
			// Time stamp it and update the old state.
			getTimeStamp((void *)&buttonState.ButtonTimer.C);
			oldState |= BUTTON_C;
		}
	}
	if((curState & BUTTON_D) == BUTTON_D){
		if(!((oldState & BUTTON_D) == BUTTON_D)){
			// First time seeing this button down.
			// Time stamp it and update the old state.
			getTimeStamp((void *)&buttonState.ButtonTimer.D);
			oldState |= BUTTON_D;
		}
	}

	// Now see if any buttons are released and figure out how long they had been pressed
	if((oldState & BUTTON_A) == BUTTON_A){
		// Button was down, is it still?
		if(!((curState & BUTTON_A) == BUTTON_A)){
			// No longer down.  How long of a press was it?
			if(timeIsTimeout((void *)&buttonState.ButtonTimer.A, BUTTON_LONG_PRESS)){
				buttonState.LongPress |= BUTTON_A;
			}
			else if(timeIsTimeout((void *)&buttonState.ButtonTimer.A, BUTTON_SHORT_PRESS)){
				buttonState.ShortPress |= BUTTON_A;
			}
			// Clear it from the old state
			oldState &= ~BUTTON_A;
		}
	}
	if((oldState & BUTTON_B) == BUTTON_B){
		// Button was down, is it still?
		if(!((curState & BUTTON_B) == BUTTON_B)){
			// No longer down.  How long of a press was it?
			if(timeIsTimeout((void *)&buttonState.ButtonTimer.B, BUTTON_LONG_PRESS)){
				buttonState.LongPress |= BUTTON_B;
			}
			else if(timeIsTimeout((void *)&buttonState.ButtonTimer.B, BUTTON_SHORT_PRESS)){
				buttonState.ShortPress |= BUTTON_B;
			}
			// Clear it from the old state
			oldState &= ~BUTTON_B;
		}
	}
	if((oldState & BUTTON_C) == BUTTON_C){
		// Button was down, is it still?
		if(!((curState & BUTTON_C) == BUTTON_C)){
			// No longer down.  How long of a press was it?
			if(timeIsTimeout((void *)&buttonState.ButtonTimer.C, BUTTON_LONG_PRESS)){
				buttonState.LongPress |= BUTTON_C;
			}
			else if(timeIsTimeout((void *)&buttonState.ButtonTimer.C, BUTTON_SHORT_PRESS)){
				buttonState.ShortPress |= BUTTON_C;
			}
			// Clear it from the old state
			oldState &= ~BUTTON_C;
		}
	}
	if((oldState & BUTTON_D) == BUTTON_D){
		// Button was down, is it still?
		if(!((curState & BUTTON_D) == BUTTON_D)){
			// No longer down.  How long of a press was it?
			if(timeIsTimeout((void *)&buttonState.ButtonTimer.D, BUTTON_LONG_PRESS)){
				buttonState.LongPress |= BUTTON_D;
			}
			else if(timeIsTimeout((void *)&buttonState.ButtonTimer.D, BUTTON_SHORT_PRESS)){
				buttonState.ShortPress |= BUTTON_D;
			}
			// Clear it from the old state
			oldState &= ~BUTTON_D;
		}
	}

}

/**
 * Initialize the button state data store
 */
void buttonInit(void){

	// Button physical layout
	//
	// ------------------
	// |     Screen     |
	// ------------------
	//  A  B  C  D  RESET
	//

	// Direction is input
	LPC_GPIO->DIR[1] &= ~(1<<20);	// Button A
	LPC_GPIO->DIR[0] &= ~(1<<1);	// Button B
	LPC_GPIO->DIR[1] &= ~(1<<19);	// Button C
	LPC_GPIO->DIR[0] &= ~(1<<16);	// Button D

	// Hysteresis - bit 5
	// Pull up - bit 4
	// Invert - bit 6 (high reads as low, since we have a pull up enabled)
	// PIO0_16 needs bit 7 for digital mode
	LPC_IOCON->PIO1_20  = (1<<6 | 1<<5 | 1<<4);			// Button A
	LPC_IOCON->PIO0_1  	= (1<<6 | 1<<5 | 1<<4);			// Button B
	LPC_IOCON->PIO1_19 	= (1<<6 | 1<<5 | 1<<4);			// Button C
	LPC_IOCON->PIO0_16 	= (1<<6 | 1<<5 | 1<<4 | 1<<7);	// Button D

	// Init the buttonState holder
	memset((void *)&buttonState, 0, sizeof(buttonState));

	buttonState.Initialized = true;

}

/**
 * Determine if the button data has been initialized
 *
 * @return status of button data initialize
 */
bool buttonIsInitialized(void){
	return buttonState.Initialized;
}

/**
 * Return the state of the short press
 *
 * @param button The button we are interested in
 * @return button short press state
 */
bool buttonIsShortPressed(BUTTON_ID button){

	// Save the state
	bool state = (buttonState.ShortPress & button) == button;

	// Clear it now that we've read it
	buttonState.ShortPress &= ~button;

	return state;

}

/**
 * Return the state of the long press
 *
 * @param button The button we are interested in
 * @return button long press state
 */
bool buttonIsLongPressed(BUTTON_ID button){

	// Save the state
	bool state = (buttonState.LongPress & button) == button;

	// Clear it now that we've read it
	buttonState.LongPress &= ~button;

	return state;

}



