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
* Example Source for NfcrdlibEx3_NFCForum.c that uses the Discovery loop for polling and performs Tag Operations and SNEP PUT operation.
* Discovery loop is configured for both POLL and LISTEN modes. This example displays detected tag information(like UID, SAK, Product Type)
* and prints information when it detects NFC Forum Tags and gets activated as a target by an external initiator/reader.
* Whenever multiple technologies are detected, example will select last detected technology to resolve.
* Example will activate device at last index whenever multiple devices are detected.
*
* If a P2P device is detected then this example performs SNEP PUT operation if both SNEP client and server are enabled.
* By default SNEP PUT of URI message (www.nxp.com) will be performed.
*
* If remote P2P device wants to perform SNEP PUT that should be received by this example then SNEP client should be disabled, in this case
* phone acts as client and on screen (Touch to Beam) pops up, when user taps the screen then SNEP PUT will be done by the phone and this
* application will receive this SNEP PUT message and displays on console only in Debug mode (Maximum SNEP PUT
* message that can be received is 1024Bytes as per default SNEP server specification).
*
* Please refer Readme.txt file for hardware pin configuration, software configuration and steps to build and
* execute the project. This file is present in the same project directory.
* $Author: Santosh Araballi (ing04223) $
* $Revision: 4001 $
* $Date: 2015-12-05 18:08:38 +0530 (Sat, 05 Dec 2015) $
*
*/

/**
* Header for hardware configuration: bus interface, reset of attached reader ID, on-board LED handling etc.
* */
#include <phhwConfig.h>
#include <cards.h>

#include <phOsal_Posix_Thread.h>

/* Local headers */
#include "NfcrdlibEx3_NFCForum.h"

#define LISTEN_PHASE_TIME_MS        300     /**< Listen Phase TIME */

/* Enable Discovery Loop Active mode. */
#define ACTIVE_MODE

/* Enable Discovery Loop Passive mode. */
#define PASSIVE_MODE

/* Enable initiator/reader mode polling configuration in Discovery Loop. */
#define INITIATOR_MODE

/* Enable listen mode configuration (target/card mode) in Discovery Loop. */
#define TARGET_MODE

/* Enable SNEP client PUT operation in this example. */
#define SNEP_CLIENT

/* Enable SNEP server to receive a SNEP PUT message in this example. */
#define SNEP_SERVER

/* Un-comment following line to enable SNEP PUT of 1K text message else SNEP PUT of URI (www.nxp.com) will be enabled by default.  */
/* #define NDEF_TEXT */

/* To be un-commented if proper de-activation in active mode is required. Only needed when
 * both initiator and target are engineering boards. Phones do not care about proper
 * ISO 18092 MAC de-activation */
/* #define TEST_ACTIVE_BOARDS */

/*******************************************************************************
**   Global Variable Declaration
*******************************************************************************/
phbalReg_Stub_DataParams_t         sBalReader;                 /**< BAL component holder */

/*HAL variables*/
phhalHw_Nfc_Ic_DataParams_t        sHal_Nfc_Ic;                /* HAL component holder for Nfc Ic's */
uint8_t                            bHalBufferTx[260];          /* HAL TX buffer. Size 260 - Based on maximum FSL */
uint8_t                            bHalBufferRx[260];          /* HAL RX buffer. Size 260 - Based on maximum FSL */
void                              *pHal;                       /* HAL pointer */

/*PAL variables*/
phpalI14443p3a_Sw_DataParams_t     spalI14443p3a;              /* PAL ISO I14443-A component */
phpalI14443p4a_Sw_DataParams_t     spalI14443p4a;              /* PAL ISO I14443-4A component */
phpalI14443p3b_Sw_DataParams_t     spalI14443p3b;              /* PAL ISO I14443-B component */
phpalI14443p4_Sw_DataParams_t      spalI14443p4;               /* PAL ISO I14443-4 component */
phpalMifare_Sw_DataParams_t        spalMifare;                 /* PAL Mifare component */
phpalFelica_Sw_DataParams_t        spalFelica;                 /* PAL Felica component */

/*
 * Following variables under #ifdef guards to enable only initiator or target
 * functionality if needed.
 */
#ifdef NXPBUILD__PHPAL_I18092MPI
phpalI18092mPI_Sw_DataParams_t     spalI18092mPI;              /* PAL ISO 18092 Initiator component */
#endif /* NXPBUILD__PHPAL_I18092MPI */

#ifdef NXPBUILD__PHPAL_I18092MT
phpalI18092mT_Sw_DataParams_t      spalI18092mT;               /* PAL MT component */
#endif /* NXPBUILD__PHPAL_I18092MT */

/*
 * Pn512/Rc523 does not support the following protocols.
 */
#ifndef NXPBUILD__PHHAL_HW_RC523
phpalSli15693_Sw_DataParams_t      spalSli15693;               /* PAL ISO 15693 component */
phalI18000p3m3_Sw_DataParams_t     salI18000p3m3;              /* AL ISO 18000p3m3 component */
phpalI18000p3m3_Sw_DataParams_t    spalI18000p3m3;             /* PAL ISO 18000p3m3 component */
#endif /* NXPBUILD__PHHAL_HW_RC523 */

/* LLCP/SNEP variables */
phlnLlcp_Sw_DataParams_t           slnLlcp;                    /* LLCP component holder */
phnpSnep_Sw_DataParams_t           snpSnepServer;              /* SNEP component holder */
phnpSnep_Sw_DataParams_t           snpSnepClient;              /* SNEP component holder */

/*Platform variables*/
phPlatform_DataParams_t            sPlatform;                  /* OSAL component holder */

/*DiscLoop variables*/
phacDiscLoop_Sw_DataParams_t       sDiscLoop;                  /* Discovery loop component */

/*Application Layer variables*/
phalT1T_Sw_DataParams_t            salT1T;                      /* AL T1T component */
phalMful_Sw_DataParams_t           salMful;                     /* AL MIFARE UltraLite/T2T */
phalFelica_Sw_DataParams_t         salFelica;                   /* AL T3T component */
phalMfdf_Sw_DataParams_t           salMfdf;                     /* AL MIFARE DESFIRE/T4T */

/* Tag Operation variables */
phalTop_Sw_DataParams_t            sTagop;                      /* AL Tag operations */
phalTop_T1T_t                      sT1Tparam;                   /* T1T parameter of Tag operation */
phalTop_T2T_t                      sT2Tparam;                   /* T2T parameter of Tag operation */
phalTop_T3T_t                      sT3Tparam;                   /* T3T parameter of Tag operation */
phalTop_T4T_t                      sT4Tparam;                   /* T4T parameter of Tag operation */

/**
* Buffer used to store NDEF data (poll mode).
* */
uint8_t    aData[500];

/* SNEP Server socket and buffers. */
phlnLlcp_Transport_Socket_t ServerSocket;
uint8_t                     bServerRxBuffer[260];

/*
 * SNEP Server application buffer to store received PUT Message.
 * Max SNEP PUT message length that can be accepted will be defined by below array length.
 *  */
uint8_t                     baSnepAppBuffer[1033];
uint32_t                    baSnepRxLen = 0;

/* SNEP Client socket and buffers. */
phlnLlcp_Transport_Socket_t ClientSocket;
uint8_t                     bClientRxBuffer[260];
uint32_t                    dwClientRxBuffLength = sizeof(bClientRxBuffer);


/* Array allocated to store LLCP General Bytes. */
uint8_t   aLLCPGeneralBytes[36];
uint8_t   bLLCPGBLength = 0;

/* ATR response holder */
uint8_t  aAtrRes[64];
uint16_t wAtrResLength;

/* DID is not used by default. */
uint8_t   bDid = 0;
uint8_t   bBst = 0x00;
uint8_t   bBrt = 0x00;
/* Set TO (time out) value in ATR_RES which will be used to set FDT/FWT on P2P initiator. */
uint8_t   bTo = 8;
/* Set LRT value to 3. AS LLCP mandates that LRT value to be 3. */
uint8_t   bLrt = 3;

/*
 * Pn512 has its own way of configuring listen parameters.
 */
#ifdef NXPBUILD__PHHAL_HW_RC523
/*
* Target mode not supported not CLRC663
*/
/* Parameters for L3 activation during Autocoll */
static uint8_t  sens_res[2]     = {0x04, 0x00};              /* ATQ bytes - needed for anti-collision */
static uint8_t  nfc_id1[3]      = {0xA1, 0xA2, 0xA3};        /* user defined bytes of the UID (one is hardcoded) - needed for anti-collision */
static uint8_t  sel_res         = 0x40;                      /* SAK (ISO18092mT) - needed for anti-collision */
static uint8_t  nfc_id3         = 0xFA;                      /* NFC3 byte - required for anti-collision */
static uint8_t  poll_res[18]    = {0x01, 0xFE, 0xB2, 0xB3, 0xB4, 0xB5,
                                   0xB6, 0xB7, 0xC0, 0xC1, 0xC2, 0xC3,
                                   0xC4, 0xC5, 0xC6, 0xC7, 0x23, 0x45 }; /* felica - needed for anti-collision */
#endif /* NXPBUILD__PHHAL_HW_RC523 */

#ifdef NXPBUILD__PHHAL_HW_PN5180
/* Parameters for L3 activation during Autocoll */
static uint8_t  sens_res[2]     = {0x04, 0x00};              /* ATQ bytes - needed for anti-collision */
static uint8_t  nfc_id1[3]      = {0xA1, 0xA2, 0xA3};        /* user defined bytes of the UID (one is hardcoded) - needed for anti-collision */
static uint8_t  sel_res         = 0x40;                      /* SAK (ISO18092mT) - needed for anti-collision */
static uint8_t  nfc_id3         = 0xFA;                      /* NFC3 byte - required for anti-collision */

static uint8_t  poll_res[18]    = {0x01, 0xFE, 0xB2, 0xB3, 0xB4, 0xB5,
                                   0xB6, 0xB7, 0xC0, 0xC1, 0xC2, 0xC3,
                                   0xC4, 0xC5, 0xC6, 0xC7, 0x23, 0x45 };
#endif
/* This is used to save and restore poll configuration if in case application
 * has updated/changed polling configuration to resolve a particular technology
 * when multiple technologies were detected
*/
static uint16_t bSavePollTechCfg  = 0;


