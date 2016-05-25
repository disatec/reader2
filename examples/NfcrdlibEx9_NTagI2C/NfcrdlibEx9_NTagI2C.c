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
 * Example Source for NfcrdlibEx9_NTagI2c that uses the NTag I2C implementation.
 * This example will load/configure Discovery loop for NTag I2C, uses POLL mode to detect the NTag I2C card.
 * Displays detected NTag I2C card information like UID, ATQA, SAK, Product, Version Info, Page Read and Write status.
 * Example will activate the device at index zero whenever multiple NTag I2C cards are detected.

 * Please refer Readme.txt file for Hardware Pin Configuration, Software Configuration and steps to build and
 * execute the project which is present in the same project directory.
 * $Author: jenkins_ cm (nxp92197) $
 * NxpNfcRdLib_R_Gate_V4.010.00.001602_ExC $Revision: 4144 $
 * $Date: 2016-01-14 22:11:33 +0530 (Thu, 14 Jan 2016) $
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
#include <phalMful.h>

#include <phacDiscLoop.h>

#define DATA_BUFFER_LEN         16         /* Buffer length */

/* Tag Version info macros */
#define NTAG_I2C_1K_VERSION     0x13       /* Tag Version: 1k */
#define NTAG_I2C_2K_VERSION     0x15       /* Tag Version: 2k */

#define PTHRU_ON_OFF            (1 << 6)   /* NC_REG Pass-through bit */

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

phalMful_Sw_DataParams_t           salMful;                    /* AL   Mifare UltraLite component */

phacDiscLoop_Sw_DataParams_t       sDiscLoop;                  /* Discovery loop component */

uint8_t                            bDataBuffer[DATA_BUFFER_LEN];  /* universal data buffer */

/** General information bytes to be sent with ATR */
const uint8_t GI[] = { 0x46,0x66,0x6D,
                       0x01,0x01,0x10,       /*VERSION*/
                       0x03,0x02,0x00,0x01,  /*WKS*/
                       0x04,0x01,0xF1        /*LTO*/
                      };

static uint8_t    aData[50];              /* ATR response holder */

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
    CHECK_SUCCESS(status);
	
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

    /* Initialize the discover component */
    status = phacDiscLoop_Sw_Init(&sDiscLoop, sizeof(phacDiscLoop_Sw_DataParams_t), &sHal_Nfc_Ic.sHal);
    CHECK_SUCCESS(status);

    /* Load profile for Discovery loop */
    LoadProfile();

    /* Initialize the Mful (NTAG I2C) AL component */
    status = phalMful_Sw_Init(&salMful, sizeof(phalMful_Sw_DataParams_t), &spalMifare, NULL, NULL, NULL);
    CHECK_STATUS(status);

    /* Read the version of the reader IC */
#if defined NXPBUILD__PHHAL_HW_RC523
    status = phhalHw_Rc523_ReadRegister(&sHal_Nfc_Ic.sHal, PHHAL_HW_RC523_REG_VERSION, &bDataBuffer[0]);
    CHECK_SUCCESS(status);
    DEBUG_PRINTF("\nReader chip PN512 Version: 0x%02x\n", bDataBuffer[0]);
#endif
#if defined NXPBUILD__PHHAL_HW_RC663
    status = phhalHw_Rc663_ReadRegister(&sHal_Nfc_Ic.sHal, PHHAL_HW_RC663_REG_VERSION, &bDataBuffer[0]);
    CHECK_SUCCESS(status);
    DEBUG_PRINTF("\nReader chip RC663 Version: 0x%02x\n", bDataBuffer[0]);
#endif

    /* Return Success */
    return PH_ERR_SUCCESS;
}

/***********************************************************************************************
 * \brief   This function demonstrates the NTag I2C application as mentioned
 *          Performs Type A Card detection and Activation and displays Tag UID, ATQA and SAK info.
 *          Performs Read Tag Version info to confirm that the detected Tag is NTag I2C.
 *          Performs Read Session Registers to check the Tag Mode. We can perform Sector/Page Read Write only in non pass-through mode.
 *          Performs Read operation and Displays content of Sector 0 Pages 5 to 8 Data.
 *          Performs Write operation i.e. writes back the sector 0 Pages 5 to 8 Data.
 * \param   *pParams
 * \return  This function will never return
 **********************************************************************************************/
