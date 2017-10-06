/* Copyright (c) 2014, Nordic Semiconductor ASA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/** @file
@brief Implementation of the ACI transport layer module
*/

#include <stdio.h>
#include "drivers/ssp.h"
#include "drivers/common.h"
#include "hal_platform.h"
#include "hal_aci_tl.h"
#include "lib_aci.h"
#include "aci_queue.h"
#include "lpc_types.h"
#include "LPC13Uxx.h"
#include "core/inc/lpc13uxx_gpio.h"
#include "core/inc/lpc13uxx_uart.h"


#define REVERSE_BITS(byte) (((reverse_lookup[(byte & 0x0F)]) << 4) + reverse_lookup[((byte & 0xF0) >> 4)])
static const uint8_t reverse_lookup[] = { 0, 8,  4, 12, 2, 10, 6, 14,1, 9, 5, 13,3, 11, 7, 15 };


//static void m_aci_data_print(hal_aci_data_t *p_data);
static void m_aci_event_check(void);
static void m_aci_isr(void);
static void m_aci_pins_set(aci_pins_t *a_pins_ptr);
static inline void m_aci_reqn_disable (void);
static inline void m_aci_reqn_enable (void);
static void m_aci_q_flush(void);
static bool m_aci_spi_transfer(hal_aci_data_t * data_to_send, hal_aci_data_t * received_data);

static uint8_t        spi_readwrite(uint8_t aci_byte);

static bool           aci_debug_print = false;

aci_queue_t    aci_tx_q;
aci_queue_t    aci_rx_q;

static aci_pins_t	 *a_pins_local_ptr;

void m_aci_data_print(hal_aci_data_t *p_data){
	const uint8_t length = p_data->buffer[0];
	uint8_t i;
	lpcprintf("%d :", length);
	for (i=0; i<=length; i++){
		lpcprintf("%02X, ", p_data->buffer[i]);
	}
	lpcprintf("\r\n");
}

/*
  Interrupt service routine called when the RDYN line goes low. Runs the SPI transfer.
 */
void PIN_INT0_IRQHandler(void){
	hal_aci_data_t data_to_send;
	hal_aci_data_t received_data;

	if ( LPC_GPIO_PIN_INT->IST & (1<<0) ){
		LPC_GPIO_PIN_INT->IST = 1<<0;		// Clear the interrupt
	}

	// Receive from queue
	if (!aci_queue_dequeue_from_isr(&aci_tx_q, &data_to_send)){
		/* queue was empty, nothing to send */
		data_to_send.status_byte = 0;
		data_to_send.buffer[0] = 0;
	}

	// Receive and/or transmit data
	m_aci_spi_transfer(&data_to_send, &received_data);

	if (!aci_queue_is_full_from_isr(&aci_rx_q) && !aci_queue_is_empty_from_isr(&aci_tx_q)){
		m_aci_reqn_enable();
	}

	// Check if we received data
	if (received_data.buffer[0] > 0){
		aci_queue_enqueue_from_isr(&aci_rx_q, &received_data);

		// If the queue is full, disable ready line interrupt
		// until we have room to store incoming messages
		if (aci_queue_is_full_from_isr(&aci_rx_q)){
			NVIC_DisableIRQ(BLE_INTERRUPT);
		}
	}

	return;
}

/*
  Checks the RDYN line and runs the SPI transfer if required.
 */
