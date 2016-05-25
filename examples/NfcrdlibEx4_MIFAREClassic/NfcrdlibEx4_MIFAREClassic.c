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
 * Example Source for NfcrdlibEx4_MIFAREClassic that uses the MIFARE Classic implementation.
 * This example will load/configure Discovery loop for MIFARE Classic, uses POLL mode to detect the MIFARE Classic card.
 * Displays detected MIFARE Classic card information like UID, ATQA, SAK, Sector Authentication, Block Read and Write status.
 * Example will activate the device at index zero whenever multiple MIFARE Classic cards are detected.

 * Please refer Readme.txt file for Hardware Pin Configuration, Software Configuration and steps to build and
 * execute the project which is present in the same project directory.
 * $Author: Santosh Araballi (ing04223) $
 * $Revision: 4001 $
 * $Date: 2015-12-05 18:08:38 +0530 (Sat, 05 Dec 2015) $
 *
 * History:
 *
 *
 */

#ifdef DEBUG
/* standard input output library */
#include <stdio.h>
#endif

/**
 * Header for hardware configuration: bus interface, reset of attached reader ID, onboard LED handling etc.
 * */
#include <phhwConfig.h>

/**
 * Reader Library Headers
 */
#include <ph_Status.h>

/*BAL Headers*/
#include <phbalReg.h>

/*PAL Headers*/
#include <phpalI14443p3a.h>
#include <phpalI14443p4.h>
#include <phpalI14443p3b.h>
#include <phpalI14443p4a.h>
#include <phpalMifare.h>

/*AL Headers*/
#include <phalMfc.h>

#include <phacDiscLoop.h>

/* Generic KeyStore Component of Reader Library Framework.*/
#include <phKeyStore.h>

/* Printf macro */
#ifdef   DEBUG
#include <stdio.h>
#define  DEBUG_FLUSH(x)      //{char buff[1]; buff[0] = x; __sys_write(0, buff, 1);}
#else
#define  DEBUG_FLUSH(x)
#endif

#define NUMBER_OF_KEYENTRIES        2
#define NUMBER_OF_KEYVERSIONPAIRS   2
#define NUMBER_OF_KUCENTRIES        1

#define DATA_BUFFER_LEN             16 /* Buffer length */
#define MFC_BLOCK_DATA_SIZE         16 /* Block Data size - 16 Bytes */

/*******************************************************************************
**   Global Variable Declaration
*******************************************************************************/
phbalReg_Stub_DataParams_t         sBalReader;                 /* BAL component holder */

    /*HAL variables*/
phhalHw_Nfc_Ic_DataParams_t        sHal_Nfc_Ic;                /* HAL component holder for Nfc Ic's */
void                              *pHal;                       /* HAL pointer */
uint8_t                            bHalBufferTx[128];          /* HAL  TX buffer */
uint8_t                            bHalBufferRx[128];          /* HAL  RX buffer */

    /*PAL variables*/
phpalI14443p3a_Sw_DataParams_t     spalI14443p3a;              /* PAL  I14443-A component */
phpalI14443p4a_Sw_DataParams_t     spalI14443p4a;              /* PAL ISO I14443-4A component */
phpalI14443p3b_Sw_DataParams_t     spalI14443p3b;              /* PAL ISO I14443-B component */
phpalI14443p4_Sw_DataParams_t      spalI14443p4;               /* PAL ISO I14443-4 component */
phpalMifare_Sw_DataParams_t        spalMifare;                 /* PAL  MIFARE component */

phacDiscLoop_Sw_DataParams_t       sDiscLoop;                  /* Discovery loop component */
phalMfc_Sw_DataParams_t            salMfc;                     /* MIFARE Classic parameter structure */

phKeyStore_Sw_DataParams_t         sSwkeyStore;                /* Sw Key parameter structure */
phKeyStore_Sw_KeyEntry_t           sKeyEntries[NUMBER_OF_KEYENTRIES];                                  /* Sw KeyEntry structure */
phKeyStore_Sw_KUCEntry_t           sKUCEntries[NUMBER_OF_KUCENTRIES];                                  /* Sw Key usage counter structure */
phKeyStore_Sw_KeyVersionPair_t     sKeyVersionPairs[NUMBER_OF_KEYVERSIONPAIRS * NUMBER_OF_KEYENTRIES]; /* Sw KeyVersionPair structure */