#ifdef NDEF_TEXT
/* SNEP PUT text message used in this example. */
static const uint8_t baSnepAppBuf[] = {
    0xC1,                   /* NDEF Record Structure */
    0x01,                   /* TYPE LENGTH */
    0x00, 0x00, 0x04, 0x01, /* PAYLOAD LENGTH */
    0x54,                   /* TYPE TEXT */
    0x02,                   /* ID UTF8 */
    0x65, 0x6E,             /* LOCALE EN */
    'N','X','P',' ','S','e','m','i','c','o','n','d','u','c','t','o','r','s',' ','c','r','e','a','t','e','s',' ','s','o','l'
    ,'u','t','i','o','n','s',' ','t','h','a','t',' ','e','n','a','b','l','e',' ','s','e','c','u','r','e',' ','c','o','n','n'
    ,'e','c','t','i','o','n','s',' ','f','o','r',' ','a',' ','s','m','a','r','t','e','r',' ','w','o','r','l','d','.',' ','B'
    ,'u','i','l','d','i','n','g',' ','o','n',' ','i','t','s',' ','e','x','p','e','r','t','i','s','e',' ','i','n',' ','H','i'
    ,'g','h',' ','P','e','r','f','o','r','m','a','n','c','e',' ','M','i','x','e','d',' ','S','i','g','n','a','l',' ','e','l'
    ,'e','c','t','r','o','n','i','c','s',',',' ','N','X','P',' ','i','s',' ','d','r','i','v','i','n','g',' ','i','n','n','o'
    ,'v','a','t','i','o','n',' ','i','n',' ','t','h','e',' ','a','u','t','o','m','o','t','i','v','e',',',' ','i','d','e','n'
    ,'t','i','f','i','c','a','t','i','o','n',' ','a','n','d',' ','m','o','b','i','l','e',' ','i','n','d','u','s','t','r','i'
    ,'e','s',',',' ','a','n','d',' ','i','n',' ','a','p','p','l','i','c','a','t','i','o','n',' ','a','r','e','a','s',' ','i'
    ,'n','c','l','u','d','i','n','g',' ','w','i','r','e','l','e','s','s',' ','i','n','f','r','a','s','t','r','u','c','t','u'
    ,'r','e',',',' ','l','i','g','h','t','i','n','g',',',' ','h','e','a','l','t','h','c','a','r','e',',',' ','i','n','d','u'
    ,'s','t','r','i','a','l',',',' ','c','o','n','s','u','m','e','r',' ','t','e','c','h',' ','a','n','d',' ','c','o','m','p'
    ,'u','t','i','n','g','.',' ','F','o','r',' ','a','p','p','l','i','c','a','t','i','o','n','s',' ','d','e','m','a','n','d'
    ,'i','n','g',' ','t','h','e',' ','h','i','g','h','e','s','t',' ','s','e','c','u','r','i','t','y',' ','a','n','d',' ','r'
    ,'e','l','i','a','b','i','l','i','t','y',' ','(','e','.','g','.',' ','s','m','a','r','t',' ','c','a','r','d','s',',',' '
    ,'s','e','c','u','r','e',' ','d','o','n','g','l','e','s',',',' ','S','A','M',' ','m','o','d','u','l','e','s','.','.','.'
    ,')',' ','w','e',' ','p','r','o','v','i','d','e',' ','d','i','f','f','e','r','e','n','t',' ','p','l','a','t','f','o','r'
    ,'m','s',' ','a','c','c','o','r','d','i','n','g',' ','t','o',' ','a','p','p','l','i','c','a','t','i','o','n',' ','n','e'
    ,'e','d','s',' ','r','a','n','g','i','n','g',' ','f','r','o','m',' ','o','u','r',' ','w','e','l','l',' ','e','s','t','a'
    ,'b','l','i','s','h','e','d',' ','W','E','-','c','o','n','t','a','c','t',' ','c','o','n','t','r','o','l','l','e','r',' '
    ,'f','a','m','i','l','y',' ','t','o',' ','o','u','r',' ','S','m','a','r','t','M','X',' ','a','n','d',' ','S','m','a','r'
    ,'t','X','A',' ','d','e','v','i','c','e','s','.',' ','O','u','r',' ','M','I','F','A','R','E',' ','c','o','n','t','a','c'
    ,'t','l','e','s','s',' ','s','m','a','r','t',' ','c','a','r','d',' ','I','C','s',' ','p','r','o','v','i','d','e',' ','t'
    ,'h','e',' ','m','o','s','t',' ','a','d','v','a','n','c','e','d',' ','c','o','m','b','i','n','a','t','i','o','n',' ','o'
    ,'f',' ','s','e','c','u','r','i','t','y',' ','a','n','d',' ','c','o','n','v','e','n','i','e','n','c','e',' ','f','o','r'
    ,' ','c','o','n','t','a','c','t','l','e','s','s',' ','i','n','t','e','r','f','a','c','i','n','g',' ','i','n',' ','a','c'
    ,'c','o','r','d','a','n','c','e',' ','w','i','t','h',' ','t','h','e',' ','I','S','O',' ','1','4','4','4','3','A',' ','i'
    ,'n','t','e','r','f','a','c','e',' ','s','t','a','n','d','a','r','d','.',' ','O','u','r',' ','I','C','O','D','E',' ','a'
    ,'n','d',' ','U','C','O','D','E',' ','f','a','m','i','l','i','e','s',' ','o','f',' ','R','F','I','D',' ','I','C','s',' '
    ,'a','r','e',' ','s','p','e','c','i','f','i','c','a','l','l','y',' ','d','e','s','i','g','n','e','d',' ','f','o','r',' '
    ,'h','i','g','h','-','v','o','l','u','m','e',' ','s','m','a','r','t',' ','l','a','b','e','l',' ','a','p','p','l','i','c'
    ,'a','t','i','o','n','s','.',' ','I','n',' ','a','d','d','i','t','i','o','n',' ','o','u','r',' ','H','I','T','A','G',' '
    ,'f','a','m','i','l','y',' ','o','f',' ','I','C','s',',',' ','w','i','t','h',' ','a','n',' ','o','p','e','r','a','t','i'
    ,'n','g',' ','r','a','n','g','e',' ','o','f',' ','m','o','r','e',' ','t','h','a','n',' ','o','n','e',' ','m','e','t','e'
    ,'r','.'
};

#else
/* Short SNEP PUT URI message used in this example. */
static const uint8_t baSnepAppBuf[] = {
    0xC1,                   /* NDEF record structure */
    0x01,                   /* TYPE LENGTH */
    0x00, 0x00, 0x00, 0x08, /* PAYLOAD LENGTH */
    0x55,                   /* TYPE TEXT */
    0x01,                   /* ID UTF8 */
    'n','x','p','.','c','o','m'
};
#endif /* NDEF_TEXT */

uint32_t   dwDataLen = sizeof(baSnepAppBuf);

/*
 * Internal variables used in this application.
 */
uint8_t bSNEPServer;
uint8_t bSNEPClient;
#define SNEP_PUT               0x01
#define SNEP_RECEIVED_PUT      0x80

/* Semaphore used in this example application to synchronize tasks to print the received SNEP message by SNEP server. */
phOsal_SemaphoreHandle_t           xTaskSema;

/*******************************************************************************
**   Function Declarations
*******************************************************************************/

phStatus_t NfcRdLibInit(void);

/**
* This function will print buffer content
* \param   *pBuff   Buffer Reference
* \param   num      data size to be print
*/
static void PRINT_BUFF(uint8_t *pBuff, uint8_t num)
{
    uint32_t    i;

    for(i = 0; i < num; i++)
    {
        DEBUG_PRINTF(" %02X",pBuff[i]);
    }
}

/**
* Reads NDEF Message if the detected tag has a valid NDEF message
*/
phStatus_t ReadNdefMessage(
                           uint8_t TopTagType
                           )
{
    phStatus_t status;
    uint8_t bTagState;
    uint16_t wDataLength = 0;

    /* Configure Top layer for specified tag type */
    status = phalTop_SetConfig(&sTagop, PHAL_TOP_CONFIG_TAG_TYPE, TopTagType);
    DEBUG_ERROR_PRINT(status);

    /* Check for NDEF presence */
    status = phalTop_CheckNdef(&sTagop, &bTagState);
    DEBUG_ERROR_PRINT(status);

    if((bTagState == PHAL_TOP_STATE_READONLY) || (bTagState == PHAL_TOP_STATE_READWRITE))
    {
        /* Read NDEF message */
        status = phalTop_ReadNdef(&sTagop, aData, &wDataLength);
        DEBUG_ERROR_PRINT(status);

        /* Print NDEF message, if not NULL NDEF */
        if(wDataLength)
        {
            DEBUG_PRINTF("\tNDEF detected...\n");
            DEBUG_PRINTF("\tNDEF length: %d\n", wDataLength);
            DEBUG_PRINTF("\tNDEF message:\n");
            //DumpBuffer(aData, wDataLength);
            DumpBuffer(aData, 50);
        }
        else
        {
            DEBUG_PRINTF("\tNDEF content is NULL...\n");
        }
    }
    else
    {
        DEBUG_PRINTF("\tNo NDEF content detected...\n");
    }

    return status;
}

