/**
 *
 * @brief Menu state machine
 * @file menu.c
 * @author hamster
 *
 * Handler for on-screen menus
 *
 * It's a somewhat ugly state machine, but hey, it's pretty verbose
 *
 */

#include <string.h>
#include "LPC13Uxx.h"
#include "lpc_types.h"
#include "core/inc/lpc13uxx_uart.h"
#include "drivers/vfd.h"
#include "drivers/common.h"
#include "drivers/buttons.h"
#include "drivers/bluetooth.h"
#include "drivers/ws2812b.h"
#include "data.h"
#include "menu.h"


DISPLAY_DATA displayData;

/**
 * Init the display state data
 */
void displayStateInit(void){

	memset(&displayData, 0, sizeof(DISPLAY_DATA));

	// Set the top line to default
	memcpy(displayData.topLine, STRING_DISPLAY_MAIN, 16);

	displayData.curDisplayState = DISPLAY_BOOTUP;
	displayData.oldDisplayState = DISPLAY_BOOTUP;
	displayData.stateChanged = true;

}

/**
 * Update the string for the top line
 *
 * @param newData A pointer to a string to copy
 * @param len Number of chars to copy
 */
void displayUpdateTopLine(uint8_t *newData, uint8_t len){

	uint8_t position = 0;

	// The top line is treated as a circular buffer, start back at zero every time
	for(uint8_t i = 0; i < len && i < 254; i++){
		displayData.topLine[position++] = newData[i];
		if(position > 16){
			position = 0;
		}
	}

	displayData.topLineChanged = true;


}

/**
 * Display something from the BLE stack
 *
 * @param state What state to show, pairing, paired or fail
 * @param pairingKey Pointer to the pairing key to use
 */
void displayPairing(PAIRING_STATE state, uint8_t *pairingKey){

	switch(state){
		case PAIRING_SHOW_KEY:
			displayData.bleDisplayCommand = DISPLAY_PAIRING_KEY;
			displayData.pairingKey = pairingKey;
			break;
		case PAIRING_SUCCESS:
			displayData.bleDisplayCommand = DISPLAY_PAIRING_SUCCESS;
			displayData.pairingKey = NULL;
			break;
		case PAIRING_FAILED:
			displayData.bleDisplayCommand = DISPLAY_PAIRING_FAIL;
			displayData.pairingKey = NULL;
			break;
	}

	displayData.bleUpdate = true;

}


/**
 * Update the display state machine
 *
 * @note Non-blocking, this is called constantly.  Make sure you set a flag if you want to update
 * something so that it isn't updated constantly.
 */
