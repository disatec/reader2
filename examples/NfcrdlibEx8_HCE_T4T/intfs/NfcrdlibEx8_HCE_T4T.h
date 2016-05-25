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
* $Author: jenkins_ cm (nxp92197) $
* NxpNfcRdLib_R_Gate_V4.010.00.001602_ExC $Revision: 4144 $
* $Date: 2016-01-14 22:11:33 +0530 (Thu, 14 Jan 2016) $
*/

#ifndef EX8_HCE_T4T_H
#define EX8_HCE_T4T_H

/**
* Reader Library Headers
*/
#include <ph_Status.h>
/* OSAL Headers */
#include <phOsal.h>
#include <phOsal_Posix_Thread.h>
/* BAL Headers */
#include <phbalReg.h>
/* PAL Headers */
#include <phpalI14443p3a.h>
#include <phpalI14443p4.h>
#include <phpalFelica.h>
#include <phpalI14443p3b.h>
#include <phpalI14443p4a.h>
#include <phpalMifare.h>
#include <phpalI14443p4mC.h>
/* AL Headers */
#include <phalT1T.h>
#include <phalTop.h>
#include <phalMful.h>
#include <phalMfdf.h>
#include <phalFelica.h>
/* CE Layer Header */
#include <phceT4T.h>
/* AC Layer Header */
#include <phacDiscLoop.h>

/**
 * LPC Headers.
 * */
//#ifdef LPC17xx
//#undef LPC11U6x
//#include <lpc17xx_gpio.h>
//#endif/*LPC17xx*/

//#ifdef LPC11U6x
//#undef LPC17xx
//#include <chip.h>
//#include <gpio_11u6x.h>
//#endif/*LPC11U6x*/

/* Hardware Configuration Header */
#include <phhwConfig.h>
/* Platform header */
#include <phPlatform.h>

/**
 * FreeRTOS Headers.
 * */
//#include <FreeRTOS.h>
//#include <task.h>
//#include <semphr.h>

/* Reader library layers required for this example */
#define NXPBUILD_EX8_MANDATORY_LAYERS                               \
            (defined(NXPBUILD__PHHAL_HW_RC523)                      \
            || defined(NXPBUILD__PHHAL_HW_PN5180))					\
            && defined(NXPBUILD__PHBAL_REG_STUB)                    \
            && defined(NXPBUILD__PHHAL_HW)                          \
            && defined(NXPBUILD__PHPAL_I14443P4MC_SW)               \
            && defined(NXPBUILD__PHAC_DISCLOOP_SW)                  \
            && defined(NXPBUILD__PHAC_DISCLOOP_TARGET)              \
            && defined(NXPBUILD__PH_OSAL_POSIX)                  \
            && defined(NXPBUILD__PHCE_T4T_SW)                       \
            && defined(NXPBUILD__PH_PLATFORM)

/* Check if all required reader library layers are enabled. */
#if NXPBUILD_EX8_MANDATORY_LAYERS

/*******************************************************************************
**  Macros
*******************************************************************************/
/* Set this to 1 for running ISO 10373-6 PICC test bench. When set to 1, code
 * for creating WTX (for TEST_COMMAND3) in ISO test bench will be enabled. */
#define ISO_10373_6_PICC_TEST_BENCH 0

/* Use application callback for processing UPDATE BINARY and proprietary
 * commands */
#define USE_APP_CALLBACK 1

/* Get content length from CC file. Initial content length is 0. */
#define GET_CCFILE_CONTENT_LEN(x) ((((uint16_t)(x[0]) << 8) & 0xFF00) | (x[1]))

/* Get content length specified in file (first 2 bytes of file) for NDEF and
 * proprietary files */
#define GET_FILE_CONTENT_LEN(x) ((((((uint16_t)(x[0]) << 8) & 0xFF00) | (x[1])) > 0)?   \
                                 (((((uint16_t)(x[0]) << 8) & 0xFF00) | (x[1])) + 2) : 0)

/* Enable(1) / Disable(0) printing error/info */
#define DETECT_ERROR 0

#if DETECT_ERROR
    #define DEBUG_ERROR_PRINT CHECK_STATUS
    #define PRINT_INFO(...) DEBUG_PRINTF(__VA_ARGS__)
