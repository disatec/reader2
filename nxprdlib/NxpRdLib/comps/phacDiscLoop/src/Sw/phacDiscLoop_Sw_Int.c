/*
*         Copyright (c), NXP Semiconductors Bangalore / India
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
* This File contains all support functions that's needed by Discovery Loop
* for performing activities defined by NFC Forum
* as part of the Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 3924 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  PC: Generated 23. Aug 2012
*
*/

/* *****************************************************************************************************************
 * Includes
 * ***************************************************************************************************************** */
#include <ph_RefDefs.h>
#include <phacDiscLoop.h>
#include <phpalI14443p3a.h>
#include <phpalI14443p3b.h>
#include <phpalI18092mPI.h>

#include "phacDiscLoop_Sw_Int_A.h"
#include "phacDiscLoop_Sw_Int_B.h"
#include "phacDiscLoop_Sw_Int_F.h"
#include "phacDiscLoop_Sw_Int_V.h"
#include "phacDiscLoop_Sw_Int_I18000p3m3.h"

#ifdef NXPBUILD__PHAC_DISCLOOP_SW
#include "phacDiscLoop_Sw_Int.h"

/*********************************************************************************/
/** MACRO DEFINATIONS                                                            */
/*********************************************************************************/

/*********************************************************************************/
/** LOCALIZED GLOBAL VARIABLES                                                   */
/*********************************************************************************/
static const uint32_t gPasTechTypeMapTable[] = {
    PHHAL_HW_CARDTYPE_ISO14443A,
    PHHAL_HW_CARDTYPE_ISO14443B,
    PHHAL_HW_CARDTYPE_FELICA_212,
    PHHAL_HW_CARDTYPE_FELICA_424,
    PHHAL_HW_CARDTYPE_ISO15693,
    PHHAL_HW_CARDTYPE_I18000P3M3
};

static const uint32_t gActTechTypeMapTable[] = {
    PHHAL_HW_CARDTYPE_I18092M_ACTIVE_106,
    PHHAL_HW_CARDTYPE_I18092M_ACTIVE_424,
    PHHAL_HW_CARDTYPE_I18092M_ACTIVE_212
};

static const pphacDiscLoop_Sw_Reslns pfColnRelsns[] = {
    &phacDiscLoop_Sw_Int_CollisionResolutionA,
    &phacDiscLoop_Sw_Int_CollisionResolutionB,
    &phacDiscLoop_Sw_Int_CollisionResolutionF,
    &phacDiscLoop_Sw_Int_CollisionResolutionF,
    &phacDiscLoop_Sw_Int_CollisionResolutionV,
    &phacDiscLoop_Sw_Int_CollisionResolutionI18000p3m3
};

static const pphacDiscLoop_Sw_DetTechs pfDetTechs[] = {
    &phacDiscLoop_Sw_DetTechTypeA,
    &phacDiscLoop_Sw_DetTechTypeB,
    &phacDiscLoop_Sw_DetTechTypeF,
    &phacDiscLoop_Sw_DetTechTypeF,
    &phacDiscLoop_Sw_DetTechTypeV,
    &phacDiscLoop_Sw_DetTechTypeI18000p3m3
};

static const pphacDiscLoop_Sw_DeviceActivate pfDeviceActivate[] = {
    &phacDiscLoop_Sw_Int_ActivateA,
    &phacDiscLoop_Sw_Int_ActivateB,
    &phacDiscLoop_Sw_Int_ActivateF,
    &phacDiscLoop_Sw_Int_ActivateF,
    &phacDiscLoop_Sw_Int_ActivateV,
    &phacDiscLoop_Sw_Int_ActivateI18000p3m3
};

/* *****************************************************************************************************************
 * Private Functions
 * ***************************************************************************************************************** */
