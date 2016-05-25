/*
*         Copyright (c), NXP Semiconductors Gratkorn / Austria
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

/** \file
* File used to configure example application for NXP Lpc11u68 and LPC176x Family MCU's.
* $Author:Bharamappa Khatagalli(nxp62726)$
* NxpNfcRdLib_R_Gate_V4.010.00.001602_ExC $Revision: 4144 $
* $Date: 2016-01-14 22:11:33 +0530 (Thu, 14 Jan 2016) $
*
*/
/*******************************************************************************
 *   History
 *   2012.07.20  ver 1.00    First Release
 *
 ******************************************************************************/

/*------------------------------------------------------------------------------
 * Includes
 -----------------------------------------------------------------------------*/
#ifndef __PHHW_CONFIG_H__
#define __PHHW_CONFIG_H__

#include <ph_NxpBuild.h>
#include <phhalHw.h>
#include <phOsal.h>

#ifdef LPC17xx
#include <phLpc1768_Build.h>
#undef LPC11U6x
#endif /* LPC17xx */

#ifdef LPC11U6x
#undef LPC17xx
#include <phLpc11u68_Build.h>
#endif /* LPC11U6x */

/*******************************************************************************
 **    Global macros and definitions
 ******************************************************************************/

#ifdef   DEBUG
#include <stdio.h>
#define  DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
#define  DEBUG_PRINTF(...)
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
/* general ports definition                         */
/* ------------------------------------------------ */
#define PORT0              0
#define PORT1              1
#define PORT2              2
#define PORT3              3
#define PORT4              4

/* ------------------------------------------------ */
/* value/state definition                           */
/* ------------------------------------------------ */
#define SET_OUT            1            /* a pin configured to output direction */
#define SET_IN             0            /* a pin configured to input direction */
#define SET_HIGH           1
#define SET_LOW            0

#ifdef LPC11U6x

#define PIN_LED				17
#define PORT_LED			PORT2
#define LED_ON				SET_LOW
#define LED_OFF				SET_HIGH

#ifdef NXPBUILD__PHHAL_HW_RC663

#ifndef TUSA
/* Reset pin */
#define PIN_RESET          	12			/* for reset pin for Blueboard CLEV663B */
#define PORT_RESET			PORT1		/* for reset pin for Blueboard CLEV663B */
#else /* TUSA */
#define PIN_RESET          	4			/* reset pin for Silica TUSA board */
#define PORT_RESET			PORT0		/* for reset pin for Silica TUSA board */
#endif /* TUSA */

/* Reader chip interface selection pins */
#define PIN_IFSEL0         	2	         // select pin to define com. interface type for RC663
#define PIN_IFSEL1         	7	         // select pin to define com. interface type for RC663
#define PORT_IFSEL0			PORT2
#define PORT_IFSEL1			PORT2

/* I2C address configuration pins */
#define PIN_AD0            	17
#define PIN_AD1            	0
#define PORT_AD0			PORT0
#define PORT_AD1			PORT1

/* Interrupt pin and macros */
#define PORT_EXT_IRQ		PORT1
#define PIN_EXT_IRQ			6

#define EXTERNAL_IRQ        PIN_INT0_IRQn
#define PRIORITY_EXT_IRQ	5

#endif /* NXPBUILD__PHHAL_HW_RC663 */

#ifdef NXPBUILD__PHHAL_HW_RC523

/* Reset pin */
#define PIN_RESET          	3      /* for reset pin for Blueboard PNEV512B */
#define PORT_RESET          PORT1      /* for reset pin for Blueboard PNEV512B */

/* I2C address configuration pins */
#define PIN_AD0            	9
#define PIN_AD1            	23
#define PIN_AD2            	14
#define PORT_AD0			PORT1
#define PORT_AD1			PORT0
#define PORT_AD2			PORT0

/* Interrupt pin and macros */
#define PORT_EXT_IRQ		PORT1
#define PIN_EXT_IRQ			4

#define EXTERNAL_IRQ        PIN_INT0_IRQn
#define PRIORITY_EXT_IRQ	5

#endif /* NXPBUILD__PHHAL_HW_RC523 */

#endif /* LPC11U6x */


#ifdef LPC17xx

/* ------------------------------------------------ */
/* general ports definition                         */
/* ------------------------------------------------ */
#define LED_PORT           PORT0    /* Port for LED */

#define LED_BIT            1 << 22      /* Bit on port for led */
#define LED_ON             1            /* Level to set port to turn on led */
#define LED_OFF            0            /* Level to set port to turn off led */

/* ------------------------------------------------ */
/* pins definition                                  */
/* ------------------------------------------------ */
#ifdef NXPBUILD__PHHAL_HW_RC663
#define PIN_AD0            1 << 17
#define PIN_AD1            1 << 15
#define PIN_IFSEL0         1 << 4    /* Select pin to define com. interface */
#define PIN_IFSEL1         1 << 5    /* Select pin to define com. interface */
#define PIN_RESET          1 << 5    /* Reset pin for Blueboard CLEV663B */
#define PIN_SSEL           1 << 10   /* SSEL pin (SSP) for Blueboard CLEV663B */
#endif /* NXPBUILD__PHHAL_HW_RC663 */

#ifdef NXPBUILD__PHHAL_HW_RC523
#define PIN_AD0            1 << 23
#define PIN_AD1            1 << 24
#define PIN_AD2            1 << 25
#define PIN_RESET          1 << 27      /* Reset pin for Blueboard PNEV512B */
#define PIN_SSEL           1 << 6       /* SSEL pin for Blueboard PNEV512B */
#endif /* NXPBUILD__PHHAL_HW_RC523 */

#ifdef NXPBUILD__PHHAL_HW_PN5180
#define PIN_AD0            	(1 << 23)
#define PIN_AD1           	(1 << 24)
#define PIN_AD2            	(1 << 25)
#define PIN_RESET          	(1 << 5)
#define PIN_DWL_REQ        	(1 << 21)
#define PIN_IRQ            	(1 << 12)
#endif /* NXPBUILD__PHHAL_HW_PN5180 */

#ifdef NXPBUILD__PHHAL_HW_RC523
void Check_Pn512IRQ_LPC1769(void);
void appDataInit( void *pHal, void *pOsal, void *pData, void  *pExitLoopback);
#endif /* NXPBUILD__PHHAL_HW_RC523 */

#endif /*LPC17xx*/


#define PORT_RST_DEV       PORT_RESET	// port of GPIO pin where is placed reset pin for reader chip
#define PIN_RST_DEV        PIN_RESET


/*******************************************************************************
 **    Function prototypes
 ******************************************************************************/
extern void Set_Interface_Link(void);
extern void GPIO_Init(void);
extern void Set_Port(void);
extern void Reset_reader_device(void);
extern void LedOn(void);
extern void LedOff(void);

void Set_Interrupt(void);

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
