/**************************************************************************/
/*!
    @file     ssp0.c
    @author   K. Townsend (microBuilder.eu)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012, K. Townsend (microBuilder.eu)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/
//#include "projectconfig.h"

#include "core/inc/lpc13uxx_gpio.h"
#include "core/inc/lpc13uxx_uart.h"
#include "drivers/common.h"
#include "ssp.h"

/**************************************************************************/
/*!
    Set SSP0 clock to slow (400 KHz)
*/
/**************************************************************************/
void ssp0ClockSlow(void){
	/* Divide by 15 for SSPCLKDIV */

	LPC_SYSCON->SSP0CLKDIV = SCB_CLKDIV_DIV40;
	/* (PCLK / (CPSDVSR * [SCR+1])) = (4,800,000 / (2 x [5 + 1])) = 400 KHz */
	LPC_SSP0->CR0 = ( (7u << 0)     // Data size = 8-bit  (bits 3:0)
		   | (0 << 4)             // Frame format = SPI (bits 5:4)
		   #if CFG_SSP0_CPOL == 1
		   | (1  << 6)            // CPOL = 1           (bit 6)
		   #else
		   | (0  << 6)            // CPOL = 0           (bit 6)
		   #endif
		   #if CFG_SSP0_CPHA == 1
		   | (1 << 7)             // CPHA = 1           (bit 7)
		   #else
		   | (0 << 7)             // CPHA = 0           (bit 7)
		   #endif
		   | SSP_SCR_5);         // Clock rate = 5     (bits 15:8)

	/* Clock prescale register must be even and at least 2 in master mode */
	LPC_SSP0->CPSR = 8;

}

/**************************************************************************/
/*!
    Set SSP0 clock to fast (2.0 MHz)
*/
/**************************************************************************/
void ssp0ClockFast(void){

	/* Divide by 1 for SSPCLKDIV */
	LPC_SYSCON->SSP0CLKDIV = SCB_CLKDIV_DIV3;

	/* (PCLK / (CPSDVSR * [SCR+1])) = (24,000,000 / (2 * [5 + 1])) = 2.0 MHz */
	LPC_SSP0->CR0 = ( (7u << 0)     // Data size = 8-bit  (bits 3:0)
		   | (0 << 4)             // Frame format = SPI (bits 5:4)
		   #if CFG_SSP0_CPOL == 1
		   | (1  << 6)            // CPOL = 1           (bit 6)
		   #else
		   | (0  << 6)            // CPOL = 0           (bit 6)
		   #endif
		   #if CFG_SSP0_CPHA == 1
		   | (1 << 7)             // CPHA = 1           (bit 7)
		   #else
		   | (0 << 7)             // CPHA = 0           (bit 7)
		   #endif
		   | SSP_SCR_5);         // Clock rate = 5     (bits 15:8)

	/* Clock prescale register must be even and at least 2 in master mode */
	LPC_SSP0->CPSR = 2;
}

/**************************************************************************/
/*!
    @brief Initialise SSP0
*/
/**************************************************************************/
void ssp0Init(void){

	uint8_t Dummy=Dummy;

	/* Reset SSP */
	LPC_SYSCON->PRESETCTRL |= (1<<0);

	/* Enable AHB clock to the SSP domain. */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 11);

	#if CFG_SSP0_MOSI_LOCATION == CFG_SSP_MOSI0_0_9
		/* Set P0.9 to SSP MOSI0 */
		LPC_IOCON->PIO0_9 = 0x01;
		LPC_GPIO->DIR[PORT0] |= (1<<28);	// MOSI is output
	#else
		// Not used
	#endif

	#if CFG_SSP0_MISO_LOCATION == CFG_SSP_MISO0_0_8
		/* Set P0.8 to SSP MISO0 */
		LPC_IOCON->PIO0_8 = 0x01;
		LPC_GPIO->DIR[PORT0] &= ~(1<<27);	// MISO is input
	#else
		// Not used
	#endif

	#if CFG_SSP0_SCK_LOCATION == CFG_SSP_SCK0_1_29
		/* Set 1.29 to SSP SCK0 */
		LPC_IOCON->PIO1_29 = 0x01;
	#else
		// Not used
	#endif

	/* Set SPI clock to low-speed by default */
	ssp0ClockFast();

	/* Clear the Rx FIFO */
	for ( int i = 0; i < SSP0_FIFOSIZE; i++ ){
		Dummy = LPC_SSP0->DR;
	}

	/* Enable device and set it to master mode, no loopback */
	LPC_SSP0->CR1 = SSP_CR1_SSE_ENABLED | SSP_CR1_MS_MASTER | SSP_CR1_LBM_NORMAL;
}

/**************************************************************************/
/*!
    @brief Sends a block of data using SS0

    @param[in]  buf
                Pointer to the data buffer
    @param[in]  length
                Block length of the data buffer
*/
/**************************************************************************/
void ssp0Send (uint8_t *buf, uint32_t length){
  uint32_t i;
  uint8_t Dummy = Dummy;

  for (i = 0; i < length; i++){
    /* Move on only if NOT busy and TX FIFO not full. */
    while ((LPC_SSP0->SR & (SSP_SR_TNF_NOTFULL | SSP_SR_BSY_BUSY)) != SSP_SR_TNF_NOTFULL);
    LPC_SSP0->DR = reverseByte(*buf);
    //LPC_SSP0->DR = *buf;
    buf++;

    while ( (LPC_SSP0->SR & (SSP_SR_BSY_BUSY|SSP_SR_RNE_NOTEMPTY)) != SSP_SR_RNE_NOTEMPTY );
    /* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO
    on MISO. Otherwise, when sspReceive is called, previous data byte
    is left in the FIFO. */
//    Dummy = LPC_SSP0->DR;

  }
  return;
}

uint8_t ssp0SendChar(uint8_t data) {
	while ( (LPC_SSP0->SR & (SSP_SR_TNF_NOTFULL | SSP_SR_BSY_BUSY)) != SSP_SR_TNF_NOTFULL )
		// Shhhh... Just Wait...
		;

	// Send data

	LPC_SSP0->DR = reverseByte(data);
	//LPC_SSP0->DR = data;
	while ( (LPC_SSP0->SR & (SSP_SR_TNF_NOTFULL | SSP_SR_BSY_BUSY)) != SSP_SR_TNF_NOTFULL )
		// Shhhh... Just Wait...
		;

    while ( (LPC_SSP0->SR & (SSP_SR_BSY_BUSY | SSP_SR_RNE_NOTEMPTY)) != SSP_SR_RNE_NOTEMPTY );

    uint8_t ret = LPC_SSP0->DR;

    return reverseByte(ret);

}

/**************************************************************************/
/*!
    @brief Receives a block of data using SSP0

    @param[in]  buf
                Pointer to the data buffer
    @param[in]  length
                Block length of the data buffer
*/
/**************************************************************************/
void ssp0Receive(uint8_t *buf, uint32_t length){
  uint32_t i;

  for ( i = 0; i < length; i++ ){
    /* As long as the receive FIFO is not empty, data can be received. */
    LPC_SSP0->DR = 0xFF;

    /* Wait until the Busy bit is cleared */
    while ( (LPC_SSP0->SR & (SSP_SR_BSY_BUSY | SSP_SR_RNE_NOTEMPTY)) != SSP_SR_RNE_NOTEMPTY );

    *buf = reverseByte(LPC_SSP0->DR);
    buf++;
  }

  return;
}

