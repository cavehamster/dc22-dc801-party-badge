/**
 *
 * @brief Driver for the VFD display
 * @file vfd.c
 * @author hamster
 *
 * Based on code developed by @d4rkm4tter - well, at one point.  It's been heavily refactored.
 *
 */

#include <stdint.h>

#include "LPC13Uxx.h"
#include "lpc_types.h"
#include "core/inc/lpc13uxx_gpio.h"
#include "drivers/common.h"
#include "drivers/ssp.h"
#include "vfd.h"
#include <string.h>

#include <inttypes.h>

static VFD_Display VFDStatus;

/**
 * Initialize the VFD display
 */
void VFDInit(void) {

	// Configure as outputs
	LPC_GPIO->DIR[PORT0] |= (1<<4);		// VFD Power enable
	LPC_GPIO->DIR[PORT1] |= (1<<14);	// VFD Strobe
	LPC_GPIO->DIR[PORT1] |= (1<<15);	// VFD Clock
	LPC_GPIO->DIR[PORT1] |= (1<<22);	// VFD Data
	
	memset(&VFDStatus, 0, sizeof(VFDStatus));

	ssp1Init();
	
	VFD_STROBE_STOP;
	
	VFDSetPower(VFD_OFF);
	//VFDSendCommand(VFD_INIT);
	//VFDSendCommand(VFD_DISABLE);

}


/**
 * Move the cursor to a position on the screen
 *
 * @param row Row, either 0 or 1
 * @param col Col, 0-16
 * @note If the row/col is out of range, no move is done
 */
void VFDSetPosition(uint8_t row, uint8_t col){

	if(row > VFD_MAX_ROW) return;
	if(col > VFD_MAX_COL) return;

	// OK, encode the position and set it
	uint8_t position = 0x80 + col;

	if(row == 0){
		position |= 0x00;
	}
	else{
		position |= 0x40;
	}

	VFDStatus.position.row = row;
	VFDStatus.position.col = col;

	VFD_STROBE_START;
	ssp1SendChar(VFD_COMMAND);
	ssp1SendChar(position);
	VFD_STROBE_STOP;

}

/**
 * Set display brightness
 *
 * @param percent amount in percent
 * @todo implement function
 */
void setBrightness(int percent){
	//byte: 0011 BR3 BR2 BR1 BR0
	//6.25% increment - 1111 to 0000

}

/**
 * Set the power state of the VFD
 *
 * @param newState Either on or off
 */
void VFDSetPower(VFD_Powerstate newState) {

	switch(newState){
		case VFD_ON:
			// Power on the VFD
			LPC_GPIO->CLR[PORT0] = 1<<4;
			VFDStatus.powerState = VFD_ON;
			break;
		case VFD_OFF:
			// Power off the VFD
			LPC_GPIO->SET[PORT0] = 1<<4;
			VFDStatus.powerState = VFD_OFF;
			break;
		default:
			// No change in state
			break;
	}

}

/**
 * Send a command to the display
 *
 * @param command
 */
void VFDSendCommand(VFD_Command command) {

	uint8_t instruction;

	switch(command){
		case VFD_CLEAR:
			// Clear the display, fills the internal buffer with 0x20 (spaces)
			instruction = VFD_INSTRUCTION_CLEAR;
			VFDSetPosition(0, 0);
			break;
		case VFD_INIT:
			// Display on, cursor on, blink on
			instruction = VFD_INSTRUCTION_INIT;
			VFDSetPosition(0, 0);
			break;
		case VFD_ENABLE:
			VFDStatus.displayOn = true;
			instruction = (0x08 | (VFDStatus.displayOn << 2) | (VFDStatus.cursorOn << 1) | VFDStatus.blinkOn);
			break;
		case VFD_DISABLE:
			VFDStatus.displayOn = false;
			instruction = (0x08 | (VFDStatus.displayOn << 2) | (VFDStatus.cursorOn << 1) | VFDStatus.blinkOn);
			break;
		case VFD_CURSOR_ON:
			VFDStatus.cursorOn = true;
			instruction = (0x08 | (VFDStatus.displayOn << 2) | (VFDStatus.cursorOn << 1) | VFDStatus.blinkOn);
			break;
		case VFD_CURSOR_OFF:
			VFDStatus.cursorOn = false;
			instruction = (0x08 | (VFDStatus.displayOn << 2) | (VFDStatus.cursorOn << 1) | VFDStatus.blinkOn);
			break;
		case VFD_CURSOR_BLINK:
			VFDStatus.blinkOn = true;
			instruction = (0x08 | (VFDStatus.displayOn << 2) | (VFDStatus.cursorOn << 1) | VFDStatus.blinkOn);
			break;
		case VFD_CURSOR_NO_BLINK:
			VFDStatus.blinkOn = false;
			instruction = (0x08 | (VFDStatus.displayOn << 2) | (VFDStatus.cursorOn << 1) | VFDStatus.blinkOn);
			break;
		default:
			break;

	}

	VFD_STROBE_START;
	ssp1SendChar(VFD_COMMAND);
	ssp1SendChar(instruction);
	VFD_STROBE_STOP;

}

/**
 * Write a string to the display
 *
 * @param str the string to write, null terminated
 * @param filler whether to send the line filler
 */
void VFDWriteString(char* str){

	int len = strlen(str);

	for(int i = 0; i< len; i++){
		VFDWriteChar(str[i]);
	}
	
}

/**
 * Send a char to the display
 *
 * @param newChar
 */
void VFDWriteChar(char newChar){

	// Figure out if we need goto the next row
	if(VFDStatus.position.col == 15){
		VFDStatus.position.row++;
		VFDStatus.position.col = 0;
		if(VFDStatus.position.row > 1){
			VFDStatus.position.row = 0;
		}
		VFDSetPosition(VFDStatus.position.row, VFDStatus.position.col);
	}

	VFD_STROBE_START;
	ssp1SendChar(VFD_INSTRUCTION_CHAR);
	ssp1SendChar(newChar);
	VFD_STROBE_STOP;

}


