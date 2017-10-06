/**
 *
 * @brief Driver for BLE
 * @file bluetooth.c
 * @author hamster
 *
 * Handle BLE events
 *
 */

#include "LPC13Uxx.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "core/inc/lpc13uxx_uart.h"
#include "drivers/common.h"
#include "drivers/bluetooth.h"
#include "drivers/ws2812b.h"
#include "menu.h"
#include "data.h"
#include "services.h"

#ifdef SERVICES_PIPE_TYPE_MAPPING_CONTENT
static services_pipe_type_mapping_t services_pipe_type_mapping[NUMBER_OF_PIPES] = SERVICES_PIPE_TYPE_MAPPING_CONTENT;
#else
#define NUMBER_OF_PIPES 0
static const services_pipe_type_mapping_t * services_pipe_type_mapping = NULL;
#endif
static hal_aci_data_t setup_msgs[NB_SETUP_MESSAGES] = SETUP_MESSAGES_CONTENT;
static struct aci_state_t aci_state;
static hal_aci_evt_t aci_data;
static hal_aci_data_t aci_cmd;

static bool BLESetupRequired 	= false;
static bool BLETimingChangeDone	= false;
static bool BLESetup = false;

static uart_over_ble_t uart_over_ble;

static bool BLEBondedFirstTime = true;

/**
 * Command the radio to reset
 * This will push it back to standby state, then we can reprogram it
 */
void BLEUpdateSetup(void){
	lib_aci_pin_reset();
	BLEInit();
}


/**
 * Setup the BLE services
 */
void BLEInit(void){

	//	Point ACI data structures to the the setup data that the nRFgo studio generated for the nRF8001
	if (NULL != services_pipe_type_mapping){
		aci_state.aci_setup_info.services_pipe_type_mapping = &services_pipe_type_mapping[0];
	}
	else{
		aci_state.aci_setup_info.services_pipe_type_mapping = NULL;
	}
	aci_state.aci_setup_info.number_of_pipes    = NUMBER_OF_PIPES;
	aci_state.aci_setup_info.setup_msgs         = (hal_aci_data_t*) setup_msgs;
	aci_state.aci_setup_info.num_setup_msgs 	= NB_SETUP_MESSAGES;

	// Call the library init
	lib_aci_init(&aci_state, false);
	aci_state.bonded = ACI_BOND_STATUS_FAILED;

}

/**
 * Check for any pending BLE event and handle it
 */
