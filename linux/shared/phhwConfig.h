/*******************************************************************************
 *   config.h:  config file for example for Linux boards
 *   Microprocessors
 *
 *   Copyright(C) 2008-2016, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2012.07.20  ver 1.00    First Release
 *   2016.01.26	 ver 2.00	 Adaptation for Linux
 *
 ******************************************************************************/

/*------------------------------------------------------------------------------
 * Includes
 -----------------------------------------------------------------------------*/
#ifndef __PHHW_CONFIG_H__
#define __PHHW_CONFIG_H__

#include <phhalHw.h>
#include <ph_NxpBuild.h>
#include <phOsal.h>

/*******************************************************************************
 **    Global macros and definitions
 ******************************************************************************/

#define RaspberryPi_Rev1_Pinout 1 //This includes PCB revision 1 of RPi boards model A & B (no P5 header)
#define	RaspberryPi_Rev2_Pinout 2 //This includes PCB revision 2 of RPi boards model A & B (P5 header present), models A+, B+ and 2B


#ifndef BOARD
#define BOARD RaspberryPi_Rev2_Pinout
#endif

//Always have debug
#ifdef   DEBUG
#    include <stdio.h>
#    define  DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
#    define  DEBUG_PRINTF(...)
#endif

/* prints if error is detected */
#define CHECK_STATUS(x)                                      \
    if ((x) != PH_ERR_SUCCESS)                               \
{                                                            \
    DEBUG_PRINTF("Line: %d   Error - (0x%04X) has occurred : 0xCCEE CC-Component ID, EE-Error code. Refer-ph_Status.h\n", __LINE__, (x));    \
}

/* prints if error is detected */
#define CHECK_SUCCESS(x)              \
    if ((x) != PH_ERR_SUCCESS)        \
{                                     \
    DEBUG_PRINTF("\nLine: %d   Error - (0x%04X) has occurred : 0xCCEE CC-Component ID, EE-Error code. Refer-ph_Status.h\n ", __LINE__, (x)); \
    return (x);                       \
}

/* ------------------------------------------------ */
/* pins definition                                  */
/* ------------------------------------------------ */

#ifdef NXPBUILD__PHHAL_HW_RC523 //Explore-NFC board

#if BOARD == RaspberryPi_Rev1_Pinout
#define SPI_CONFIG		   "0:0:0" //PN512 is connected to SPI bus 0, CS pin 0, no busy pin
#define PIN_CS_IN		   0
#define PIN_IRQ			   23
#define PIN_NRST		   7


#elif BOARD == RaspberryPi_Rev2_Pinout
#define SPI_CONFIG		   "0:0:0" //PN512 is connected to SPI bus 0, CS pin 0, no busy pin
#define PIN_CS_IN		   2
#define PIN_IRQ			   23
#define PIN_NRST		   7


#endif

#endif /* NXPBUILD__PHHAL_HW_RC523 */

#ifdef NXPBUILD__PHHAL_HW_RC663 //Explore-NFC board

#if BOARD == RaspberryPi_Rev1_Pinout
#define SPI_CONFIG		   "0:0:0" //RC663 is connected to SPI bus 0, CS pin 0, no busy pin
#define PIN_CS_IN		   2
#define PIN_IRQ			   23
#define PIN_NRST		   24

#define PIN_IFSEL0         27
#define PIN_IFSEL1          22


#elif BOARD == RaspberryPi_Rev2_Pinout
#define SPI_CONFIG		   "0:0:0" //RC663 is connected to SPI bus 0, CS pin 0, no busy pin
#define PIN_CS_IN		   2
#define PIN_IRQ			   23
#define PIN_NRST		   24

#define PIN_IFSEL0         27
#define PIN_IFSEL1          22


#endif

#endif /* NXPBUILD__PHHAL_HW_RC663 */

#ifdef NXPBUILD__PHHAL_HW_PN5180

#if BOARD == RaspberryPi_Rev1_Pinout
#define SPI_CONFIG		   "0:0:25" //PN5180 is connected to SPI bus 0, CS pin 0, busy pin GPIO 25
#define PIN_IRQ			   23
#define PIN_NRST		   7

#elif BOARD == RaspberryPi_Rev2_Pinout
#define SPI_CONFIG		   "0:0:25" //PN5180 is connected to SPI bus 0, CS pin 0, busy pin GPIO 25
#define PIN_IRQ			   23
#define PIN_NRST		   7

#endif

#endif /* NXPBUILD__PHHAL_HW_PN5180 */

/*******************************************************************************
 **    Function prototypes
 ******************************************************************************/
int Set_Interface_Link(void);
void Cleanup_Interface_Link(void);
int Set_Interrupt();
void Cleanup_Interrupt();
void Reset_reader_device(void);

void Configure_Device(phhalHw_Nfc_Ic_DataParams_t* pHal);

/*******************************************************************************
 * Reader IC specific soft reset function - command
 ******************************************************************************/
#ifdef NXPBUILD__PHHAL_HW_RC523
    #define SoftReset_reader_device(pHal)    phhalHw_Rc523_Cmd_SoftReset(pHal)
#endif
#ifdef NXPBUILD__PHHAL_HW_RC663
    #define SoftReset_reader_device(pHal)    phhalHw_Rc663_Cmd_SoftReset(pHal)
#endif

#endif    /* __PHHW_CONFIG_H__ */

/*******************************************************************************
 **                            End Of File
 ******************************************************************************/
