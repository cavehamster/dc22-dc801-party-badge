/**
 *
 * @brief Driver for the EEPROM
 * @file eeprom.h
 * @author hamster
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
 */

#ifndef DRIVERS_EEPROM_H_
#define DRIVERS_EEPROM_H_

/** Flash Programming Entry Location */
#define FLASH_ENTRY_LOCATION    (0x1fff1ff1)

/** The maximum number of flash programing parameters */
#define FLASH_PARAMETER_NUM     (5)

/** The maximum number of flash programing results */
#define FLASH_RESULT_NUM        (4)

/** Flash programming function type */
typedef void (*FLASH_ENTRY_T)(unsigned int[], unsigned int[]);

/**
 * @brief Flash command code definitions
 */
typedef enum {
	FLASH_PREPARE = 50,				/*!< Prepare sector(s) for write operation */
	FLASH_COPY_RAM_TO_FLASH = 51,	/*!< Copy RAM to Flash */
	FLASH_ERASE = 52,				/*!< Erase sector(s) */
	FLASH_BLANK_CHECK = 53,			/*!< Blank check sector(s) */
	FLASH_READ_PART_ID = 54,		/*!< Read chip part ID */
	FLASH_READ_BOOT_VER = 55,		/*!< Read chip boot code version */
	FLASH_COMPARE = 56,				/*!< Compare memory areas */
	FLASH_REINVOKE_ISP = 57,		/*!< Reinvoke ISP */
	FLASH_READ_UID = 58,			/*!< Read UID */
	FLASH_ERASE_PAGE = 59,			/*!< Erase page */
	FLASH_EEPROM_WRITE = 61,		/*!< EEPROM Write */
	FLASH_EEPROM_READ = 62,			/*!< EEPROM Read */
}  FLASH_CMD_CODE_T;

/**
 * @brief Flash status code definitions
 */
typedef enum {
	FLASH_CMD_SUCCESS,				/*!< Command is executed successfully. */
	FLASH_INVALID_COMMAND,			/*!< Invalid command. */
	FLASH_SRC_ADDR_ERROR,				/*!< Source address is not on a word boundary. */
	FLASH_DST_ADDR_ERROR,				/*!< Destination address is not on a correct boundary. */
	FLASH_SRC_ADDR_NOT_MAPPED,		/*!< Source address is not mapped in the memory map. */
	FLASH_DST_ADDR_NOT_MAPPED,		/*!< Destination address is not mapped in the memory map. */
	FLASH_COUNT_ERROR,				/*!< Byte count is not multiple of 4 or is not a permitted value. */
	FLASH_INVALID_SECTOR,				/*!< Sector number is invalid. */
	FLASH_SECTOR_NOT_BLANK,				/*!< Sector is not blank. */
	FLASH_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION,	/*!< Command to prepare sector for write operation was not executed. */
	FLASH_COMPARE_ERROR,				/*!< Source and destination data is not same. */
	FLASH_BUSY,						/*!< Flash programming hardware interface is busy. */
} FLASH_STATUS_CODE_T;

/** EEPROM size */
#define EEPROM_SIZE                 (4 << 10)

/**
 * @brief Command parameter table structure
 */
typedef struct {
	uint32_t cmd;			/*!< Command code */
	uint32_t pParams[FLASH_PARAMETER_NUM];	/*!< Parameters*/
} FLASH_COMMAND_T;

/**
 * @brief Command result table structure
 */
typedef struct {
	uint32_t status;		/*!< Status code */
	uint32_t pResult[FLASH_RESULT_NUM];		/*!< Results*/
} FLASH_OUTPUT_T;