void NfcrdlibEx9_NTagI2C(void *pParams)
{
    phStatus_t  status = 0;
    uint16_t    wTagsDetected = 0;
    uint8_t     bTagType;
    uint8_t     *pBuff;
    uint16_t    numOfBytes;
    uint8_t     bSectorNum;
    uint8_t     bMoreCardsAvailable;

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
                /* Bit b3 is set to zero, [Digital] 4.8.2 */
                 /* Mask out all other bits except for b7 and b6 */
                 bTagType = (sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].aSak & 0x60);
                 bTagType = bTagType >> 5;

                 /* Check for Type 2 Tag */
                 if (bTagType == PHAC_DISCLOOP_TYPEA_TYPE2_TAG_CONFIG_MASK)
                 {
                    /* Check for NTAG I2C */
                    if ((sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].aAtqa[0] == 0x44)&&
                        (sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].aAtqa[1] == 0x00)&&
                        (sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].aSak == 0x00))
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

                            /* Print Tag info */
                            DEBUG_PRINTF ("\n\nType A T2-tag detected ");

                            /* Read Tag Version
                             * Byte 0: Fixed Header(0x00)
                             * Byte 1: Vendor ID(NXP Semiconductor: 0x04)
                             * Byte 2: Product Type(NTag: 0x04)
                             * Byte 6: Storage size(1k: 0x13 and 2k: 0x15)
                             */
                            status = phalMful_GetVersion(&salMful,&bDataBuffer[0]);

                            /* Check for Error */
                            if ((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
                            {
                                DEBUG_PRINTF ("\nUnable to Read Tag Version information");
                                DEBUG_PRINTF("\n\nExecution aborted!!!\n");
                                break;
                            }

                            /* Check for Tag Info */
                            if((bDataBuffer[0] == 0x00) && (bDataBuffer[1] == 0x04) && (bDataBuffer[2] == 0x04))
                            {
                                DEBUG_PRINTF ("\nProduct: NTag I2C ");
                                if(bDataBuffer[6] == NTAG_I2C_1K_VERSION)
                                {
                                    DEBUG_PRINTF ("1k Version ");
                                }
                                else if(bDataBuffer[6] == NTAG_I2C_2K_VERSION)
                                {
                                    DEBUG_PRINTF ("2k Version ");
                                }
                                else
                                {
                                    DEBUG_PRINTF ("\nInvalid Tag Version");
                                    DEBUG_PRINTF("\n\nExecution aborted!!!\n");
                                    break;
                                }
                            }

                            /* Read Tag Mode */
                            /* Select Sector 3 to Read Session Registers */
                            status = phalMful_SectorSelect(&salMful,3);
                            if ((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
                            {
                                DEBUG_PRINTF ("\nSector 3 selection Failed");
                                DEBUG_PRINTF ("\nUnable to Read Tag Mode");
                                DEBUG_PRINTF("\n\nExecution aborted!!!\n");
                                break;
                            }

                            /* Read Session registers */
                            status = phalMful_Read(&salMful, 0xF8, bDataBuffer);

                            /* check for Error */
                            if ((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
                            {
                                DEBUG_PRINTF ("\nSession Registers Read Failed");
                                DEBUG_PRINTF ("\nUnable to Read Tag Mode");
                                DEBUG_PRINTF("\n\nExecution aborted!!!\n");
                                break;
                            }

                            /* check for Pass-through mode */
                            if ((bDataBuffer[0] & PTHRU_ON_OFF))
                            {
                                DEBUG_PRINTF ("\nTag is Configured in Pass-through Mode");
                                DEBUG_PRINTF ("\nCan not perform Sector/Page Read Write Operation");
                                DEBUG_PRINTF("\n\nExecution aborted!!!\n");
                                break;
                            }

                            /* --- Read Operation --- */
                            /* Select Sector 0 */
                            status = phalMful_SectorSelect(&salMful, 0);

                            /* check for Success */
                            if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
                            {
                                /* Read Sector 0 Page 5 to 8 Data */
                                status = phalMful_FastRead(&salMful, 5, 8, &pBuff, &numOfBytes);

                                /* check for Success */
                                if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
                                {
                                    DEBUG_PRINTF ("\n\nRead Success");

                                    /* Copy Sector0 Page 5 to 6 Data */
                                    memcpy(bDataBuffer, pBuff, numOfBytes);

                                    /* Display contents of Sector 0 Pages 5 to 6 Data */
                                    for (bSectorNum = 5; bSectorNum <= 8; bSectorNum++)
                                    {
                                        DEBUG_PRINTF ("\nSector 0 Page %d Data: ", bSectorNum);
                                        PRINT_BUFF(&bDataBuffer[0 + ((bSectorNum - 5) * 4)], 4);
                                    }
                                }
                            }

                            /* Check for Error */
                            if ((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
                            {
                                DEBUG_PRINTF ("\nRead Operation Failed!!!");
                                DEBUG_PRINTF("\n\nExecution aborted!!!\n");
                                break;
                            }

                            /* End of Read Operation */
                            DEBUG_PRINTF("\n\n --- End of Read Operation --- \n");

                            /* --- Write Operation --- */
                            DEBUG_PRINTF("\nInitiation of Write operation to write back the Sector 0 Pages 5 to 8 Data \n");

                            /* Select Sector 0 */
                            status = phalMful_SectorSelect(&salMful, 0);

                            /* check for Success */
                            if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
                            {
                                /* Write data to Sector 0 Page 5 to 8 */
                                for (bSectorNum = 5; bSectorNum <= 8; bSectorNum++)
                                {
                                    /* Page Write */
                                    status = phalMful_Write(&salMful, bSectorNum, &bDataBuffer[0 + ((bSectorNum - 5) * 4)]);

                                    /* Check for Success */
                                    if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
                                    {
                                        DEBUG_PRINTF("\nWrite Success: Sector 0 Page %d", bSectorNum);
                                    }
                                    else
                                    {
                                        DEBUG_PRINTF ("\nWrite Failed: Sector 0 Page %d ", bSectorNum);
                                        break;
                                    }
                                }
                            }

                            /* Check for Error */
                            if ((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
                            {
                                DEBUG_PRINTF ("\nWrite Operation Failed!!! ");
                                DEBUG_PRINTF("\n\nExecution aborted!!!\n");
                                break;
                            }

                            /* End of Write Operation */
                            DEBUG_PRINTF("\n\n --- End of Write Operation --- \n");

                            /* End of example */
                            DEBUG_PRINTF("\n\n --- End of Example --- \n\n");

                            DEBUG_PRINTF("\nPlease Remove the Card\n\n");

                        }while(0);

                        /* Field RESET */
                        status = phhalHw_FieldReset(pHal);
                        CHECK_STATUS(status);

                        /* Make sure that example application is not detecting the same card continuously */
                        do
                        {
                            /* Activate Card */
                        	status = phpalI14443p3a_ActivateCard(
                            		sDiscLoop.pPal1443p3aDataParams,
                            		sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].aUid,
                            		sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].bUidSize,
                            		sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].aUid,
                                    &sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].bUidSize,
                                    &sDiscLoop.sTypeATargetInfo.aTypeA_I3P3[0].aSak,
                                    &bMoreCardsAvailable);

                            /* Check for Status */
                            if (status != PH_ERR_SUCCESS)
                            {
                                break; /* Card Removed, break from the loop */
                            }

                            /* Send HaltA */
                            phpalI14443p3a_HaltA(sDiscLoop.pPal1443p3aDataParams);

                            /* Delay - 5 milli seconds*/
                            status = phhalHw_Wait(sDiscLoop.pHalDataParams, PHHAL_HW_TIME_MILLISECONDS, 5);
                            CHECK_STATUS(status);

                        }while(1);
                    }
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
    DEBUG_PRINTF("\n *** NTag I2C Example *** \n");

    (void)NfcrdlibEx9_NTagI2C(&sDiscLoop);

    return 0;
}

/***********************************************************************************************
 *                            End Of File
 **********************************************************************************************/
