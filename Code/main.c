/**
 *
 * @brief dc801 defcon 22 badge, revisted!
 * @file main.c
 * @author hamster
 *
 * Hot dog it works!
 *
 * This new app will:
 *
 * 1) Burn your retina with the RGB neopixel
 * 2) Spew sensitive data via BLE
 * 3) Tell wild lies about you via the display
 *
 *
 * At first boot, the badge will display a banner.  Below it is the word 'Menu' which should clue you in that
 * you can press the A button to enter the menu, and an 8 char username.  The username is also broadcast as the
 * Bluetooth device name.
 *
 * Initially, BLE is disabled.  Enter the menu system to change your username, and enable BLE.
 *
 * Once enabled, the badge will begin broadcasting that is a DC801 Defcon 25 badge.  Why 25?  Well... no other badges
 * recognize Defcon 22 badges, so I thought perhaps it would be more fun if they could see this one.  If you remember
 * the actual Defcon 25 badge, you recall there was a game of infection.  I didn't implement it on this badge, for reasons
 * I will get into in a moment.
 *
 * The badge will then have BLE enabled, but it won't be pairable.  A new menu option will appear to allow pairing.  Turn it on.
 * Connect to your badge with your phone or whatever - I recommend using the Nordic nRF Connect app for Android.  Once you connect to
 * the badge, you will see a list of characteristics that the badge broadcasts.  Some are readable without pairing.
 *
 * If you pair with the badge, or click on an item that needs you to be paired, your phone should pop up a 'device wants to pair
 * with you' item, and the pairing key will display on your badge.  Enter it in.  If you get it wrong or take more than 30 seconds,
 * pairing will fail.  The badge will show if it worked out or not.
 *
 * Once you are paired, the LED will glow to show that you are paired.  Now, you can use the Nordic UART characteristic to send data
 * to the badge.  Known bug here: if you try to read data, the BLE stack on the badge will reset.  Not that it has anything interesting
 * to say anyway.  In the nRF Connect app, that's the TX characteristic - it's backwards from what you would expect.  Use the RX one to send
 * a string to the badge.  Holy hot cakes!  The top line changes!  This is implemented as a circular buffer, in that if you send more
 * than 16 chars it will wrap back around, but not in a way that's useful.  Everytime you send a new string, it will reset to position 0,0
 * and draw the string again.  Get funky with it!
 *
 * To delete any stored pairing data from your badge, there is a menu option for that.  It might or might not be useful for you - the badge
 * wants to re-pair most of the time after a power cycle anyway.  Probably a bug here for me to fix.
 *
 * To reset the badge to factory condition, hold down button A while powering it on or resetting it.  The display will say 'Cleared!'
 * once the stored data has been wiped.
 *
 * Also in the characteristics, there are various strings present, and the nRF8001 chip temperature in C can be read.
 *
 *
 * Now a few other notes.
 *
 *
 * Bluetooth and the nRF8001
 *
 * The nRF8001 BLE chip really mucks with your head.  For instance, the advertisement data can't be changed without resetting the chip.
 * This data, along with all the rest of the config, is spelled out in the setup messages in services.h.  That whole thing is generated
 * with the nRF Go Studio app or whatever it's called.  There is a document out there that describes most of the setup data, but it's
 * almost entirely a black box.
 *
 * In order to change the advertised name, I've employed a hack.  I counted the actual offset in the setup data where the name is set,
 * and set that as a #define.  Then, when you update your username, I go write that username to the setup messages directly, and
 * force the chip to reset.  When it resets, this is detected, and the setup routine re-generates the CRC for the setup data and
 * programs the BLE chip.
 *
 * So why no vector?  Since the advertised data needs to change periodically to play the game (to change your random number), the BLE
 * would need to reset fairly often as well.  While it is possible to ask the nRF8001 to re-connect to your phone after a reset, I felt
 * that it's a better experience to not have the BLE stack reset itself somewhat randomly.  Further, the nRF8001 is a peripheral only -
 * it can't handle any central duties, so it cannot scan for any other BLE devices.  This means it would only be able to play half the
 * game anyway.  Ah well.
 *
 *
 * Programming
 *
 * To program, hook the USB up to your computer.  Hold down button B and tap reset.  It should then show up as a USB stick or hard
 * drive as CRP_DISABLED.  Delete 'firmware.bin' off the stick.  Then drag and drop the new 'firmware.bin' onto the stick.  After it has
 * copied, tap reset to boot to the new code.
 *
 * Notes here: firmware.bin must be exactly 64k.  It's always padded out to 64k.  It'll show as 0k on the 'hard drive' though.  That's
 * OK.  You can't read it back off the device, unfortunately.  The reason you have to delete first is that most operating systems
 * will copy then delete the old one, and there is not enough space for both.  It's gotta be an empty drive.
 *
 *
 * Charging
 *
 * The VFD inrush pulls enough that if your battery is flat and you are tyring to charge it with a wimpy USB port/charger, it's really
 * not going to go well for you.  An easy workaround is to force the device into programming mode, as per above.  The display won't turn on
 * and you can just leave it plugged in for an hour or so to get the battery charged enough that you can then play with it.  Just tap
 * reset to return to normal mode.
 *
 *
 * TODO and etc
 *
 * 1) The speaker is not doing anything at the moment.  This is pretty easy to fix, and I might well get to it soon.
 *
 * 2) There is no way the system can tell what the battery voltage is.  It can only tell if your USB is plugged in or not.  Sorry!
 *
 * 3) No on/off switch or sleep mode currently implemented.  Charge often!
 *
 * 4) If your BLE won't work, make sure you read the readme on github and moved the bodge wire on the back of the board.  If you did that,
 *    and you set BLE to be enabled, well.. you might have a bad BLE chip.  Inspect the board under a microscope, replace the chip, or
 *    live with it.  These were never tested for functionality at the 'factory' as it were so who knows.
 *
 * 5) If your LED doesn't show all 3 colors (red, green, blue) at bootup, you probably have a bad diode.  Neopixels hate reflow solder.
 *    You can buy a new one from Adafruit or whatever, and replace it yourself.  It's not hard, but... it's a pain.  Careful not to
 *    rip the pads off the board like I did...
 *
 *
 * Anyway - suggestions for improvement, pull requests, snide comments, or just pointing out how awful my code is is all welcome.
 * Hit me up on twitter - @hamster
 *
 * Enjoy!
 *
 *
 */

