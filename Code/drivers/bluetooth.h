/**
 *
 * @brief Driver for BLE
 * @file bluetooth.h
 * @author hamster
 *
 * Handle BLE events
 *
 */

#ifndef DRIVERS_BLUETOOTH_H_
#define DRIVERS_BLUETOOTH_H_

#include "drivers/ble/lib_aci.h"
#include "drivers/ble/aci_setup.h"

#define MANU_CRYPTO				0x0C97
#define MANU_QUEERCON			0x04D3
#define MANU_DC801				0x0801
#define MANU_DC503				0x0503
#define MANU_BENDER				0x049E
#define MANU_DCZIA				0x5050
#define MANU_DCZIA_SHOTBOT		0x5051

#define SETUP_NAME_SIZE_OFFSET 			4
#define SETUP_NAME_OFFSET 				6
#define SETUP_NAME_START_BYTE			10
#define SETUP_MANU_DATA_OFFSET 			40
#define SETUP_MANU_DATA_START_BYTE		6
#define SETUP_CRC_OFFSET 				41

#define UART_OVER_BLE_DISCONNECT      	0x01
#define UART_OVER_BLE_LINK_TIMING_REQ 	0x02
#define UART_OVER_BLE_TRANSMIT_STOP   	0x03
#define UART_OVER_BLE_TRANSMIT_OK     	0x04

typedef struct{
    uint8_t uart_rts_local;  /* State of the local UART RTS  */
    uint8_t uart_rts_remote; /* State of the remote UART RTS */
} uart_over_ble_t;

typedef enum{
	BLE_BROADCAST_VECTOR,
	BLE_BROADCAST_PARTY,
	BLE_BROADCAST_RESET
} BLE_BROADCAST;

void BLEInit(void);
//void BLEEnable(bool enable);
void BLEUpdateSetup(void);
void BLEHandle(void);
void updateSetupCRC(void);
void BLEDeletePairing(void);
void BLESetManufacturer(uint16_t manu);
void BLESetName(uint8_t *name);
void BLEUartInit(void);
aci_status_code_t BLEBondDataRestore(aci_state_t *aci_stat, bool *bonded_first_time_state);
bool BLEBondDataSave(aci_state_t *aci_stat);

#endif /* DRIVERS_BLUETOOTH_H_ */
