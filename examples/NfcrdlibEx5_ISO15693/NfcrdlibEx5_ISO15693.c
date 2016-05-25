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
 * Example Source for NfcrdlibEx5_ISO15693 that uses the ISO15693 implementation.
 * This example will load/configure Discovery loop for ISO15693, uses POLL mode to detect the ISO15693 card.
 * Displays detected ISO15693 card information like UID, Card Type, Block Read and Block Write operation status.
 * Example will activate the device at index zero whenever multiple MIFARE Classic cards are detected.
 *
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

/* BAL Headers */
#include <phbalReg.h>

/* PAL Headers */
#include <phpalSli15693.h>

#include <phpalI14443p3a.h>
#include <phpalI14443p4.h>
#include <phpalI14443p3b.h>
#include <phpalI14443p4a.h>
#include <phpalMifare.h>

/* AL Headers */
#include <phalI15693.h>

#include <phacDiscLoop.h>

#define ISO15693_NXP_TAG_ID             0x04    /* ISO15693 NXP Cards specific code */
#define ISO15693_UID_NXP_IDPOS          6       /* ISO15693 NXP Cards specific code position (UID6) */
#define ISO15693_UID_SIZE_BITS    		64      /* ISO15693 UID Size in Bits */

#define ISO15693_MFGID_CARDSEL_IDPOS    4       /* NXP Card Type detection (UID4) */
#define ISO15693_MFGID_CARDSEL_MSK      0x18    /* NXP Card Type detection Mask value */
#define ISO15693_MFGID_CARDSEL_BITPOS   3       /* NXP Card Type detection Bit position in UID4 */

/* NXP Card Type(SLI/SLI-S/SLI-L or SLIX/SLIX-S/SLIX-L) detection (UID5) */
#define ISO15693_UID_CARDSEL_IDPOS      5

/* NXP Card Type states */
#define ISO15693_MFGID_SLI_STATE        0
#define ISO15693_MFGID_SLIX_STATE       2
#define ISO15693_MFGID_SLIX2_STATE      1

/* SLI Card Type states */
#define ISO15693_UID_SLI_STATE          1
#define ISO15693_UID_SLI_S_STATE        2
#define ISO15693_UID_SLI_L_STATE        3

/* SLIX Card Type states */
#define ISO15693_UID_SLIX_STATE         1
#define ISO15693_UID_SLIX_S_STATE       2
#define ISO15693_UID_SLIX_L_STATE       3

/*******************************************************************************
**   Global Variable Declaration
*******************************************************************************/
phbalReg_Stub_DataParams_t          sBalReader;         /* BAL component holder */
phhalHw_Nfc_Ic_DataParams_t         sHal_Nfc_Ic;        /* HAL component holder for Nfc Ic's */
void                                *pHal;              /* HAL pointer */
uint8_t                             bHalBufferTx[128];  /* HAL  TX buffer */
uint8_t                             bHalBufferRx[128];  /* HAL  RX buffer */

    /*PAL variables*/
phpalI14443p3a_Sw_DataParams_t     spalI14443p3a;              /* PAL  I14443-A component */
phpalI14443p4a_Sw_DataParams_t     spalI14443p4a;              /* PAL ISO I14443-4A component */
phpalI14443p3b_Sw_DataParams_t     spalI14443p3b;              /* PAL ISO I14443-B component */
phpalI14443p4_Sw_DataParams_t      spalI14443p4;               /* PAL ISO I14443-4 component */
phpalMifare_Sw_DataParams_t        spalMifare;                 /* PAL  MIFARE component */