/**
* This function will retrieve and print tag information like UID, NFC type etc..
* \param   pDataParams      The discovery loop data parameters
* \param   wNumberOfTags    Total number of tags detected
* \param   wTagsDetected    Technology detected
*/
static void GetTagInfo(phacDiscLoop_Sw_DataParams_t  *pDataParams, uint16_t wNumberOfTags, uint16_t wTagsDetected)
{
    uint8_t bIndex;
    uint8_t bTagType;
    phStatus_t status = 0;

    /* Required if DETECT_ERROR is not set. DETECT_ERROR is required for debugging purpose only */
    PH_UNUSED_VARIABLE(status);

    /* Check for Type A tag */
    if (PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_POS_BIT_MASK_A))
    {
        /* Check for T1T */
        if(pDataParams->sTypeATargetInfo.bT1TFlag)
        {
            DEBUG_PRINTF("\tType A : T1T detected \n");

            /* Check for NDEF and Read */
            status = ReadNdefMessage(PHAL_TOP_TAG_TYPE_T1T_TAG);
            if (status != PH_ERR_SUCCESS)
            {
                DEBUG_ERROR_PRINT(status);
            }
        }
        else
        {
            DEBUG_PRINTF("\tTechnology : Type A");
            /* Loop through all the detected tags (if multiple tags are
            * detected) */
            for(bIndex = 0; bIndex < wNumberOfTags; bIndex++)
            {
                DEBUG_PRINTF ("\n\t\tCard : %d",bIndex + 1);
                DEBUG_PRINTF ("\n\t\tUID  :");
                PRINT_BUFF( pDataParams->sTypeATargetInfo.aTypeA_I3P3[bIndex].aUid,
                    pDataParams->sTypeATargetInfo.aTypeA_I3P3[bIndex].bUidSize);
                DEBUG_PRINTF ("\n\t\tSAK: 0x%x",pDataParams->sTypeATargetInfo.aTypeA_I3P3[bIndex].aSak);

                if ((pDataParams->sTypeATargetInfo.aTypeA_I3P3[bIndex].aSak & (uint8_t) ~0xFB) == 0)
                {
                    /* Bit b3 is set to zero, [Digital] 4.8.2 */
                    /* Mask out all other bits except for b7 and b6 */
                    bTagType = (pDataParams->sTypeATargetInfo.aTypeA_I3P3[bIndex].aSak & 0x60);
                    bTagType = bTagType >> 5;

                    /* Switch to tag type */
                    switch(bTagType)
                    {
                    case PHAC_DISCLOOP_TYPEA_TYPE2_TAG_CONFIG_MASK:
                        DEBUG_PRINTF ("\n\t\tType : Type 2 tag\n");

                        if (!DetectClassic(
                            pDataParams->sTypeATargetInfo.aTypeA_I3P3[bIndex].aAtqa,
                            &pDataParams->sTypeATargetInfo.aTypeA_I3P3[bIndex].aSak))
                        {
                            /* Check for NDEF and Read NDEF if mifare Ultralite card is detected. */
                            status = ReadNdefMessage(PHAL_TOP_TAG_TYPE_T2T_TAG);
                            DEBUG_ERROR_PRINT(status);
                        }
                        break;

                    case PHAC_DISCLOOP_TYPEA_TYPE4A_TAG_CONFIG_MASK:
                        DEBUG_PRINTF ("\n\t\tType : Type 4A tag\n");

                        /* Check for NDEF and Read NDEF */
                        status = ReadNdefMessage(PHAL_TOP_TAG_TYPE_T4T_TAG);
                        DEBUG_ERROR_PRINT(status);
                        break;

                    case PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TAG_CONFIG_MASK:
                        DEBUG_PRINTF ("\n\t\tType : P2P\n");
                        break;

                    case PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TYPE4A_TAG_CONFIG_MASK:
                        DEBUG_PRINTF ("\n\t\tType : Type NFC_DEP and 4A tag\n");
                        break;

                    default:
                        break;
                    }
                }
            }
        }
    }

    /* Check for Type B tag */
    if (PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_POS_BIT_MASK_B))
    {
        DEBUG_PRINTF("\tTechnology: Type B");
        /* Loop through all the Type B tags detected and print the PUPI */
        for (bIndex = 0; bIndex < wNumberOfTags; bIndex++)
        {
            DEBUG_PRINTF ("\n\t\tCard : %d",bIndex + 1);
            DEBUG_PRINTF ("\n\t\tUID  :");
            /* PUPI Length is always 4 bytes */
            PRINT_BUFF( pDataParams->sTypeBTargetInfo.aTypeB_I3P3[bIndex].aPupi, 0x04);
        }
        DEBUG_PRINTF("\n");
    }

    /* Check for Type F tag */
    if( PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_POS_BIT_MASK_F212) ||
        PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_POS_BIT_MASK_F424))
    {
        DEBUG_PRINTF("\tTechnology: Type F");

        /* Loop through all the type F tags and print the IDm */
        for (bIndex = 0; bIndex < wNumberOfTags; bIndex++)
        {
            DEBUG_PRINTF ("\n\t\tCard : %d",bIndex + 1);
            DEBUG_PRINTF ("\n\t\tUID  :");
            PRINT_BUFF( pDataParams->sTypeFTargetInfo.aTypeFTag[bIndex].aIDmPMm,
                PHAC_DISCLOOP_FELICA_IDM_LENGTH );
            /* Check data rate  */
            if(pDataParams->sTypeFTargetInfo.aTypeFTag[bIndex].bBaud != PHAC_DISCLOOP_CON_BITR_212)
            {
                DEBUG_PRINTF ("\n\t\tBit Rate: 424 kbps");
            }
            else
            {
                DEBUG_PRINTF ("\n\t\tBit Rate: 212 kbps");
            }
            if ((pDataParams->sTypeFTargetInfo.aTypeFTag[bIndex].aIDmPMm[0] == 0x01) &&
                (pDataParams->sTypeFTargetInfo.aTypeFTag[bIndex].aIDmPMm[1] == 0xFE))
            {
                /* This is type F tag with P2P capabilities */
                DEBUG_PRINTF ("\n\t\tType : P2P\n");
            }
            else
            {
                /* This is Type F T3T tag */
                DEBUG_PRINTF ("\n\t\tType : Type 3 tag\n");

                /* Check for NDEF and read NDEF */
                status = ReadNdefMessage(PHAL_TOP_TAG_TYPE_T3T_TAG);
                DEBUG_ERROR_PRINT(status);
            }
        }
    }
#ifndef NXPBUILD__PHHAL_HW_RC523
    if (PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_POS_BIT_MASK_V))
    {
        DEBUG_PRINTF("\tTechnology  : Type V / ISO 15693 / T5T");
        /* Loop through all the Type V tags detected and print the UIDs */
        for (bIndex = 0; bIndex < wNumberOfTags; bIndex++)
        {
            DEBUG_PRINTF ("\n\t\tCard: %d",bIndex + 1);
            DEBUG_PRINTF ("\n\t\tUID :");
            PRINT_BUFF( pDataParams->sTypeVTargetInfo.aTypeV[bIndex].aUid, 0x08);
        }
        DEBUG_PRINTF("\n");
    }

    if (PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_POS_BIT_MASK_18000P3M3))
    {
        DEBUG_PRINTF("\tTechnology  : ISO 18000p3m3 / EPC Gen2");
        /* Loop through all the 18000p3m3 tags detected and print the UII */
        for (bIndex = 0; bIndex < wNumberOfTags; bIndex++)
        {
            DEBUG_PRINTF("\n\t\tCard: %d",bIndex + 1);
            DEBUG_PRINTF("\n\t\tUII :");
            PRINT_BUFF(
                pDataParams->sI18000p3m3TargetInfo.aI18000p3m3[bIndex].aUii,
                (pDataParams->sI18000p3m3TargetInfo.aI18000p3m3[bIndex].wUiiLength / 8));
        }
        DEBUG_PRINTF("\n");
    }
#endif /* NXPBUILD__PHHAL_HW_RC523 */
}

/**
* This function will print error information received from Reader Library
* \param   wStatus      Error status
*/
static void PrintErrorInfo(phStatus_t wStatus)
{
    PRINT_ERROR("\t Error Info Comp:");

    switch(wStatus & 0xFF00)
    {
    case PH_COMP_BAL:
        PRINT_ERROR("\t PH_COMP_BAL");
        break;
    case PH_COMP_HAL:
        PRINT_ERROR("\t PH_COMP_HAL");
        break;
    case PH_COMP_PAL_ISO14443P3A:
        PRINT_ERROR("\t PH_COMP_PAL_ISO14443P3A");
        break;
    case PH_COMP_PAL_ISO14443P3B:
        PRINT_ERROR("\t PH_COMP_PAL_ISO14443P3B");
        break;
    case PH_COMP_PAL_ISO14443P4A:
        PRINT_ERROR("\t PH_COMP_PAL_ISO14443P4A");
        break;
    case PH_COMP_PAL_ISO14443P4:
        PRINT_ERROR("\t PH_COMP_PAL_ISO14443P4");
        break;
    case PH_COMP_PAL_FELICA:
        PRINT_ERROR("\t PH_COMP_PAL_FELICA");
        break;
    case PH_COMP_PAL_EPCUID:
        PRINT_ERROR("\t PH_COMP_PAL_EPCUID");
        break;
    case PH_COMP_PAL_SLI15693:
        PRINT_ERROR("\t PH_COMP_PAL_SLI15693");
        break;
    case PH_COMP_PAL_I18000P3M3:
        PRINT_ERROR("\t PH_COMP_PAL_I18000P3M3");
        break;
    case PH_COMP_PAL_I18092MPI:
        PRINT_ERROR("\t PH_COMP_PAL_I18092MPI");
        break;
    case PH_COMP_PAL_I18092MT:
        PRINT_ERROR("\t PH_COMP_PAL_I18092MT");
        break;
    case PH_COMP_PAL_I14443P4MC:
        PRINT_ERROR("\t PH_COMP_PAL_I14443P4MC");
        break;
    case PH_COMP_AC_DISCLOOP:
        PRINT_ERROR("\t PH_COMP_AC_DISCLOOP");
        break;
    case PH_COMP_OSAL:
        PRINT_ERROR("\t PH_COMP_OSAL");
        break;
    case PH_COMP_LN_LLCP:
        PRINT_ERROR("\t PH_COMP_LN_LLCP");
        break;
    case PH_COMP_NP_SNEP:
        PRINT_ERROR("\t PH_COMP_NP_SNEP");
        break;
    default:
        PRINT_ERROR("\t 0x%x",(wStatus & PH_COMP_MASK));
        break;
    }

    PRINT_ERROR("\t type:");

    switch(wStatus & PH_ERR_MASK)
    {
    case PH_ERR_SUCCESS_INCOMPLETE_BYTE:
        PRINT_ERROR("\t PH_ERR_SUCCESS_INCOMPLETE_BYTE");
        break;
    case PH_ERR_IO_TIMEOUT:
        PRINT_ERROR("\t PH_ERR_IO_TIMEOUT");
        break;
    case PH_ERR_INTEGRITY_ERROR:
        PRINT_ERROR("\t PH_ERR_INTEGRITY_ERROR");
        break;
    case PH_ERR_COLLISION_ERROR:
        PRINT_ERROR("\t PH_ERR_COLLISION_ERROR");
        break;
    case PH_ERR_BUFFER_OVERFLOW:
        PRINT_ERROR("\t PH_ERR_BUFFER_OVERFLOW");
        break;
    case PH_ERR_FRAMING_ERROR:
        PRINT_ERROR("\t PH_ERR_FRAMING_ERROR");
        break;
    case PH_ERR_PROTOCOL_ERROR:
        PRINT_ERROR("\t PH_ERR_PROTOCOL_ERROR");
        break;
    case PH_ERR_RF_ERROR:
        PRINT_ERROR("\t PH_ERR_RF_ERROR");
        break;
    case PH_ERR_EXT_RF_ERROR:
        PRINT_ERROR("\t PH_ERR_EXT_RF_ERROR");
        break;
    case PH_ERR_NOISE_ERROR:
        PRINT_ERROR("\t PH_ERR_NOISE_ERROR");
        break;
    case PH_ERR_ABORTED:
        PRINT_ERROR("\t PH_ERR_ABORTED");
        break;
    case PH_ERR_INTERNAL_ERROR:
        PRINT_ERROR("\t PH_ERR_INTERNAL_ERROR");
        break;
    case PH_ERR_INVALID_DATA_PARAMS:
        PRINT_ERROR("\t PH_ERR_INVALID_DATA_PARAMS");
        break;
    case PH_ERR_INVALID_PARAMETER:
        PRINT_ERROR("\t PH_ERR_INVALID_PARAMETER");
        break;
    case PH_ERR_PARAMETER_OVERFLOW:
        PRINT_ERROR("\t PH_ERR_PARAMETER_OVERFLOW");
        break;
    case PH_ERR_UNSUPPORTED_PARAMETER:
        PRINT_ERROR("\t PH_ERR_UNSUPPORTED_PARAMETER");
        break;
    case PH_ERR_OSAL_ERROR:
        PRINT_ERROR("\t PH_ERR_OSAL_ERROR");
        break;
    case PHAC_DISCLOOP_LPCD_NO_TECH_DETECTED:
        PRINT_ERROR("\t PHAC_DISCLOOP_LPCD_NO_TECH_DETECTED");
        break;
    case PHAC_DISCLOOP_COLLISION_PENDING:
        PRINT_ERROR("\t PHAC_DISCLOOP_COLLISION_PENDING");
        break;
    default:
        PRINT_ERROR("\t 0x%x",(wStatus & PH_ERR_MASK));
        break;
    }
    PRINT_ERROR("\n");
}