phStatus_t phacDiscLoop_Sw_Int_ListenMode(
                                          phacDiscLoop_Sw_DataParams_t *pDataParams
                                          )
{
#ifdef NXPBUILD__PHAC_DISCLOOP_TARGET
    phStatus_t PH_MEMLOC_REM status = PH_ERR_INTERNAL_ERROR;
    uint8_t    PH_MEMLOC_REM bRetryCount = pDataParams->sTargetParams.bRetryCount;
    uint16_t   PH_MEMLOC_REM wTempMode = 0x00;

    /* get active listen cfg except F424 */
    wTempMode = pDataParams->bActLisTechCfg & 0x07;

    /* Map Active F424 configuration with Type F bit in Autocoll */
    if(pDataParams->bActLisTechCfg & PHAC_DISCLOOP_POS_BIT_MASK_F424)
    {
        wTempMode = wTempMode | PHAC_DISCLOOP_POS_BIT_MASK_F212;
    }

    wTempMode = (uint16_t) (wTempMode << 8);

    /* get passive listen cfg except F424 */
    wTempMode = wTempMode | (pDataParams->bPasLisTechCfg & 0x07);

    /* Map Passice F424 configuration with Type F bit in Autocoll */
    if(pDataParams->bPasLisTechCfg & PHAC_DISCLOOP_POS_BIT_MASK_F424)
    {
        wTempMode = wTempMode | PHAC_DISCLOOP_POS_BIT_MASK_F212;
    }

    /* AUTOCOLL retry loop */
    do
    {
        status = phhalHw_Autocoll(
            pDataParams->pHalDataParams,
            wTempMode,
            &pDataParams->sTargetParams.pRxBuffer,
            &pDataParams->sTargetParams.wRxBufferLen,
            &pDataParams->sTargetParams.wProtParams);
    }while(((status & PH_ERR_MASK) != PH_ERR_SUCCESS) && (bRetryCount--));

    /* Return RF OFF error, if external RF is OFF */
    if((status & PH_ERR_MASK) == PH_ERR_EXT_RF_ERROR)
    {
        return PH_ADD_COMPCODE(PHAC_DISCLOOP_EXTERNAL_RFOFF, PH_COMP_AC_DISCLOOP);
    }
    else
    {
        /* Return, if error */
        PH_CHECK_SUCCESS(status);
    }

    return PH_ADD_COMPCODE(PHAC_DISCLOOP_ACTIVATED_BY_PEER, PH_COMP_AC_DISCLOOP);
#else /* NXPBUILD__PHAC_DISCLOOP_TARGET */
    return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_AC_DISCLOOP);
#endif /* NXPBUILD__PHAC_DISCLOOP_TARGET */
}

phStatus_t phacDiscLoop_Sw_Int_Stop(
                                    phacDiscLoop_Sw_DataParams_t *pDataParams
                                    )
{
    /* RFU */
    pDataParams = pDataParams;
    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);
}