uint8_t                            bDataBuffer[DATA_BUFFER_LEN];  /* universal data buffer */

uint8_t                            bSak;                      /* SAK  card type information */
uint16_t                           wAtqa;                     /* ATQA card type information */

/** General information bytes to be sent with ATR */
const uint8_t GI[] = { 0x46,0x66,0x6D,
                       0x01,0x01,0x10,       /*VERSION*/
                       0x03,0x02,0x00,0x01,  /*WKS*/
                       0x04,0x01,0xF1        /*LTO*/
                      };

static uint8_t    aData[50];              /* ATR response holder */

/* Set the key for the MIFARE (R) Classic cards. */
uint8_t Key[6] = {0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU};

/* Don't change the following line */
uint8_t Original_Key[6] = {0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU};

/***********************************************************************************************
 * \brief   Print a given array of integers on the console
 * \param   *pBuff   Buffer Reference
 * \param   size      data size to be print
 **********************************************************************************************/
#ifdef   DEBUG
static void PRINT_BUFF(uint8_t *pBuff, uint8_t size)
{
    uint8_t bBufIdx;

    for(bBufIdx = 0; bBufIdx < size; bBufIdx++)
    {
        DEBUG_PRINTF(" %02X",pBuff[bBufIdx]);
    }
}
#else
#define  PRINT_BUFF(x, y)
#endif /* DEBUG */

/***********************************************************************************************
 * \brief   This function will enable the required polling and configuration for discovery loop.
 * \param   none
 * \return  status  Returns the function status
 **********************************************************************************************/
static phStatus_t LoadProfile(void)
{
    phStatus_t status = PH_ERR_SUCCESS;

    sDiscLoop.pPal1443p3aDataParams  = &spalI14443p3a;
	sDiscLoop.pPal1443p3bDataParams  = &spalI14443p3b;
    sDiscLoop.pPal1443p4aDataParams  = &spalI14443p4a;
    sDiscLoop.pPal14443p4DataParams  = &spalI14443p4;
    sDiscLoop.pHalDataParams         = &sHal_Nfc_Ic.sHal;
	
	/* These lines are added just to SIGSEG fault when non 14443-3 card is detected */
	/* Assign the GI for Type A */
    sDiscLoop.sTypeATargetInfo.sTypeA_P2P.pGi       = (uint8_t *)GI;
    sDiscLoop.sTypeATargetInfo.sTypeA_P2P.bGiLength = sizeof(GI);
    /* Assign the GI for Type F */
    sDiscLoop.sTypeFTargetInfo.sTypeF_P2P.pGi       = (uint8_t *)GI;
    sDiscLoop.sTypeFTargetInfo.sTypeF_P2P.bGiLength = sizeof(GI);
    /* Assign ATR response for Type A */
    sDiscLoop.sTypeATargetInfo.sTypeA_P2P.pAtrRes   = aData;
    /* Assign ATR response for Type F */
    sDiscLoop.sTypeFTargetInfo.sTypeF_P2P.pAtrRes   = aData;
    /* Assign ATS buffer for Type A */
    sDiscLoop.sTypeATargetInfo.sTypeA_I3P4.pAts     = aData;
	/* ******************************************************************************************** */	
	
    /* Passive Bailout bitmap configuration */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_BAIL_OUT, 0x00);
    CHECK_STATUS(status);

    /* Passive poll bitmap configuration. Poll for only Type A Tags. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_PAS_POLL_TECH_CFG, PHAC_DISCLOOP_POS_BIT_MASK_A);
    CHECK_STATUS(status);

    /* Turn OFF Passive Listen. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_PAS_LIS_TECH_CFG, 0x00);
    CHECK_STATUS(status);

    /* Turn OFF active listen. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ACT_LIS_TECH_CFG, 0x00);
    CHECK_STATUS(status);

    /* Turn OFF Active Poll */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ACT_POLL_TECH_CFG, 0x00);
    CHECK_STATUS(status);

    /* Disable LPCD feature. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ENABLE_LPCD, PH_OFF);
    CHECK_STATUS(status);

    /* reset collision Pending */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_COLLISION_PENDING, PH_OFF);
    CHECK_STATUS(status);

    /* whether anti-collision is supported or not. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ANTI_COLL, PH_ON);
    CHECK_STATUS(status);

    /* Device limit for Type A */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_TYPEA_DEVICE_LIMIT, 1);
    CHECK_STATUS(status);

    /* Discovery loop Operation mode */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_OPE_MODE, RD_LIB_MODE_NFC);

    /* Bailout on Type A detect */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_BAIL_OUT, PHAC_DISCLOOP_POS_BIT_MASK_A);
    CHECK_SUCCESS(status);

    /* Return Status */
    return status;
}