#ifdef NXPBUILD__PHHAL_HW_PN5180
/* Configure LPCD (for PN5180) */
phStatus_t ConfigureLPCD(void)
{
    /**
     * PHHAL_HW_CONFIG_SET_LPCD_WAKEUPTIME_MS  0x0070U     //< Used value for wakeup counter in msecs, i.e. after this amount of time IC will wakes up from standby.
     * PHHAL_HW_CONFIG_LPCD_MODE               0x0071U     //< Used to set options  PHHAL_HW_PN5180_LPCD_MODE_DEFAULT or PHHAL_HW_PN5180_LPCD_MODE_POWERDOWN_GUARDED
     * PHHAL_HW_CONFIG_LPCD_REF                0x0072U     //< Used to set or get LPCD Ref
     */
        phStatus_t status;
        uint16_t wConfig = PHHAL_HW_CONFIG_LPCD_REF;
        uint16_t wValue;

        status = phhalHw_Pn5180_Int_LPCD_GetConfig(pHal, wConfig, &wValue);
        CHECK_STATUS(status);

        wValue = PHHAL_HW_PN5180_LPCD_MODE_POWERDOWN;
        wConfig = PHHAL_HW_CONFIG_LPCD_MODE;

        status = phhalHw_Pn5180_Int_LPCD_SetConfig(
            pHal,
            wConfig,
            wValue
    );

        status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ENABLE_LPCD, PH_ON);
        CHECK_STATUS(status);

        return status;
}
#endif

/**
* This function will load/configure Discovery loop with default values based on interested profiles.
* Application can read these values from EEPROM area and load/configure Discovery loop via SetConfig
* \param   bProfile      Reader Library Profile
* \note    Values used below are default and is for demonstration purpose.
*/
static phStatus_t LoadDiscoveryConfiguration()
{
    phStatus_t status = PH_ERR_SUCCESS;

    /* Passive Bailout bitmap configuration. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_BAIL_OUT, 0x00);
    CHECK_STATUS(status);

    /* Set LRI value for Type-A polling to 3. LLCP mandates that LRI value to be 3. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_TYPEA_P2P_LRI, 3);
    CHECK_STATUS(status);

    /* Set LRI value for Type-F polling to 3. LLCP mandates that LRI value to be 3. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_TYPEF_P2P_LRI, 3);
    CHECK_STATUS(status);

#ifdef INITIATOR_MODE

#ifdef PASSIVE_MODE

#ifdef NXPBUILD__PHHAL_HW_RC523
    /* Passive poll bitmap configuration. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_PAS_POLL_TECH_CFG, (PHAC_DISCLOOP_POS_BIT_MASK_A |
        PHAC_DISCLOOP_POS_BIT_MASK_B | PHAC_DISCLOOP_POS_BIT_MASK_F212 | PHAC_DISCLOOP_POS_BIT_MASK_F424));
    CHECK_STATUS(status);

#else
    /* Passive poll bitmap configuration. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_PAS_POLL_TECH_CFG, (PHAC_DISCLOOP_POS_BIT_MASK_A  |
        PHAC_DISCLOOP_POS_BIT_MASK_B | PHAC_DISCLOOP_POS_BIT_MASK_F212 | PHAC_DISCLOOP_POS_BIT_MASK_F424 |
        PHAC_DISCLOOP_POS_BIT_MASK_V | PHAC_DISCLOOP_POS_BIT_MASK_18000P3M3));
    CHECK_STATUS(status);

#endif

    /* Passive CON_DEVICE limit for Type A. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_TYPEA_DEVICE_LIMIT, 1);
    CHECK_STATUS(status);

    /* Passive CON_DEVICE limit for Type F. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_TYPEF_DEVICE_LIMIT, 1);
    CHECK_STATUS(status);

#else

    /* Passive poll bitmap configuration */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_PAS_POLL_TECH_CFG, 0x00);
    CHECK_STATUS(status);

#endif /* PASSIVE_MODE */

#ifdef ACTIVE_MODE

    /* Active poll bitmap configuration. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ACT_POLL_TECH_CFG,
        (PHAC_DISCLOOP_ACT_POS_BIT_MASK_106 | PHAC_DISCLOOP_ACT_POS_BIT_MASK_212 | PHAC_DISCLOOP_ACT_POS_BIT_MASK_424));
    CHECK_STATUS(status);

#else

    /* Active poll bitmap configuration. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ACT_POLL_TECH_CFG, 0x00);
    CHECK_STATUS(status);

#endif /* ACTIVE_MODE */

#else

    /* Passive poll bitmap configuration. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_PAS_POLL_TECH_CFG, 0x00);
    CHECK_STATUS(status);

    /* Active poll bitmap configuration. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ACT_POLL_TECH_CFG, 0x00);
    CHECK_STATUS(status);

#endif /* INITIATOR_MODE */

#ifdef TARGET_MODE

#ifdef PASSIVE_MODE

    /* Passive listen bitmap configuration. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_PAS_LIS_TECH_CFG,
        (PHAC_DISCLOOP_POS_BIT_MASK_A | PHAC_DISCLOOP_POS_BIT_MASK_F212 | PHAC_DISCLOOP_POS_BIT_MASK_F424));
    CHECK_STATUS(status);

#else

    /* Passive listen bitmap configuration. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_PAS_LIS_TECH_CFG, 0x00);
    CHECK_STATUS(status);

#endif /* PASSIVE_MODE */

#ifdef ACTIVE_MODE

    /* Active listen bitmap configuration. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ACT_LIS_TECH_CFG,
        (PHAC_DISCLOOP_POS_BIT_MASK_A | PHAC_DISCLOOP_POS_BIT_MASK_F212 | PHAC_DISCLOOP_POS_BIT_MASK_F424));
    CHECK_STATUS(status);

#else

    /* Active listen bitmap configuration. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ACT_LIS_TECH_CFG, 0x00);
    CHECK_STATUS(status);

#endif /* ACTIVE_MODE */