phStatus_t phacDiscLoop_Sw_Int_ActivePollMode(
                                              phacDiscLoop_Sw_DataParams_t *pDataParams
                                              )
{
#if defined (NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS)    \
        || defined (NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS)
    phStatus_t PH_MEMLOC_REM   status;
#endif
    uint8_t    PH_MEMLOC_COUNT bIndex;
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS
    uint8_t    PH_MEMLOC_BUF   aNfcId3[10];
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS */

    /* Reset detected technologies */
    pDataParams->bDetectedTechs = 0x00;

    /* Loop through all the supported active technologies */
    for (bIndex = 0; bIndex < PHAC_DISCLOOP_ACT_POLL_MAX_TECHS_SUPPORTED; bIndex++)
    {
        switch(pDataParams->bActPollTechCfg & (PH_ON << bIndex))
        {
            case PHAC_DISCLOOP_ACT_POS_BIT_MASK_106:
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS
                /* Configure for active communication at 106 kbps */
                status = phhalHw_ApplyProtocolSettings(
                    pDataParams->pHalDataParams,
                    PHHAL_HW_CARDTYPE_I18092M_ACTIVE_106);
                PH_CHECK_SUCCESS(status);

                /* Perform I-RFCA and Switch on RF Field after Apply protocol settings and wait for guard time, if in Active mode. */
                PH_CHECK_SUCCESS_FCT(status, phacDiscLoop_Sw_Int_FieldOn(pDataParams));
                PH_CHECK_SUCCESS_FCT(status, phhalHw_Wait(pDataParams->pHalDataParams, PHHAL_HW_TIME_MICROSECONDS, pDataParams->wActPollGTimeUs));

                /* Send ATR Request */
                status = phpalI18092mPI_Atr(
                    pDataParams->pPal18092mPIDataParams,
                    pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aUid,
                    pDataParams->sTypeATargetInfo.sTypeA_P2P.bDid,
                    pDataParams->sTypeATargetInfo.sTypeA_P2P.bLri,
                    pDataParams->sTypeATargetInfo.sTypeA_P2P.bNadEnable,
                    pDataParams->sTypeATargetInfo.sTypeA_P2P.bNad,
                    pDataParams->sTypeATargetInfo.sTypeA_P2P.pGi,
                    pDataParams->sTypeATargetInfo.sTypeA_P2P.bGiLength,
                    pDataParams->sTypeATargetInfo.sTypeA_P2P.pAtrRes,
                    &(pDataParams->sTypeATargetInfo.sTypeA_P2P.bAtrResLength));
                if((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
                {
                    /* Set the corresponding detected bit */
                    pDataParams->bDetectedTechs |= PH_ON << bIndex;
                    return PH_ADD_COMPCODE(PHAC_DISCLOOP_ACTIVE_TARGET_ACTIVATED, PH_COMP_AC_DISCLOOP);
                }

                /* If Integrity error occurred then switch to Passive Polling without polling for higher Active Data rates. */
                if ((status & PH_ERR_MASK) == PH_ERR_INTEGRITY_ERROR)
                {
                    return PH_ADD_COMPCODE(PHAC_DISCLOOP_NO_TECH_DETECTED, PH_COMP_AC_DISCLOOP);
                }
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS */
                break;

            case PHAC_DISCLOOP_ACT_POS_BIT_MASK_212:
            case PHAC_DISCLOOP_ACT_POS_BIT_MASK_424:
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS

                /* Field OFF required between Active poll modes. Wait for recovery time and then apply other protocol settings. */
                PH_CHECK_SUCCESS_FCT(status, phhalHw_FieldOff(pDataParams->pHalDataParams));
                PH_CHECK_SUCCESS_FCT(status, phhalHw_Wait(pDataParams->pHalDataParams, PHHAL_HW_TIME_MICROSECONDS, pDataParams->wActPollGTimeUs));

                /* Configure for active communication at 212 or 424 kbps */
                if((PH_ON << bIndex) == PHAC_DISCLOOP_ACT_POS_BIT_MASK_212)
                {
                    status = phhalHw_ApplyProtocolSettings(
                        pDataParams->pHalDataParams,
                        PHHAL_HW_CARDTYPE_I18092M_ACTIVE_212);
                }
                else
                {
                    status = phhalHw_ApplyProtocolSettings(
                        pDataParams->pHalDataParams,
                        PHHAL_HW_CARDTYPE_I18092M_ACTIVE_424);
                }
                PH_CHECK_SUCCESS(status);

                /* Perform I-RFCA and Switch on RF Field after Apply protocol settings and wait for guard time, if in Active mode. */
                PH_CHECK_SUCCESS_FCT(status, phacDiscLoop_Sw_Int_FieldOn(pDataParams));
                PH_CHECK_SUCCESS_FCT(status, phhalHw_Wait(pDataParams->pHalDataParams, PHHAL_HW_TIME_MICROSECONDS, pDataParams->wActPollGTimeUs));

                /* Form NFCID3 */
                memcpy(aNfcId3, pDataParams->sTypeFTargetInfo.aTypeFTag[0].aIDmPMm, PHAC_DISCLOOP_FELICA_IDM_LENGTH);  /* PRQA S 3200 */
                aNfcId3[8] = 0x00;
                aNfcId3[9] = 0x00;

                /* Send ATR Request */
                status = phpalI18092mPI_Atr(
                    pDataParams->pPal18092mPIDataParams,
                    aNfcId3,
                    pDataParams->sTypeFTargetInfo.sTypeF_P2P.bDid,
                    pDataParams->sTypeFTargetInfo.sTypeF_P2P.bLri,
                    pDataParams->sTypeFTargetInfo.sTypeF_P2P.bNadEnable,
                    pDataParams->sTypeFTargetInfo.sTypeF_P2P.bNad,
                    pDataParams->sTypeFTargetInfo.sTypeF_P2P.pGi,
                    pDataParams->sTypeFTargetInfo.sTypeF_P2P.bGiLength,
                    pDataParams->sTypeFTargetInfo.sTypeF_P2P.pAtrRes,
                    &(pDataParams->sTypeFTargetInfo.sTypeF_P2P.bAtrResLength));
                if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
                {
                    /* Set the corresponding detected bit */
                    pDataParams->bDetectedTechs |= PH_ON << bIndex;
                    return PH_ADD_COMPCODE(PHAC_DISCLOOP_ACTIVE_TARGET_ACTIVATED, PH_COMP_AC_DISCLOOP);
                }

                /* If Integrity error occurred then switch to Passive Polling without polling for higher Active Data rates. */
                if ((status & PH_ERR_MASK) == PH_ERR_INTEGRITY_ERROR)
                {
                    return PH_ADD_COMPCODE(PHAC_DISCLOOP_NO_TECH_DETECTED, PH_COMP_AC_DISCLOOP);
                }
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS */
                break;

            default:
                break;
        }
    }

    /* No Target present. */
    return PH_ADD_COMPCODE(PHAC_DISCLOOP_NO_TECH_DETECTED, PH_COMP_AC_DISCLOOP);
}

phStatus_t phacDiscLoop_Sw_Int_PollMode(
                                        phacDiscLoop_Sw_DataParams_t *pDataParams
                                       )
{
    phStatus_t PH_MEMLOC_REM status;
    uint8_t    PH_MEMLOC_REM bResolveTech;
    uint8_t    PH_MEMLOC_REM bTechType;
    uint8_t    PH_MEMLOC_REM bNumOfTechsFound = 0;

    status = PH_ERR_INVALID_PARAMETER;

    if(pDataParams->bPollState == PHAC_DISCLOOP_POLL_STATE_DETECTION)
    {
        if((pDataParams->bOpeMode == RD_LIB_MODE_NFC) ||
           (pDataParams->bOpeMode == RD_LIB_MODE_ISO))
        {
            /* Perform Technology detection Activity */
            status = phacDiscLoop_Sw_Int_TechDetectActivity(
                pDataParams,
                pDataParams->bPasPollBailOut,
                pDataParams->bPasPollTechCfg,
                &bNumOfTechsFound);
        }
        else if(pDataParams->bOpeMode == RD_LIB_MODE_EMVCO)
        {
            /* Perform Technology detection Activity */
            status = phacDiscLoop_Sw_Int_EmvcoTechDetectActivity(
                pDataParams,
                pDataParams->bPasPollTechCfg,
                &bNumOfTechsFound);
        }
        else
        {
            /* Do Nothing */
        }

        if ((pDataParams->bLpcdEnabled) &&
           (pDataParams->bPasPollTechCfg) &&
           ((status & PH_ERR_MASK) == PHAC_DISCLOOP_NO_TECH_DETECTED))
        {
            /* LPCD is success but card presence does not exist/errors */
            return PH_ADD_COMPCODE(PHAC_DISCLOOP_LPCD_NO_TECH_DETECTED, PH_COMP_AC_DISCLOOP);
        }

        if((status & PH_ERR_MASK) == PHAC_DISCLOOP_TECH_DETECTED)
        {
            pDataParams->bPollState = PHAC_DISCLOOP_POLL_STATE_COLLISION_RESOLUTION;
        }
    }

    /* Go for collision resolution if single tech found */
    if(pDataParams->bPollState == PHAC_DISCLOOP_POLL_STATE_COLLISION_RESOLUTION)
    {
        /* Get Technology to be resolved */
        bResolveTech = pDataParams->bDetectedTechs & pDataParams->bPasPollTechCfg;
        if(bResolveTech)
        {
            if(bResolveTech & (uint8_t)(bResolveTech - 1))
            {
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_AC_DISCLOOP);
            }
        }
        else
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_AC_DISCLOOP);
        }

        PHAC_DISCLOOP_GET_BIT_POS(bResolveTech, bTechType);

        status = phacDiscLoop_Sw_Int_ColsnReslnActivity(pDataParams, (bTechType - 1));
        if((status & PH_ERR_MASK) == PHAC_DISCLOOP_DEVICE_RESOLVED)
        {
            /* Activation */
            status = phacDiscLoop_Sw_Int_ActivateDevice(pDataParams, (bTechType - 1), 0x00);
        }
    }

    if(pDataParams->bPollState == PHAC_DISCLOOP_POLL_STATE_REMOVAL)
    {
        if(pDataParams->bOpeMode == RD_LIB_MODE_EMVCO)
        {
            status = phacDiscLoop_Sw_Int_EmvcoRemovalProcedure(pDataParams);
        }
    }
    return PH_ADD_COMPCODE(status, PH_COMP_AC_DISCLOOP);
}