void BLEHandle(void){

	static TIME BLEDataUpdateTimer = { 0, 0 };

	// If the data update timer is up, ask for the updated temp and battery to set in the pipes
	if(timeIsTimeout(&BLEDataUpdateTimer, 1000)){
		getTimeStamp(&BLEDataUpdateTimer);
		if(BLESetup){
			lib_aci_get_temperature();
			lib_aci_get_battery_level();
		}
	}

	// We enter the if statement only when there is a ACI event available to be processed
	if (lib_aci_event_get(&aci_state, &aci_data)){
		aci_evt_t * aci_evt;

		aci_evt = &aci_data.evt;
		switch(aci_evt->evt_opcode){
			// When the device is reset at power on, or via the RESET pin, it will send
			// ACI_EVT_DEVICE_STARTED when it is ready
			case ACI_EVT_DEVICE_STARTED:
			{
				aci_state.data_credit_available = aci_evt->params.device_started.credit_available;
				switch(aci_evt->params.device_started.device_mode){
				case ACI_DEVICE_SETUP:
					// Device is in setup mode, requires setup
					lpcprintf("BLE mode: Setup\r\n");
					if(dataStore.BLEEnabled){
						lpcprintf("BLE: Enabled\r\n");
						BLESetupRequired = true;
					}
					else{
						lpcprintf("BLE: Disabled\r\n");
						BLESetupRequired = false;
					}
					BLESetup = false;
					break;

				case ACI_DEVICE_STANDBY:
					// Device is in standby
					lpcprintf("BLE mode: Standby\r\n");
					BLESetup = true;
					if (aci_evt->params.device_started.hw_error){
						// Magic number used to make sure the HW error event is handled correctly.
						delay_ms(20);
					}
					else{
						// Set the firmware rev string
						lpcprintf("BLE: Set firmware version\r\n");
						uint8_t version[] = FIRMWARE_REV;
						lib_aci_set_local_data(&aci_state, PIPE_DEVICE_INFORMATION_FIRMWARE_REVISION_STRING_SET,
								(uint8_t *)&version, 4);

						if(dataStore.BLEPairable){
							// Device is pairable
							if(dataGetBLEDataMsgs() > 0){
								if (ACI_STATUS_TRANSACTION_COMPLETE == BLEBondDataRestore(&aci_state, &BLEBondedFirstTime)){
									lpcprintf("Bond restored successfully");
								}
								else{
									lpcprintf("Bond restore failed. Delete the bond and try again.\r\n");
								}
							}
							else{
								lpcprintf("No bond stored\r\n");
							}

							// Start bonding as all proximity devices need to be bonded to be usable
							if (ACI_BOND_STATUS_SUCCESS != aci_state.bonded){
								lib_aci_bond(180, 0x0100);
								lpcprintf("BLE command: advertise, bondable\r\n");
							}
							else{
								//connect to an already bonded device
								//Use lib_aci_direct_connect for faster re-connections with PC, not recommended to use with iOS/OS X
								lib_aci_connect(100, 0x0100);
								lpcprintf("BLE command: advertise, connecting\r\n");
							}
						}
						else{
							// Device is not pairable
							lib_aci_broadcast(100, 0x0100);
							lpcprintf("BLE command: advertise only\r\n");
						}
					}
					break;
				}
			}
			break; // ACI Device Started Event

			case ACI_EVT_CMD_RSP:
				// If an ACI command response event comes with an error -> stop
				if (ACI_STATUS_SUCCESS != aci_evt->params.cmd_rsp.cmd_status){
					// ACI ReadDynamicData and ACI WriteDynamicData will have status codes of
					// TRANSACTION_CONTINUE and TRANSACTION_COMPLETE
					// all other ACI commands will have status code of ACI_STATUS_SCUCCESS for a successful command
					lpcprintf("BLE error response to command %02X\r\n", aci_evt->params.cmd_rsp.cmd_opcode);
				}
				else{
					lpcprintf("BLE: Response %02X (%d bytes): ", aci_evt->evt_opcode, aci_evt->len);
					int len = aci_evt->len;
					if(len > HAL_ACI_MAX_LENGTH){
						len = HAL_ACI_MAX_LENGTH;
					}

					uint8_t data[32];
					memset(data, 0, 32);
					memcpy(data, &aci_evt->params.cmd_rsp, len);
					for(int i = 0; i < len - 1; i++){
						lpcprintf("%02X ", data[i]);
					}
					lpcprintf("\r\n");

					switch(aci_evt->params.cmd_rsp.cmd_opcode){
						case ACI_CMD_RADIO_RESET:
							// The radio has successfully reset
							// Reconfigure it
							BLESetup = false;
							lpcprintf("BLE: soft reset, reconfigure\r\n");
							BLESetupRequired = true;
							break;
						case ACI_CMD_CONNECT:
							// We're advertising now
							BLESetup = true;
							lpcprintf("BLE: Advertising\r\n");
							break;
						case ACI_CMD_SET_LOCAL_DATA:
							// Local data was set
							lpcprintf("BLE: Data set\r\n");
							break;
						case ACI_CMD_GET_BATTERY_LEVEL:
							// Got the battery level
						{
							// Battery level is val * 3.52mV - BUT the nrf8001 chip only sees regulated voltage!
							uint16_t batt = aci_evt->params.cmd_rsp.params.get_battery_level.battery_level;

							// Since the chip can only see regulated voltage and not batt voltage, we just send 100%
							batt = 100;
							lib_aci_set_local_data(&aci_state, PIPE_BATTERY_BATTERY_POWER_STATE_SET, (uint8_t *)&batt, 1);
						}
							break;
						case ACI_CMD_GET_TEMPERATURE:
							// Got the temperature
						{
							int32_t temperature = aci_evt->params.cmd_rsp.params.get_temperature.temperature_value * 25;
							temperature &= 0x00FFFFFF; //Mask the exponent part
							temperature |= 0xFE000000; //Exponent is -2 since we multipled by 100

							uint8_t temp_data[5];	// Flags byte + float value
							temp_data[0] = 0x00;	// degrees C, no timestamp
							memcpy(&temp_data[1], &temperature, 4);

							lib_aci_set_local_data(&aci_state, PIPE_DEVICE_INFORMATION_TEMPERATURE_MEASUREMENT_SET, temp_data, 5);
						}
							break;
						default:
							break;
					}

				}
				break;

			case ACI_EVT_DATA_CREDIT:
				aci_state.data_credit_available = aci_state.data_credit_available + aci_evt->params.data_credit.credit;
				break;

			case ACI_EVT_CONNECTED:
				lpcprintf("BLE: Connected\r\n");
				BLEUartInit();
				BLETimingChangeDone             = false;
				aci_state.data_credit_available = aci_state.data_credit_total;

				// Turn on a light
				ws2812bSetPixel(29, 96, 158);
				break;

			case ACI_EVT_TIMING:
				lpcprintf("BLE: Link connection interval changed\r\n");
				lib_aci_set_local_data(&aci_state,
						PIPE_UART_OVER_BTLE_UART_LINK_TIMING_CURRENT_SET,
						(uint8_t *)&(aci_evt->params.timing.conn_rf_interval), /* Byte aligned */
						PIPE_UART_OVER_BTLE_UART_LINK_TIMING_CURRENT_SET_MAX_SIZE);

				if((ACI_BOND_STATUS_SUCCESS == aci_state.bonded) &&
						(true == BLEBondedFirstTime) &&
						(GAP_PPCP_MAX_CONN_INT >= aci_state.connection_interval) &&
						(GAP_PPCP_MIN_CONN_INT <= aci_state.connection_interval) &&
						(lib_aci_is_pipe_available(&aci_state, PIPE_UART_OVER_BTLE_UART_TX_TX)) &&
						(lib_aci_is_pipe_available(&aci_state, PIPE_LINK_LOSS_ALERT_ALERT_LEVEL_RX_ACK_AUTO))){

					//Timing change already done: Provide time for the the peer to finish
					lpcprintf("Terminating connection\r\n");
					lib_aci_disconnect(&aci_state, ACI_REASON_TERMINATE);
				}

				break;

			case ACI_EVT_PIPE_STATUS:
				lpcprintf("BLE: Pipe Status\r\n");

				if(lib_aci_is_pipe_available(&aci_state, PIPE_LINK_LOSS_ALERT_ALERT_LEVEL_RX_ACK_AUTO)){
					lpcprintf("BLE Pipe: Link loss\r\n");
					if(BLEBondDataSave(&aci_state)){
						// Flush to EEPROM
						dataChanged();
					}
				}
				if(lib_aci_is_pipe_available(&aci_state, PIPE_UART_OVER_BTLE_UART_TX_TX)){
					lpcprintf("BLE Pipe: UART\r\n");
				}

				if ((false == BLETimingChangeDone) &&
						(lib_aci_is_pipe_available(&aci_state, PIPE_UART_OVER_BTLE_UART_TX_TX)) &&
						(lib_aci_is_pipe_available(&aci_state, PIPE_LINK_LOSS_ALERT_ALERT_LEVEL_RX_ACK_AUTO))){
					lib_aci_change_timing_GAP_PPCP(); // change the timing on the link as specified in the nRFgo studio -> nRF8001 conf. -> GAP.
					// Used to increase or decrease bandwidth
					BLETimingChangeDone = true;
				}

				if ((ACI_BOND_STATUS_SUCCESS == aci_state.bonded) &&
						(lib_aci_is_pipe_available(&aci_state, PIPE_LINK_LOSS_ALERT_ALERT_LEVEL_RX_ACK_AUTO))){
					lpcprintf("Phone Detected\r\n");
				}

				break;

			case ACI_EVT_DISCONNECTED:
				lpcprintf("BLE: Disconnected/Advertising timed out\r\n");
				if (ACI_BOND_STATUS_SUCCESS == aci_state.bonded){

					// Turn off the light
					ws2812bSetPixel(0, 0, 0);

					if (ACI_STATUS_EXTENDED == aci_evt->params.disconnected.aci_status){
						//Link was disconnected
						if (BLEBondedFirstTime){
							BLEBondedFirstTime = false;
							//Store away the dynamic data of the nRF8001 in the Flash or EEPROM of the MCU
							// so we can restore the bond information of the nRF8001 in the event of power loss
							dataChanged();
						}
						if (0x24 == aci_evt->params.disconnected.btle_status){
							lpcprintf("Phone has deleted the bonding/pairing information");
						}

					}
					if(BLEBondDataSave(&aci_state)){
						dataChanged();
					}

					// Try to connect using bond data
					lib_aci_connect(180, 0x0100 );
					lpcprintf("Using existing bond stored in EEPROM.\r\n");
					lpcprintf("Advertising started. Connecting.\r\n");
				}
				else{

					if(BLEBondDataSave(&aci_state)){
						dataChanged();
					}

					//There is no existing bond. Advertise pairing ability.
					lib_aci_bond(180, 0x0050);
					lpcprintf("BLE command: advertise\r\n");
				}
				break;

			case ACI_EVT_PIPE_ERROR:
				//See the appendix in the nRF8001 Product Specication for details on the error codes
				lpcprintf("BLE: Pipe Error: Pipe #:%02X Pipe Error Code: %02X\n\r",
						aci_evt->params.pipe_error.pipe_number, aci_evt->params.pipe_error.error_code);

				//Increment the credit available as the data packet was not sent.
				//The pipe error also represents the Attribute protocol Error Response sent from the peer and that should not be counted
				//for the credit.
				if (ACI_STATUS_ERROR_PEER_ATT_ERROR != aci_evt->params.pipe_error.error_code){
					aci_state.data_credit_available++;
				}
				break;

			case ACI_EVT_DATA_RECEIVED:
				lpcprintf("BLE: Pipe #%02X %d bytes: \r\n", aci_evt->params.data_received.rx_data.pipe_number, aci_evt->len - 2);
				{
					for(uint8_t i=0; i < aci_evt->len - 2; i++){
						lpcprintf("%c", aci_evt->params.data_received.rx_data.aci_data[i]);
					}

					// Update the display with the new data
					displayUpdateTopLine(aci_evt->params.data_received.rx_data.aci_data, aci_evt->len - 2);
				}
				lpcprintf("\r\n");
				break;

			case ACI_EVT_HW_ERROR:
				// Hardware error, display it, and restart the advertising
				lpcprintf("BLE: HW error: %02X", aci_evt->params.hw_error.line_num);

				for(uint8_t counter = 0; counter <= (aci_evt->len - 3); counter++){
					lpcprintf("%02X ", aci_evt->params.hw_error.file_name[counter]);
				}
				lpcprintf("\r\n");

				if(dataStore.BLEPairable){
					lib_aci_bond(180, 0x0050);
					lpcprintf("BLE command: advertise, bondable\r\n");
				}
				else{
					lib_aci_broadcast(180, 0x0050);
					lpcprintf("BLE command: advertise only\r\n");
				}

				break;
			case ACI_EVT_DISPLAY_PASSKEY:
				lpcprintf("BLE: passkey ");
				displayPairing(PAIRING_SHOW_KEY, aci_evt->params.display_passkey.passkey);
				for(int i = 0; i < 6; i++){
					lpcprintf("%c", aci_evt->params.display_passkey.passkey[i]);
				}
				lpcprintf("\r\n");
				break;

				//		case ACI_EVT_KEY_REQUEST:
				//			lpcprintf("Key request\r\n");
				//			lib_aci_set_key(0x01, "012345", 6);
				//			break;

			case ACI_EVT_BOND_STATUS:
				aci_state.bonded = aci_evt->params.bond_status.status_code;
				lpcprintf("BLE: Bond Status: ");
				if(aci_evt->params.bond_status.status_code == 0){
					displayPairing(PAIRING_SUCCESS, NULL);
					lpcprintf("success\r\n");
				}
				else{
					displayPairing(PAIRING_FAILED, NULL);
					lpcprintf("failed\r\n");
				}
				lpcprintf(" Status: %02X\r\n", aci_evt->params.bond_status.status_code);
				lpcprintf(" Source: %02X\r\n", aci_evt->params.bond_status.status_source);
				lpcprintf(" SecMode1: %02X\r\n", aci_evt->params.bond_status.secmode1_bitmap);
				lpcprintf(" SecMode2: %02X\r\n", aci_evt->params.bond_status.secmode2_bitmap);
				lpcprintf(" KeyExchSlave: %02X\r\n", aci_evt->params.bond_status.keys_exchanged_slave);
				lpcprintf(" KeyExchMaster: %02X\r\n", aci_evt->params.bond_status.keys_exchanged_master);
				break;

			default:
				lpcprintf("BLE: Op code is %02X\r\n", aci_evt->evt_opcode);
				break;

		}
	}
	else
	{
		//lpcprintf("No ACI Events available\r\n");
		// No event in the ACI Event queue
	}

	/* setup_required is set to true when the device starts up and enters setup mode.
	 * It indicates that do_aci_setup() should be called. The flag should be cleared if
	 * do_aci_setup() returns ACI_STATUS_TRANSACTION_COMPLETE.
	 */
	if(BLESetupRequired){

		// Update the CRC on the setup data
		updateSetupCRC();

		uint8_t ret = do_aci_setup(&aci_state);
		if (SETUP_SUCCESS == ret){
			lpcprintf("BLE: Setup success\r\n");
			BLESetupRequired = false;
		}
		else{
			lpcprintf("BLE: Setup fail: %d\r\n", ret);
			// Setup failed, hard reset the chip
			lib_aci_pin_reset();
			delay_ms(100);
		}
	}
}