#else

    /* Passive listen bitmap configuration. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_PAS_LIS_TECH_CFG, 0x00);
    CHECK_STATUS(status);

    /* Active listen bitmap configuration. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ACT_LIS_TECH_CFG, 0x00);
    CHECK_STATUS(status);

#endif /* TARGET_MODE */

    /* Disable LPCD feature. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ENABLE_LPCD, PH_OFF);
    CHECK_STATUS(status);

    /* Reset collision pending */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_COLLISION_PENDING, PH_OFF);
    CHECK_STATUS(status);

    /* Set anti-collision is supported. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ANTI_COLL, PH_ON);
    CHECK_STATUS(status);

    /* Set Discovery loop mode to NFC mode. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_OPE_MODE, RD_LIB_MODE_NFC);
    CHECK_STATUS(status);

    return status;
}

/**
* This function will initialize reader library components
*/
phStatus_t NfcRdLibInit(void)
{
    phStatus_t  status;

    /* Initialize the Reader BAL (Bus Abstraction Layer) component */
    status = phbalReg_Stub_Init(&sBalReader, sizeof(phbalReg_Stub_DataParams_t));
    CHECK_STATUS(status);

    /* Initialize the Platform component */
    status = phPlatform_Init(&sPlatform);
    CHECK_SUCCESS(status);

    /* Initialize the OSAL Events. */
    status = phOsal_Event_Init();
    CHECK_STATUS(status);

	//Start interrupt thread
    Set_Interrupt();

    /* Set HAL type in BAL */
#ifdef NXPBUILD__PHHAL_HW_PN5180
    status = phbalReg_SetConfig(
        &sBalReader,
        PHBAL_REG_CONFIG_HAL_HW_TYPE,
        PHBAL_REG_HAL_HW_PN5180);
#endif
#ifdef NXPBUILD__PHHAL_HW_RC523
    status = phbalReg_SetConfig(
        &sBalReader,
        PHBAL_REG_CONFIG_HAL_HW_TYPE,
        PHBAL_REG_HAL_HW_RC523);
#endif
#ifdef NXPBUILD__PHHAL_HW_RC663
    status = phbalReg_SetConfig(
        &sBalReader,
        PHBAL_REG_CONFIG_HAL_HW_TYPE,
        PHBAL_REG_HAL_HW_RC663);
#endif
    CHECK_STATUS(status);

#ifdef NXPBUILD__PHHAL_HW_PN5180
    //DG: Added, specific to Linux
    status = phbalReg_SetPort(
        &sBalReader,
        "0:0:25"); //PN5180 is connected to SPI bus 0, CS pin 0, busy pin GPIO 25
#endif
#ifdef NXPBUILD__PHHAL_HW_RC523
    //DG: Added, specific to Linux
    status = phbalReg_SetPort(
        &sBalReader,
        "0:0:0"); //PN512 is connected to SPI bus 0, CS pin 0, no busy pin
#endif
#ifdef NXPBUILD__PHHAL_HW_RC663
    //DG: Added, specific to Linux
    status = phbalReg_SetPort(
        &sBalReader,
        "0:0:0"); //RC663 is connected to SPI bus 0, CS pin 0, no busy pin
#endif
    CHECK_STATUS(status);
	
    /* Open BAL */
    status = phbalReg_OpenPort(&sBalReader);
    CHECK_STATUS(status);

    /* Initialize the Reader HAL (Hardware Abstraction Layer) component */
    status = phhalHw_Nfc_IC_Init(
        &sHal_Nfc_Ic,
        sizeof(phhalHw_Nfc_Ic_DataParams_t),
        &sBalReader,
        0,
        bHalBufferTx,
        sizeof(bHalBufferTx),
        bHalBufferRx,
        sizeof(bHalBufferRx)
        );

    /* Set the parameter to use the SPI interface */
    sHal_Nfc_Ic.sHal.bBalConnectionType = PHHAL_HW_BAL_CONNECTION_SPI;

    Configure_Device(&sHal_Nfc_Ic);

    /* Set the generic pointer */
    pHal = &sHal_Nfc_Ic.sHal;

    /* Initialize the I14443-A PAL component */
    status = phpalI14443p3a_Sw_Init(&spalI14443p3a, sizeof(phpalI14443p3a_Sw_DataParams_t), &sHal_Nfc_Ic.sHal);
    CHECK_STATUS(status);

    /* Initialize the I14443-A PAL component */
    status = phpalI14443p4a_Sw_Init(&spalI14443p4a, sizeof(phpalI14443p4a_Sw_DataParams_t), &sHal_Nfc_Ic.sHal);
    CHECK_STATUS(status);

    /* Initialize the I14443-4 PAL component */
    status = phpalI14443p4_Sw_Init(&spalI14443p4, sizeof(phpalI14443p4_Sw_DataParams_t), &sHal_Nfc_Ic.sHal);
    CHECK_STATUS(status);

    /* Initialize the I14443-B PAL  component */
    status = phpalI14443p3b_Sw_Init(&spalI14443p3b, sizeof(spalI14443p3b), &sHal_Nfc_Ic.sHal);
    CHECK_STATUS(status);

    /* Initialize the Felica PAL component */
    status = phpalFelica_Sw_Init(&spalFelica, sizeof(phpalFelica_Sw_DataParams_t), &sHal_Nfc_Ic.sHal);
    CHECK_STATUS(status);

    /* Initialize the Mifare PAL component */
    status = phpalMifare_Sw_Init(&spalMifare, sizeof(phpalMifare_Sw_DataParams_t), &sHal_Nfc_Ic.sHal, &spalI14443p4);
    CHECK_STATUS(status);

#ifndef NXPBUILD__PHHAL_HW_RC523
    /* Initialize the 1800p3m3 PAL component */
    status = phpalI18000p3m3_Sw_Init(&spalI18000p3m3, sizeof(phpalI18000p3m3_Sw_DataParams_t), pHal);
    CHECK_STATUS(status);

    /* Initialize the 18000p3m3 AL component */
    status = phalI18000p3m3_Sw_Init(&salI18000p3m3, sizeof(phalI18000p3m3_Sw_DataParams_t), &spalI18000p3m3);
    CHECK_STATUS(status);

    /* Initialize the 15693 PAL component */
    status = phpalSli15693_Sw_Init(&spalSli15693, sizeof(phpalSli15693_Sw_DataParams_t), pHal);
    CHECK_STATUS(status);
#endif

    /* Initialize the 18092 initiator PAL component */
    status = phpalI18092mPI_Sw_Init(&spalI18092mPI, sizeof(phpalI18092mPI_Sw_DataParams_t), pHal);
    CHECK_STATUS(status);

    /* Initialize the 18092 target PAL component */
    status = phpalI18092mT_Sw_Init(&spalI18092mT, sizeof(phpalI18092mT_Sw_DataParams_t), pHal, NULL);
    CHECK_SUCCESS(status);
    spalI18092mT.pPlatform = &sPlatform;

    /* Initialize the Felica AL component */
    status = phalFelica_Sw_Init(&salFelica, sizeof(phalFelica_Sw_DataParams_t), &spalFelica);
    CHECK_STATUS(status);

    /* Initialize the T1T AL component */
    status = phalT1T_Sw_Init(&salT1T, sizeof(phalT1T_Sw_DataParams_t), &spalI14443p3a);
    CHECK_STATUS(status);

    /* Initialize the Mful AL component */
    status = phalMful_Sw_Init(&salMful, sizeof(phalMful_Sw_DataParams_t), &spalMifare, NULL, NULL, NULL);
    CHECK_STATUS(status);

    /* Initialize the MF DesFire EV1 component */
    status = phalMfdf_Sw_Init(&salMfdf, sizeof(phalMfdf_Sw_DataParams_t), &spalMifare, NULL, NULL, NULL, &sHal_Nfc_Ic.sHal);
    CHECK_STATUS(status);

    /* Initialize the Top component */
    status = phalTop_Sw_Init(&sTagop, sizeof(phalTop_Sw_DataParams_t), &sT1Tparam, &sT2Tparam, &sT3Tparam, &sT4Tparam, NULL);
    CHECK_STATUS(status);

    ((phalTop_T1T_t *)(sTagop.pT1T))->pAlT1TDataParams = &salT1T;
    ((phalTop_T2T_t *)(sTagop.pT2T))->pAlT2TDataParams = &salMful;
    ((phalTop_T3T_t *)(sTagop.pT3T))->pAlT3TDataParams = &salFelica;
    ((phalTop_T4T_t *)(sTagop.pT4T))->pAlT4TDataParams = &salMfdf;

    /* Initialize the discover loop component */
    status = phacDiscLoop_Sw_Init(&sDiscLoop, sizeof(phacDiscLoop_Sw_DataParams_t), &sHal_Nfc_Ic.sHal);
    CHECK_STATUS(status);

#ifdef NXPBUILD__PHHAL_HW_RC523
    /* Set listen parameters in HAL buffer used during Autocoll */
    status = phhalHw_Rc523_SetListenParameters(&sHal_Nfc_Ic.sHal, &sens_res[0], &nfc_id1[0], sel_res, &poll_res[0], nfc_id3);
    CHECK_SUCCESS(status);
#endif

#ifdef NXPBUILD__PHHAL_HW_PN5180
    /* Set listen parameters in HAL buffer used during Autocoll */
    status = phhalHw_Pn5180_SetListenParameters(&sHal_Nfc_Ic.sHal, &sens_res[0], &nfc_id1[0], sel_res, &poll_res[0], nfc_id3);
    CHECK_SUCCESS(status);
#endif

    sDiscLoop.pPal1443p3aDataParams  = &spalI14443p3a;
    sDiscLoop.pPal1443p3bDataParams  = &spalI14443p3b;
    sDiscLoop.pPal1443p4aDataParams  = &spalI14443p4a;
    sDiscLoop.pPal14443p4DataParams  = &spalI14443p4;
#ifndef NXPBUILD__PHHAL_HW_RC523
    sDiscLoop.pPal18000p3m3DataParams= &spalI18000p3m3;
    sDiscLoop.pAl18000p3m3DataParams = &salI18000p3m3;
    sDiscLoop.pPalSli15693DataParams = &spalSli15693;
#endif /* NXPBUILD__PHHAL_HW_RC523 */
    sDiscLoop.pPal18092mPIDataParams = &spalI18092mPI;
    sDiscLoop.pPalFelicaDataParams   = &spalFelica;
    sDiscLoop.pAlT1TDataParams       = &salT1T;
    sDiscLoop.pHalDataParams         = &sHal_Nfc_Ic.sHal;

    slnLlcp.sLocalLMParams.wMiu = 0x00; /* 128 bytes only */
    slnLlcp.sLocalLMParams.wWks = 0x11; /* SNEP & LLCP */
    slnLlcp.sLocalLMParams.bLto = 100; /* Maximum LTO */
    slnLlcp.sLocalLMParams.bOpt = 0x02;
    slnLlcp.sLocalLMParams.bAvailableTlv = PHLN_LLCP_TLV_MIUX_MASK | PHLN_LLCP_TLV_WKS_MASK |
        PHLN_LLCP_TLV_LTO_MASK | PHLN_LLCP_TLV_OPT_MASK;

    /* Initialize LLCP component */
    status = phlnLlcp_Sw_Init(&slnLlcp, sizeof(phlnLlcp_Sw_DataParams_t), aLLCPGeneralBytes, &bLLCPGBLength);
    CHECK_STATUS(status);

    /* Assign the GI for Type A */
    sDiscLoop.sTypeATargetInfo.sTypeA_P2P.pGi                        = (uint8_t *)aLLCPGeneralBytes;
    sDiscLoop.sTypeATargetInfo.sTypeA_P2P.bGiLength                  = bLLCPGBLength;

    /* Assign the GI for Type F */
    sDiscLoop.sTypeFTargetInfo.sTypeF_P2P.pGi                        = (uint8_t *)aLLCPGeneralBytes;
    sDiscLoop.sTypeFTargetInfo.sTypeF_P2P.bGiLength                  = bLLCPGBLength;

    /* Assign ATR response for Type A */
    sDiscLoop.sTypeATargetInfo.sTypeA_P2P.pAtrRes                    = aAtrRes;

    /* Assign ATR response for Type F */
    sDiscLoop.sTypeFTargetInfo.sTypeF_P2P.pAtrRes                    = aAtrRes;

    /* Assign ATS buffer for Type A */
    sDiscLoop.sTypeATargetInfo.sTypeA_I3P4.pAts                      = aAtrRes;

    /* Configure discover loop based on the enabled macros i.e. ACTIVE_MODE, PASSIVE_MODE, INITIATOR_MODE, TARGET_MODE. */
    LoadDiscoveryConfiguration();

    /* Set max retry count of 1 in PAL 18092 Initiator to allow only one MAC recovery cycle. */
    status = phpalI18092mPI_SetConfig(&spalI18092mPI, PHPAL_I18092MPI_CONFIG_MAXRETRYCOUNT, 0x01);
    CHECK_STATUS(status);

    return PH_ERR_SUCCESS;
}

/**
* This Task demonstrates the usage of discovery loop.
* It detects and reports the NFC technology type and performs read NDEF in case any NFC forum tags are detected.
* If a P2P device is detected then LLCP activate will be called to perform Client/Server operation from another Task.
* \param   pDataParams      The discovery loop data parameters
* \note    This Task will never return and Discovery loop run will be called in a infinite while loop.
*/
void *TReaderLibrary(void  *pDataParams)
{
    phStatus_t    status = PHAC_DISCLOOP_LPCD_NO_TECH_DETECTED;
    phStatus_t    statustmp;
    uint16_t      wTagsDetected = 0;
    uint16_t      wNumberOfTags = 0;
    uint16_t      wEntryPoint;
    uint16_t      wValue;
    uint16_t      wGeneralBytesLength;
    uint8_t     * pGeneralBytes;
    uint16_t      wGtLength;
    uint8_t       bIndex;
    uint8_t       bActiveMode = 0;
    uint8_t       bDeactivation = 0;
    uint8_t       bGBLen = 0;
    uint16_t      i = 0;

    Set_Interrupt();

    /* Initialize library */
    status = NfcRdLibInit();
    CHECK_STATUS(status);

#if defined INITIATOR_MODE
    /* Start in poll mode */
    wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_POLL;
    status = PHAC_DISCLOOP_LPCD_NO_TECH_DETECTED;
#elif defined TARGET_MODE
    /* Start in listen mode */
    wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_LISTEN;
#endif

    while(1)
    {
#ifndef NXPBUILD__PHHAL_HW_RC523
        /* Configure LPCD */
        if((status & PH_ERR_MASK) == PHAC_DISCLOOP_LPCD_NO_TECH_DETECTED)
        {
            status = ConfigureLPCD();
            CHECK_STATUS(status);
        }
#endif /* NXPBUILD__PHHAL_HW_RC523 */
        /* Reset local variables used. */
        bGBLen = 0;

        /* Set Discovery poll state to detection */
        status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_NEXT_POLL_STATE, PHAC_DISCLOOP_POLL_STATE_DETECTION);
        CHECK_STATUS(status);

        /* Switch off RF field */
        status = phhalHw_FieldOff(pHal);
        CHECK_STATUS(status);

        /* Start discovery loop */
        status = phacDiscLoop_Run(pDataParams, wEntryPoint);
        if((status & PH_ERR_MASK) == PHAC_DISCLOOP_MULTI_TECH_DETECTED)
        {
            DEBUG_PRINTF (" \n Multiple technology detected: \n");

            status = phacDiscLoop_GetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &wTagsDetected);
            CHECK_STATUS(status);

            if(PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_POS_BIT_MASK_A))
            {
                DEBUG_PRINTF (" \tType A detected... \n");
            }
            if(PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_POS_BIT_MASK_B))
            {
                DEBUG_PRINTF (" \tType B detected... \n");
            }
            if(PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_POS_BIT_MASK_F212))
            {
                DEBUG_PRINTF (" \tType F detected with baud rate 212... \n");
            }
            if(PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_POS_BIT_MASK_F424))
            {
                DEBUG_PRINTF (" \tType F detected with baud rate 424... \n");
            }