phpalSli15693_Sw_DataParams_t       spalSli15693;       /* PAL ISO 15693 component */
phacDiscLoop_Sw_DataParams_t        sDiscLoop;          /* Discovery loop component */
phalI15693_Sw_DataParams_t          salI15693;          /* 15693 SW Data Params */

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
	sDiscLoop.pPalSli15693DataParams = &spalSli15693;
    sDiscLoop.pHalDataParams         = &sHal_Nfc_Ic.sHal;

    /* Passive Bailout bitmap configuration */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_BAIL_OUT, 0x00);
    CHECK_STATUS(status);

    /* Passive poll bitmap configuration. Poll for only Type V(ISO15693) Tags. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_PAS_POLL_TECH_CFG, PHAC_DISCLOOP_POS_BIT_MASK_V);
    CHECK_STATUS(status);

    /* Turn OFF Passive Listen */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_PAS_LIS_TECH_CFG, 0x00);
    CHECK_STATUS(status);

    /* Turn OFF active listen */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ACT_LIS_TECH_CFG, 0x00);
    CHECK_STATUS(status);

    /* Turn OFF Active Poll */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ACT_POLL_TECH_CFG, 0x00);
    CHECK_STATUS(status);

    /* Disable LPCD feature */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ENABLE_LPCD, PH_OFF);
    CHECK_STATUS(status);

    /* Reset collision Pending */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_COLLISION_PENDING, PH_OFF);
    CHECK_STATUS(status);

    /* Whether anti-collision is supported or not. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_ANTI_COLL, PH_ON);
    CHECK_STATUS(status);

    /* Device limit for Type V (ISO 15693) */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_TYPEV_DEVICE_LIMIT, 1);
    CHECK_STATUS(status);

    /* Passive polling Tx Guard times in micro seconds. */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_GTV_VALUE_US, 5000);
    CHECK_STATUS(status);

    /* Discovery loop Operation mode */
    phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_OPE_MODE, RD_LIB_MODE_NFC);

    /* Bailout on Type V (ISO 15693) detect */
    status = phacDiscLoop_SetConfig(&sDiscLoop, PHAC_DISCLOOP_CONFIG_BAIL_OUT, PHAC_DISCLOOP_POS_BIT_MASK_V);
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
#if defined NXPBUILD__PHHAL_HW_RC663
    uint8_t bChipVersion;
#endif
    /* Initialize the Reader BAL (Bus Abstraction Layer) component */
    status = phbalReg_Stub_Init(&sBalReader, sizeof(phbalReg_Stub_DataParams_t));
    CHECK_SUCCESS(status);

    /* Initialize the OSAL Events. */
    status = phOsal_Event_Init();
    CHECK_STATUS(status);

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

    /* Initialize the 15693 PAL component */
    status = phpalSli15693_Sw_Init(&spalSli15693, sizeof(phpalSli15693_Sw_DataParams_t), pHal);
    CHECK_STATUS(status);

    /* Initialize the discover component */
    status = phacDiscLoop_Sw_Init(&sDiscLoop, sizeof(phacDiscLoop_Sw_DataParams_t), &sHal_Nfc_Ic.sHal);
    CHECK_SUCCESS(status);

    /* Load NFC profile for Discovery loop */
    LoadProfile();

    /* Initialize ISO15693 application layer */
    status = phalI15693_Sw_Init(&salI15693, sizeof(salI15693), sDiscLoop.pPalSli15693DataParams);
    CHECK_SUCCESS(status);

    /* Read Chip Version */
#if defined NXPBUILD__PHHAL_HW_RC663
    status = phhalHw_Rc663_ReadRegister(&sHal_Nfc_Ic.sHal, PHHAL_HW_RC663_REG_VERSION, &bChipVersion);
    CHECK_SUCCESS(status);
    DEBUG_PRINTF("\nReader chip RC663: 0x%02x\n", bChipVersion);
#endif

    /* Return Success */
    return PH_ERR_SUCCESS;
}

/***********************************************************************************************
 * \brief   This functions prints the Card type information like SLI, SLIX etc.
 * \param   *pUID   UID Pointer
 * \return  status  Returns the function status
 **********************************************************************************************/