/**
 * Calculate the CRC on the setup data, and write it out to the setup data
 */
void updateSetupCRC(void){

	uint16_t crc_seed = INITIAL_CRC_SEED;

	uint8_t msg_len;
	for(int crc_loop = 0; crc_loop < NB_SETUP_MESSAGES; crc_loop++){

		if(NB_SETUP_MESSAGES - 1 == crc_loop){
			msg_len = setup_msgs[crc_loop].buffer[0] - 1;
		}
		else{
			msg_len = setup_msgs[crc_loop].buffer[0] + 1;
		}

		crc_seed = crc_16_ccitt(crc_seed, &setup_msgs[crc_loop].buffer[0], msg_len);

	}

	setup_msgs[SETUP_CRC_OFFSET].buffer[5] = (crc_seed >> 8) & 0xFF;
	setup_msgs[SETUP_CRC_OFFSET].buffer[6] = crc_seed & 0xFF;

}

/**
 * Set the manufacturer in the advertisement block FF
 */
void BLESetManufacturer(uint16_t manu){

	setup_msgs[SETUP_MANU_DATA_OFFSET].buffer[SETUP_MANU_DATA_START_BYTE] = manu & 0xff;
	setup_msgs[SETUP_MANU_DATA_OFFSET].buffer[SETUP_MANU_DATA_START_BYTE + 1] = (manu >> 8) & 0xff;

}