#ifndef NXPBUILD__PHHAL_HW_RC523
            if(PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_POS_BIT_MASK_V))
            {
                DEBUG_PRINTF(" \tType V / ISO 15693 / T5T detected... \n");
            }
#endif /* NXPBUILD__PHHAL_HW_RC523 */

            /* Store user configured poll configuration. */
            status = phacDiscLoop_GetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_PAS_POLL_TECH_CFG, &bSavePollTechCfg);
            CHECK_STATUS(status);

            /* Select last detected technology to resolve. */
            for(bIndex = 0; bIndex < PHAC_DISCLOOP_PASS_POLL_MAX_TECHS_SUPPORTED; bIndex++)
            {
                if(PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, (1 << bIndex)))
                {
                    /* Configure for one of the detected technology. */
                    status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_PAS_POLL_TECH_CFG, (1 << bIndex));
                    CHECK_STATUS(status);
                }
            }

            DEBUG_PRINTF ("\tResolving selected technology \n");

            /* Set Discovery loop poll state to collision resolution. */
            status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_NEXT_POLL_STATE, PHAC_DISCLOOP_POLL_STATE_COLLISION_RESOLUTION);
            CHECK_STATUS(status);

            /* Start Discovery loop in pool mode from collision resolution phase */
            status = phacDiscLoop_Run(pDataParams, wEntryPoint);

            if((status & PH_ERR_MASK) == PHAC_DISCLOOP_MULTI_DEVICES_RESOLVED)
            {
                /* Get detected technology type */
                status = phacDiscLoop_GetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &wTagsDetected);
                CHECK_STATUS(status);

                /* Get number of tags detected */
                status = phacDiscLoop_GetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_NR_TAGS_FOUND, &wNumberOfTags);
                CHECK_STATUS(status);

                DEBUG_PRINTF (" \n Multiple cards resolved: %d cards \n",wNumberOfTags);
                GetTagInfo(pDataParams, wNumberOfTags, wTagsDetected);

                if(wNumberOfTags > 1)
                {
                    /* Get 1st Detected Technology and Activate device at index 0*/
                    for(bIndex = 0; bIndex < PHAC_DISCLOOP_PASS_POLL_MAX_TECHS_SUPPORTED; bIndex++)
                    {
                        if(PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, (1 << bIndex)))
                        {
                            DEBUG_PRINTF("\t Activating one card...\n");
                            status = phacDiscLoop_ActivateCard(pDataParams, bIndex, 0);
                            break;
                        }
                    }

                    if(((status & PH_ERR_MASK) == PHAC_DISCLOOP_DEVICE_ACTIVATED) ||
                        ((status & PH_ERR_MASK) == PHAC_DISCLOOP_PASSIVE_TARGET_ACTIVATED))
                    {
                        /* Get Activated Technology Type */
                        status = phacDiscLoop_GetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &wTagsDetected);
                        CHECK_STATUS(status);

                        /* Print card details */
                        GetTagInfo(pDataParams, 0x01, wTagsDetected);
                        DEBUG_PRINTF("\tActivation successful\n");
                    }
                    else
                    {
                        DEBUG_PRINTF("\tCard activation failed\n");
                    }
                }
            }
            else if((status & PH_ERR_MASK) == PHAC_DISCLOOP_DEVICE_ACTIVATED)
            {
                DEBUG_PRINTF (" \tCard detected and activated successfully. \n");

                /* Get activated technology type */
                status = phacDiscLoop_GetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &wTagsDetected);
                CHECK_STATUS(status);

                /*Print card info*/
                GetTagInfo(pDataParams, 0x01, wTagsDetected);
            }
            else if ((status & PH_ERR_MASK) == PHAC_DISCLOOP_PASSIVE_TARGET_ACTIVATED)
            {
                DEBUG_PRINTF (" \nPassive P2P target detected and activated successfully after collision resolution: \n");

#ifdef NXPBUILD__PHPAL_I18092MPI
                /* Assign the PAL 18092 initiator parameters once LLPC should be activated in initiator mode. */
                slnLlcp.pPalI18092DataParams = &spalI18092mPI;

                /* Get the ATR_RES length. */
                status = phacDiscLoop_GetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_TYPEF_P2P_ATR_RES_LEN, &wGtLength);
                CHECK_STATUS(status);

                bGBLen = (uint8_t)wGtLength;

                if (bGBLen >= PHLN_LLCP_ATR_RES_MIN_LEN + 3)
                {
                    /* Activate LLCP with the received ATR_RES in initiator mode. */
                    status = phlnLlcp_Activate(&slnLlcp,
                        &aAtrRes[PHLN_LLCP_ATR_RES_MIN_LEN],
                        (bGBLen - PHLN_LLCP_ATR_RES_MIN_LEN),
                        PHLN_LLCP_INITIATOR
                        );

                    phOsal_Event_Consume(E_PH_OSAL_EVT_LLCP_ACTIVATED, E_PH_OSAL_EVT_SRC_LIB);

                    if (!(((status & PH_ERR_MASK) == PH_ERR_LLCP_DEACTIVATED) ||
                        ((status & PH_ERR_MASK) == PH_ERR_PEER_DISCONNECTED)))
                    {
                        if ((status & PH_ERR_MASK) == PH_ERR_ABORTED)
                        {
                            PRINT_ERROR("LLCP exited because of LTO timeout. \n");
                        }
                        else if ((status & PH_ERR_MASK) == PH_ERR_RF_ERROR)
                        {
                            PRINT_ERROR("LLCP exited because of RF error. \n");
                        }
                        else
                        {
                            PRINT_ERROR("\n LLCP exited unexpectedly:");
                            PrintErrorInfo(status);
                        }
                    }
                    else
                    {
                        bDeactivation = 1;
                    }

                    /* Perform LLCP DeInit procedure to release acquired resources. */
                    status = phlnLlcp_DeInit(&slnLlcp);
                    CHECK_STATUS(status);

                    if (bSNEPClient == SNEP_PUT)
                    {
                        DEBUG_PRINTF("\t SNEP Client : Performed SNEP PUT\n\t PUT data length is:%d\n\n", dwDataLen);
                    }

                    /*
                     * Waits on SNEP server to receive SNEP message from a client.
                     * If the server gives this semaphore, then it indicates that
                     * the message has been received and gets printed.
                     */
                    status = phOsal_Semaphore_Take(xTaskSema, 1000);
                    if ((bSNEPServer == SNEP_RECEIVED_PUT) && (status == PH_ERR_SUCCESS))
                    {
                        DEBUG_PRINTF("\t SNEP Server : Received PUT message of length %d bytes\n", baSnepRxLen);

                        for(i = 0; i < baSnepRxLen; i++)
                        {
                            if((baSnepAppBuffer[i] < 0x20) || (baSnepAppBuffer[i] > 0x7e))
                            {
                                baSnepAppBuffer[i] = '.';
                            }
                        }
                        baSnepAppBuffer[baSnepRxLen] = '\0';
                        DEBUG_PRINTF("\t PUT message : %s\n", baSnepAppBuffer);
                        baSnepRxLen = 0;
                    }
                    bSNEPClient = 0;
                    bSNEPServer = 0;
                }
                else
                {
                    PRINT_ERROR("Received ATR_RES length is wrong. \n");
                }
