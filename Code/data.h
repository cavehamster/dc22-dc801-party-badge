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

#ifndef DATA_H_
#define DATA_H_

// This len is based on the service configuration
#define BLEDataLen		265

// We have to make sure this struct is packed as we'll be saving it out to EEPROM as a big hunk of data
typedef struct __attribute__((packed)) {
	uint8_t configured;
	uint8_t username[9];
	uint8_t BLEDynamicDataMsgs;
	uint8_t BLEDynamicData[BLEDataLen];
	bool BLEEnabled;
	bool BLEPairable;
} BADGE_DATASTORE;

typedef struct{
	bool update;
	bool lock;
} BADGE_UPDATE_DATA;

// Magic number that means the stored data is good to use
#define	DATA_CONFIGURED		0x23

#define DEFAULT_USERNAME	"dc801"
#define DEFAULT_VECTOR		"_Helga_"

void dataClear(void);
void dataInit(void);
void dataChanged(void);
void dataFlush(void);
uint8_t dataGetBLEDataMsgs(void);
uint8_t * dataGetBLEData(void);

#ifdef DATA_MODULE
 #undef EXTERN
 #define EXTERN
#else
 #undef EXTERN
 #define EXTERN extern
#endif

// The datastore
EXTERN BADGE_DATASTORE dataStore;

// Data changed?
EXTERN BADGE_UPDATE_DATA dataUpdate;

#endif /* DATA_H_ */