/**
 * Set the BLE name in the advertisement
 */
void BLESetName(uint8_t *name){

	uint8_t len = strlen(name);

	// Always write out 8 bytes
	for(uint8_t i = 0; i < 8; i++){
		if(i < len){
			setup_msgs[SETUP_NAME_OFFSET].buffer[i + SETUP_NAME_START_BYTE] = name[i];
		}
		else{
			setup_msgs[SETUP_NAME_OFFSET].buffer[i + SETUP_NAME_START_BYTE] = 0;
		}
	}

	// Set the length of the string
	setup_msgs[SETUP_NAME_OFFSET].buffer[SETUP_NAME_SIZE_OFFSET] = len;

}

/**
 * Initialize the BLE UART
 */
void BLEUartInit(void){
    uart_over_ble.uart_rts_local = true;
}


/**
 * Read the Dymamic data from the EEPROM and send then as ACI Write Dynamic Data to the nRF8001
 * This will restore the nRF8001 to the situation when the Dynamic Data was Read out
 */
aci_status_code_t BLEBondDataRestore(aci_state_t *aci_stat, bool *bonded_first_time_state){

	aci_evt_t *aci_evt;
	uint16_t offset = 0;
	uint8_t num_msgs = dataGetBLEDataMsgs();
	uint8_t len = 0;
	uint8_t *bleData = dataGetBLEData();

	//Read from data store and send to the chip
	while(1){

		// Set the length
		len = bleData[offset];
		aci_cmd.buffer[0] = len;
		offset++;

		for (uint8_t i=1; i<=len; i++){
			aci_cmd.buffer[i] = bleData[offset];
			offset++;
		}
		//Send the ACI Write Dynamic Data
		if (!hal_aci_tl_send(&aci_cmd)){
			lpcprintf("BLEBondDataRestore: Cmd Q Full");
			return ACI_STATUS_ERROR_INTERNAL;
		}

		//Spin in the while loop waiting for an event
		while (1){
			if (lib_aci_event_get(aci_stat, &aci_data)){
				aci_evt = &aci_data.evt;

				if (ACI_EVT_CMD_RSP != aci_evt->evt_opcode){
					//Got something other than a command response evt -> Error
					lpcprintf("bond_data_restore: Expected cmd rsp evt. Got: %02X\r\n", aci_evt->evt_opcode);
					return ACI_STATUS_ERROR_INTERNAL;
				}
				else{
					num_msgs--;

					//ACI Evt Command Response
					if (ACI_STATUS_TRANSACTION_COMPLETE == aci_evt->params.cmd_rsp.cmd_status){
						//Set the state variables correctly
						*bonded_first_time_state = false;
						aci_stat->bonded = ACI_BOND_STATUS_SUCCESS;

						delay_ms(10);

						return ACI_STATUS_TRANSACTION_COMPLETE;
					}
					if (0 >= num_msgs){
						//should have returned earlier
						return ACI_STATUS_ERROR_INTERNAL;
					}
					if (ACI_STATUS_TRANSACTION_CONTINUE == aci_evt->params.cmd_rsp.cmd_status){
						//break and write the next ACI Write Dynamic Data
						break;
					}
				}
			}
		}
	}
}