#endif /* NXPBUILD__PHPAL_I18092MPI */
            }
            else
            {
                PRINT_ERROR("\tFailed to resolve selected technology.\n");
            }

            /* Re-Store user configured poll configuration. */
            status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_PAS_POLL_TECH_CFG, bSavePollTechCfg);
            CHECK_STATUS(status);

        }/* PHAC_DISCLOOP_MULTI_TECH_DETECTED - End */
        else if (((status & PH_ERR_MASK) == PHAC_DISCLOOP_NO_TECH_DETECTED) ||
            ((status & PH_ERR_MASK) == PHAC_DISCLOOP_NO_DEVICE_RESOLVED))
        {
#if defined INITIATOR_MODE
            /* Switch to poll mode */
            wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_POLL;
#elif defined TARGET_MODE
            /* Switch to listen mode */
            wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_LISTEN;
#endif
        }
        else if((status & PH_ERR_MASK) == PHAC_DISCLOOP_EXTERNAL_RFON)
        {
            /*
            * If external RF is detected during POLL, return back so that the application
            * can restart the loop in LISTEN mode
            */
#ifdef TARGET_MODE
            /* Start in listen mode */
            wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_LISTEN;
#endif /* TARGET_MODE */
        }
        else if((status & PH_ERR_MASK) == PHAC_DISCLOOP_EXTERNAL_RFOFF)
        {
            /*
            * Enters here if in the target/card mode and external RF is not available
            * Wait for LISTEN timeout till an external RF is detected.
            * Application may choose to go into standby at this point.
            */
            status = phOsal_Event_Consume(E_PH_OSAL_EVT_RF, E_PH_OSAL_EVT_SRC_ISR);
            CHECK_STATUS(status);

            status = phhalHw_SetConfig(pHal, PHHAL_HW_CONFIG_RFON_INTERRUPT, PH_ON);
            CHECK_STATUS(status);

            status = phOsal_Event_WaitAny(E_PH_OSAL_EVT_RF, LISTEN_PHASE_TIME_MS , NULL);
            if((status & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT)
            {
                /* Switch to poll mode */
                wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_POLL;
            }
            else
            {
#ifdef TARGET_MODE
                /* Switch to listen mode */
                wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_LISTEN;
#endif /* TARGET_MODE */
            }
        }
        else if((status & PH_ERR_MASK) == PHAC_DISCLOOP_ACTIVATED_BY_PEER)
        {
            DEBUG_PRINTF ("\nDevice activated in listen mode. \n");
#ifdef NXPBUILD__PHPAL_I18092MT
            /* Assign the PAL 18092 target parameters once LLPC should be activated in listen mode. */
            slnLlcp.pPalI18092DataParams = &spalI18092mT;

            memcpy(aAtrRes, poll_res, 10);
            aAtrRes[10] = bBst;
            aAtrRes[11] = bBrt;
            aAtrRes[12] = bTo;
            aAtrRes[13] = bLrt << 4 | 0x02; /* Frame Size is 254 and ATR_RES contains General Bytes. */
            memcpy(&aAtrRes[14], aLLCPGeneralBytes, bLLCPGBLength);
            wAtrResLength = 14 + bLLCPGBLength;

            status = phpalI18092mT_Activate(&spalI18092mT,
                sDiscLoop.sTargetParams.pRxBuffer,
                sDiscLoop.sTargetParams.wRxBufferLen,
                aAtrRes,
                wAtrResLength,
                &pGeneralBytes,
                &wGeneralBytesLength
                );
            if((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
            {
                /* Activate LLCP with the received ATR_REQ in target mode. */
                status = phlnLlcp_Activate(&slnLlcp,
                    pGeneralBytes,
                    wGeneralBytesLength,
                    PHLN_LLCP_TARGET
                    );

                phOsal_Event_Consume(E_PH_OSAL_EVT_LLCP_ACTIVATED, E_PH_OSAL_EVT_SRC_LIB);

                if (!(((status & PH_ERR_MASK) == PH_ERR_LLCP_DEACTIVATED) ||
                    ((status & PH_ERR_MASK) == PH_ERR_PEER_DISCONNECTED)))
                {
                    if ((status & PH_ERR_MASK) == PH_ERR_ABORTED)
                    {
#ifdef TEST_ACTIVE_BOARDS
                        PRINT_ERROR("LLCP exited because of LTO timeout. \n");
#else
                        /* Error print is not performed when Testing with 2 Pn512, as WUP and RLS request needs to be
                         * sent for proper De-Activation of LLCP else LTO Timeout is expected to happen on Target. */
#endif /* TEST_ACTIVE_BOARDS */
                    }
                    else if ((status & PH_ERR_MASK) == PH_ERR_EXT_RF_ERROR)
                    {
                        PRINT_ERROR("LLCP exited because of external RF Off. \n");
                    }
                    else if ((status & PH_ERR_MASK) == PH_ERR_RF_ERROR)
                    {
                        PRINT_ERROR("LLCP exited because of active RF error. \n");
                    }
                    else
                    {
                        PRINT_ERROR("\n LLCP exited unexpectedly:");
                        PrintErrorInfo(status);
                    }
                }

                if (bSNEPClient == SNEP_PUT)
                {
                    DEBUG_PRINTF("\t SNEP Client : Performed SNEP PUT\n\t PUT data length is:%d\n\n", dwDataLen);
                }

                /*
                 * Waits on SNEP server to receive SNEP message from a client.
                 * If the server gives this semaphore, then it indicates that
                 * the message has been received and gets printed.
                 */
                status = phOsal_Semaphore_Take(xTaskSema, 1000);
                if ((bSNEPServer == SNEP_RECEIVED_PUT) && (status == PH_ERR_SUCCESS))
                {
                    DEBUG_PRINTF("\t SNEP Server : Received PUT message of length %d bytes\n", baSnepRxLen);

                    for(i = 0; i < baSnepRxLen; i++)
                    {
                        if((baSnepAppBuffer[i] < 0x20) || (baSnepAppBuffer[i] > 0x7e))
                        {
                            baSnepAppBuffer[i] = '.';
                        }
                    }
                    baSnepAppBuffer[baSnepRxLen] = '\0';
                    DEBUG_PRINTF("\t PUT Message : %s\n", baSnepAppBuffer);
                    baSnepRxLen = 0;
                }
                bSNEPClient = 0;
                bSNEPServer = 0;

                /* Perform LLCP DeInit procedure to release acquired resources. */
                status = phlnLlcp_DeInit(&slnLlcp);
                CHECK_STATUS(status);
            }
            else
            {
                PRINT_ERROR("\n phpalI18092mT_Activate failed :");
                PrintErrorInfo(status);
            }
#endif /* NXPBUILD__PHPAL_I18092MT */

            /* On successful activated by Peer, switch to POLL mode */
            wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_POLL;
        }
        else if(((status & PH_ERR_MASK) == PHAC_DISCLOOP_PASSIVE_TARGET_ACTIVATED)
            ||((status & PH_ERR_MASK) == PHAC_DISCLOOP_ACTIVE_TARGET_ACTIVATED)
            ||((status & PH_ERR_MASK) == PHAC_DISCLOOP_MERGED_SEL_RES_FOUND))

        {
#ifdef NXPBUILD__PHPAL_I18092MPI
            /* Get detected technology type */
            statustmp = phacDiscLoop_GetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &wTagsDetected);
            CHECK_STATUS(statustmp);

            if ((status & PH_ERR_MASK) == PHAC_DISCLOOP_ACTIVE_TARGET_ACTIVATED)
            {
                bActiveMode = 1;
                if(PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_ACT_POS_BIT_MASK_106))
                {
                    DEBUG_PRINTF ("\nActive P2P target detected and activated successfully at 106kbps. \n");

                    /* Get the ATR_RES length. */
                    status = phacDiscLoop_GetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_TYPEA_P2P_ATR_RES_LEN, &wGtLength);
                    CHECK_STATUS(status);
                }
                else if((PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_ACT_POS_BIT_MASK_212)) ||
                    (PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_ACT_POS_BIT_MASK_424)))
                {
                    DEBUG_PRINTF ("\nActive P2P target detected and activated successfully at 212/424kbps. \n");

                    /* Get the ATR_RES length. */
                    status = phacDiscLoop_GetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_TYPEF_P2P_ATR_RES_LEN, &wGtLength);
                    CHECK_STATUS(status);
                }
                else
                {
                    DEBUG_PRINTF("\nUnknown Active P2P target detected. \n");
                }

                /* Save the ATR_RES length. */
                bGBLen = (uint8_t)wGtLength;
            }
            else if ((status & PH_ERR_MASK) == PHAC_DISCLOOP_MERGED_SEL_RES_FOUND)
            {
                DEBUG_PRINTF (" \n Merged SAK: Device having T4T and NFC-DEP support detected.\n");

                /* Send ATR_REQ to activate device in P2P mode. */
                status = phpalI18092mPI_Atr(&spalI18092mPI,
                    sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].aUid,
                    0x00,
                    0x03,
                    0x00,
                    0x00,
                    aLLCPGeneralBytes,
                    sDiscLoop.sTypeATargetInfo.sTypeA_P2P.bGiLength,
                    aAtrRes,
                    &bGBLen
                    );
                CHECK_STATUS(status);
            }
            else
            {
                if(PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_POS_BIT_MASK_A))
                {
                    DEBUG_PRINTF ("\nPassive P2P target detected and activated successfully at 106kbps. \n");

                    /* Get the ATR_RES Length. */
                    status = phacDiscLoop_GetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_TYPEA_P2P_ATR_RES_LEN, &wGtLength);
                    CHECK_STATUS(status);
                }
                else if((PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_POS_BIT_MASK_F212)) ||
                    (PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_POS_BIT_MASK_F424)))
                {
                    DEBUG_PRINTF ("\nPassive P2P target detected and activated successfully at 212/424kbps. \n");

                    /* Get the ATR_RES Length. */
                    status = phacDiscLoop_GetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_TYPEF_P2P_ATR_RES_LEN, &wGtLength);
                    CHECK_STATUS(status);
                }
                else
                {
                    DEBUG_PRINTF("\nUnknown passive P2P target detected. \n");
                }

                /* Save the ATR_RES length. */
                bGBLen = (uint8_t)wGtLength;
            }

            /* Assign the PAL 18092 Initiator Parameters once LLPC should be activated in Initiator Mode. */
            slnLlcp.pPalI18092DataParams = &spalI18092mPI;

            if (bGBLen >= (PHLN_LLCP_ATR_RES_MIN_LEN + 3))
            {
                /* Activate LLCP with the received ATR_RES in initiator mode. */
                status = phlnLlcp_Activate(&slnLlcp,
                    &aAtrRes[PHLN_LLCP_ATR_RES_MIN_LEN],
                    (bGBLen - PHLN_LLCP_ATR_RES_MIN_LEN),
                    PHLN_LLCP_INITIATOR
                    );

                phOsal_Event_Consume(E_PH_OSAL_EVT_LLCP_ACTIVATED, E_PH_OSAL_EVT_SRC_LIB);

                if (!(((status & PH_ERR_MASK) == PH_ERR_LLCP_DEACTIVATED) ||
                    ((status & PH_ERR_MASK) == PH_ERR_PEER_DISCONNECTED)))
                {
                    if ((status & PH_ERR_MASK) == PH_ERR_ABORTED)
                    {
                        PRINT_ERROR("LLCP exited because of LTO timeout. \n");
                    }
                    else if ((status & PH_ERR_MASK) == PH_ERR_RF_ERROR)
                    {
                        PRINT_ERROR("LLCP exited because of active RF error. \n");
                    }
                    else
                    {
                        PRINT_ERROR("\n LLCP exited unexpectedly:");
                        PrintErrorInfo(status);
                    }
                }
                else
                {
                    bDeactivation = 1;
                }

                /* Perform LLCP DeInit procedure to release acquired resources. */
                status = phlnLlcp_DeInit(&slnLlcp);
                CHECK_STATUS(status);

                /* When LLCP Activated in Active Mode then WUP and RLS has to be performed so that
                * Target exits properly. Otherwise Target exits LLCP because of LTO Timeout.
                * Needed only when testing with 2 board. Mobile phones don't require this. */
                if ((bDeactivation == 1) && (bActiveMode == 1))
                {
#ifdef TEST_ACTIVE_BOARDS
                    /* Send WUP_REQ. */
                    statustmp = phpalI18092mPI_Wakeup(&spalI18092mPI);
                    if (statustmp == PH_ERR_SUCCESS)
                    {
                        /* Send RLS_REQ. */
                        status = phpalI18092mPI_Deselect(&spalI18092mPI, PHPAL_I18092MPI_DESELECT_RLS);
                    }
                    CHECK_STATUS(statustmp);
#endif /* TEST_ACTIVE_BOARDS */
                    bActiveMode = 0;
                    bDeactivation = 0;
                }

                if (bSNEPClient == SNEP_PUT)
                {
                    DEBUG_PRINTF("\t SNEP Client : Performed SNEP PUT\n\t PUT data length is:%d\n\n", dwDataLen);
                }

                /*
                 * Waits on SNEP server to receive SNEP message from a client.
                 * If the server gives this semaphore, then it indicates that
                 * the message has been received and gets printed.
                 */
                status = phOsal_Semaphore_Take(xTaskSema, 1000);
                if ((bSNEPServer == SNEP_RECEIVED_PUT) && (status == PH_ERR_SUCCESS))
                {
                    DEBUG_PRINTF("\t SNEP Server : Received PUT message of length %d bytes\n", baSnepRxLen);

                    for(i = 0; i < baSnepRxLen; i++)
                    {
                        if((baSnepAppBuffer[i] < 0x20) || (baSnepAppBuffer[i] > 0x7e))
                        {
                            baSnepAppBuffer[i] = '.';
                        }
                    }
                    baSnepAppBuffer[baSnepRxLen] = '\0';
                    DEBUG_PRINTF("\t PUT Message : %s\n", baSnepAppBuffer);
                    baSnepRxLen = 0;
                }
                bSNEPClient = 0;
                bSNEPServer = 0;
            }
            else
            {
                PRINT_ERROR("Received ATR_RES doesn't contain LLCP magic bytes. Raw P2P is possible. \n");
            }
#endif /* NXPBUILD__PHPAL_I18092MPI */

#if defined INITIATOR_MODE
            /* Start in Poll mode */
            wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_POLL;
#elif defined TARGET_MODE
            /* Start in listen mode */
            wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_LISTEN;
