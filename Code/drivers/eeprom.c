/**
 *
 * @brief Driver for the EEPROM
 * @file eeprom.c
 * @author hamster
 *
 * Interface to the ARM Thumb instructions for accessing the EEPROM
 *
 *  Adapted in large part from the lpcopen source code
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 *
 *
 */

#include "LPC13Uxx.h"
#include "lpc_types.h"
#include <string.h>
#include "drivers/common.h"
#include "drivers/eeprom.h"


/**
 * Read part identification number
 *
 * @param pOutput Pointer to where we want the data
 */
void FLASH_ReadPartID(FLASH_READ_PART_ID_OUTPUT_T *pOutput){
	FLASH_READ_PART_ID_COMMAND_T command;
	command.cmd = FLASH_READ_PART_ID;
	FLASH_Execute((FLASH_COMMAND_T *) &command, (FLASH_OUTPUT_T *) pOutput);
}

/**
 * Read boot code version number
 *
 * @param pOutput Pointer to where we want the data
 */
void FLASH_ReadBootCodeVersion(FLASH_READ_BOOTCODE_VER_OUTPUT_T *pOutput){
	FLASH_READ_BOOTCODE_VER_COMMAND_T command;
	command.cmd = FLASH_READ_BOOT_VER;
	FLASH_Execute((FLASH_COMMAND_T *) &command, (FLASH_OUTPUT_T *) pOutput);
}

/**
 * Function to reinvoke ISP
 *
 * Not used in this project
 */
void FLASH_ReInvokeISP(void){
	FLASH_REINVOKE_ISP_COMMAND_T command;
	FLASH_REINVOKE_ISP_OUTPUT_T output;

	command.cmd = FLASH_REINVOKE_ISP;
	FLASH_Execute((FLASH_COMMAND_T *) &command, (FLASH_OUTPUT_T *) &output);
}

/**
 * Read the UID of the chip
 *
 * @param pOutput Pointer to where we want the data
 */
void FLASH_ReadUID(FLASH_READ_UID_OUTPUT_T *pOutput){
	FLASH_READ_UID_COMMAND_T command;
	command.cmd = FLASH_READ_UID;
	FLASH_Execute((FLASH_COMMAND_T *) &command, (FLASH_OUTPUT_T *) pOutput);
}

/**
 * Read len bytes of data from EEPROM
 *
 * @param data Where to store the read data
 * @param len Number of bytes to read
 * @return true if read went OK
 */
bool FLASH_ReadData(unsigned int *data, unsigned int len){

	EEPROM_READ_COMMAND_T rCommand;
	EEPROM_READ_OUTPUT_T rOutput;

	uint32_t buffer[0x40 / sizeof(uint32_t)];
	memset(buffer, 0, sizeof(buffer));
	uint8_t *ptr = (uint8_t *) buffer;

	// read from flash
	rCommand.cmd = FLASH_EEPROM_READ;
	rCommand.eepromAddr = 0;
	rCommand.ramAddr = (uint32_t) data;
	rCommand.byteNum = len;
	rCommand.cclk = SystemCoreClock / 1000;

	EEPROM_Read(&rCommand, &rOutput);

	if (rOutput.status != FLASH_CMD_SUCCESS) {
		return FALSE;
	}
	else{
		return TRUE;
	}
}


/**
 * Write len bytes to the EEPROM
 *
 * @param data Pointer to data to write
 * @param len Bytes to write (you want these word aligned and even)
 * @return true if write went OK
 */
bool FLASH_WriteData(unsigned int *data, unsigned int len){

	EEPROM_WRITE_COMMAND_T wCommand;
	EEPROM_WRITE_OUTPUT_T wOutput;

	// write to flash
	wCommand.cmd = FLASH_EEPROM_WRITE;
	wCommand.eepromAddr = 0;
	wCommand.ramAddr = (uint32_t) data;
	wCommand.byteNum = len + 4;
	wCommand.cclk = SystemCoreClock / 1000;

	__disable_irq();
	EEPROM_Write(&wCommand, &wOutput);
	__enable_irq();

	if (wOutput.status != FLASH_CMD_SUCCESS) {
		return FALSE;
	}
	else{
		return TRUE;
	}

}