void displayStateMachine(void){
	SELECTION selection = SELECTION_WAITING;
	TIME difference = { 0, 0 };

	if(displayData.bleUpdate){
		// Bluetooth wants to show something on the screen for a moment
		displayData.curDisplayState = displayData.bleDisplayCommand;
		displayData.stateChanged = true;
		displayData.bleUpdate = false;
	}

	switch(displayData.curDisplayState){
		case DISPLAY_BOOTUP:
			if(displayData.stateChanged){
				displayData.oldDisplayState = DISPLAY_BOOTUP;
				displayData.stateChanged = false;
				lpcprintf("Enter state bootup\r\n");
				getTimeStamp(&displayData.timeStamp);
				VFDSendCommand(VFD_CLEAR);
				VFDWriteString(STRING_DISPLAY_BOOTUP_1);
				VFDSetPosition(1,0);
				VFDWriteString(STRING_DISPLAY_BOOTUP_2);
				ws2812bSetPixel(128, 0, 0);
			}

			timeElapsedSince(&displayData.timeStamp, &difference);

			if(difference.Milliseconds > 500 && difference.Milliseconds < 600){
				ws2812bSetPixel(0, 128, 0);
			}
			if(difference.Seconds == 1 && difference.Milliseconds < 100){
				ws2812bSetPixel(0, 0, 128);
			}
			if(timeIsTimeout(&displayData.timeStamp, 1500)){
				ws2812bSetPixel(0, 0, 0);
				displayData.curDisplayState = DISPLAY_MAIN;
				displayData.stateChanged = true;
			}
			break;

		case DISPLAY_SETUP:
			break;
		case DISPLAY_MAIN:
			if(displayData.stateChanged){
				displayData.oldDisplayState = DISPLAY_MAIN;
				displayData.stateChanged = false;
				lpcprintf("Enter state main\r\n");
				VFDSendCommand(VFD_CLEAR);
				VFDSendCommand(VFD_CURSOR_OFF);
				VFDWriteString(displayData.topLine);
				VFDSetPosition(1, 0);
				VFDWriteString(STRING_DISPLAY_MENU_MAIN);
				VFDSetPosition(1, 8);
				VFDWriteString(dataStore.username);
			}

			if(displayData.topLineChanged){
				displayData.topLineChanged = false;
				VFDSetPosition(0, 0);
				VFDWriteString(displayData.topLine);
			}

			if(buttonIsShortPressed(BUTTON_A)){
				// Goto the next menu item
				displayData.curDisplayState = DISPLAY_SET_USERNAME;
				displayData.stateChanged = true;
			}
			break;

		case DISPLAY_SET_USERNAME:
			if(displayData.stateChanged){
				displayData.oldDisplayState = DISPLAY_SET_USERNAME;
				displayData.stateChanged = false;
				getTimeStamp(&displayData.buttonTimer);
				lpcprintf("Enter state set username\r\n");
				VFDSendCommand(VFD_CLEAR);
				VFDWriteString(STRING_DISPLAY_SET_USERNAME);
				VFDSetPosition(1, 0);
				VFDWriteString(STRING_DISPLAY_MENU_ITEM);
			}

			if(buttonIsShortPressed(BUTTON_A)){
				// Goto the next menu item
				displayData.curDisplayState = DISPLAY_ENABLE_BLE;
				displayData.stateChanged = true;
			}
			if(buttonIsShortPressed(BUTTON_B) || buttonIsShortPressed(BUTTON_C)){
				// Set the username - display the instructions
				memcpy(displayData.string, dataStore.username, 8);
				displayData.curDisplayState = DISPLAY_INSTRUCTIONS;
				displayData.stateChanged = true;
			}
			if(buttonIsShortPressed(BUTTON_D)){
				// Goto the previous menu item
				displayData.curDisplayState = DISPLAY_MAIN;
				displayData.stateChanged = true;
			}

			// Has a button been pressed?
			if(timeIsTimeout(&displayData.buttonTimer, MENU_ITEM_TIMEOUT)){
				displayData.curDisplayState = DISPLAY_MAIN;
				displayData.stateChanged = true;
			}
			break;

		case DISPLAY_ENABLE_BLE:
			if(displayData.stateChanged){
				displayData.oldDisplayState = DISPLAY_ENABLE_BLE;
				displayData.stateChanged = false;
				getTimeStamp(&displayData.buttonTimer);
				lpcprintf("Enter state enable BLE\r\n");
				VFDSendCommand(VFD_CLEAR);
				VFDWriteString(STRING_DISPLAY_ENABLE_BLE);
				VFDSetPosition(1, 0);
				VFDWriteString(STRING_DISPLAY_MENU_ITEM);
			}

			if(buttonIsShortPressed(BUTTON_A)){
				// Goto the next menu item
				if(dataStore.BLEEnabled){
					displayData.curDisplayState = DISPLAY_SET_PAIRABLE;
				}
				else{
					displayData.curDisplayState = DISPLAY_MAIN;
				}
				displayData.stateChanged = true;
			}
			if(buttonIsShortPressed(BUTTON_B) || buttonIsShortPressed(BUTTON_C)){
				// Enable BLE?
				displayData.curDisplayState = DISPLAY_INPUT_BINARY_ON_OFF;
				displayData.stateChanged = true;
			}
			if(buttonIsShortPressed(BUTTON_D)){
				// Goto the previous menu item
				displayData.curDisplayState = DISPLAY_SET_USERNAME;
				displayData.stateChanged = true;
			}

			// Has a button been pressed?
			if(timeIsTimeout(&displayData.buttonTimer, MENU_ITEM_TIMEOUT)){
				displayData.curDisplayState = DISPLAY_MAIN;
				displayData.stateChanged = true;
			}
			break;

		case DISPLAY_SET_PAIRABLE:
			if(displayData.stateChanged){
				displayData.oldDisplayState = DISPLAY_SET_PAIRABLE;
				displayData.stateChanged = false;
				getTimeStamp(&displayData.buttonTimer);
				lpcprintf("Enter state set pairable\r\n");
				VFDSendCommand(VFD_CLEAR);
				VFDWriteString(STRING_DISPLAY_SET_PAIRABLE);
				VFDSetPosition(1, 0);
				VFDWriteString(STRING_DISPLAY_MENU_ITEM);
			}

			if(buttonIsShortPressed(BUTTON_A)){
				// Goto the next menu item
				displayData.curDisplayState = DISPLAY_DELETE_PAIRING;
				displayData.stateChanged = true;
			}
			if(buttonIsShortPressed(BUTTON_B) || buttonIsShortPressed(BUTTON_C)){
				// Set pairable?
				displayData.curDisplayState = DISPLAY_INPUT_BINARY_YES_NO;
				displayData.stateChanged = true;
			}
			if(buttonIsShortPressed(BUTTON_D)){
				// Goto the previous menu item
				displayData.curDisplayState = DISPLAY_ENABLE_BLE;
				displayData.stateChanged = true;
			}

			// Has a button been pressed?
			if(timeIsTimeout(&displayData.buttonTimer, MENU_ITEM_TIMEOUT)){
				displayData.curDisplayState = DISPLAY_MAIN;
				displayData.stateChanged = true;
			}
			break;

		case DISPLAY_DELETE_PAIRING:
			if(displayData.stateChanged){
				displayData.oldDisplayState = DISPLAY_DELETE_PAIRING;
				displayData.stateChanged = false;
				getTimeStamp(&displayData.buttonTimer);
				lpcprintf("Enter state delete pairing\r\n");
				VFDSendCommand(VFD_CLEAR);
				VFDWriteString(STRING_DISPLAY_DELETE_PAIRING);
				VFDSetPosition(1, 0);
				VFDWriteString(STRING_DISPLAY_MENU_ITEM);
			}

			if(buttonIsShortPressed(BUTTON_A)){
				// Goto the next menu item
				displayData.curDisplayState = DISPLAY_MAIN;
				displayData.stateChanged = true;
			}
			if(buttonIsShortPressed(BUTTON_B) || buttonIsShortPressed(BUTTON_C)){
				// Delete pairing?
				BLEDeletePairing();
			}
			if(buttonIsShortPressed(BUTTON_D)){
				// Goto the previous menu item
				displayData.curDisplayState = DISPLAY_SET_PAIRABLE;
				displayData.stateChanged = true;
			}

			// Has a button been pressed?
			if(timeIsTimeout(&displayData.buttonTimer, MENU_ITEM_TIMEOUT)){
				displayData.curDisplayState = DISPLAY_MAIN;
				displayData.stateChanged = true;
			}
			break;


		case DISPLAY_INSTRUCTIONS:
			if(displayData.stateChanged){
				displayData.stateChanged = false;
				lpcprintf("Enter state display instructions\r\n");
				VFDSendCommand(VFD_CLEAR);
				VFDWriteString(STRING_DISPLAY_STRING_INSTRUCTION_1);
				VFDSetPosition(1, 0);
				VFDWriteString(STRING_DISPLAY_STRING_INSTRUCTION_2);
				getTimeStamp(&displayData.timeStamp);
			}

			// Display the instructions for a couple of seconds
			if(timeIsTimeout(&displayData.timeStamp, 2000)){
				// Go to the input state
				displayData.curDisplayState = DISPLAY_INPUT_STRING;
				displayData.stateChanged = true;
			}
			break;

		case DISPLAY_INPUT_STRING:
			if(displayData.stateChanged){
				displayData.stateChanged = false;
				displayData.position = 0;
				VFDSendCommand(VFD_CURSOR_BLINK);
				lpcprintf("Enter state input string\r\n");
				VFDSendCommand(VFD_CLEAR);
				VFDWriteString(displayData.string);
				VFDSetPosition(1, 0);
				VFDWriteString(STRING_DISPLAY_STRING_ENTRY);
				VFDSetPosition(0, 0);
			}

			if(buttonIsShortPressed(BUTTON_A)){
				// Nevermind
				displayData.curDisplayState = DISPLAY_MAIN;
				displayData.stateChanged = true;
			}
			if(buttonIsShortPressed(BUTTON_B)){
				// Increment
				if(displayData.string[displayData.position] > LAST_ALLOWED_CHAR || displayData.string[displayData.position] < FIRST_ALLOWED_CHAR){
					displayData.string[displayData.position] = FIRST_LETTER - 1;
				}
				if(displayData.string[displayData.position] < LAST_ALLOWED_CHAR){
					displayData.string[displayData.position]++;
				}
				else{
					displayData.string[displayData.position] = FIRST_ALLOWED_CHAR;
				}
				VFDSetPosition(0, 0);
				VFDWriteString(displayData.string);
				VFDSetPosition(0, displayData.position);
			}
			if(buttonIsShortPressed(BUTTON_C)){
				// Decrement
				if(displayData.string[displayData.position] > LAST_ALLOWED_CHAR || displayData.string[displayData.position] < FIRST_ALLOWED_CHAR){
					displayData.string[displayData.position] = LAST_LETTER + 1;
				}
				if(displayData.string[displayData.position] > FIRST_ALLOWED_CHAR){
					displayData.string[displayData.position]--;
				}
				else{
					displayData.string[displayData.position] = LAST_ALLOWED_CHAR;
				}
				VFDSetPosition(0, 0);
				VFDWriteString(displayData.string);
				VFDSetPosition(0, displayData.position);
			}
			if(buttonIsShortPressed(BUTTON_D)){
				// Next char
				displayData.position++;
				if(displayData.position > 7){
					displayData.position = 0;
				}
				VFDSetPosition(0, displayData.position);
			}
			if(buttonIsLongPressed(BUTTON_D)){
				// Save and return
				displayData.curDisplayState = DISPLAY_MAIN;
				displayData.stateChanged = true;
				VFDSendCommand(VFD_CURSOR_NO_BLINK);
				VFDSendCommand(VFD_CURSOR_OFF);
				if(displayData.oldDisplayState == DISPLAY_SET_USERNAME){
					lpcprintf("New username: %s\r\n", displayData.string);
					memcpy(dataStore.username, displayData.string, 8);
					dataStore.username[8] = 0;
					dataUpdate.update = true;
					BLESetName(dataStore.username);
					BLEUpdateSetup();
				}
			}
			break;

		case DISPLAY_INPUT_BINARY_YES_NO:
		case DISPLAY_INPUT_BINARY_ON_OFF:
			if(displayData.stateChanged){
				displayData.stateChanged = false;
				displayData.position = 0;
				lpcprintf("Enter state get binary choice\r\n");
				VFDSendCommand(VFD_CLEAR);
				VFDSetPosition(0, 0);
				VFDWriteString(STRING_DISPLAY_ENABLE_BLE);
				VFDSetPosition(1, 0);
				if(displayData.oldDisplayState == DISPLAY_INPUT_BINARY_YES_NO){
					VFDWriteString(STRING_DISPLAY_BINARY_ENTRY_YES_NO);
				}
				else{
					VFDWriteString(STRING_DISPLAY_BINARY_ENTRY_ON_OFF);
				}
			}

			if(buttonIsShortPressed(BUTTON_A)){
				// Nevermind
				displayData.curDisplayState = DISPLAY_MAIN;
				displayData.stateChanged = true;
			}
			if(buttonIsShortPressed(BUTTON_C)){
				// On
				selection = SELECTION_TRUE;
				lpcprintf("Selected TRUE\r\n");
			}
			if(buttonIsShortPressed(BUTTON_D)){
				// Off
				selection = SELECTION_TRUE;
				lpcprintf("Selected FALSE\r\n");
			}

			if(selection != SELECTION_WAITING){
				// User made a selection
				switch(displayData.oldDisplayState){
					case DISPLAY_ENABLE_BLE:
						if(selection == SELECTION_TRUE){
							dataStore.BLEEnabled = true;
						}
						else{
							dataStore.BLEEnabled = false;
						}
						break;
					case DISPLAY_SET_PAIRABLE:
						if(selection == SELECTION_TRUE){
							dataStore.BLEPairable = true;
						}
						else{
							dataStore.BLEPairable = false;
						}
						break;

				}

				// Return to the previous menu
				displayData.curDisplayState = displayData.oldDisplayState;
				displayData.stateChanged = true;

				// Update the BLE
				BLEUpdateSetup();

				// Set the update to flash flag
				dataUpdate.update = true;
			}

			break;

		case DISPLAY_PAIRING_KEY:
			if(displayData.stateChanged){
				displayData.stateChanged = false;
				lpcprintf("Enter state show pairing key\r\n");
				VFDSendCommand(VFD_CLEAR);
				VFDWriteString(STRING_DISPLAY_PAIRING_KEY);
				ws2812bSetPixel(128, 128, 128);
				VFDSetPosition(1,0);
				for(int i = 0; i < 6; i++){
					VFDWriteChar(displayData.pairingKey[i]);
				}
			}
			break;
		case DISPLAY_PAIRING_FAIL:
			if(displayData.stateChanged){
				displayData.stateChanged = false;
				lpcprintf("Enter pairing fail\r\n");
				getTimeStamp(&displayData.timeStamp);
				VFDSendCommand(VFD_CLEAR);
				VFDWriteString(STRING_DISPLAY_PAIRING_FAILURE);
				ws2812bSetPixel(128, 0, 0);
			}
			if(timeIsTimeout(&displayData.timeStamp, PAIRING_STATUS_TIMEOUT)){
				displayData.curDisplayState = DISPLAY_MAIN;
				displayData.stateChanged = true;
				ws2812bSetPixel(0, 0, 0);
			}
			break;
		case DISPLAY_PAIRING_SUCCESS:
			if(displayData.stateChanged){
				displayData.stateChanged = false;
				lpcprintf("Enter pairing fail\r\n");
				getTimeStamp(&displayData.timeStamp);
				VFDSendCommand(VFD_CLEAR);
				VFDWriteString(STRING_DISPLAY_PAIRING_SUCCESS);
				ws2812bSetPixel(0, 128, 0);
			}
			if(timeIsTimeout(&displayData.timeStamp, PAIRING_STATUS_TIMEOUT)){
				displayData.curDisplayState = DISPLAY_MAIN;
				displayData.stateChanged = true;
				ws2812bSetPixel(0, 0, 0);
			}
			break;

	}

}