static void m_aci_event_check(void){
	hal_aci_data_t data_to_send;
	hal_aci_data_t received_data;

	// No room to store incoming messages
	if (aci_queue_is_full(&aci_rx_q)){
		return;
	}

	// If the ready line is disabled and we have pending messages outgoing we enable the request line
	if(LPC_GPIO->B1[16] == 1){
		if (!aci_queue_is_empty(&aci_tx_q)){
			m_aci_reqn_enable();
		}

		return;
	}

	// Receive from queue
	if (!aci_queue_dequeue(&aci_tx_q, &data_to_send)){
		/* queue was empty, nothing to send */
		data_to_send.status_byte = 0;
		data_to_send.buffer[0] = 0;
	}

	// Receive and/or transmit data
	m_aci_spi_transfer(&data_to_send, &received_data);

	/* If there are messages to transmit, and we can store the reply, we request a new transfer */
	if (!aci_queue_is_full(&aci_rx_q) && !aci_queue_is_empty(&aci_tx_q)){
		m_aci_reqn_enable();
	}

	// Check if we received data
	if (received_data.buffer[0] > 0){
		if (!aci_queue_enqueue(&aci_rx_q, &received_data)){
			// Receive Buffer full.
			lpcprintf("BLE: queue full, reset\r\n");
			hal_aci_tl_q_flush();
			hal_aci_tl_pin_reset();
		}
	}

	return;
}

static inline void m_aci_reqn_disable (void){
	LPC_GPIO->SET[ PORT0 ] = (1 << 22);
}

static inline void m_aci_reqn_enable (void){
	LPC_GPIO->CLR[ PORT0 ] = (1 << 22);
}

static void m_aci_q_flush(void){
	NVIC_DisableIRQ(BLE_INTERRUPT);
	/* re-initialize aci cmd queue and aci event queue to flush them*/
	aci_queue_init(&aci_tx_q);
	aci_queue_init(&aci_rx_q);
	NVIC_EnableIRQ(BLE_INTERRUPT);
}

static bool m_aci_spi_transfer(hal_aci_data_t * data_to_send, hal_aci_data_t * received_data){
	uint8_t byte_cnt;
	uint8_t byte_sent_cnt;
	uint8_t max_bytes;


	m_aci_reqn_enable();

	// Send length, receive header
	byte_sent_cnt = 0;
	received_data->status_byte = ssp0SendChar(data_to_send->buffer[byte_sent_cnt++]);
	// Send first byte, receive length from slave
	received_data->buffer[0] = ssp0SendChar(data_to_send->buffer[byte_sent_cnt++]);
	if (0 == data_to_send->buffer[0]){
		max_bytes = received_data->buffer[0];
	}
	else{
		// Set the maximum to the biggest size. One command byte is already sent
		max_bytes = (received_data->buffer[0] > (data_to_send->buffer[0] - 1))
                                        		  ? received_data->buffer[0]
																		  : (data_to_send->buffer[0] - 1);
	}

	if (max_bytes > HAL_ACI_MAX_LENGTH){
		max_bytes = HAL_ACI_MAX_LENGTH;
	}


	// Transmit/receive the rest of the packet
	for (byte_cnt = 0; byte_cnt < max_bytes; byte_cnt++){
		received_data->buffer[byte_cnt+1] =  ssp0SendChar(data_to_send->buffer[byte_sent_cnt++]);
	}

	// RDYN should follow the REQN line in approx 100ns
	m_aci_reqn_disable();


	return (max_bytes > 0);
}

void hal_aci_tl_debug_print(bool enable){
	aci_debug_print = enable;
}

void hal_aci_tl_pin_reset(void){

	LPC_GPIO->DIR[PORT1] |= (1<<21);

	LPC_GPIO->SET[PORT1] |= (1<<21);
	delay_ms(10);
	LPC_GPIO->CLR[PORT1] |= (1<<21);	// Reset the proc
	delay_ms(50);
	LPC_GPIO->SET[PORT1] |= (1<<21);	// Pull reset high
	delay_ms(150);

}

bool hal_aci_tl_event_peek(hal_aci_data_t *p_aci_data){

	m_aci_event_check();

	if (aci_queue_peek(&aci_rx_q, p_aci_data)){
		return true;
	}

	return false;
}