phStatus_t phacDiscLoop_Sw_Int_EmvcoRemovalProcedure(
                                                     phacDiscLoop_Sw_DataParams_t * pDataParams
                                                     )
{
#if (defined NXPBUILD__PHAC_DISCLOOP_TYPEB_I3P3B_TAGS) && (defined NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P3_TAGS)

    phStatus_t PH_MEMLOC_REM   status;
    uint8_t    PH_MEMLOC_COUNT bPollingCount = 0;

    PH_CHECK_SUCCESS_FCT(status, phhalHw_FieldOff(pDataParams->pHalDataParams));

    PH_CHECK_SUCCESS_FCT(status, phhalHw_Wait(
        pDataParams->pHalDataParams,
        PHHAL_HW_TIME_MICROSECONDS,
        PHAC_DISCLOOP_EMVCO_RESET_DELAY)
        );

    /* Set poll state to Detection */
    pDataParams->bPollState = PHAC_DISCLOOP_POLL_STATE_DETECTION;

    if(PHAC_DISCLOOP_CHECK_ANDMASK(pDataParams->bDetectedTechs, PHAC_DISCLOOP_POS_BIT_MASK_A))
    {
        pDataParams->bDetectedTechs = 0x00;

        PH_CHECK_SUCCESS_FCT(status, phhalHw_ApplyProtocolSettings(
            pDataParams->pHalDataParams,
            PHHAL_HW_CARDTYPE_ISO14443A)
            );

        /* Perform Field ON after Apply protocol settings. */
        PH_CHECK_SUCCESS_FCT(status, phacDiscLoop_Sw_Int_RfcaOff_FieldOn(pDataParams));

        while(bPollingCount < PHAC_DISCLOOP_EMVCO_RETRY_COUNT)
        {
            PH_CHECK_SUCCESS_FCT(status, phhalHw_Wait(
                pDataParams->pHalDataParams,
                PHHAL_HW_TIME_MICROSECONDS,
                pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_A])
                );

            status = phpalI14443p3a_WakeUpA(
                pDataParams->pPal1443p3aDataParams,
                pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aAtqa);
            if ((status & PH_ERR_MASK) != PH_ERR_IO_TIMEOUT)
            {
                status = phpalI14443p3a_HaltA(pDataParams->pPal1443p3aDataParams);
                bPollingCount = 0;
            }
            else
            {
                bPollingCount++;
            }
        }/* while(bPollingCount < Max_Retry_Count) */
    }
    else
    {
        pDataParams->bDetectedTechs = 0x00;

        PH_CHECK_SUCCESS_FCT(status, phhalHw_ApplyProtocolSettings(
            pDataParams->pHalDataParams,
            PHHAL_HW_CARDTYPE_ISO14443B)
            );

        /* Perform Field ON after Apply protocol settings. */
        PH_CHECK_SUCCESS_FCT(status, phacDiscLoop_Sw_Int_RfcaOff_FieldOn(pDataParams));
        while(bPollingCount < PHAC_DISCLOOP_EMVCO_RETRY_COUNT)
        {
            PH_CHECK_SUCCESS_FCT(status, phhalHw_Wait(
                pDataParams->pHalDataParams,
                PHHAL_HW_TIME_MICROSECONDS,
                pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_B])
                );

            status = phpalI14443p3b_WakeUpB(
                pDataParams->pPal1443p3bDataParams,
                0,
                0,
                0,
                pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].aAtqB,
                &pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].bAtqBLength);
            if ((status & PH_ERR_MASK) != PH_ERR_IO_TIMEOUT)
            {
                bPollingCount = 0;
            }
            else
            {
                bPollingCount++;
            }
        }/* while(bPollingCount < Max_Retry_Count) */
    }/* else */
    if ((status & PH_ERR_MASK) != PH_ERR_IO_TIMEOUT)
    {
        return status;
    }
    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);
