/**
* This file is autogenerated by nRFgo Studio 1.21.2RC1.4
* Seriously, I didn't rubber glove this stuff
*/

#ifndef SETUP_MESSAGES_H__
#define SETUP_MESSAGES_H__

#include "drivers/ble/hal_platform.h"
#include "drivers/ble/aci.h"

#define SETUP_ID 0
#define SETUP_FORMAT 3 /** nRF8001 D */
#define ACI_DYNAMIC_DATA_SIZE 263

/* Service: GATT - Characteristic: Service Changed - Pipe: TX_ACK */
#define PIPE_GATT_SERVICE_CHANGED_TX_ACK          1
#define PIPE_GATT_SERVICE_CHANGED_TX_ACK_MAX_SIZE 4

/* Service: UART over BTLE - Characteristic: UART RX - Pipe: RX */
#define PIPE_UART_OVER_BTLE_UART_RX_RX          2
#define PIPE_UART_OVER_BTLE_UART_RX_RX_MAX_SIZE 20

/* Service: UART over BTLE - Characteristic: UART TX - Pipe: TX */
#define PIPE_UART_OVER_BTLE_UART_TX_TX          3
#define PIPE_UART_OVER_BTLE_UART_TX_TX_MAX_SIZE 20

/* Service: UART over BTLE - Characteristic: UART Control Point - Pipe: TX */
#define PIPE_UART_OVER_BTLE_UART_CONTROL_POINT_TX          4
#define PIPE_UART_OVER_BTLE_UART_CONTROL_POINT_TX_MAX_SIZE 9

/* Service: UART over BTLE - Characteristic: UART Control Point - Pipe: RX */
#define PIPE_UART_OVER_BTLE_UART_CONTROL_POINT_RX          5
#define PIPE_UART_OVER_BTLE_UART_CONTROL_POINT_RX_MAX_SIZE 9

/* Service: UART over BTLE - Characteristic: UART Link Timing Current - Pipe: SET */
#define PIPE_UART_OVER_BTLE_UART_LINK_TIMING_CURRENT_SET          6
#define PIPE_UART_OVER_BTLE_UART_LINK_TIMING_CURRENT_SET_MAX_SIZE 6

/* Service: Device Information - Characteristic: Firmware Revision String - Pipe: SET */
#define PIPE_DEVICE_INFORMATION_FIRMWARE_REVISION_STRING_SET          7
#define PIPE_DEVICE_INFORMATION_FIRMWARE_REVISION_STRING_SET_MAX_SIZE 4

/* Service: Device Information - Characteristic: Temperature Measurement - Pipe: SET */
#define PIPE_DEVICE_INFORMATION_TEMPERATURE_MEASUREMENT_SET          8
#define PIPE_DEVICE_INFORMATION_TEMPERATURE_MEASUREMENT_SET_MAX_SIZE 13

/* Service: Battery - Characteristic: Battery Power State - Pipe: SET */
#define PIPE_BATTERY_BATTERY_POWER_STATE_SET          9
#define PIPE_BATTERY_BATTERY_POWER_STATE_SET_MAX_SIZE 1

/* Service: DC801 Parameters - Characteristic: Handle - Pipe: TX */
#define PIPE_DC801_PARAMETERS_HANDLE_TX          10
#define PIPE_DC801_PARAMETERS_HANDLE_TX_MAX_SIZE 8

/* Service: DC801 Parameters - Characteristic: Handle - Pipe: RX */
#define PIPE_DC801_PARAMETERS_HANDLE_RX          11
#define PIPE_DC801_PARAMETERS_HANDLE_RX_MAX_SIZE 8

/* Service: DC801 Parameters - Characteristic: Handle - Pipe: SET */
#define PIPE_DC801_PARAMETERS_HANDLE_SET          12
#define PIPE_DC801_PARAMETERS_HANDLE_SET_MAX_SIZE 8

/* Service: DC801 Parameters - Characteristic: Vector - Pipe: TX_ACK */
#define PIPE_DC801_PARAMETERS_VECTOR_TX_ACK          13
#define PIPE_DC801_PARAMETERS_VECTOR_TX_ACK_MAX_SIZE 8

/* Service: DC801 Parameters - Characteristic: Vector - Pipe: RX */
#define PIPE_DC801_PARAMETERS_VECTOR_RX          14
#define PIPE_DC801_PARAMETERS_VECTOR_RX_MAX_SIZE 8