#include "LPC13Uxx.h"
#include "lpc_types.h"
#include <string.h>
#include "core/inc/lpc13uxx_gpio.h"
#include "core/inc/lpc13uxx_timer32.h"
#include "core/inc/lpc13uxx_uart.h"
#include "drivers/common.h"
#include "drivers/buttons.h"
#include "drivers/bluetooth.h"
#include "drivers/vfd.h"
#include "drivers/ws2812b.h"
#include "drivers/eeprom.h"
#include "data.h"
#include "menu.h"
#include "main.h"

volatile TIME sysTime = {0, 0};

/**
 * Increment system time at each tick
 *
 * This is called from an interrupt once a millisecond
 */
void SysTick_Handler(void){

	// Increment the ticker
	if(++sysTime.Milliseconds >= 1000){
		sysTime.Seconds++;
		sysTime.Milliseconds = 0;
	}

	if(buttonIsInitialized()){
		buttonSample();
	}

	if(sysTime.Milliseconds == 0){
		dataFlush();
	}

}

/**
 * Buckle up ladies and gents and other non-binary gender personas (TRIGGERED!) - here it is!
 */
int main(void) {

	// Update the clock and configure the systick handler to fire every millisecond
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/1000);

	// Enable GPIO clock
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

	// Debug LED on back of board
	LPC_GPIO->DIR[PORT1] |= (1<<24);
	LPC_GPIO->SET[PORT1] &= ~(1<<24);

	// Initialize the serial port
	UARTInit(115200);
	lpcprintf("Boot\r\n");

	// Clear the state of the RGB LED to off
	ws2812bInit();
	ws2812bSetPixel(0,0,0);

	// Clear the screen and display the boot message
	VFDInit();
	VFDSetPower(VFD_ON);
	delay_ms(20);
	VFDSendCommand(VFD_CLEAR);
	VFDSendCommand(VFD_ENABLE);
	VFDSendCommand(VFD_CURSOR_OFF);

	// Initialize the buttons
	buttonInit();
	// Is button A held down?
	if(LPC_GPIO->B1[20]){
		// Clear the stored data to reset the badge to default
		ws2812bSetPixel(128, 0, 0);
		dataClear();
		lpcprintf("Saved data cleared\r\n");
		VFDWriteString("Cleared!");
		ws2812bSetPixel(0, 128, 0);
		while(LPC_GPIO->B1[20]);
		ws2812bSetPixel(0, 0, 0);
	}

	// Initialize the datastore
	dataInit();

	// Init the display state
	displayStateInit();

	// Poke BLE to init itself
	lib_aci_pin_reset();
	BLESetManufacturer(MANU_DC801);
	BLESetName(dataStore.username);
	BLEInit();

	// Done with the init, start up the app

	// Just for giggles, since we can, an example of how to get the PartID and Bootcode
	FLASH_READ_PART_ID_OUTPUT_T partID;
	memset(&partID, 0, sizeof(partID));
	FLASH_ReadPartID(&partID);

	lpcprintf("PartID: (%d) %04X\r\n", partID.status, partID.partID);

	FLASH_READ_BOOTCODE_VER_OUTPUT_T bootcodeVer;
	memset(&bootcodeVer, 0, sizeof(bootcodeVer));
	FLASH_ReadBootCodeVersion(&bootcodeVer);

	lpcprintf("Bootcode: (%d) %02X %02X\r\n", bootcodeVer.status, bootcodeVer.major, bootcodeVer.minor);

	// Main event loop
	// Call the helper functions to maintain the system status

	while(1){

		// Update the display
		displayStateMachine();

		// Handle BLE events
		BLEHandle();

// Here's how to get button presses.  Note that reading the press consumes the
// press - the next read won't return the button down unless it's down again
//
//		if(buttonIsShortPressed(BUTTON_A)){
//			lpcprintf("Button A Short Press\r\n");
//		}
//
//		if(buttonIsLongPressed(BUTTON_A)){
//			lpcprintf("Button A Long Press\r\n");
//		}
//
//		if(buttonIsShortPressed(BUTTON_B)){
//			lpcprintf("Button B Short Press\r\n");
//		}
//
//		if(buttonIsLongPressed(BUTTON_B)){
//			lpcprintf("Button B Long Press\r\n");
//		}
//
//		if(buttonIsShortPressed(BUTTON_C)){
//			lpcprintf("Button C Short Press\r\n");
//		}
//
//		if(buttonIsLongPressed(BUTTON_C)){
//			lpcprintf("Button C Long Press\r\n");
//		}
//
//		if(buttonIsShortPressed(BUTTON_D)){
//			lpcprintf("Button D Short Press\r\n");
//		}
//
//		if(buttonIsLongPressed(BUTTON_D)){
//			lpcprintf("Button D Long Press\r\n");
//		}


// A little routine to go through a bunch of colors
// It used msTicks instead of the time functions, so you'll have to
// update it to make it work again, if you want to enable this.
//
//		if((msTicks - ledTimer) > 10){
//			ledTimer = msTicks;
//			// Update the neopixel
//
//			if(red){
//				r++;
//				if(r == 0){
//					red = false;
//					green = true;
//				}
//			}
//			if(green){
//				g++;
//				if(g == 0){
//					green = false;
//					blue = true;
//				}
//			}
//			if(blue){
//				b++;
//				if(b == 0){
//					blue = false;
//					red = true;
//				}
//			}
//
//			ws2812bSetPixel(r, g, b);
//		}

	}

	return(0);

}