/* Flash Programming Command Description
   Command                  Parameters              Return Code                                 Result
   ----------------------------------------------------------------------------------------------------------
   FLASH_PREPARE            Start Sector Number     CMD_SUCCESS                                 None
                            End Sector Number       BUSY
                                                    INVALID_SECTOR
   FLASH_COPY_RAM2FLASH     Destination Flash Addr  CMD_SUCCESS                                 None
                            Source RAM Addr         SRC_ADDR_ERROR
                            Number of bytes written SRC_ADDR_NOT_MAPPED
                            CCLK in kHz             DST_ADDR_NOT_MAPPED
                                                    COUNT_ERROR
                                                    SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION
                                                    BUSY
   FLASH_ERASE              Start Sector Number     CMD_SUCCESS                                 None
                            Emd Sector Number       BUSY
                            CCLK in kHz             SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION
                                                    INVALID_SECTOR
   FLASH_BLANK_CHECK        Start Sector Number     CMD_SUCCESS                                 Non Blank Sector Offset(if Status code is SECTOR_NOT_BLANK)
                            End Sector Number       BUSY                                        Content of non blank word location
                                                    SECTOR_NOT_BLANK
                                                    INVALID_SECTOR
   FLASH_READ_PART_ID       None                    CMD_SUCCESS                                 Part ID
   FLASH_READ_BOOT_VER      None                    CMD_SUCCESS                                 <byte1(Major)>.<byte0(Minor)>
   FLASH_COMPARE            Destination Addr        CMD_SUCCESS                                 Offset of the first mismatch
                            Source Address          COMPARE_ERROR
                            Number of bytes compared COUNT_ERROR
                                                    ADDR_ERROR
                                                    ADDR_NOT_MAPPED
   FLASH_REINVOKE_ISP       None                    None                                        None
   FLASH_READ_UID           None                    CMD_SUCCESS                                 The first 32-bit word
                                                                                                The second 32-bit word.
                                                                                                The third 32-bit word.
                                                                                                The fourth 32-bit word
   FLASH_ERASE_PAGE         Start Page Number       CMD_SUCCESS                                 None
                            End Page Number         BUSY
                            CCLK in kHz             SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION
                                                    INVALID_SECTOR
   FLASH_EEPROM_WRITE       EEPROM Addr             CMD_SUCCESS                                 None
                            RAM Addr                SRC_ADDR_NOT_MAPPED
                            Number of bytes written DST_ADDR_NOT_MAPPED
                            CCLK in kHz
   FLASH_EEPROM_READ        EEPROM Addr             CMD_SUCCESS                                 None
                            RAM Addr                SRC_ADDR_NOT_MAPPED
                            Number of bytes read    DST_ADDR_NOT_MAPPED
                            CCLK in kHz
 */

/**
 * @brief [Prepare sectors] command parameter table structure
 */
typedef struct {
	uint32_t cmd;		/*!< Command code */
	uint32_t start;		/*!< Start Sector Number */
	uint32_t end;		/*!<End Sector Number (should be greater than or equal to start sector number).*/
} FLASH_PREPARE_SECTORS_COMMAND_T;

/**
 * @brief [Prepare sectors] command result table structure
 */
typedef struct {
	uint32_t status;			/*!< Status code */
} FLASH_PREPARE_SECTORS_OUTPUT_T;

/**
 * @brief [Copy Ram to Flash] command parameter table structure
 */
typedef struct {
	uint32_t cmd;		/*!< Command code */
	uint32_t dst;		/*!< Destination flash address where data bytes are to be written (256 byte boundary) */
	uint32_t src;		/*!<Source RAM address from which data bytes are to be read (a word boudary).*/
	uint32_t byteNum;	/*!<Number of bytes to be written. Should be 256 | 512 | 1024 | 4096.*/
	uint32_t cclk;		/*!<System Clock Frequency (CCLK) in kHz.*/
} FLASH_COPY_RAM_TO_FLASH_COMMAND_T;

/**
 * @brief [Copy Ram to Flash] command result table structure
 */
typedef struct {
	uint32_t status;			/*!< Status code */
} FLASH_COPY_RAM_TO_FLASH_OUTPUT_T;

/**
 * @brief [Erase Sector(s)] command parameter table structure
 */