#else
    return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_AC_DISCLOOP);
#endif
}

phStatus_t phacDiscLoop_Sw_Int_TechDetectActivity(
                                                  phacDiscLoop_Sw_DataParams_t *pDataParams,
                                                  uint8_t bPasPollBailOut,
                                                  uint8_t bDetectConfig,
                                                  uint8_t* pNumOfTechsDetect
                                                  )
{
    phStatus_t PH_MEMLOC_REM   status = PHAC_DISCLOOP_NO_TECH_DETECTED;
    uint8_t    PH_MEMLOC_COUNT bTechIndex;
    uint8_t    PH_MEMLOC_REM   bNumOfTechsFound = 0;

    pDataParams->bDetectedTechs = 0x00;
    pDataParams->bNumOfCards = 0x00;

    /* Poll for requested NFC-Tech Type presence. */
    for (bTechIndex = 0; bTechIndex < PHAC_DISCLOOP_PASS_POLL_MAX_TECHS_SUPPORTED; bTechIndex++)
    {
        /* In NFC mode, poll proprietary technologies only if NFC technologies are not detected */
        if((pDataParams->bOpeMode == RD_LIB_MODE_NFC) && (bNumOfTechsFound)
           && (bTechIndex == PHAC_DISCLOOP_POLL_MAX_NFC_TECHS_SUPPORTED))
        {
            break;
        }

        if (bDetectConfig & (PH_ON << bTechIndex))
        {
            /* Apply Protocol Setting for Selected Tech  */
            PH_CHECK_SUCCESS_FCT(status, phhalHw_ApplyProtocolSettings(pDataParams->pHalDataParams,
                (uint8_t)(gPasTechTypeMapTable[bTechIndex] & 0xFF)));

            /* Perform I-RFCA and Switch on RF Field after Apply protocol settings, if in NFC and ISO mode. */
            PH_CHECK_SUCCESS_FCT(status, phacDiscLoop_Sw_Int_FieldOn(pDataParams));

            if(PHAC_DISCLOOP_TECH_TYPE_F424 == bTechIndex)
            {
#if (defined NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS) || (defined NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS)
                PH_CHECK_SUCCESS_FCT(status, phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TYPEF_CUR_BAUD, PHAC_DISCLOOP_CON_BITR_424));
#else
                return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_AC_DISCLOOP);
#endif
            }
            else if(PHAC_DISCLOOP_TECH_TYPE_F212 == bTechIndex)
            {
#if (defined NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS) || (defined NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS)
                PH_CHECK_SUCCESS_FCT(status, phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TYPEF_CUR_BAUD, PHAC_DISCLOOP_CON_BITR_212));
#else
                return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_AC_DISCLOOP);