#endif
        }
        else if((status & PH_ERR_MASK) == PHAC_DISCLOOP_MULTI_DEVICES_RESOLVED)
        {
            /*
            * Multiple cards resolved. It enters here if DEVICE LIMIT > 1 and more than one devices are
            * detected and resolved.
            */
            DEBUG_PRINTF (" \n Multiple cards resolved: \n");

            /* Get detected technology type */
            status = phacDiscLoop_GetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &wTagsDetected);
            CHECK_STATUS(status);

            /* Get number of tags detected */
            status = phacDiscLoop_GetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_NR_TAGS_FOUND, &wNumberOfTags);
            CHECK_STATUS(status);

            DEBUG_PRINTF ("\tNumber of tags: %d \n",wNumberOfTags);
            GetTagInfo(pDataParams, wNumberOfTags, wTagsDetected);

            if(wNumberOfTags > 1)
            {
                /* Get 1st detected technology and activate device at index 0 */
                for(bIndex = 0; bIndex < PHAC_DISCLOOP_PASS_POLL_MAX_TECHS_SUPPORTED; bIndex++)
                {
                    if(PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, (1 << bIndex)))
                    {
                        DEBUG_PRINTF("\t Activating device @ index 0\n");
                        status = phacDiscLoop_ActivateCard(pDataParams, bIndex, 0);
                        break;
                    }
                }

                if( ((status & PH_ERR_MASK) == PHAC_DISCLOOP_DEVICE_ACTIVATED) ||
                    ((status & PH_ERR_MASK) == PHAC_DISCLOOP_PASSIVE_TARGET_ACTIVATED))
                {
                    /* Get detected technology type */
                    status = phacDiscLoop_GetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &wTagsDetected);
                    CHECK_STATUS(status);

                    GetTagInfo(pDataParams, 0x01, wTagsDetected);
                    DEBUG_PRINTF("\t\t Activation successful\n");

#if defined INITIATOR_MODE
                    /* Start in poll mode */
                    wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_POLL;
#elif  defined TARGET_MODE
                    /* Start in listen mode */
                    wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_LISTEN;
#endif
                }
                else
                {
                    DEBUG_PRINTF("\t\tCard activation failed\n");
                }
            }
        }
        else if((status & PH_ERR_MASK) == PHAC_DISCLOOP_DEVICE_ACTIVATED)
        {
            DEBUG_PRINTF (" \n Card detected and activated successfully \n");
            status = phacDiscLoop_GetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_NR_TAGS_FOUND, &wNumberOfTags);
            CHECK_STATUS(status);

            /* Get detected technology type */
            status = phacDiscLoop_GetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &wTagsDetected);
            CHECK_STATUS(status);

            DEBUG_PRINTF ("\tNumber of tags: %d \n",wNumberOfTags);
            GetTagInfo(pDataParams, wNumberOfTags, wTagsDetected);

#if defined INITIATOR_MODE

            /* Start in poll mode */
            wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_POLL;
#elif defined TARGET_MODE
            /* Start in listen mode */
            wEntryPoint = PHAC_DISCLOOP_ENTRY_POINT_LISTEN;
#endif
        }
        else
        {
            if((status & PH_ERR_MASK) == PHAC_DISCLOOP_FAILURE)
            {
                status = phacDiscLoop_GetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_ADDITIONAL_INFO, &wValue);
                CHECK_STATUS(status);
                PRINT_ERROR("\n Discovery loop:");
                PrintErrorInfo(wValue);
            }
            else
            {
                PRINT_ERROR("\n Discovery loop:");
                PrintErrorInfo(status);
            }
        }
        /* Reset state of layers. */
        (void)phalTop_Reset(&sTagop);
        (void)phpalI14443p4_ResetProtocol(&spalI14443p4);
        (void)phpalI18092mPI_ResetProtocol(&spalI18092mPI);
        (void)phpalI18092mT_ResetProtocol(&spalI18092mT);
    }
}

#ifdef SNEP_SERVER
/**
* Task 3 : SNEP server task
* This task gets created from TSnepDispatch and will register SNEP server socket on LLCP running in reader
* library thread and will wait for remote client to get connected and perform SNEP PUT operation.
*/
void *TSNEPServer(void * pLlcp)
{
    phStatus_t                  status = 0;
    uint32_t                    dwServerRxBuffLength = sizeof(bServerRxBuffer);
    uint32_t                    baSnepAppBufSize = sizeof(baSnepAppBuffer) - 1;
    uint8_t                     bClientReq;

    xTaskSema = phOsal_Semaphore_Create(1, 0);
    if (xTaskSema == NULL)
    {
        DEBUG_PRINTF ("Server task application semaphore could not be created.\n");
    }

    baSnepRxLen = 0;
    bSNEPServer = 0;

    /* Initialize SNEP server component. */
    status = phnpSnep_Sw_Init(&snpSnepServer, sizeof(phnpSnep_Sw_DataParams_t), &slnLlcp, &ServerSocket);
    CHECK_STATUS(status);

    /* Perform server initialization to handle remote SNEP client connection. */
    status = phnpSnep_ServerInit(&snpSnepServer, phnpSnep_Default_Server, NULL, bServerRxBuffer, dwServerRxBuffLength);
    if (status == PH_ERR_SUCCESS)
    {
        do
        {
            /* Handle client PUT request. */
            status = phnpSnep_ServerListen(&snpSnepServer, 0, NULL, NULL, &bClientReq);

            if (status == PH_ERR_SUCCESS)
            {
                status = phnpSnep_ServerSendResponse(&snpSnepServer, bClientReq, NULL, 0, baSnepAppBufSize, baSnepAppBuffer, &baSnepRxLen);
            }
        }while(!status);

        if (!(((status & PH_ERR_MASK) == PH_ERR_PEER_DISCONNECTED) ||
            ((status & PH_ERR_MASK) == PH_ERR_LLCP_DEACTIVATED)) ||
            ((status & PH_ERR_MASK) == PH_ERR_SUCCESS))
        {
            CHECK_STATUS(status);
        }
        else
        {
            if (baSnepRxLen > 0)
            {
                /* Print only if server received PUT message of length greater than 0 bytes. */
                bSNEPServer = SNEP_RECEIVED_PUT;
            }
        }
        status = phOsal_Semaphore_Give(xTaskSema);
    }
    else
    {
        /* Server initialization is un-successful as peer did not send CONNECT PDU. */
    }
//    phOsal_Event_Consume(E_PH_OSAL_EVT_LLCP_ACTIVATED, E_PH_OSAL_EVT_SRC_LIB);

    /* Perform server de-init. */
    status = phnpSnep_ServerDeInit(&snpSnepServer);
    CHECK_STATUS(status);
}
#endif /* SNEP_SERVER */

#ifdef SNEP_CLIENT
/**
* Task 4 : SNEP client task.
* This task gets created from TSnepDispatch and will register client socket on LLCP running in reader
* library thread and will connect to remote SNEP server and performs SNEP PUT operation.
*/
void *TSNEPClient(void * pLlcp)
{
    phStatus_t status = 0;
    
    /* Initialize SNEP client component */
    status = phnpSnep_Sw_Init(&snpSnepClient, sizeof(snpSnepClient), &slnLlcp, &ClientSocket);
    CHECK_STATUS(status);

    /* Perform SNEP client Initialization and connect to remote SNEP server. */
    status = phnpSnep_ClientInit(&snpSnepClient, phnpSnep_Default_Server, NULL,
            bClientRxBuffer, dwClientRxBuffLength);
    if (status == PH_ERR_SUCCESS)
    {
        status = phnpSnep_Put(&snpSnepClient, (uint8_t *)baSnepAppBuf, dwDataLen);
        ClientSocket.fReady = true;

//        phOsal_Event_Consume(E_PH_OSAL_EVT_LLCP_ACTIVATED, E_PH_OSAL_EVT_SRC_LIB);

        if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
        {
            status = phnpSnep_ClientDeInit(&snpSnepClient);
            if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
            {
                status = phlnLlcp_Deactivate(&slnLlcp);
            }
            bSNEPClient = SNEP_PUT;
        }
        else
        {
            status = phlnLlcp_Transport_Socket_Unregister(snpSnepClient.plnLlcpDataParams,
                    snpSnepClient.psSocket);
            CHECK_STATUS(status);
        }
    }
    else
    {
//        phOsal_Event_Consume(E_PH_OSAL_EVT_LLCP_ACTIVATED, E_PH_OSAL_EVT_SRC_LIB);

        /* Client initialization is un-successful as failed to connect to remote server.
         * Release RTOS memory by performing socket unregister. */
        status = phlnLlcp_Transport_Socket_Unregister(snpSnepClient.plnLlcpDataParams,
                snpSnepClient.psSocket);
        CHECK_STATUS(status);
    }
} 

#endif /* SNEP_CLIENT */


/**
* Task 3 : SNEP dispatcher task.
* This task will first block on LLCP activated event from reader library thread, once
* event is obtained this task will register two new tasks. TSNEPClient and TSNEPServer
* \note : This task will run in a infinite while loop.
*/
void *TSnepDispatch(void * pLlcp)
{
    phStatus_t status = 0, ret = 0;
    pthread_t thread_SnepClient, thread_SnepServer;

    while (1)
    {
        /* Wait until LLCP activation is complete. */
        status = phlnLlcp_WaitForActivation(&slnLlcp);
        CHECK_STATUS(status);

        /* Cretae the SNEP Server and SNEP Client tasks*/
#ifdef SNEP_SERVER
	    ret = phOsal_Posix_Thread_Create_Extra(&thread_SnepServer, &TSNEPServer, &slnLlcp);
	    if(ret)
	    {
            printf("TSNEPServer thread creation failed...Error: %ld\n", ret);
            return NULL;
	    }	        
#endif /* SNEP_SERVER */

#ifdef SNEP_CLIENT
	    ret = phOsal_Posix_Thread_Create_Extra(&thread_SnepClient, TSNEPClient, &slnLlcp);
	    if(ret)
	    {
            printf("TSNEPClient thread creation failed...Error: %ld\n", ret);
            return NULL;
	    }
#endif /* SNEP_CLIENT */
        
        phOsal_Posix_Thread_Join_Extra(&thread_SnepServer, NULL);
        phOsal_Posix_Thread_Join_Extra(&thread_SnepClient, NULL);
        phOsal_Event_Consume(E_PH_OSAL_EVT_LLCP_ACTIVATED, E_PH_OSAL_EVT_SRC_LIB);
    }
}

/*******************************************************************************
**   Main Function
*******************************************************************************/
int main (void)
{
    phStatus_t ret = 0;

    /* Set the interface link for the internal chip communication */
    ret = Set_Interface_Link();
    if(ret)
    {
    	return 1;
    }

    /* Perform a hardware reset */
    Reset_reader_device();

    DEBUG_PRINTF("\n NFC Forum Example: \n");
	
    ret = phOsal_Posix_Thread_Create(E_PH_OSAL_EVT_DEST_LIB, TReaderLibrary, &sDiscLoop);
    if(ret)
    {
            printf("TReaderLibrary thread creation failed...Error: %ld\n", ret);
            return -1;
    }
    
    ret = phOsal_Posix_Thread_Create(E_PH_OSAL_EVT_DEST_APP, TSnepDispatch, &sDiscLoop);
    
    phOsal_Posix_Thread_Join(E_PH_OSAL_EVT_DEST_LIB, NULL);
    phOsal_Posix_Thread_Join(E_PH_OSAL_EVT_DEST_APP, NULL);

    Cleanup_Interface_Link();

    return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