typedef struct {
	uint32_t cmd;		/*!< Command code */
	uint32_t start;		/*!< Start Sector Number */
	uint32_t end;		/*!<End Sector Number (should be greater than or equal to start sector number).*/
	uint32_t cclk;		/*!<System Clock Frequency (CCLK) in kHz.*/
} FLASH_ERASE_SECTORS_COMMAND_T;

/**
 * @brief [Erase Sector(s)] command result table structure
 */
typedef struct {
	uint32_t status;			/*!< Status code */
} FLASH_ERASE_SECTORS_OUTPUT_T;

/**
 * @brief [Blank check sector(s)] command parameter table structure
 */
typedef struct {
	uint32_t cmd;		/*!< Command code */
	uint32_t start;		/*!< Start Sector Number */
	uint32_t end;		/*!<End Sector Number (should be greater than or equal to start sector number).*/
} FLASH_BLANK_CHECK_SECTORS_COMMAND_T;

/**
 * @brief [Blank check sector(s)] command result table structure
 */
typedef struct {
	uint32_t status;			/*!< Status code */
	uint32_t firstNonBlankLoc;	/*!< Offset of the first non blank word location if the Status Code is SECTOR_NOT_BLANK.*/
	uint32_t firstNonBlankVal;	/*!<Contents of non blank word location.*/
} FLASH_BLANK_CHECK_SECTORS_OUTPUT_T;

/**
 * @brief [Read Part Identification number] command parameter table structure
 */
typedef struct {
	uint32_t cmd;		/*!< Command code */
} FLASH_READ_PART_ID_COMMAND_T;

/**
 * @brief [Read Part Identification number] command result table structure
 */
typedef struct {
	uint32_t status;	/*!< Status code */
	uint32_t partID;	/*!< Part Identification Number*/
} FLASH_READ_PART_ID_OUTPUT_T;

/**
 * @brief [Read Boot code version number] command parameter table structure
 */
typedef struct {
	uint32_t cmd;		/*!< Command code */
} FLASH_READ_BOOTCODE_VER_COMMAND_T;

/**
 * @brief [Read Boot code version number] command result table structure
 */
typedef struct {
	uint32_t status;	/*!< Status code */
	uint8_t minor;		/*!< Minor*/
	uint8_t major;		/*!< Major*/
} FLASH_READ_BOOTCODE_VER_OUTPUT_T;

/**
 * @brief [Compare memory] command parameter table structure
 */
typedef struct {
	uint32_t cmd;		/*!< Command code */
	uint32_t dst;		/*!<Starting flash or RAM address of data bytes to be compared (a word boundary) */
	uint32_t src;		/*!<Starting flash or RAM address of data bytes to be compared (a word boudary).*/
	uint32_t byteNum;	/*!<Number of bytes to be compared; should be a multiple of 4.*/
} FLASH_COMPARE_MEM_COMMAND_T;

/**
 * @brief [Compare memory] command result table structure
 */
typedef struct {
	uint32_t status;	/*!< Status code */
	uint32_t offset;	/*!< Offset of the first mismatch if the Status Code is COMPARE_ERROR.*/
} FLASH_COMPARE_MEM_OUTPUT_T;

/**
 * @brief [Reinvoke ISP] command parameter table structure
 */
typedef struct {
	uint32_t cmd;		/*!< Command code */
} FLASH_REINVOKE_ISP_COMMAND_T;

/**
 * @brief [Reinvoke ISP] command result table structure
 */
typedef struct {
	uint32_t status;	/*!< Status code */
} FLASH_REINVOKE_ISP_OUTPUT_T;

/**
 * @brief [ReadUID] command parameter table structure
 */
typedef struct {
	uint32_t cmd;		/*!< Command code */
} FLASH_READ_UID_COMMAND_T;

/**
 * @brief [ReadUID] command result table structure
 */
typedef struct {
	uint32_t status;	/*!< Status code */
	uint32_t id[4];		/*!< UID*/
} FLASH_READ_UID_OUTPUT_T;

/**
 * @brief [Erase page(s)] command parameter table structure
 */