phStatus_t DisplayCardTypeInfo(uint8_t *pUID)
{
    uint8_t bCardType;
    phStatus_t  status = PH_ERR_SUCCESS;

    /* Check for IS)15693 NXP TAG */
    if (pUID[ISO15693_UID_NXP_IDPOS] != ISO15693_NXP_TAG_ID)
    {
        /* Print Product type */
        DEBUG_PRINTF("\nProduct: Non NXP ISO15693 Tag Detected\n");

        /* Return Status */
        return (PH_COMP_PAL_SLI15693 | PH_ERR_INVALID_DATA_PARAMS);
    }

    /* Read SLI Card type information from UID (Byte 4) */
    bCardType = ((pUID[ISO15693_MFGID_CARDSEL_IDPOS] & ISO15693_MFGID_CARDSEL_MSK) >> ISO15693_MFGID_CARDSEL_BITPOS);

    /* Switch based on Card Type(SLI/SLIX/SLIX2) */
    switch (bCardType)
    {
    case ISO15693_MFGID_SLI_STATE:          /* SLI Card state */
        /* Switch based on Card Type(SLI/SLI-S/SLI-L) */
        switch (pUID[ISO15693_UID_CARDSEL_IDPOS])
        {
        case ISO15693_UID_SLI_STATE:        /* SLI Card state */
            /* Print Product type */
            DEBUG_PRINTF("\nProduct: ICODE SLI\n");
            break;

        case ISO15693_UID_SLI_S_STATE:      /* SLI-S Card state */
            /* Print Product type */
            DEBUG_PRINTF("\nProduct: ICODE SLI-S\n");
            break;

        case ISO15693_UID_SLI_L_STATE:      /* SLI-L Card state */
            /* Print Product type */
            DEBUG_PRINTF("\nProduct: ICODE SLI-L\n");
            break;

        default:                            /* default */
            /* Print Product type */
            DEBUG_PRINTF("\nProduct: Unidentified Tag\n");
            /* Update status */
            status = (PH_COMP_PAL_SLI15693 | PH_ERR_INVALID_DATA_PARAMS);
            break;
        }
        break;

    case ISO15693_MFGID_SLIX_STATE:         /* SLIX Card state */
        /* Switch based on Card Type(SLIX/SLIX-S/SLIX-L) */
        switch (pUID[ISO15693_UID_CARDSEL_IDPOS])
        {
        case ISO15693_UID_SLIX_STATE:       /* SLIX Card state */
            /* Print Product type */
            DEBUG_PRINTF("\nProduct: ICODE SLIX\n");
            break;

        case ISO15693_UID_SLIX_S_STATE:     /* SLIX-S Card state */
            /* Print Product type */
            DEBUG_PRINTF("\nProduct: ICODE SLIX-S\n");
            break;

        case ISO15693_UID_SLIX_L_STATE:     /* SLIX-L Card state */
            /* Print Product type */
            DEBUG_PRINTF("\nProduct: ICODE SLIX-L\n");
            break;

        default:                            /* default */
            /* Print Product type */
            DEBUG_PRINTF("\nProduct: Unidentified Tag\n");
            /* Update status */
            status = (PH_COMP_PAL_SLI15693 | PH_ERR_INVALID_DATA_PARAMS);
            break;
        }
        break;

    case ISO15693_MFGID_SLIX2_STATE:        /* SLIX2 Card state */
        /* Print Product type */
        DEBUG_PRINTF("\nProduct: ICODE SLIX2\n");
        break;

    default:                                /* default */
        /* Print Product type */
        DEBUG_PRINTF("\nProduct: Unidentified Tag\n");
        /* Update status */
        status = (PH_COMP_PAL_SLI15693 | PH_ERR_INVALID_DATA_PARAMS);
        break;
    }

    /* Return Status */
    return status;
}

/***********************************************************************************************
 * \brief   This function demonstrates the Type V (ISO 15693) card detection, Block Read and Write operation.
 * \param   *pParams
 * \return  This function will never return
 **********************************************************************************************/
