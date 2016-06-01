/*
*         Copyright (c), NXP Semiconductors Bangalore / India
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

/* Local headers */
#include "NfcrdlibEx8_HCE_T4T.h"
#include <phOsal_Posix_Thread.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>


#define LENGHT_BUFFER 32000

char buffer_message[LENGHT_BUFFER];
int lenght_message;
bool bStopThreads = false;
bool bThreadsStarted = false;


/* Check if all required reader library layers are enabled. */
#if NXPBUILD_EX8_MANDATORY_LAYERS

/*******************************************************************************
**  Reader Library Data Parameters
*******************************************************************************/
NxpRdLib_Sw_DataParams_t        NxpRdLib;

/*******************************************************************************
**  Local Variables
*******************************************************************************/
/**
 * HAL parameters
 * */
uint8_t  bHalBufferTx[300]; /* HAL TX buffer */
uint8_t  bHalBufferRx[300]; /* HAL RX buffer */

/**
 * Parameters for L3 activation during AUTOCOLL (used in
 * phhalHw_Rc523_SetListenParameters)
 * */
/* ATQ bytes */
static uint8_t  sens_res[2]     = {0x04, 0x00};
/* UID bytes (one byte defined by HW) */
static uint8_t  nfc_id1[3]      = {0xA1, 0xA2, 0xA3};
static uint8_t  sel_res         = 0x20;   /* SAK (ISO14443P4 Card) */
static uint8_t  nfc_id3         = 0xFA;   /* NFCID3 byte */
static uint8_t  poll_res[18]    = {0x01, 0xFE, 0xB2, 0xB3, 0xB4, 0xB5,
                                   0xB6, 0xB7, 0xC0, 0xC1, 0xC2, 0xC3,
                                   0xC4, 0xC5, 0xC6, 0xC7, 0x23, 0x45};
/**
 * Parameters for PAL 14443p4mC ATS configuration (Activation).
 * Refer phpalI14443p4mC_Activate for more info.
 * Used in listen mode / card emulation.
 * */
uint8_t aAtsRes[5]  = {0x05, 0x77, 0x00, 0x88, 0x00}; /* ATS Response send for
                                                       * RATS: |TL|T0|TA|TB|TC|
                                                       */
uint16_t wAtsLength = 5;                              /* ATS length */

/**
 * Parameters for T4T CE (card emulation layer)
 * */
/* Mandatory Capability Container (CC) file. It's set using
 * phceT4T_SetElementaryFile. */
uint8_t aCcFile[23] = {0};

/* Mandatory NDEF file. It's set using phceT4T_SetElementaryFile. */
//uint8_t aNdefFile[1024] = {0x00, 0x10, 0xD1, 0x01, 0x0C, 0x55, 0x00, 0x77, 0x77,
//                          0x77, 0x2E, 0x6E, 0x78, 0x70, 0x2E, 0x63, 0x6F, 0x6D};

//uint8_t aNdefFile[3000] = {0x00, 0x10, 0xD1, 0x01, 0x0C, 0x55, 0x00, 0x77, 0x77,
//                          0x77, 0x2E, 0x6E, 0x78, 0x70, 0x2E, 0x63, 0x6F, 0x6D};


uint8_t aNdefFile[32800] = {0x02, 0xFF, 0xC1, 0x01, 0x00,0x00,0x02,0xF8, 0x54, 0x05, 'e','s','-','E','S',0x77, 0x77,
                          0x77, 0x2E, 0x6E, 0x78, 0x70, 0x2E, 0x63, 0x6F, 0x6D, 0x6D, 0x6D};

#ifdef NXPBUILD__PHCE_T4T_PROPRIETARY
/* Optional proprietary file (not used in this example. Added to demonstrate
 * support). It's set using phceT4T_SetElementaryFile. */
uint8_t aProprietaryFile[100] = {0};
#endif /* NXPBUILD__PHCE_T4T_PROPRIETARY */