typedef struct {
	uint32_t cmd;		/*!< Command code */
	uint32_t start;		/*!< Start Page Number */
	uint32_t end;		/*!<End Page Number (should be greater than or equal to start page number).*/
	uint32_t cclk;		/*!<System Clock Frequency (CCLK) in kHz.*/
} FLASH_ERASE_PAGES_COMMAND_T;

/**
 * @brief [Erase page(s)] command result table structure
 */
typedef struct {
	uint32_t status;	/*!< Status code */
} FLASH_ERASE_PAGES_OUTPUT_T;

/**
 * @brief [Write EEPROM] command parameter table structure
 */
typedef struct {
	uint32_t cmd;			/*!< Command code */
	uint32_t eepromAddr;	/*!< EEPROM address.*/
	uint32_t ramAddr;		/*!< RAM address.*/
	uint32_t byteNum;		/*!<Number of bytes to be written.*/
	uint32_t cclk;			/*!<System Clock Frequency (CCLK) in kHz.*/
} EEPROM_WRITE_COMMAND_T;

/**
 * @brief [Write EEPROM] command result table structure
 */
typedef struct {
	uint32_t status;	/*!< Status code */
} EEPROM_WRITE_OUTPUT_T;

/**
 * @brief [Read EEPROM] command parameter table structure
 */
typedef struct {
	uint32_t cmd;		/*!< Command code */
	uint32_t eepromAddr;	/*!< EEPROM address.*/
	uint32_t ramAddr;		/*!< RAM address.*/
	uint32_t byteNum;		/*!<Number of bytes to be written.*/
	uint32_t cclk;			/*!<System Clock Frequency (CCLK) in kHz.*/
} EEPROM_READ_COMMAND_T;

/**
 * @brief [Read EEPROM] command result table structure
 */
typedef struct {
	uint32_t status;	/*!< Status code */
} EEPROM_READ_OUTPUT_T;


/**
 * @brief	Get sector number of the specified address
 * @param	adr		: flash address.
 * @return	sector number
 */
static inline uint32_t FLASH_GetSecNum(uint32_t adr)
{
	return adr >> 12;
}

/**
 * @brief	Execute flash programming command
 * @param	pCommand	: Command information
 * @param	pOutput	: Output information
 * @return	Nothing
 */
static inline void FLASH_Execute(FLASH_COMMAND_T *pCommand, FLASH_OUTPUT_T *pOutput)
{
	((FLASH_ENTRY_T) FLASH_ENTRY_LOCATION)((unsigned int *) pCommand, (unsigned int *) pOutput);
}

/**
 * @brief	Prepare sector(s) for write operation
 * @param	pCommand	: Command information
 * @param	pOutput	: Output information
 * @return	Nothing
 * @note	This command must be executed before executing "Copy RAM to flash" or "Erase Sector(s)" command. <br>
 *			The boot sector can not be prepared by this command.
 */
static inline void FLASH_PrepareSectors(FLASH_PREPARE_SECTORS_COMMAND_T *pCommand,
											 FLASH_PREPARE_SECTORS_OUTPUT_T *pOutput)
{
	FLASH_Execute((FLASH_COMMAND_T *) pCommand, (FLASH_OUTPUT_T *) pOutput);
}

/**
 * @brief	Copy RAM to flash
 * @param	pCommand	: Command information
 * @param	pOutput	: Output information
 * @return	Nothing
 * @note	The affected sectors should be prepared first by calling "Prepare Sector for Write Operation" command.<br>
 *		The boot sector can not be written by this command.
 */
static inline void FLASH_CopyRamToFlash(FLASH_COPY_RAM_TO_FLASH_COMMAND_T *pCommand,
											 FLASH_COPY_RAM_TO_FLASH_OUTPUT_T *pOutput)
{
	FLASH_Execute((FLASH_COMMAND_T *) pCommand, (FLASH_OUTPUT_T *) pOutput);
}

/**
 * @brief	Erase Sector(s)
 * @param	pCommand	: Command information
 * @param	pOutput	: Output information
 * @return	Nothing
 * @note	 The boot sector can not be erased by this command.
 */