void NfcrdlibEx5_ISO15693(void *pParams)
{
    phStatus_t  status = 0;
    uint16_t    wTagsDetected = 0;
    uint8_t     bBlock = 0x03;
    uint8_t     *pRxbuffer;
    uint16_t    bDataLength;
    uint8_t     aTempUid[8];
    uint8_t     aReceivedUid[8];
    uint8_t		bDsfid = 0;

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
            /* Check for Type V(ISO 15693) tag detection */
            if(PHAC_DISCLOOP_CHECK_ANDMASK(wTagsDetected, PHAC_DISCLOOP_POS_BIT_MASK_V))
            {
                DEBUG_PRINTF("\nType V / ISO 15693 / T5T Detected \n");

                /* Print UID */
                DEBUG_PRINTF ("\nUID: ");
                PRINT_BUFF(sDiscLoop.sTypeVTargetInfo.aTypeV[0].aUid, 0x08);

                /* Copy UID */
                memcpy(aReceivedUid, sDiscLoop.sTypeVTargetInfo.aTypeV[0].aUid, 0x08);

                /* Check and display Card type info */
                if (DisplayCardTypeInfo(sDiscLoop.sTypeVTargetInfo.aTypeV[0].aUid) == PH_ERR_SUCCESS)
                {
                    do
                    {
                        /* Data length */
                        bDataLength = 0x04;

                        /* Block Read */
                        DEBUG_PRINTF("\nRead Data from Block %d", bBlock);

                        /* Read single block */
                        status = phalI15693_ReadSingleBlock(&salI15693,
                                                            PHAL_I15693_OPTION_OFF,
                                                            bBlock,
                                                            &pRxbuffer,
                                                            &bDataLength);
                        /* Check for Status */
                        if(status != PH_ERR_SUCCESS)
                        {
                            /* Print Error info */
                            DEBUG_PRINTF ("\nRead operation Failed!!!");
                            DEBUG_PRINTF("\nExecution aborted!!!\n");
                            break;
                        }

                        /* Read Success */
                        DEBUG_PRINTF("\nRead Success");
                        DEBUG_PRINTF("\nThe content of Block %d is:", bBlock);
                        PRINT_BUFF (pRxbuffer, bDataLength);
                        DEBUG_PRINTF("\n\n --- End of Read Operation ---");

                        /* Block Write */
                        DEBUG_PRINTF("\n\nWrite data to Block %d", bBlock);

                        /* Write single block */
                        status = phalI15693_WriteSingleBlock(&salI15693,
                                                             PHAL_I15693_OPTION_OFF,
                                                             bBlock,
                                                             pRxbuffer,
                                                             bDataLength);
                        /* Check for Status */
                        if(status != PH_ERR_SUCCESS)
                        {
                            /* Print Error info */
                            DEBUG_PRINTF ("\nWrite operation Failed!!!");
                            DEBUG_PRINTF("\nExecution aborted!!!\n");
                            break;
                        }

                        /* Write Success */
                        DEBUG_PRINTF ("\nWrite Success");
                        DEBUG_PRINTF("\n\n --- End of Write Operation ---");

                        DEBUG_PRINTF("\n\n --- End of Example ---\n\n");
                    }while(0);
                }

                DEBUG_PRINTF("\nPlease Remove the Card\n\n");

                /* Field RESET */
                status = phhalHw_FieldReset(pHal);
                CHECK_STATUS(status);

                /* Make sure that example application is not detecting the same card continuously */
                do
                {
                	/* Clear UID buffer */
                	memset(aTempUid, 0x00, 0x08);

                	/* Inventory request */
                    status = phpalSli15693_Inventory(sDiscLoop.pPalSli15693DataParams,
                        (PHPAL_SLI15693_FLAG_NBSLOTS | PHPAL_SLI15693_FLAG_DATA_RATE),
                        0,
                        aReceivedUid,
                        ISO15693_UID_SIZE_BITS,
                        &bDsfid,
                        aTempUid);

                    /* Check for Status */
                    if (status != PH_ERR_SUCCESS)
                    {
                        break; /* Card Removed, break from the loop */
                    }

                    /* Delay - 5 milli seconds*/
                    status = phhalHw_Wait(sDiscLoop.pHalDataParams, PHHAL_HW_TIME_MILLISECONDS, 5);
                    CHECK_STATUS(status);

                }while(1);
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
    DEBUG_PRINTF("\n *** ISO 15693 Example *** \n");

    (void)NfcrdlibEx5_ISO15693(&sDiscLoop);

    return 0;
}

/***********************************************************************************************
 *                            End Of File
 **********************************************************************************************/