/* Service: DC801 Parameters - Characteristic: Vector - Pipe: SET */
#define PIPE_DC801_PARAMETERS_VECTOR_SET          15
#define PIPE_DC801_PARAMETERS_VECTOR_SET_MAX_SIZE 8

/* Service: Link Loss Alert - Characteristic: Alert Level - Pipe: RX_ACK_AUTO */
#define PIPE_LINK_LOSS_ALERT_ALERT_LEVEL_RX_ACK_AUTO          16
#define PIPE_LINK_LOSS_ALERT_ALERT_LEVEL_RX_ACK_AUTO_MAX_SIZE 1


#define NUMBER_OF_PIPES 16

#define SERVICES_PIPE_TYPE_MAPPING_CONTENT {\
  {ACI_STORE_LOCAL, ACI_TX_ACK},   \
  {ACI_STORE_LOCAL, ACI_RX},   \
  {ACI_STORE_LOCAL, ACI_TX},   \
  {ACI_STORE_LOCAL, ACI_TX},   \
  {ACI_STORE_LOCAL, ACI_RX},   \
  {ACI_STORE_LOCAL, ACI_SET},   \
  {ACI_STORE_LOCAL, ACI_SET},   \
  {ACI_STORE_LOCAL, ACI_SET},   \
  {ACI_STORE_LOCAL, ACI_SET},   \
  {ACI_STORE_LOCAL, ACI_TX},   \
  {ACI_STORE_LOCAL, ACI_RX},   \
  {ACI_STORE_LOCAL, ACI_SET},   \
  {ACI_STORE_LOCAL, ACI_TX_ACK},   \
  {ACI_STORE_LOCAL, ACI_RX},   \
  {ACI_STORE_LOCAL, ACI_SET},   \
  {ACI_STORE_LOCAL, ACI_RX_ACK_AUTO},   \
}

#define GAP_PPCP_MAX_CONN_INT 0x28 /**< Maximum connection interval as a multiple of 1.25 msec , 0xFFFF means no specific value requested */
#define GAP_PPCP_MIN_CONN_INT  0x10 /**< Minimum connection interval as a multiple of 1.25 msec , 0xFFFF means no specific value requested */
#define GAP_PPCP_SLAVE_LATENCY 0
#define GAP_PPCP_CONN_TIMEOUT 0x14 /** Connection Supervision timeout multiplier as a multiple of 10msec, 0xFFFF means no specific value requested */