static inline void FLASH_EraseSectors(FLASH_ERASE_SECTORS_COMMAND_T *pCommand,
										   FLASH_ERASE_SECTORS_OUTPUT_T *pOutput)
{
	FLASH_Execute((FLASH_COMMAND_T *) pCommand, (FLASH_OUTPUT_T *) pOutput);
}

/**
 * @brief	Blank check sector(s)
 * @param	pCommand	: Command information
 * @param	pOutput	: Output information
 * @return	Nothing
 */
static inline void FLASH_BlankCheckSectors(FLASH_BLANK_CHECK_SECTORS_COMMAND_T *pCommand,
												FLASH_BLANK_CHECK_SECTORS_OUTPUT_T *pOutput)
{
	FLASH_Execute((FLASH_COMMAND_T *) pCommand, (FLASH_OUTPUT_T *) pOutput);
}

/**
 * @brief	Read Part Identification number
 * @param	pOutput	: Output information
 * @return	Nothing
 */
void FLASH_ReadPartID(FLASH_READ_PART_ID_OUTPUT_T *pOutput);

/**
 * @brief	Read Boot code version number
 * @param	pOutput	: Output information
 * @return	Nothing
 */
void FLASH_ReadBootCodeVersion(FLASH_READ_BOOTCODE_VER_OUTPUT_T *pOutput);

/**
 * @brief	Compare memory
 * @param	pCommand	: parameters
 * @param	pOutput	: results
 * @return	Nothing
 * @note	The result may not be correct when the source or destination includes any
 * of the first 512 bytes starting from address zero. The first 512 bytes can be re-mapped to RAM.
 */
static inline void FLASH_CompareMem(FLASH_COMPARE_MEM_COMMAND_T *pCommand,
										 FLASH_COMPARE_MEM_OUTPUT_T *pOutput)
{
	FLASH_Execute((FLASH_COMMAND_T *) pCommand, (FLASH_OUTPUT_T *) pOutput);
}

/**
 * @brief	Reinvoke ISP
 * @return	Nothing
 */
void FLASH_ReInvokeISP(void);

/**
 * @brief	Read UID
 * @param	pOutput	: Output information
 * @return	Nothing
 */
void FLASH_ReadUID(FLASH_READ_UID_OUTPUT_T *pOutput);

/**
 * @brief	Erase Page(s)
 * @param	pCommand	: Command information
 * @param	pOutput	: Output information
 * @return	Nothing
 */
static inline void FLASH_ErasePages(FLASH_ERASE_PAGES_COMMAND_T *pCommand,
										 FLASH_ERASE_PAGES_OUTPUT_T *pOutput)
{
	FLASH_Execute((FLASH_COMMAND_T *) pCommand, (FLASH_OUTPUT_T *) pOutput);
}


/**
 * @brief	Write EEPROM
 * @param	pCommand	: Command information
 * @param	pOutput	: Output information
 * @return	Nothing
 * @note		The top 64 bytes of the EEPROM memory are reserved and cannot be written to.
 */
static inline void EEPROM_Write(EEPROM_WRITE_COMMAND_T *pCommand,
									 EEPROM_WRITE_OUTPUT_T *pOutput)
{
	FLASH_Execute((FLASH_COMMAND_T *) pCommand, (FLASH_OUTPUT_T *) pOutput);
}

/**
 * @brief	Read EEPROM
 * @param	pCommand	: Command information
 * @param	pOutput	: Output information
 * @return	Nothing
 */
static inline void EEPROM_Read(EEPROM_READ_COMMAND_T *pCommand,
									EEPROM_READ_OUTPUT_T *pOutput)
{
	FLASH_Execute((FLASH_COMMAND_T *) pCommand, (FLASH_OUTPUT_T *) pOutput);
}


bool FLASH_ReadData(unsigned int *data, unsigned int len);
bool FLASH_WriteData(unsigned int *data, unsigned int len);

#endif /* DRIVERS_EEPROM_H_ */