#endif
            }
            else
            {
                /* Do nothing */
            }

            /* Perform Tech detection. */
            status = pfDetTechs[bTechIndex](pDataParams);

            if ((status & PH_ERR_MASK) == PHAC_DISCLOOP_TECH_DETECTED)
            {
                /*Set the corresponding detected bit. */
                pDataParams->bDetectedTechs |= PH_ON << bTechIndex;
                bNumOfTechsFound++;
                /* Since Device detected at 212 Baud, Skip polling at 424 */
                if((pDataParams->bDetectedTechs & PHAC_DISCLOOP_POS_BIT_MASK_F212) && (bDetectConfig & PHAC_DISCLOOP_POS_BIT_MASK_F424))
                {
                    *pNumOfTechsDetect = bNumOfTechsFound;
                    bTechIndex++;
                }
            }

            if( (bPasPollBailOut & (PH_ON << bTechIndex)) && bNumOfTechsFound )
            {
                /* Bailout set, Returning to application */
                *pNumOfTechsDetect = bNumOfTechsFound;
                break;
            }
        }
    }

    if (bNumOfTechsFound == 1)
    {
        *pNumOfTechsDetect = bNumOfTechsFound;
        return PH_ADD_COMPCODE(PHAC_DISCLOOP_TECH_DETECTED, PH_COMP_AC_DISCLOOP);
    }
    else if(bNumOfTechsFound > 1)
    {
        *pNumOfTechsDetect = bNumOfTechsFound;
        return PH_ADD_COMPCODE(PHAC_DISCLOOP_MULTI_TECH_DETECTED, PH_COMP_AC_DISCLOOP);
    }
    else if(PH_ERR_IO_TIMEOUT == (status & PH_ERR_MASK))
    {
        return PH_ADD_COMPCODE(PHAC_DISCLOOP_NO_TECH_DETECTED, PH_COMP_AC_DISCLOOP);
    }
    else
    {
        return status; /* Other Failure */
    }
}

