/**
 *
 * @brief datastore
 * @file data.c
 * @author hamster
 *
 *  A basic datastore implementation.  This is shared data, using some interface functions
 *  to keep it somewhat protected but available globally
 *
 */

#include "LPC13Uxx.h"
#include "lpc_types.h"
#include <string.h>
#include "core/inc/lpc13uxx_uart.h"
#include "drivers/bluetooth.h"
#include "drivers/eeprom.h"
#include "drivers/common.h"

#define DATA_MODULE
#include "data.h"

/**
 * Clear all data and store to EEPROM
 */
void dataClear(void){
	memset(&dataStore, 0, sizeof(dataStore));
	dataUpdate.update = true;

	while(dataUpdate.update == true){
		// Stay here until the update happens
	}
}

/**
 * Initialize the datastore
 */
void dataInit(void){

	// Set it to all zero
	memset(&dataStore, 0, sizeof(dataStore));

	// Clear the update flags
	memset(&dataUpdate, 0, sizeof(dataUpdate));

	lpcprintf("Datastore is %d bytes\r\n", sizeof(dataStore));

	// Read in the data from the EEPROM
	bool ret = FLASH_ReadData((unsigned int *)&dataStore, sizeof(dataStore));

	if(ret == TRUE){
		lpcprintf("Data read success\r\n");

		lpcprintf("Configured: %02X\r\n", dataStore.configured);
		lpcprintf("Username: ");
		for(int i = 0; i < 8; i++){
			lpcprintf("%c", dataStore.username[i]);
		}
		lpcprintf("\r\n");
		lpcprintf("Flags: BLE %d Pairable %d\r\n",
				dataStore.BLEEnabled,
				dataStore.BLEPairable);
		lpcprintf("BLE data: ");
		for(int i = 0; i < 260; i++){
			lpcprintf("%02X ", dataStore.BLEDynamicData[i]);
		}
		lpcprintf("\r\n");

	}
	else{
		lpcprintf("Data read fail\r\n");
	}

	// Check if the data has been configured
	if(dataStore.configured != DATA_CONFIGURED){
		// There was no data stored or it was corrupt.  Reset the struct to zeros, and write it back out
		lpcprintf("Not configured, storing data...\r\n");
		memset(&dataStore, 0, sizeof(dataStore));
		dataStore.configured = DATA_CONFIGURED;
		memcpy(dataStore.username, DEFAULT_USERNAME, 8);

		dataUpdate.update = true;

		while(dataUpdate.update == true){
			// Wait for the IRQ to update the datastore before proceeding
		}

	}
	else{
		lpcprintf("Configured\r\n");
	}

}

/**
 * Flush the current data out to EEPROM, if it has changed
 *
 * @note This gets called once a second from an interrupt to keep the data update in EEPROM if it has changed
 */
void dataFlush(void){

	if(dataUpdate.update){
		dataUpdate.update = false;
		lpcprintf("Store: ");

		if(FLASH_WriteData((unsigned int *)&dataStore, sizeof(dataStore))){
			lpcprintf("success\r\n");
		}
		else{
			lpcprintf("fail\r\n");
		}
	}


}

/**
 * Set the flag that the data is ready to be stored
 */
void dataChanged(void){
	dataUpdate.update = true;
}

/**
 * Get how many bytes of BLE dynamic data is stored
 *
 * @return How many bytes are stored?
 */
uint8_t dataGetBLEDataMsgs(void){
	return dataStore.BLEDynamicDataMsgs;
}

/**
 * Get a pointer to the start of the BLE dynamic data
 *
 * @return The pointer to the start of the BLE data
 */
uint8_t * dataGetBLEData(void){
	return &dataStore.BLEDynamicData[0];
}