/***********************************************************************************************
 * \brief   Initializes the Reader Library
 * \param   none
 * \return  status  Returns the function status
 **********************************************************************************************/
phStatus_t NfcRdLibInit(void)
{
    phStatus_t status;

    /* Initialize the Reader BAL (Bus Abstraction Layer) component */
    status = phbalReg_Stub_Init(&sBalReader, sizeof(phbalReg_Stub_DataParams_t));
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

    status = phbalReg_SetPort(
        &sBalReader,
        SPI_CONFIG);
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
                                sizeof(bHalBufferRx));
    CHECK_SUCCESS(status);

    /* Set the parameter to use the SPI interface */
    sHal_Nfc_Ic.sHal.bBalConnectionType = PHHAL_HW_BAL_CONNECTION_SPI;

	Configure_Device(&sHal_Nfc_Ic);

    /* Set the generic pointer */
    pHal = &sHal_Nfc_Ic.sHal;

    /* Initializing specific objects for the communication with MIFARE (R) Classic cards.
     * The MIFARE (R) Classic card is compliant of ISO 14443-3 and ISO 14443-4
     */

	 /* Initialize the I14443-A PAL layer */
    status = phpalI14443p3a_Sw_Init(&spalI14443p3a, sizeof(phpalI14443p3a_Sw_DataParams_t), &sHal_Nfc_Ic.sHal);
    CHECK_STATUS(status);

    /* Initialize the I14443-A PAL component */
    status = phpalI14443p4a_Sw_Init(&spalI14443p4a, sizeof(phpalI14443p4a_Sw_DataParams_t), &sHal_Nfc_Ic.sHal);
    CHECK_STATUS(status);

    /* Initialize the I14443-4 PAL component */
    status = phpalI14443p4_Sw_Init(&spalI14443p4, sizeof(phpalI14443p4_Sw_DataParams_t), &sHal_Nfc_Ic.sHal);
    CHECK_STATUS(status);

    /* Initialize the I14443-B PAL  component */
    status = phpalI14443p3b_Sw_Init(&spalI14443p3b, sizeof(phpalI14443p3b_Sw_DataParams_t), &sHal_Nfc_Ic.sHal);
    CHECK_STATUS(status);
	
    /* Initialize the MIFARE PAL component */
    status = phpalMifare_Sw_Init(&spalMifare, sizeof(phpalMifare_Sw_DataParams_t), &sHal_Nfc_Ic.sHal, NULL);
    CHECK_STATUS(status);

    /* Initialize the keystore component */
    status = phKeyStore_Sw_Init(
                                &sSwkeyStore,
                                sizeof(phKeyStore_Sw_DataParams_t),
                                &sKeyEntries[0],
                                NUMBER_OF_KEYENTRIES,
                                &sKeyVersionPairs[0],
                                NUMBER_OF_KEYVERSIONPAIRS,
                                &sKUCEntries[0],
                                NUMBER_OF_KUCENTRIES);
    CHECK_SUCCESS(status);

    /* Initialize the discover component */
    status = phacDiscLoop_Sw_Init(&sDiscLoop, sizeof(phacDiscLoop_Sw_DataParams_t), &sHal_Nfc_Ic.sHal);
    CHECK_SUCCESS(status);

    /* Load profile for Discovery loop */
    LoadProfile();

    /* load a Key to the Store */
    /* Note: If You use Key number 0x00, be aware that in SAM
      this Key is the 'Host authentication key' !!! */
    status = phKeyStore_FormatKeyEntry(&sSwkeyStore, 1, PH_KEYSTORE_KEY_TYPE_MIFARE);
    CHECK_STATUS(status);

    /* Set Key Store */
    status = phKeyStore_SetKey(&sSwkeyStore, 1, 0, PH_KEYSTORE_KEY_TYPE_MIFARE, &Key[0], 0);
    CHECK_STATUS(status);

    /* Initialize the MIFARE (R) Classic AL component - set NULL because
     * the keys are loaded in E2 by the function */
    /* phKeyStore_SetKey */
    status = phalMfc_Sw_Init(&salMfc, sizeof(phalMfc_Sw_DataParams_t), &spalMifare, &sSwkeyStore);
    CHECK_STATUS(status);

    /* Read the version of the reader IC */