phStatus_t phacDiscLoop_Sw_Int_EmvcoTechDetectActivity(
                                                  phacDiscLoop_Sw_DataParams_t *pDataParams,
                                                  uint8_t bDetectConfig,
                                                  uint8_t* pNumOfTechsDetect
                                                  )
{
#if (defined NXPBUILD__PHAC_DISCLOOP_TYPEB_I3P3B_TAGS) && (defined NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P3_TAGS)

    phStatus_t PH_MEMLOC_REM status = PHAC_DISCLOOP_NO_TECH_DETECTED;
    uint8_t    PH_MEMLOC_REM bNumOfTechsFound = 0;
    uint8_t    PH_MEMLOC_REM bStopTechDetection = PH_OFF;

    pDataParams->bDetectedTechs = 0x00;

    /* EMVCo: Run until a tag is discovered */
    while(!bStopTechDetection )
    {
        /* Is Polling for Type A is enable*/
        if(bDetectConfig & PHAC_DISCLOOP_POS_BIT_MASK_A)
        {
            if(!(pDataParams->bDetectedTechs & PHAC_DISCLOOP_POS_BIT_MASK_A))
            {
                /* Apply Protocol Setting for Selected Technology  */
                PH_CHECK_SUCCESS_FCT(status, phhalHw_ApplyProtocolSettings(
                    pDataParams->pHalDataParams,
                    (uint8_t)(gPasTechTypeMapTable[0] & 0xFF)));

                /* Disable RFCA and switch on RF, if in EMV mode. */
                PH_CHECK_SUCCESS_FCT(status, phacDiscLoop_Sw_Int_RfcaOff_FieldOn(pDataParams));

                /* Perform Tech detection. */
                status = pfDetTechs[0](pDataParams);
                if ((status & PH_ERR_MASK) == PHAC_DISCLOOP_TECH_DETECTED)
                {
                    /*Set the corresponding detected bit. */
                    pDataParams->bDetectedTechs |= PHAC_DISCLOOP_POS_BIT_MASK_A;
                    bNumOfTechsFound++;
                }
            }
            else
            {
                bStopTechDetection = PH_ON;
            }
        }

        if((bDetectConfig & PHAC_DISCLOOP_POS_BIT_MASK_B) && (!bStopTechDetection))
        {
            if(!(pDataParams->bDetectedTechs & PHAC_DISCLOOP_POS_BIT_MASK_B))
            {
                /* Apply Protocol Setting for Selected Technology  */
                PH_CHECK_SUCCESS_FCT(status, phhalHw_ApplyProtocolSettings(
                    pDataParams->pHalDataParams,
                    (uint8_t)(gPasTechTypeMapTable[1] & 0xFF))
                    );

                /* Perform Field ON after Apply protocol settings. */
                PH_CHECK_SUCCESS_FCT(status, phacDiscLoop_Sw_Int_RfcaOff_FieldOn(pDataParams));

                /* Perform Technology detection. */
                status = pfDetTechs[1](pDataParams);
                if ((status & PH_ERR_MASK) == PHAC_DISCLOOP_TECH_DETECTED)
                {
                    /*Set the corresponding detected bit. */
                    pDataParams->bDetectedTechs |= PHAC_DISCLOOP_POS_BIT_MASK_B;
                    bNumOfTechsFound++;
                }
            }
            else
            {
                bStopTechDetection = PH_ON;
            }
        }

        if(bNumOfTechsFound == 0)
        {
            bStopTechDetection = PH_ON;
        }
    } /* End of while(!bStopTechDetection ) */

    if (bNumOfTechsFound == 1)
    {
        *pNumOfTechsDetect = bNumOfTechsFound;
        status = PHAC_DISCLOOP_TECH_DETECTED;
    }
    else if(bNumOfTechsFound > 1)
    {
        *pNumOfTechsDetect = bNumOfTechsFound;
        status = PHAC_DISCLOOP_MULTI_TECH_DETECTED;
    }
    else if(PH_ERR_IO_TIMEOUT == (status & PH_ERR_MASK))
    {
        status = PHAC_DISCLOOP_NO_TECH_DETECTED;
    }
    else
    {
        /* Do Nothing */
    }
    return PH_ADD_COMPCODE(status, PH_COMP_AC_DISCLOOP);
#else
    return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_AC_DISCLOOP);
#endif
}

