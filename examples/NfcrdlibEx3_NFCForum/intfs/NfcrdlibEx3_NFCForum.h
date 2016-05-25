/*
*         Copyright (c), NXP Semiconductors Gratkorn / Austria
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
* particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

/** \file
* NFC Type 4A Card Emulation application.
* $Author: Santosh Araballi (ing04223) $
* $Revision: 4047 $
* $Date: 2015-12-15 16:53:08 +0530 (Tue, 15 Dec 2015) $
*/

#ifndef EX3_NFCFORUM_H
#define EX3_NFCFORUM_H

/**
* Reader Library Headers
*/
#include <ph_Status.h>
/* OSAL Headers */
#include <phOsal.h>
/* BAL Headers */
#include <phbalReg.h>
/* PAL Headers */
#include <phpalI14443p3a.h>
#include <phpalI14443p4.h>
#include <phpalFelica.h>
#include <phpalI14443p3b.h>
#include <phpalI14443p4a.h>
#include <phpalMifare.h>
#include <phpalI18092mPI.h>
#include <phpalI18092mT.h>
#include <phpalSli15693.h>
#include <phpalI18000p3m3.h>
#include <phalI18000p3m3.h>
/* AL Headers */
#include <phalT1T.h>
#include <phalTop.h>
#include <phalMful.h>
#include <phalMfdf.h>
#include <phalFelica.h>
/* LLCP and SNEP Layer Headers */
#include <phlnLlcp.h>
#include <phnpSnep.h>
/* AC Layer Header */
#include <phacDiscLoop.h>

/**
 * LPC Headers.
 * */
#ifdef LPC17xx
#undef LPC11U6x
#include <lpc17xx_gpio.h>
#endif/*LPC17xx*/

#ifdef LPC11U6x
#undef LPC17xx
#include <chip.h>
#include <gpio_11u6x.h>
#endif/*LPC11U6x*/

/* Hardware Configuration Header */
#include <phhwConfig.h>
/* Platform header */
#include <phPlatform.h>

/*******************************************************************************
**  Macros
*******************************************************************************/

/* Enable(1) / Disable(0) printing error in detection (discovery loop
 * poll mode) */
#define DETECT_ERROR 0

#if DETECT_ERROR
    #define DEBUG_ERROR_PRINT CHECK_STATUS
    #define PRINT_ERROR(...) DEBUG_PRINTF(__VA_ARGS__)
#else
    #define DEBUG_ERROR_PRINT(x)
    #define PRINT_ERROR(...)
#endif

/* Print system critical errors (with which execution can't proceed further)
 * and halt the execution. */
#define ERROR_CRITICAL(...) DEBUG_PRINTF(__VA_ARGS__); while(1);

/**
 * Initialize reader library.
 * */
phStatus_t NfcRdLibInit(void);

/**
 * Read NDEF message from detected tag.
 * */
phStatus_t ReadNdefMessage(
    uint8_t TopTagType);      /* [in] Tag type from which to read NDEF */

/**
 * Reader library thread. This is the main thread which starts discovery loop
 * in listen mode and poll mode (one at a time) and manages switching between
 * both listen and poll modes.
 * */
void *TReaderLibrary(
    void *pDataParams);        /* [in] Input parameter to thread. */

/**
* SNEP Server Task. This Task registers SNEP Server and waits for  Remote Client
* to PUT SNEP Message.
* Refer phnpSnep.h for more info.
* APIs which does PAL or HAL exchange should not be called from application
* thread.
*/
void *TSNEPServer(
    void * pLlcp             /* [in] Input parameter to thread. */
    );

/**
* SNEP Client Task. This Task registers SNEP Client and waits till Remote Server
* accepts connection and then perform SNEP PUT action.
* Refer phnpSnep.h for more info.
* APIs which does PAL or HAL exchange should not be called from application
* thread.
*/
void *TSNEPClient(
    void * pLlcp             /* [in] Input parameter to thread. */
    );

void *TSnepDispatch(
    void * pLlcp
    );

#endif /* EX3_NFCFORUM_H */