/**
 * Application buffer. Used in phceT4T_Init. Its needed for data exchange
 * between application thread and reader library thread. Refer phceT4T_Init in
 * phceT4T.h for more info.
 * */
uint8_t aAppBuffer[300];

/**
 * RTOS objects.
 * Semaphores for synchronization between application thread and reader library
 * thread.
 * */
phOsal_SemaphoreHandle_t appstart = NULL; /* Used to synchronize start of one
                                             cycle of application processing */
phOsal_SemaphoreHandle_t appexit = NULL;  /* Used to synchronize end of one
                                             cycle of application processing */


phOsal_SemaphoreHandle_t readerEnd = NULL;  /* Used to synchronize end of reader */

phOsal_SemaphoreHandle_t applicationEnd = NULL;  /* Used to synchronize end of application */


#if ISO_10373_6_PICC_TEST_BENCH
phOsal_SemaphoreHandle_t timerwtx = NULL; /* Used in PICC ISO 10373-6 test bench
                                             to create WTX */
#endif /* ISO_10373_6_PICC_TEST_BENCH */

/*******************************************************************************
**  Local function definitions
*******************************************************************************/
/*
 * WTX Callback called from WTX timer of 14443p3mC PAL.
 */
void WtxCallBack(void *pContext)
{
    phOsal_Event_Post(
        E_PH_OSAL_EVT_RTO_TIMEOUT,
        E_PH_OSAL_EVT_SRC_ISR,
        E_PH_OSAL_EVT_DEST_LIB);

#if ISO_10373_6_PICC_TEST_BENCH
    /* Trigger Application callback (AppProcessCmdCallback) about WTX for ISO
     * 10373-6 test bench.*/
    (void)xSemaphoreGiveFromISR(timerwtx, pdFAIL);
#endif /* ISO_10373_6_PICC_TEST_BENCH */
}

#if USE_APP_CALLBACK
/**
 * Application callback to handle UPDATE BINARY and proprietary commands.
 * Refer phceT4T_AppCallback_t in phceT4T.h for more info.
 * */