#if defined NXPBUILD__PHHAL_HW_RC523
    status = phhalHw_Rc523_ReadRegister(&sHal_Nfc_Ic.sHal, PHHAL_HW_RC523_REG_VERSION, &bDataBuffer[0]);
    CHECK_SUCCESS(status);
    DEBUG_PRINTF("\nReader chip PN512: 0x%02x\n", bDataBuffer[0]);
#endif
#if defined NXPBUILD__PHHAL_HW_RC663
    status = phhalHw_Rc663_ReadRegister(&sHal_Nfc_Ic.sHal, PHHAL_HW_RC663_REG_VERSION, &bDataBuffer[0]);
    CHECK_SUCCESS(status);
    DEBUG_PRINTF("\nReader chip RC663: 0x%02x\n", bDataBuffer[0]);
#endif

    /* Return Success */
    return PH_ERR_SUCCESS;
}

/***********************************************************************************************
 * \brief   This function demonstrates the MIFARE Classic card detection, Block authentication,
 *          Block Read and Block Write operation.
 * \param   *pParams
 * \return  This function will never return
 **********************************************************************************************/
void NfcrdlibEx4_MIFAREClassic(void *pParams)
{
    phStatus_t  status = 0;
    uint16_t    wTagsDetected = 0;
    uint8_t     bUid[PHAC_DISCLOOP_I3P3A_MAX_UID_LENGTH];
    uint8_t     bUidSize;

    /* Set Interrupt */
    Set_Interrupt();

    /* Initialize library */
    status = NfcRdLibInit();
    CHECK_STATUS(status);

    while(1)    /* Continuous loop */
    {
        DEBUG_PRINTF("\nReady to detect\n");
        
        do
        {
            /* Field OFF */
            status = phhalHw_FieldOff(pHal);
            CHECK_STATUS(status);

            /* Configure Discovery loop for Poll Mode */
            status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_NEXT_POLL_STATE, PHAC_DISCLOOP_POLL_STATE_DETECTION);
            CHECK_STATUS(status);

            /* Run Discovery loop */
            status = phacDiscLoop_Run(&sDiscLoop, PHAC_DISCLOOP_ENTRY_POINT_POLL);

        }while((status & PH_ERR_MASK) != PHAC_DISCLOOP_DEVICE_ACTIVATED); /* Exit on Card detection */

        /* Card detected */
        /* Get the tag types detected info */
        status = phacDiscLoop_GetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_TECH_DETECTED, &wTagsDetected);

        /* Check for Status */
        if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
        {
            /* Check for Type A tag detection */
            if (PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_POS_BIT_MASK_A))
            {
                /* Check for MIFARE Classic */
                if (0x08 == (sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].aSak & 0x08))
                {
                    do
                    {
                        /* Print UID */
                        DEBUG_PRINTF ("\nUID: ");
                        PRINT_BUFF(sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].aUid,
                                sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].bUidSize);

                        /* Print ATQA  and SAK */
                        DEBUG_PRINTF("\nATQA:");
                        PRINT_BUFF(sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].aAtqa, 2);
                        DEBUG_PRINTF ("\nSAK: 0x%x",sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].aSak);

                        /* Print Product type */
                        DEBUG_PRINTF("\nProduct: MIFARE Classic \n");

                        bUidSize = sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].bUidSize;
                        memcpy(bUid, sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].aUid, bUidSize);

                        /* Authenticate with the Key
                         * We can authenticate at any block of a sector and we will get the access to all blocks of the same sector
                         * For example authenticating at block 5, we will get the access to blocks 4, 5, 6 and 7.
                         */
                        /* Send authentication for block 6 */
                        status = phalMfc_Authenticate(&salMfc, 6, PHHAL_HW_MFC_KEYA, 1, 0, bUid, bUidSize);

                        /* Check for Status */
                        if ((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
                        {
                            /* Print Error info */
                            DEBUG_PRINTF("\nAuthentication Failed!!!");
                            DEBUG_PRINTF("\nPlease correct the used key");
                            DEBUG_PRINTF("\nExecution aborted!!!\n");
                            break;
                        }

                        DEBUG_PRINTF("\nAuthentication Successful");

                        /* Empty the bDataBuffer */
                        memset(bDataBuffer, '\0', DATA_BUFFER_LEN);

                        DEBUG_PRINTF("\nRead data from Block 4");

                        /* Read data from block 4 */
                        status = phalMfc_Read(&salMfc, 4, bDataBuffer);

                        /* Check for Status */
                        if (status != PH_ERR_SUCCESS)
                        {
                            /* Print Error info */
                            DEBUG_PRINTF("\nRead operation failed!!!\n");
                            DEBUG_PRINTF("\nExecution aborted!!!\n\n");
                            break; /* Break from the loop*/
                        }

                        DEBUG_PRINTF("\nRead Success");
                        DEBUG_PRINTF("\nThe content of Block 4 is:\n");

                        PRINT_BUFF(&bDataBuffer[0], MFC_BLOCK_DATA_SIZE);
                        DEBUG_PRINTF("\n\n --- End of Read Operation --- \n");

                        DEBUG_PRINTF("\nWrite data to Block 4");

                        /* Write data to block 4 */
                        status = phalMfc_Write(&salMfc, 4, bDataBuffer);

                        /* Check for Status */
                        if (status != PH_ERR_SUCCESS)
                        {
                            /* Print Error info */
                            DEBUG_PRINTF("\nWrite operation failed!!!\n");
                            DEBUG_PRINTF("\nExecution aborted!!!\n");
                            break; /* Break from the loop*/
                        }

                        DEBUG_PRINTF("\nWrite Success");
                        DEBUG_PRINTF("\n\n --- End of Write Operation --- ");

                        /* End of example */
                        DEBUG_PRINTF("\n\n --- End of Example --- \n\n");

                    }while(0);

                    DEBUG_PRINTF("\nPlease Remove the Card\n\n");

                    /* Field RESET */
                    status = phhalHw_FieldReset(pHal);
                    CHECK_STATUS(status);

                    /* Make sure that example application is not detecting the same card continuously */
                    do
                    {
                        /* Send WakeUpA */
                        status = phpalI14443p3a_WakeUpA(sDiscLoop.pPal1443p3aDataParams,
                                                        sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].aAtqa);

                        /* Check for Status */
                        if (status != PH_ERR_SUCCESS)
                        {
                            break; /* Card Removed, break from the loop */
                        }

                        /* Send HaltA */
                        status = phpalI14443p3a_HaltA(sDiscLoop.pPal1443p3aDataParams);
                        CHECK_STATUS(status);

                        /* Delay - 5 milli seconds*/
                        status = phhalHw_Wait(sDiscLoop.pHalDataParams, PHHAL_HW_TIME_MILLISECONDS, 5);
                        CHECK_STATUS(status);

                    }while(1);
                }
            }
        }
    }
}

/*******************************************************************************
**   Main Function
*******************************************************************************/
int main (void)
{
	int ret = 0;

    /* Set the interface link for the internal chip communication */
    ret = Set_Interface_Link();
    if(ret)
    {
    	return 1;
    }

    /* Perform a hardware reset */
    Reset_reader_device();

    /* Print Example application name */
    DEBUG_PRINTF("\n *** MIFARE Classic Example *** \n");

    (void)NfcrdlibEx4_MIFAREClassic(&sDiscLoop);

    return 0;
}

/***********************************************************************************************
 *                            End Of File
 **********************************************************************************************/