#define NB_SETUP_MESSAGES 42
#define SETUP_MESSAGES_CONTENT {\
    {0x00,\
        {\
            0x07,0x06,0x00,0x00,0x03,0x02,0x42,0x07,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x10,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x0b,0x00,0x10,0x00,0x01,0x00,0x00,0x06,0x00,0x00,\
            0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x10,0x1c,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
            0x00,0x00,0x40,0x10,0x00,0x00,0x40,0x10,0x10,0x90,0x01,0xff,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x10,0x38,0xff,0xff,0x02,0x58,0x1e,0x05,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x04,0x00,0x00,\
            0x40,0x10,0x00,0x00,0x00,0x04,0x03,0x00,0x03,0x00,0x03,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x05,0x06,0x10,0x54,0x01,0x02,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0x00,0x04,0x04,0x02,0x02,0x00,0x01,0x28,0x00,0x01,0x00,0x18,0x04,0x04,0x05,0x05,0x00,\
            0x02,0x28,0x03,0x01,0x02,0x03,0x00,0x00,0x2a,0x04,0x04,0x14,\
        },\
    },\
    {0x00, /* Advertised name in here */\
        {\
            0x1f,0x06,0x20,0x1c,0x08,0x00,0x03,0x2a,0x00,0x01,0x64,0x63,0x38,0x30,0x31,0x20,0x20,0x20,0x6d,0x69,\
            0x2e,0x63,0x6f,0x6d,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0x38,0x05,0x05,0x00,0x04,0x28,0x03,0x01,0x02,0x05,0x00,0x01,0x2a,0x06,0x04,0x03,0x02,\
            0x00,0x05,0x2a,0x01,0x01,0xdc,0x19,0x04,0x04,0x05,0x05,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0x54,0x06,0x28,0x03,0x01,0x02,0x07,0x00,0x04,0x2a,0x06,0x04,0x09,0x08,0x00,0x07,0x2a,\
            0x04,0x01,0x10,0x00,0x28,0x00,0x00,0x00,0x14,0x00,0x04,0x04,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0x70,0x02,0x02,0x00,0x08,0x28,0x00,0x01,0x01,0x18,0x04,0x04,0x05,0x05,0x00,0x09,0x28,\
            0x03,0x01,0x22,0x0a,0x00,0x05,0x2a,0x26,0x0c,0x05,0x04,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0x8c,0x0a,0x2a,0x05,0x01,0x00,0x00,0x00,0x00,0x46,0x34,0x03,0x02,0x00,0x0b,0x29,0x02,\
            0x01,0x00,0x00,0x04,0x04,0x10,0x10,0x00,0x0c,0x28,0x00,0x01,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0xa8,0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x01,0x00,0x40,0x6e,\
            0x04,0x04,0x13,0x13,0x00,0x0d,0x28,0x03,0x01,0x04,0x0e,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0xc4,0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x02,0x00,0x40,0x6e,\
            0x44,0x30,0x14,0x00,0x00,0x0e,0x00,0x02,0x02,0x00,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
            0x00,0x04,0x04,0x13,0x13,0x00,0x0f,0x28,0x03,0x01,0x10,0x10,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0xfc,0x00,0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x03,0x00,0x40,\
            0x6e,0x14,0x00,0x14,0x00,0x00,0x10,0x00,0x03,0x02,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x21,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
            0x00,0x00,0x46,0x34,0x03,0x02,0x00,0x11,0x29,0x02,0x01,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x21,0x34,0x00,0x04,0x04,0x13,0x13,0x00,0x12,0x28,0x03,0x01,0x14,0x13,0x00,0x9e,0xca,0xdc,\
            0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x04,0x00,0x40,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x21,0x50,0x6e,0x54,0x30,0x09,0x00,0x00,0x13,0x00,0x04,0x02,0x00,0x00,0x00,0x00,0x00,0x00,\
            0x00,0x00,0x00,0x46,0x34,0x03,0x02,0x00,0x14,0x29,0x02,0x01,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x21,0x6c,0x00,0x00,0x04,0x04,0x13,0x13,0x00,0x15,0x28,0x03,0x01,0x02,0x16,0x00,0x9e,0xca,\
            0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x05,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x21,0x88,0x40,0x6e,0x06,0x0c,0x07,0x06,0x00,0x16,0x00,0x05,0x02,0xff,0xff,0xff,0xff,0xff,\
            0xff,0x04,0x04,0x02,0x02,0x00,0x17,0x28,0x00,0x01,0x0a,0x18,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x21,0xa4,0x04,0x04,0x05,0x05,0x00,0x18,0x28,0x03,0x01,0x02,0x19,0x00,0x24,0x2a,0x06,0x0c,\
            0x05,0x04,0x00,0x19,0x2a,0x24,0x01,0x44,0x43,0x32,0x32,0x06,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x21,0xc0,0x04,0x08,0x07,0x00,0x1a,0x29,0x04,0x01,0x19,0x00,0x00,0x00,0x01,0x00,0x00,0x04,\
            0x04,0x05,0x05,0x00,0x1b,0x28,0x03,0x01,0x02,0x1c,0x00,0x26,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x21,0xdc,0x2a,0x06,0x0c,0x05,0x04,0x00,0x1c,0x2a,0x26,0x01,0x30,0x31,0x2e,0x30,0x06,0x04,\
            0x08,0x07,0x00,0x1d,0x29,0x04,0x01,0x19,0x00,0x00,0x00,0x01,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x21,0xf8,0x00,0x00,0x04,0x04,0x05,0x05,0x00,0x1e,0x28,0x03,0x01,0x02,0x1f,0x00,0x29,0x2a,\
            0x06,0x0c,0x06,0x05,0x00,0x1f,0x2a,0x29,0x01,0x44,0x43,0x38,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x22,0x14,0x30,0x31,0x06,0x04,0x08,0x07,0x00,0x20,0x29,0x04,0x01,0x19,0x00,0x00,0x00,0x01,\
            0x00,0x00,0x04,0x04,0x05,0x05,0x00,0x21,0x28,0x03,0x01,0x02,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x22,0x30,0x22,0x00,0x1c,0x2a,0x04,0x0c,0x0d,0x01,0x00,0x22,0x2a,0x1c,0x01,0x00,0x00,0x00,\
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x22,0x4c,0x02,0x02,0x00,0x23,0x28,0x00,0x01,0x0f,0x18,0x04,0x04,0x05,0x05,0x00,0x24,0x28,\
            0x03,0x01,0x02,0x25,0x00,0x1a,0x2a,0x06,0x0c,0x02,0x01,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x22,0x68,0x25,0x2a,0x1a,0x01,0x00,0x04,0x04,0x02,0x02,0x00,0x26,0x28,0x00,0x01,0x01,0x08,\
            0x04,0x04,0x05,0x05,0x00,0x27,0x28,0x03,0x01,0x16,0x28,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x22,0x84,0x11,0x80,0x54,0x3c,0x08,0x05,0x00,0x28,0x80,0x11,0x01,0x64,0x63,0x38,0x30,0x31,\
            0x00,0x00,0x00,0x46,0x34,0x03,0x02,0x00,0x29,0x29,0x02,0x01,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x22,0xa0,0x00,0x00,0x04,0x04,0x05,0x05,0x00,0x2a,0x28,0x03,0x01,0x26,0x2b,0x00,0x12,0x80,\
            0x66,0x3c,0x09,0x08,0x00,0x2b,0x80,0x12,0x01,0x5f,0x48,0x65,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x22,0xbc,0x6c,0x67,0x61,0x5f,0x00,0x46,0x34,0x03,0x02,0x00,0x2c,0x29,0x02,0x01,0x00,0x00,\
            0x04,0x04,0x02,0x02,0x00,0x2d,0x28,0x00,0x01,0x03,0x18,0x04,\
        },\
    },\
    {0x00,\
        {\
            0x1b,0x06,0x22,0xd8,0x04,0x05,0x05,0x00,0x2e,0x28,0x03,0x01,0x0a,0x2f,0x00,0x06,0x2a,0x46,0x3c,0x02,\
            0x01,0x00,0x2f,0x2a,0x06,0x01,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x40,0x00,0x2a,0x05,0x01,0x00,0x04,0x04,0x00,0x0a,0x00,0x0b,0x00,0x02,0x02,0x00,0x08,0x04,\
            0x00,0x0e,0x00,0x00,0x00,0x03,0x02,0x00,0x02,0x04,0x00,0x10,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x40,0x1c,0x00,0x11,0x00,0x04,0x02,0x00,0x0a,0x04,0x00,0x13,0x00,0x14,0x00,0x05,0x02,0x00,\
            0x80,0x04,0x00,0x16,0x00,0x00,0x2a,0x26,0x01,0x00,0x80,0x04,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x40,0x38,0x00,0x1c,0x00,0x00,0x2a,0x1c,0x01,0x00,0x80,0x04,0x00,0x22,0x00,0x00,0x2a,0x1a,\
            0x01,0x00,0x80,0x04,0x00,0x25,0x00,0x00,0x80,0x11,0x01,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1d,0x06,0x40,0x54,0x8a,0x04,0x00,0x28,0x00,0x29,0x80,0x12,0x01,0x00,0x8c,0x04,0x00,0x2b,0x00,0x2c,\
            0x2a,0x06,0x01,0x04,0x00,0x04,0x00,0x2f,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x13,0x06,0x50,0x00,0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x00,0x00,0x40,0x6e,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x08,0x06,0x60,0x1c,0x00,0x00,0x00,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x19,0x06,0x70,0x00,0x19,0x02,0xdc,0x19,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
            0x00,0x00,0x00,0x00,0x00,0x00,\
        },\
    },\
    {0x00,\
        { /* Manufacturer data in here */\
            0x19,0x06,0x70,0x14,0xff,0x0c,0x01,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
			0x00,0x00,0x00,0x00,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x06,0x06,0xf0,0x00,0x03,0xcc,0x2b,\
        },\
    },\
}


//    {0x00,\
//        {\
//            0x19,0x06,0x70,0x00,0x19,0x02,0xdc,0x19,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
//            0x00,0x00,0x00,0x00,0x00,0x00,\
//        },\
//    },\
//    {0x00,\
//        {\
//            0x19,0x06,0x70,0x14,0xff,0x0c,0x01,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
//			0x00,0x00,0x00,0x00,0x00,0x00,\
//        },\
//    },\

#endif