bool hal_aci_tl_event_get(hal_aci_data_t *p_aci_data){
	bool was_full;


	if (!aci_queue_is_full(&aci_rx_q)){
		m_aci_event_check();
	}

	was_full = aci_queue_is_full(&aci_rx_q);

	if (aci_queue_dequeue(&aci_rx_q, p_aci_data)){
		if (aci_debug_print){
			lpcprintf(" E");
			m_aci_data_print(p_aci_data);
		}

		if (was_full){
			/* Enable RDY line interrupt again */
			NVIC_EnableIRQ(BLE_INTERRUPT);
		}

		/* Attempt to pull REQN LOW since we've made room for new messages */
		if (!aci_queue_is_full(&aci_rx_q) && !aci_queue_is_empty(&aci_tx_q)){
			m_aci_reqn_enable();
		}

		return true;
	}

	//lpcprintf("hal_aci_tl_event_get done\r\n");

	return false;
}

void hal_aci_tl_init(aci_pins_t *a_pins, bool debug){
	aci_debug_print = debug;

	// Input pins
	LPC_GPIO->DIR[PORT1] &= ~(1<<17);	// ACTIVE
	LPC_GPIO->DIR[PORT1] &= ~(1<<16);	// /RDYN
	LPC_IOCON->PIO1_16 &= ~(1<<4);		// Pull up resistor enabled

	// Output pins
	LPC_GPIO->DIR[PORT0] |= (1<<22);	// /REQN is an output to BLE
	LPC_GPIO->SET[PORT0] |= (1<<22);	// Pull the REQN high until we need it
	LPC_GPIO->DIR[PORT1] |= (1<<21);	// Reset pin

	// Init the SPI
	ssp0Init();

	/* Initialize the ACI Command queue. This must be called after the delay above. */
	aci_queue_init(&aci_tx_q);
	aci_queue_init(&aci_rx_q);

	/* Pin reset the nRF8001, required when the nRF8001 setup is being changed */
	hal_aci_tl_pin_reset();

	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<19) | (1<<24);		// Enable the pin interrupts reg and group0 interrupts
	LPC_SYSCON->PINSEL[0] = 16 + 24 | (1<<5);			// Add RDYN line to group0

	LPC_GPIO_PIN_INT->ISEL &= ~(1<<0);					// Pin register mode register, set group0 to edge sensitive

	LPC_GPIO_PIN_INT->IENF |= (1<<0);					// Falling edge enable register, set group0 enabled
	LPC_GPIO_PIN_INT->SIENF |= (1<<0);    				// Falling edge set register, set group0 set
	LPC_GPIO_PIN_INT->IST = (1<<0);						// Pin interrupt status register, clear any pending irq for group0

	delay_ms(150); //Wait for the nRF8001 to get hold of its lines - the lines float for a few ms after the reset

	NVIC_EnableIRQ(BLE_INTERRUPT);						// Enable the IRQ

}

bool hal_aci_tl_send(hal_aci_data_t *p_aci_cmd){
	const uint8_t length = p_aci_cmd->buffer[0];
	bool ret_val = false;

	if (length > HAL_ACI_MAX_LENGTH){
		return false;
	}

	ret_val = aci_queue_enqueue(&aci_tx_q, p_aci_cmd);
	if (ret_val){
		if(!aci_queue_is_full(&aci_rx_q)){
			// Lower the REQN only when successfully enqueued
			m_aci_reqn_enable();
		}

		if (aci_debug_print){
			lpcprintf("C"); //ACI Command
			m_aci_data_print(p_aci_cmd);
		}
	}

	return ret_val;
}

bool hal_aci_tl_rx_q_empty (void){
	return aci_queue_is_empty(&aci_rx_q);
}

bool hal_aci_tl_rx_q_full (void){
	return aci_queue_is_full(&aci_rx_q);
}

bool hal_aci_tl_tx_q_empty (void){
	return aci_queue_is_empty(&aci_tx_q);
}

bool hal_aci_tl_tx_q_full (void){
	return aci_queue_is_full(&aci_tx_q);
}

void hal_aci_tl_q_flush (void){
	m_aci_q_flush();
}