phStatus_t AppProcessCmdCallback(
                                 uint8_t bState,
                                 uint16_t wRxOption,
                                 uint8_t *pRxData,
                                 uint16_t wRxDataLen,
                                 uint16_t *pStatusWord,
                                 uint8_t **ppTxData,
                                 uint16_t *pTxDataLen
                                 )
{
    phStatus_t status;
    uint16_t wSelectedFileId;
    uint8_t *pSelectedFile;
    uint32_t dwSelectedFileSize;
    uint8_t bWriteAccess;
    uint32_t dwFileOffset;
#if ISO_10373_6_PICC_TEST_BENCH
    /* Command to create WTX while running 10373-6 PICC test bench. */
    uint8_t aWaitForWtx[4] = {0xDE, 0xAD, 0xFE, 0xED};
#endif /* ISO_10373_6_PICC_TEST_BENCH */

    if(bState == PHCE_T4T_STATE_FILE_UPDATE)
    {
        /* Get Selected file info */
        status = phceT4T_GetSelectedFileInfo(
            &NxpRdLib.sCeT4T,
            &wSelectedFileId,
            &pSelectedFile,
            &dwSelectedFileSize,
            &bWriteAccess,
            &dwFileOffset);

        if((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
        {
            /* Update file */
            (void)memcpy(&pSelectedFile[dwFileOffset], pRxData, wRxDataLen);

            /* File update success */
            *pStatusWord = 0x9000;
            *ppTxData = NULL;
            *pTxDataLen = 0;
        }
        /* This condition shall never come */
        else
        {
            DEBUG_PRINTF("Application Callback: Getting file Info failed...\n");
            /* Could not update file */
            *pStatusWord = 0x6A82;
            *ppTxData = NULL;
            *pTxDataLen = 0;

            /* Return same error back */
            return (status & PH_ERR_MASK);
        }
    }
    else
    {
        /* ---Do the proprietary command handling here--- */

#if ISO_10373_6_PICC_TEST_BENCH
        /* Create WTX while running 10373-6 PICC test bench. Waits for WTX
         * timer expire, if received "DEADFEED..." */
        if(memcmp(&pRxData[1], aWaitForWtx, 4) == 0)
        {
            /* Wait for trigger from WTX callback */
            status = phOsal_Semaphore_Take(timerwtx, 0xFFFFFFFF);
            if(status != PH_ERR_SUCCESS)
            {
                DEBUG_PRINTF("Application Callback: Getting WTX Semaphore failed...\n");
                while(1);
            }
        }
#endif /* ISO_10373_6_PICC_TEST_BENCH */

        /* No proprietary support */
        *pStatusWord = 0x6D00;
        *ppTxData = NULL;
        *pTxDataLen = 0;
    }

    return PH_ERR_SUCCESS;
}
#endif /* USE_APP_CALLBACK */

/**
 * Do card emulation by starting discovery loop in listen mode and activating
 * ISO 14443p4mC.
 * This function calls phceT4T_Activate() to do T4T emulation.
 * Refer phceT4T_Activate() in phceT4T.h for more info.
 * */
void T4TCardEmulation(void)
{
    phStatus_t  status = 0;

    /* Start discovery loop in listen mode */
    status = phacDiscLoop_Run(
        &NxpRdLib.sDiscLoop,
        PHAC_DISCLOOP_ENTRY_POINT_LISTEN);

    if((status & PH_ERR_MASK) == PHAC_DISCLOOP_EXTERNAL_RFOFF)
    {
        /* Reset SLEEP_AF state (Respond to WupA ("52")/ReqA ("26")) as Field
         * Off is observed.*/
        status = phhalHw_SetConfig(
            &NxpRdLib.sHal_Nfc_Ic.sHal,
            PHHAL_HW_CONFIG_MFHALTED,
            PH_OFF);
        CHECK_STATUS(status);

#ifdef NXPBUILD__PHHAL_HW_RC523
        /* Generate random UID */
        status = phhalHw_Rc523_Cmd_GenerateRandomId(&NxpRdLib.sHal_Nfc_Ic.sHal);
        DEBUG_ERROR_PRINT(status);
#endif /* NXPBUILD__PHHAL_HW_RC523 */
    }

    if((status & PH_ERR_MASK) == PHAC_DISCLOOP_ACTIVATED_BY_PEER)
    {
        /* Validate RATS and send ATS */
        status = phpalI14443p4mC_Activate(
            &NxpRdLib.sPalI14443p4mC,
            NxpRdLib.sDiscLoop.sTargetParams.pRxBuffer,
            (uint8_t)NxpRdLib.sDiscLoop.sTargetParams.wRxBufferLen,
            aAtsRes,
            wAtsLength);

        if((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
        {
            if(status == (PH_COMP_PAL_I14443P4MC | PH_ERR_PROTOCOL_ERROR))
            {
                /* Invalid RATS: Set to SLEEP_AF state if needed. */
            }
            else if(status == (PH_COMP_PAL_I14443P4MC | PH_ERR_INVALID_PARAMETER))
            {
                /* Invalid ATS */
                printf("ATS configured is invalid...\n");
            }
            else
            {
                DEBUG_ERROR_PRINT(status);
            }
        }
        else
        {
            /* Trigger AppProcessCmd in Application thread to start */
            status = phOsal_Semaphore_Give(appstart);
            if(status != PH_ERR_SUCCESS)
            {
                printf("ReaderLibThread: Releasing Semaphore failed...\n");
            }

            /* Activates HCE. This handles all the main functionalities of
             * card emulation...receive request from reader...process
             * received request...pass UPDATE BINARY/proprietary commands to
             * application thread if enabled...and send the response back
             * to reader. */
            status = phceT4T_Activate(&NxpRdLib.sCeT4T);
            if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS_DESELECTED)
            {
                /* Set SLEEP_AF state (Respond only to WupA ("52"), not to
                 * ReqA ("26")), so set MFHalted bit. After this setting is
                 * done "phhalHw_Autocoll" should be called immediately from
                 * Application so that HAL will know if there is any Field
                 * reset in "phhalHw_Autocoll" API and reset this setting. */
                status = phhalHw_SetConfig(
                    &NxpRdLib.sHal_Nfc_Ic.sHal,
                    PHHAL_HW_CONFIG_MFHALTED,
                    PH_ON);
                CHECK_STATUS(status);
            }

            /* Wait for AppProcessCmd in Application thread to exit */
            status = phOsal_Semaphore_Take(appexit, 0xFFFFFFFF);
            if(status != PH_ERR_SUCCESS)
            {
                printf("ReaderLibThread: Getting Semaphore failed...\n");
            }
        }

        /* Reset Reader library layers */
        status = phpalI14443p4mC_ResetProtocol(&NxpRdLib.sPalI14443p4mC);
        DEBUG_ERROR_PRINT(status);

        status = phceT4T_Reset(&NxpRdLib.sCeT4T);
        DEBUG_ERROR_PRINT(status);
    }

    /* Clear all events */
    status = phOsal_Event_Consume(
        E_PH_OSAL_EVT_TYPE_ALL,
        E_PH_OSAL_EVT_SRC_APP);
    DEBUG_ERROR_PRINT(status);
}

/**
 * Application thread. This calls phceT4T_AppProcessCmd() to handle update
 * binary and proprietary commands.
 * Refer phceT4T_AppProcessCmd in phceT4T.h for more info.
 * APIs which does PAL or HAL exchange should not be called from application
 * thread.
 * */
void TApplication(
                  void *pParams
                  )
{
    phStatus_t  status;

    printf("Application thread started...\n");

    while(!bStopThreads)
    {
        /* Wait for trigger from reader library thread to start */
        status = phOsal_Semaphore_Take(appstart, 0xFFFFFFFF);
        if (bStopThreads)
        {
            phOsal_Semaphore_Give(applicationEnd);
            return;
            
        }    
        
        if(status != PH_ERR_SUCCESS)
        {
            printf("AppThread: Getting Semaphore failed...\n");
        }

        /* Start Application processing of HCE */
#if USE_APP_CALLBACK
        /* Use application callback to handle UPDATE_BINARY and proprietary
         * commands. */
        status = phceT4T_AppProcessCmd(
            &NxpRdLib.sCeT4T,
            AppProcessCmdCallback);
#else /* USE_APP_CALLBACK */
        /* Application callback not used. Proprietary commands are not handled
         * (unsupported command is returned as status) and UPDATE BINARY will be
         * handled internally by AppProcessCmd. */
        status = phceT4T_AppProcessCmd(
            &NxpRdLib.sCeT4T,
            NULL);
#endif /* USE_APP_CALLBACK */

        /* Trigger reader library thread about the phceT4T_AppProcessCmd exit */
        status = phOsal_Semaphore_Give(appexit);
        if(status != PH_ERR_SUCCESS)
        {
            printf("AppThread: Releasing Semaphore failed...\n");
        }
    }
}

/**
 * Reader library thread. This is the main thread which starts discovery loop
 * in listen mode and poll mode (one at a time) and manages switching between
 * both listen and poll modes.
 * */
void TReaderLibrary(
                    void *pParams
                    )
{
    phStatus_t  status;

    printf("Reader Library thread started...\n");
	
    /* Enable interrupt */
    Set_Interrupt();

#ifdef NXPBUILD__PHHAL_HW_RC523
    /* Set Listen Parameters mode */
    status = phhalHw_Rc523_SetListenParameters(
        &NxpRdLib.sHal_Nfc_Ic.sHal,
        &sens_res[0],
        &nfc_id1[0],
        sel_res,
        &poll_res[0],
        nfc_id3);
    CHECK_STATUS(status);
#endif /* NXPBUILD__PHHAL_HW_RC523 */

#ifdef NXPBUILD__PHHAL_HW_PN5180
    /* Set Listen Parameters mode */
    status = phhalHw_Pn5180_SetListenParameters(
        &NxpRdLib.sHal_Nfc_Ic.sHal,
        &sens_res[0],
        &nfc_id1[0],
        sel_res,
        &poll_res[0],
        nfc_id3);
    CHECK_STATUS(status);
#endif /* NXPBUILD__PHHAL_HW_PN5180 */

    /* Be in card emulation mode */
    while(!bStopThreads)
    {
        		/* T4T Card Emulation mode */
        T4TCardEmulation();
    }
    
    Exit_Interrupt();
    Cleanup_Interrupt();
    phOsal_Semaphore_Give(readerEnd);
}

/**
 * Configure 14443p4mC and ceT4T parameters.
 * */
phStatus_t ConfigureCardEmulation(void)
{
    phStatus_t status = PH_ERR_SUCCESS;

    /* Set CC File */
    status = phceT4T_SetElementaryFile(
        &NxpRdLib.sCeT4T,
        PHCE_T4T_FILE_CC,
        aCcFile,
        0xE103,
        sizeof(aCcFile),
        GET_CCFILE_CONTENT_LEN(aCcFile));
    CHECK_SUCCESS(status);

    int i= 0;
    int languageLength = aNdefFile[9]; 
    for (;i<lenght_message;i++)
        aNdefFile[i+10+languageLength]=buffer_message[i];
    
    int lengthmessage = i+languageLength+1;
    aNdefFile [6] = (lengthmessage & 0xFF00) >> 8;
    aNdefFile [7] = (lengthmessage & 0xFF);
    
    int lenghttotal = i + languageLength + 8;
    aNdefFile [0] = (lenghttotal & 0xFF00) >> 8;
    aNdefFile [1] = (lenghttotal & 0xFF);
    
    
    printf("longitud mensaje %i\n", sizeof(aNdefFile));
    printf("long en el mensaje%i\n", GET_FILE_CONTENT_LEN(aNdefFile));

    /* Set NDEF File */
    status = phceT4T_SetElementaryFile(
        &NxpRdLib.sCeT4T,
        PHCE_T4T_FILE_NDEF,
        aNdefFile,
        0xE104,
        sizeof(aNdefFile),
        GET_FILE_CONTENT_LEN(aNdefFile));
    CHECK_SUCCESS(status);

#ifdef NXPBUILD__PHCE_T4T_PROPRIETARY
    /* Set Proprietary File */
    status = phceT4T_SetElementaryFile(
        &NxpRdLib.sCeT4T,
        PHCE_T4T_FILE_PROPRIETARY,
        aProprietaryFile,
        0xE105,
        sizeof(aProprietaryFile),
        GET_FILE_CONTENT_LEN(aProprietaryFile));
    CHECK_SUCCESS(status);
#endif /* NXPBUILD__PHCE_T4T_PROPRIETARY */

#if USE_APP_CALLBACK
    /* Enable support for proprietary command handling by application
     * callback. */
    status = phceT4T_SetConfig(
        &NxpRdLib.sCeT4T,
        PHCE_T4T_CONFIG_RECEIVE_PROPRIETARY,
        PH_ON);
    CHECK_SUCCESS(status);
#endif /* USE_APP_CALLBACK */

    /* Configure MLe (This is optional; default MLe is 255) */
    status = phceT4T_SetConfig(
        &NxpRdLib.sCeT4T,
        PHCE_T4T_CONFIG_MLE,
        250);
    CHECK_SUCCESS(status);

    /* Configure MLc (This is optional; default MLc is 255) */
    status = phceT4T_SetConfig(
        &NxpRdLib.sCeT4T,
        PHCE_T4T_CONFIG_MLC,
        250);
    CHECK_SUCCESS(status);

    /* Configure WTX time (WTX will be triggered after this much % of FWT) */
    status = phpalI14443p4mC_SetConfig(
        &NxpRdLib.sPalI14443p4mC,
        PHPAL_I14443P4MC_CONFIG_WT_PERCENTAGE,
        40);
    CHECK_SUCCESS(status);

    return PH_ERR_SUCCESS;
}

/**
 * Configure discovery loop parameters.
 * */
phStatus_t ConfigureDiscoveryLoop(void)
{
    phStatus_t status = PH_ERR_SUCCESS;

    /* Enable passive Listen. */
    status = phacDiscLoop_SetConfig(
        &NxpRdLib.sDiscLoop,
        PHAC_DISCLOOP_CONFIG_PAS_LIS_TECH_CFG,
        0x01);
    CHECK_SUCCESS(status);

    /* Turn OFF active listen. */
    status = phacDiscLoop_SetConfig(
        &NxpRdLib.sDiscLoop,
        PHAC_DISCLOOP_CONFIG_ACT_LIS_TECH_CFG,
        0x00);
    CHECK_SUCCESS(status);

    return PH_ERR_SUCCESS;
}

/*
 * Initialize all required layers of Reader Library
 */
phStatus_t NfcRdLibInit(void)
{
    phStatus_t  status;

#ifdef NXPBUILD__PHBAL_REG_STUB
    /* Initialize the Reader BAL (Bus Abstraction Layer) component */
    status = phbalReg_Stub_Init(
        &NxpRdLib.sBalReader,
        sizeof(phbalReg_Stub_DataParams_t));
    CHECK_SUCCESS(status);
#endif /* NXPBUILD__PHBAL_REG_STUB */

#ifdef NXPBUILD__PH_PLATFORM
    /* Initialize the OSAL Stub component */
    status = phPlatform_Init(&NxpRdLib.sPlatform);
    CHECK_SUCCESS(status);
#endif /* NXPBUILD__PH_PLATFORM */

#ifdef NXPBUILD__PH_PLATFORM
    /* Initialize the timer component */
    status = phPlatform_Timer_Init(&NxpRdLib.sPlatform);
    CHECK_SUCCESS(status);
#endif /* NXPBUILD__PH_PLATFORM */

#ifdef NXPBUILD__PHHAL_HW_PN5180
    status = phbalReg_SetConfig(
        &NxpRdLib.sBalReader,
        PHBAL_REG_CONFIG_HAL_HW_TYPE,
        PHBAL_REG_HAL_HW_PN5180);
#endif
#ifdef NXPBUILD__PHHAL_HW_RC523
    status = phbalReg_SetConfig(
        &NxpRdLib.sBalReader,
        PHBAL_REG_CONFIG_HAL_HW_TYPE,
        PHBAL_REG_HAL_HW_RC523);
#endif

    status = phbalReg_SetPort(
        &NxpRdLib.sBalReader,
        SPI_CONFIG);
    CHECK_STATUS(status);

    /* Open BAL */
    status = phbalReg_OpenPort(&NxpRdLib.sBalReader);
    CHECK_SUCCESS(status);

#ifdef NXPBUILD__PHHAL_HW
    /* Initialize the Reader HAL (Hardware Abstraction Layer) component */
    status = phhalHw_Nfc_IC_Init(
        &NxpRdLib.sHal_Nfc_Ic,
        sizeof(phhalHw_Nfc_Ic_DataParams_t),
        &NxpRdLib.sBalReader,
        0,
        bHalBufferTx,
        sizeof(bHalBufferTx),
        bHalBufferRx,
        sizeof(bHalBufferRx));
    CHECK_SUCCESS(status);

    /* Set the parameter to use the SPI interface */
    NxpRdLib.sHal_Nfc_Ic.sHal.bBalConnectionType = PHHAL_HW_BAL_CONNECTION_SPI;
#endif /* NXPBUILD__PHHAL_HW */

#ifdef NXPBUILD__PHPAL_I14443P4MC_SW
    /* Initialize 14443p4mC PAL component */
    status = phpalI14443p4mC_Sw_Init(
        &NxpRdLib.sPalI14443p4mC,
        sizeof(phpalI14443p4mC_Sw_DataParams_t),
        &NxpRdLib.sHal_Nfc_Ic.sHal,
        WtxCallBack);
    CHECK_SUCCESS(status);

    /* Set OSAL pointer in PAL */
    NxpRdLib.sPalI14443p4mC.pPlatform = &NxpRdLib.sPlatform;
#endif /* NXPBUILD__PHPAL_I14443P4MC_SW */

#ifdef NXPBUILD__PHAC_DISCLOOP_SW
    /* Initialize the Discovery Loop component */
    status = phacDiscLoop_Sw_Init(
        &NxpRdLib.sDiscLoop,
        sizeof(phacDiscLoop_Sw_DataParams_t),
        &NxpRdLib.sHal_Nfc_Ic.sHal);
    CHECK_SUCCESS(status);
#endif /* NXPBUILD__PHAC_DISCLOOP_SW */

#ifdef NXPBUILD__PHCE_T4T_SW
    /* Initialize CE T4T */
    status = phceT4T_Sw_Init(
        &NxpRdLib.sCeT4T,
        sizeof(phceT4T_Sw_DataParams_t),
        &NxpRdLib.sPalI14443p4mC,
        aAppBuffer,
        sizeof(aAppBuffer));
    CHECK_SUCCESS(status);
#endif /* NXPBUILD__PHCE_T4T_SW */

    return PH_ERR_SUCCESS;
}

/*
 * Initialize Hardware, Library and RTOS resources.
 */
void Initialize(void)
{
    phStatus_t status;

    /* Set the interface link */
    Set_Interface_Link();
    
    /* Perform a hardware reset */
    Reset_reader_device();

    /* Initialize OSAL event */
    status = phOsal_Event_Init();
    if((status & PH_ERR_MASK) != 0x00)
    {
        printf("OSAL event creation failed...\n");
    }

    /* Initialize library */
    status = NfcRdLibInit();
    if((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
    {
        printf("Reader library initialization failed...\n");
    }
    
    Configure_Device(&NxpRdLib.sHal_Nfc_Ic);

    /* Configure Discovery loop parameters */
    status = ConfigureDiscoveryLoop();
    if((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
    {
        printf("Configuring discovery loop failed...\n");
    }

    /* Configure Card Emulation parameters */
    status = ConfigureCardEmulation();
    if((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
    {
        printf("Configuring card emulation failed...\n");
    }

    /* Create semaphore for sync. between threads */
    appexit = phOsal_Semaphore_Create(1, 0);
    if(appexit == NULL)
    {
        printf("Semaphore creation failed...\n");
    }

    /* Create semaphore for sync. between threads */
    appstart = phOsal_Semaphore_Create(1, 0);
    if(appstart == NULL)
    {
        printf("Semaphore creation failed...\n");
    }

    /* Create semaphore for sync. between threads */
    applicationEnd = phOsal_Semaphore_Create(1, 0);
    if(applicationEnd == NULL)
    {
        printf("Semaphore creation failed...\n");
    }

    /* Create semaphore for sync. between threads */
    readerEnd = phOsal_Semaphore_Create(1, 0);
    if(readerEnd == NULL)
    {
        printf("Semaphore creation failed...\n");
    }

    

#if ISO_10373_6_PICC_TEST_BENCH
    /* Create semaphore for testing WTX */
    timerwtx = phOsal_Semaphore_Create(1, 0);
    if(timerwtx == NULL)
    {
        DEBUG_PRINTF("Semaphore creation failed...\n");
        while(1);
    }
#endif /* ISO_10373_6_PICC_TEST_BENCH */
}

#endif /* NXPBUILD_EX8_MANDATORY_LAYERS */


/* Creates a datagram server.  The port 
   number is passed as an argument.  This
   server runs forever */

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>


void error(const char *msg)
{
    perror(msg);
    exit(0);
}


// Start listening and card Emulation
void StartThreads()
{
    int ret, *retval1, *retval2;
    
    ret = phOsal_Posix_Thread_Create(E_PH_OSAL_EVT_DEST_LIB, (void* (*)(void *))TReaderLibrary, NULL);
	if(ret)
	{
		printf("Application thread creation failed...Error: %ld\n", ret);
		return;
	}

	ret = phOsal_Posix_Thread_Create(E_PH_OSAL_EVT_DEST_APP, (void* (*)(void *))TApplication, NULL);
	if(ret)
	{
		printf("Application thread creation failed...Error: %ld\n", ret);
		return;
	}
	
	phOsal_Posix_Thread_Join(E_PH_OSAL_EVT_DEST_LIB, (void *) retval1);
    //phOsal_Posix_Thread_Join(E_PH_OSAL_EVT_DEST_APP, (void *) retval2);
        bThreadsStarted = true;

}

void StopThreads()
{
    if (!bThreadsStarted)
        return;
    int * retval2;
    bStopThreads = true;
    phOsal_Semaphore_Give(appstart);


    phStatus_t status = phOsal_Semaphore_Take(applicationEnd, 0xFFFFFFFF);
    if(status != PH_ERR_SUCCESS)
    {
        printf("AppThread: Getting Semaphore failed...\n");
    }
    
    status = phOsal_Semaphore_Take(readerEnd, 0xFFFFFFFF);
    if(status != PH_ERR_SUCCESS)
    {
        printf("AppThread: Getting Semaphore failed...\n");
    }

    //phOsal_Posix_Thread_Join(E_PH_OSAL_EVT_DEST_APP, (void *) retval2);
    bStopThreads = false;
    bThreadsStarted = false;

}



// Returns bytes returned by read
int readsocket(char* buffer, int lenbuffer)
{
   int sockfd, newsockfd, portno, clilen;
   //char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;
   
   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 5001;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
      
   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   Initialize();
   
   while(1)
   {

        /* Accept actual connection from the client */
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

        if (newsockfd < 0) {
           perror("ERROR on accept");
           exit(1);
        }


        /* If connection is established then start communicating */
        bzero(buffer,lenbuffer);
        while ((n = read( newsockfd,buffer,lenbuffer )) > 0)
        {
   
            StopThreads();
            if (n < 0) {
               perror("ERROR reading from socket");
               exit(1);
             }

            printf("Here is the message: %s\n",buffer);

            /* Write a response to the client */
            /*n = write(newsockfd,"I got your message",18);

            if (n < 0) {
               perror("ERROR writing to socket");
               exit(1);
            }*/

            lenght_message = n;
            Initialize();

            StartThreads();
        }
        close(newsockfd);
   }
    return n;
}

/**
 * Read socket thread. 
 * */
void TReadSocket(
                    void *pParams
                    )
{
    while (1)
    {
        lenght_message = readsocket(buffer_message, LENGHT_BUFFER);    
        Initialize();
    }
}

/*
 * Main Function
 */
int main(int argc, char * argv[])
{
#if NXPBUILD_EX8_MANDATORY_LAYERS
    int ret, *retval1, *retval2;

	DEBUG_PRINTF("\nCard Emulation Type4 Application...\n");

    if (argc==1)
    {
        TReadSocket(NULL);
        //ret = phOsal_Posix_Thread_Create(E_PH_OSAL_EVT_DEST_APP2, (void* (*)(void *))TReadSocket, NULL);
    if (ret)
    {
		printf("Application thread creation failed...Error: %ld\n", ret);
		return -1;
        
    }
        
    }
    else
    {
        if (strcmp(argv[0],"-c"))
        {
            int nroChars = atoi(argv[2]);
            for (int i=0;i<nroChars;i++)
            {
                buffer_message[i] = 'C';
            }
            buffer_message[nroChars] = 0;
            lenght_message = nroChars;
        }

        Initialize();

        StartThreads();

    }
        
        /* Initialize platform (HW and library) */


#else /* NXPBUILD_EX8_MANDATORY_LAYERS */
    DEBUG_PRINTF("\n\tERROR: Missing required reader library components in build...");
#endif /* NXPBUILD_EX8_MANDATORY_LAYERS */

    return 0;
}






/*******************************************************************************
**                            End Of File
*******************************************************************************/
