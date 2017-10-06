/**
 *
 * @brief Menu state machine
 * @file menu.h
 * @author hamster
 *
 * Handler for on-screen menus
 *
 * It's a somewhat ugly state machine, but hey, it's pretty verbose
 *
 */

#ifndef MENU_H_
#define MENU_H_

// Characters allowed when getting a string from the user
#define FIRST_ALLOWED_CHAR	' '
#define LAST_ALLOWED_CHAR	'~'
#define FIRST_LETTER		'A'
#define LAST_LETTER 		'z'

// Strings we show on the screen
#define STRING_DISPLAY_BOOTUP_1				"DC801       BOOT"
#define STRING_DISPLAY_BOOTUP_2				"       Defcon 22"
#define STRING_DISPLAY_SETUP				"First Time Setup"
#define STRING_DISPLAY_MAIN					" -=[ DC 801 ]=-"
#define STRING_DISPLAY_SET_USERNAME			"Set Username?"
#define STRING_DISPLAY_SET_VECTOR			"Set Vector?"
#define STRING_DISPLAY_ENABLE_BLE			"Toggle BLE?"
#define STRING_DISPLAY_SET_PAIRABLE			"BLE Pairable?"
#define STRING_DISPLAY_DELETE_PAIRING		"Delete Pairing?"
#define STRING_DISPLAY_STRING_INSTRUCTION_1	"Hold select to "
#define STRING_DISPLAY_STRING_INSTRUCTION_2	"save -->  Select"
#define STRING_DISPLAY_MENU_MAIN			"Menu"
#define STRING_DISPLAY_ESC					"Esc"
#define STRING_DISPLAY_MENU_ITEM			"Next Select Back"
#define STRING_DISPLAY_STRING_ENTRY			"Esc +  -  Select"
#define STRING_DISPLAY_BINARY_ENTRY_ON_OFF	"Esc       On Off"
#define STRING_DISPLAY_BINARY_ENTRY_YES_NO	"Esc      Yes No "
#define STRING_DISPLAY_PAIRING_KEY			"Pairing Key"
#define STRING_DISPLAY_PAIRING_SUCCESS		"Paired!"
#define STRING_DISPLAY_PAIRING_FAILURE		"Failed to pair!"

// After this many ms of inactivity, return to the menu root
#define MENU_ITEM_TIMEOUT					5000
#define PAIRING_STATUS_TIMEOUT				1500

// Display states
typedef enum{
	DISPLAY_NOT_A_STATE,
	DISPLAY_BOOTUP,
	DISPLAY_SETUP,
	DISPLAY_MAIN,
	DISPLAY_SET_USERNAME,
	DISPLAY_ENABLE_BLE,
	DISPLAY_SET_PAIRABLE,
	DISPLAY_DELETE_PAIRING,

	DISPLAY_PAIRING_KEY,
	DISPLAY_PAIRING_FAIL,
	DISPLAY_PAIRING_SUCCESS,

	DISPLAY_INPUT_STRING,
	DISPLAY_INPUT_BINARY_ON_OFF,
	DISPLAY_INPUT_BINARY_YES_NO,
	DISPLAY_INSTRUCTIONS,

	NUM_DISPLAY_STATES
} DISPLAY_STATE;

typedef enum{
	SELECTION_WAITING,
	SELECTION_TRUE,
	SELECTION_FALSE
} SELECTION;

typedef enum{
	PAIRING_SHOW_KEY,
	PAIRING_SUCCESS,
	PAIRING_FAILED
} PAIRING_STATE;

typedef struct{
	uint8_t topLine[17];
	bool topLineChanged;
	DISPLAY_STATE curDisplayState;
	DISPLAY_STATE oldDisplayState;
	bool stateChanged;
	bool bleUpdate;
	DISPLAY_STATE bleDisplayCommand;
	TIME timeStamp;
	TIME buttonTimer;
	int position;
	uint8_t string[8];
	uint8_t *pairingKey;
} DISPLAY_DATA;

void displayStateInit(void);
void displayUpdateTopLine(uint8_t *newData, uint8_t len);
void displayPairing(PAIRING_STATE state, uint8_t *pairingKey);
void displayStateMachine(void);


#endif /* MENU_H_ */