phStatus_t phacDiscLoop_Sw_Int_ColsnReslnActivity(
                                                  phacDiscLoop_Sw_DataParams_t *pDataParams,
                                                  uint8_t bTechType
                                                  )
{
    phStatus_t PH_MEMLOC_REM wStatus;

    /* Configure HW for the selected technology */
    wStatus = phhalHw_ApplyProtocolSettings(
        pDataParams->pHalDataParams,
        (uint8_t)(gPasTechTypeMapTable[bTechType] & 0xFF));

    if( wStatus != PH_ERR_SUCCESS)
    {
        return wStatus;
    }

    /* Since we are interested in one Technology, Reset DetectedTechs information */
    pDataParams->bDetectedTechs = 0x00;

    /*Re-set number of card*/
    pDataParams->bNumOfCards = 0x00;

    /* Call the selected collision resolution function */
    wStatus = pfColnRelsns[bTechType](pDataParams);
    if((wStatus & PH_ERR_MASK) == PH_ERR_SUCCESS)
    {
        /* Collision Resolution activity is successful, update Detected Technology */
        pDataParams->bDetectedTechs = (PH_ON << bTechType);

        if(pDataParams->bNumOfCards > 1)
        {
            return PH_ADD_COMPCODE(PHAC_DISCLOOP_MULTI_DEVICES_RESOLVED, PH_COMP_AC_DISCLOOP);
        }
        else
        {
            return PH_ADD_COMPCODE(PHAC_DISCLOOP_DEVICE_RESOLVED, PH_COMP_AC_DISCLOOP);
        }
    }

    if((wStatus & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT)
    {
        return PH_ADD_COMPCODE(PHAC_DISCLOOP_NO_DEVICE_RESOLVED, PH_COMP_AC_DISCLOOP);
    }

    return wStatus;
}

phStatus_t phacDiscLoop_Sw_Int_ActivateDevice(
                                              phacDiscLoop_Sw_DataParams_t * pDataParams,
                                              uint8_t bTechType,
                                              uint8_t bTagIndex
                                              )
{
    phStatus_t PH_MEMLOC_REM status;

    /* Since we are interested in one Technology, Reset DetectedTechs information */
    pDataParams->bDetectedTechs = 0x00;

    /* Activate selected tag */
    status = pfDeviceActivate[bTechType](pDataParams, bTagIndex);
    if((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
    {
        /* Set Corresponding Tech Bit Position */
        pDataParams->bDetectedTechs = PH_ON << bTechType;
        return PH_ADD_COMPCODE(PHAC_DISCLOOP_DEVICE_ACTIVATED, PH_COMP_AC_DISCLOOP);
    }
    else if(((status & PH_ERR_MASK) == PHAC_DISCLOOP_MERGED_SEL_RES_FOUND)
       || ((status & PH_ERR_MASK) == PHAC_DISCLOOP_PASSIVE_TARGET_ACTIVATED))
    {
        /* Set Corresponding Tech Bit Position */
        pDataParams->bDetectedTechs = PH_ON << bTechType;
    }
    else
    {
        /* Do nothing */
    }

    return status;
}

phStatus_t phacDiscLoop_Sw_Int_RfcaOff_FieldOn(
                                               phacDiscLoop_Sw_DataParams_t * pDataParams
                                               )
{
    phStatus_t PH_MEMLOC_REM status = PH_ERR_SUCCESS;

    /* Disable RFCA (if supported) */
    status = phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RFCA, PH_OFF);
    if (((status & PH_ERR_MASK) != PH_ERR_UNSUPPORTED_PARAMETER) && ((status & PH_ERR_MASK) != PH_ERR_SUCCESS))
    {
        return status;
    }

    /* Switch on RF without performing I-RFCA. */
    PH_CHECK_SUCCESS_FCT(status, phhalHw_FieldOn(pDataParams->pHalDataParams));

    return status;
}

phStatus_t phacDiscLoop_Sw_Int_FieldOn(
                                       phacDiscLoop_Sw_DataParams_t * pDataParams
                                       )
{
    phStatus_t PH_MEMLOC_REM status = PH_ERR_SUCCESS;

    /* Enable RFCA (if supported) */
    status = phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RFCA, PH_ON);
    if(((status & PH_ERR_MASK) != PH_ERR_UNSUPPORTED_PARAMETER) && ((status & PH_ERR_MASK) != PH_ERR_SUCCESS))
    {
        return status;
    }

    /* Perform I-RFCA and Switch on RF Field. */
    status = phhalHw_FieldOn(pDataParams->pHalDataParams);
    if((status & PH_ERR_MASK) == PH_ERR_RF_ERROR)
    {
        /* External RF is ON */
        return PH_ADD_COMPCODE(PHAC_DISCLOOP_EXTERNAL_RFON, PH_COMP_AC_DISCLOOP);
    }
    else
    {
        return PH_ADD_COMPCODE(status, PH_COMP_AC_DISCLOOP);
    }
}

uint8_t phacDiscLoop_Sw_Int_IsValidPollStatus(
                                              phStatus_t wStatus
                                              )
{
    if((((wStatus) & PH_ERR_MASK) == PH_ERR_SUCCESS)         ||
       (((wStatus) & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR) ||
       (((wStatus) & PH_ERR_MASK) == PH_ERR_FRAMING_ERROR)   ||
       (((wStatus) & PH_ERR_MASK) == PH_ERR_INTEGRITY_ERROR) ||
       (((wStatus) & PH_ERR_MASK) == PH_ERR_PROTOCOL_ERROR)
       )
    {
        return PH_ON;
    }
    else
    {
        return PH_OFF;
    }
}

#endif /* NXPBUILD__PHAC_DISCLOOP_SW */