#else /* DETECT_ERROR */
    #define DEBUG_ERROR_PRINT(x)
    #define PRINT_INFO(...)
#endif /* DETECT_ERROR */

/* Print system critical errors (with which execution can't proceed further)
 * and halt the execution. */
#define ERROR_CRITICAL(...) DEBUG_PRINTF(__VA_ARGS__); while(1);

/*******************************************************************************
**  Reader Library Data Parameters
*******************************************************************************/
typedef struct
{
#ifdef NXPBUILD__PHBAL_REG_STUB
    /* BAL data parameter */
    phbalReg_Stub_DataParams_t       sBalReader;
#endif /* NXPBUILD__PHBAL_REG_STUB */

#ifdef NXPBUILD__PHHAL_HW
    /* HAL data parameter */
    phhalHw_Nfc_Ic_DataParams_t      sHal_Nfc_Ic;
#endif /* NXPBUILD__PHHAL_HW */

    /* PAL data parameters */
#ifdef NXPBUILD__PHPAL_I14443P4MC_SW
    /* PAL 14443p4mC */
    phpalI14443p4mC_Sw_DataParams_t  sPalI14443p4mC;
#endif /* NXPBUILD__PHPAL_I14443P4MC_SW */

    /* Application Layer data parameters */
#ifdef NXPBUILD__PHAC_DISCLOOP_SW
    /* DiscLoop data parameter */
    phacDiscLoop_Sw_DataParams_t     sDiscLoop;
#endif /* NXPBUILD__PHAC_DISCLOOP_SW */

#ifdef NXPBUILD__PHCE_T4T_SW
    /* CE data parameter */
    phceT4T_Sw_DataParams_t          sCeT4T;
#endif /* NXPBUILD__PHCE_T4T_SW */

#ifdef NXPBUILD__PH_PLATFORM
    /* OSAL data parameter */
    phPlatform_DataParams_t          sPlatform;
#endif /* NXPBUILD__PH_PLATFORM */
}NxpRdLib_Sw_DataParams_t;

/*******************************************************************************
**  Forward declarations
*******************************************************************************/
/*
 * WTX Callback called from WTX timer of 14443p3mC PAL.
 */
void WtxCallBack(
    void *pContext);          /* [in] Application context; (not used here) */

/**
 * Configure discovery loop parameters.
 * */
phStatus_t ConfigureDiscoveryLoop(void);

/**
 * Initialize reader library.
 * */
phStatus_t NfcRdLibInit(void);

#if USE_APP_CALLBACK
/**
 * Application callback to handle UPDATE BINARY and proprietary commands.
 * Refer phceT4T_AppCallback_t in phceT4T.h for more info.
 * */
phStatus_t AppProcessCmdCallback(
    uint8_t bState,        /* [in] Tag State indicating received command type */
    uint16_t wRxOption,    /* [in] Indicating received data is partial or not */
    uint8_t *pRxData,      /* [in] Received Data */
    uint16_t wRxDataLen,   /* [in] Length of received data */
    uint16_t *pStatusWord, /* [out] Status Word to be sent (part of R-APDU) */
    uint8_t **ppTxData,    /* [out] Data to be transmitted (part of R-APDU) */
    uint16_t *pTxDataLen); /* [out] Length of data to be transmitted */
#endif /* USE_APP_CALLBACK */

/**
 * Application thread. This calls phceT4T_AppProcessCmd() to handle update
 * binary and proprietary commands.
 * Refer phceT4T_AppProcessCmd in phceT4T.h for more info.
 * APIs which does PAL or HAL exchange should not be called from application
 * thread.
 * */
void TApplication(
    void *pParams);        /* [in] Input parameter to thread (not used here) */

/**
 * Do card emulation by starting discovery loop in listen mode and activating
 * ISO 14443p4mC.
 * Refer phceT4T_Activate() in phceT4T.h for info.
 * */
void T4TCardEmulation(void);

/**
 * Reader library thread. This is the main thread which starts discovery loop
 * in listen mode.
 * */
void TReaderLibrary(
    void *pParams);        /* [in] Input parameter to thread (not used here) */

/**
 * Initialize library and hardware.
 * */
void Initialize(void);

#endif /* NXPBUILD_EX8_MANDATORY_LAYERS */

#endif /* EX8_HCE_T4T_H */