/**
 * Get the dynamic data from the chip and store it out to the EEPROM
 */
bool BLEBondDataSave(aci_state_t *aci_stat){

	// The size of the dynamic data for a specific Bluetooth Low Energy configuration
	// is present in the ublue_setup.gen.out.txt generated by the nRFgo studio as "dynamic data size".

	uint8_t *bleData = dataGetBLEData();
	uint16_t offset = 0;

	bool status = false;
	aci_evt_t * aci_evt = NULL;
	uint8_t read_dyn_num_msgs = 0;

	//Start reading the dynamic data
	lib_aci_read_dynamic_data();
	read_dyn_num_msgs++;

	while (1){
		if (true == lib_aci_event_get(aci_stat, &aci_data)){
			aci_evt = &aci_data.evt;

			if (ACI_EVT_CMD_RSP != aci_evt->evt_opcode ){
				//Got something other than a command response evt -> Error
				status = false;
				lpcprintf("DData: wanted RSP got %02X\r\n", aci_evt->evt_opcode);
				break;
			}

			if (ACI_STATUS_TRANSACTION_COMPLETE == aci_evt->params.cmd_rsp.cmd_status){

				// Save this data into the storage space for EEPROM
				lpcprintf("DData: (%d) %02X ", aci_evt->len, ACI_CMD_WRITE_DYNAMIC_DATA);
				for(int i = 0; i < aci_evt->len; i++){
					lpcprintf("%02X ", aci_evt->params.cmd_rsp.params.padding[i]);
					bleData[offset++] = aci_evt->params.cmd_rsp.params.padding[i];
				}
				lpcprintf("\r\n");

				lpcprintf("DData total: %d\r\n", read_dyn_num_msgs);

				status = true;

				break;
			}

			if (!(ACI_STATUS_TRANSACTION_CONTINUE == aci_evt->params.cmd_rsp.cmd_status)){

				// We failed the read dymanic data
				// Clear the saved data from memory

				memset(bleData, 0, BLEDataLen);
				status = false;
				lpcprintf("DData: wanted continue, got %02X\r\n", aci_evt->params.cmd_rsp.cmd_status);
				break;
			}
			else{

				// Save this data into the storage space for EEPROM
				lpcprintf("DData: (%d) %02X ", aci_evt->len, ACI_CMD_WRITE_DYNAMIC_DATA);
				for(int i = 0; i < aci_evt->len; i++){
					lpcprintf("%02X ", aci_evt->params.cmd_rsp.params.padding[i]);
					bleData[offset++] = aci_evt->params.cmd_rsp.params.padding[i];
				}
				lpcprintf("\r\n");

				//Read the next dynamic data message
				lib_aci_read_dynamic_data();
				read_dyn_num_msgs++;
			}
		}
	}
	return status;
}

/**
 * Delete the pairing data and store to EEPROM
 * Also will reset the BLE stack
 */
void BLEDeletePairing(void){

	uint8_t *bleData = dataGetBLEData();
	memset(bleData, 0, BLEDataLen);
	dataChanged();

	BLEUpdateSetup();
}

