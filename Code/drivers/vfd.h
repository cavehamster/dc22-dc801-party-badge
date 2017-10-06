/**
 *
 * @brief Driver for the VFD screen
 * @file vfd.h
 * @author hamster
 *
 */

#ifndef _VFD_H
#define _VFD_H

#define VFD_MAX_ROW	1
#define VFD_MAX_COL	15

#define VFD_COMMAND						0xF8
#define VFD_INSTRUCTION_CHAR			0xFA
#define VFD_INSTRUCTION_CLEAR			0x01
#define VFD_INSTRUCTION_INIT			0x0F

// Strobe and Clock Macros
#define VFD_STROBE_START 	{ uint8_t i; LPC_GPIO->CLR[ 1 ] = (1 << 14); for( i=0; i<48; i++ ) __NOP(); }
#define VFD_STROBE_STOP 	{ uint8_t i; LPC_GPIO->SET[ 1 ] = (1 << 14); for( i=0; i<48; i++ ) __NOP(); }


typedef struct {
	uint8_t row;
	uint8_t col;
} VFD_Position;

typedef enum {
	VFD_NOTINITIALIZED,
	VFD_INITIALIZING,
	VFD_INITIALIZED,
	VFD_POWEREDOFF,
	VFD_BUSY
} VFD_Status;

typedef enum {
	VFD_OFF,
	VFD_ON
} VFD_Powerstate;

typedef enum {
	VFD_INIT,
	VFD_CLEAR,
	VFD_ENABLE,
	VFD_DISABLE,
	VFD_CURSOR_ON,
	VFD_CURSOR_OFF,
	VFD_CURSOR_BLINK,
	VFD_CURSOR_NO_BLINK
} VFD_Command;

typedef struct{
	bool displayOn;
	bool cursorOn;
	bool blinkOn;
	VFD_Position position;
	VFD_Powerstate powerState;
	VFD_Status status;
} VFD_Display;

void VFDInit(void);
void VFDWriteString(char* string);
void VFDSetPower(VFD_Powerstate newState);
void VFDWriteChar(char newChar);
void VFDSendCommand (VFD_Command newCommand);
void VFDSetPosition(uint8_t row, uint8_t col);


#endif
