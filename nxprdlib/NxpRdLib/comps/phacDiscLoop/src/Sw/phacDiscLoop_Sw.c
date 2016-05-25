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
* This File contains all functions that's needed by Discovery Loop
* for Starting the discovery loop, Set & Get Configuration and Card Activation,
* as part of the Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 3924 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  PC: Generated 23. Aug 2012
*
*/
#include <ph_Status.h>
#include <ph_RefDefs.h>
#include <phacDiscLoop.h>
#include <phhalHw.h>
#include <phOsal.h>

#include <phpalI14443p3a.h>
#include <phpalI14443p3b.h>
#include <phpalI14443p4a.h>
#include <phpalI14443p4.h>
#include <phpalFelica.h>
#include <phpalI18000p3m3.h>
#include <phpalSli15693.h>

#ifdef NXPBUILD__PHAC_DISCLOOP_SW
#include "phacDiscLoop_Sw.h"
#include "phacDiscLoop_Sw_Int.h"

phStatus_t phacDiscLoop_Sw_Init(
                                phacDiscLoop_Sw_DataParams_t * pDataParams,
                                uint16_t wSizeOfDataParams,
                                void * pHalDataParams
                                )
{
    if (sizeof(phacDiscLoop_Sw_DataParams_t) != wSizeOfDataParams)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_DATA_PARAMS, PH_COMP_AC_DISCLOOP);
    }

    PH_ASSERT_NULL(pDataParams);

    /* Initialize the private data */
    pDataParams->wId                      = PH_COMP_AC_DISCLOOP | PHAC_DISCLOOP_SW_ID;
    pDataParams->pHalDataParams           = pHalDataParams;
    pDataParams->pPal1443p3aDataParams    = NULL;
    pDataParams->pPal1443p3bDataParams    = NULL;
    pDataParams->pPal14443p4DataParams    = NULL;
    pDataParams->pPalFelicaDataParams     = NULL;
    pDataParams->pPal1443p4aDataParams    = NULL;
    pDataParams->pPalSli15693DataParams   = NULL;
    pDataParams->pPal18000p3m3DataParams  = NULL;
    pDataParams->pAl18000p3m3DataParams   = NULL;
    pDataParams->pPal18092mPIDataParams   = NULL;
    pDataParams->pAlT1TDataParams         = NULL;

    /* Default Operation Mode is NFC */
    pDataParams->bOpeMode                 = RD_LIB_MODE_NFC;
    pDataParams->bNfcActivityVersion      = PHAC_DISCLOOP_NFC_ACTIVITY_VERSION_1_1;

    /* Default listen and poll mode technologies */
    pDataParams->bPollState               = PHAC_DISCLOOP_POLL_STATE_DETECTION;

#ifdef NXPBUILD__PHHAL_HW_RC523
    /* Poll for Type A, Type B, Type F212 and F424 */
    pDataParams->bPasPollTechCfg          = 0x0F;
#else /* NXPBUILD__PHHAL_HW_RC523 */
    /* Poll for Type A, Type B, Type F212 and F424, Type V(15693), 18000p3m3 */
    pDataParams->bPasPollTechCfg          = 0x3F;

#endif /* NXPBUILD__PHHAL_HW_RC523 */

#ifdef NXPBUILD__PHHAL_HW_RC663
    pDataParams->bActLisTechCfg           = 0x00;
    pDataParams->bPasLisTechCfg           = 0x00;
    pDataParams->bActPollTechCfg          = 0x00;
#else /* NXPBUILD__PHHAL_HW_RC663 */
    /* Poll for Type A, Type F212 and F424 */
    pDataParams->bActPollTechCfg          = 0x07;

    /* Listen for Type A, Type F212 and F424 */
    pDataParams->bActLisTechCfg           = 0x0D;
    pDataParams->bPasLisTechCfg           = 0x0D;
#endif /* NXPBUILD__PHHAL_HW_RC663 */

    pDataParams->bUseAntiColl             = PH_ON;
    pDataParams->bLpcdEnabled             = PH_OFF;

    pDataParams->bNumOfCards              = 0x00;
    pDataParams->bDetectedTechs           = 0x00;
    pDataParams->bCollPend                = 0x00;

    /* Poll device limits */
    pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_A]           = PHAC_DISCLOOP_TYPEA_DEFAULT_DEVICE_LIMIT;
    pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_B]           = PHAC_DISCLOOP_TYPEB_DEFAULT_DEVICE_LIMIT;
    pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_F212]        = PHAC_DISCLOOP_TYPEF_DEFAULT_DEVICE_LIMIT;
    pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_F424]        = PHAC_DISCLOOP_TYPEF_DEFAULT_DEVICE_LIMIT;
    pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_V]           = PHAC_DISCLOOP_TYPEV_DEFAULT_DEVICE_LIMIT;
    pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_18000P3M3]   = PHAC_DISCLOOP_I18000P3M3_DEFAULT_DEVICE_LIMIT;

    /* Guard times */
    pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_A]         = PHAC_DISCLOOP_TYPEA_DEFAULT_GT;
    pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_B]         = PHAC_DISCLOOP_TYPEB_DEFAULT_GT;
    pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_V]         = PHAC_DISCLOOP_TYPEV_DEFAULT_GT;
    pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_18000P3M3] = PHAC_DISCLOOP_I18000P3M3_DEFAULT_GT;
    /* Guard time GTFB */
    pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_F212]      = PHAC_DISCLOOP_TYPEF_DEFAULT_GT;
    /* Guard time GTBF */
    pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_F424]      = PHAC_DISCLOOP_B_TO_F_DEFAULT_GT;
    pDataParams->wActPollGTimeUs                                     = PHAC_DISCLOOP_TYPEA_DEFAULT_GT;

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS
    pDataParams->sTypeATargetInfo.bTotalTagsFound        = 0;

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS
    pDataParams->sTypeATargetInfo.sTypeA_P2P.bDid        = 0;
    pDataParams->sTypeATargetInfo.sTypeA_P2P.bLri        = 0;
    pDataParams->sTypeATargetInfo.sTypeA_P2P.bNadEnable  = 0;
    pDataParams->sTypeATargetInfo.sTypeA_P2P.bNad        = 0;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS
    pDataParams->sTypeATargetInfo.sTypeA_I3P4.bFsdi  = 0x08;
    pDataParams->sTypeATargetInfo.sTypeA_I3P4.bCid   = 0x00;
    pDataParams->sTypeATargetInfo.sTypeA_I3P4.bDri   = 0x00;
    pDataParams->sTypeATargetInfo.sTypeA_I3P4.bDsi   = 0x00;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS */
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS
    pDataParams->sTypeBTargetInfo.bTotalTagsFound   = 0x00;
    pDataParams->sTypeBTargetInfo.bAfiReq           = 0x00;
    pDataParams->sTypeBTargetInfo.bFsdi             = 0x08;
    pDataParams->sTypeBTargetInfo.bCid              = 0x00;
    pDataParams->sTypeBTargetInfo.bDri              = 0x00;
    pDataParams->sTypeBTargetInfo.bDsi              = 0x00;
    pDataParams->sTypeBTargetInfo.bExtendedAtqBbit  = 0x00;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS
    pDataParams->sTypeFTargetInfo.bTotalTagsFound  = 0x00;
    pDataParams->sTypeFTargetInfo.aSystemCode[0]   = 0xFF;
    pDataParams->sTypeFTargetInfo.aSystemCode[1]   = 0xFF;
    pDataParams->sTypeFTargetInfo.bTimeSlot        = PHPAL_FELICA_NUMSLOTS_4;

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS
    pDataParams->sTypeFTargetInfo.sTypeF_P2P.bDid        = 0;
    pDataParams->sTypeFTargetInfo.sTypeF_P2P.bLri        = 0;
    pDataParams->sTypeFTargetInfo.sTypeF_P2P.bNadEnable  = 0;
    pDataParams->sTypeFTargetInfo.sTypeF_P2P.bNad        = 0;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS */
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS
    pDataParams->sTypeVTargetInfo.bTotalTagsFound  = 0x00;
    pDataParams->sTypeVTargetInfo.bFlag            = PHPAL_SLI15693_FLAG_DATA_RATE;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_I18000P3M3_TAGS
    pDataParams->sI18000p3m3TargetInfo.bTotalTagsFound  = 0;
    pDataParams->sI18000p3m3TargetInfo.bM               = PHPAL_I18000P3M3_M_MANCHESTER_4;
    pDataParams->sI18000p3m3TargetInfo.bDr              = PHPAL_I18000P3M3_LF_847KHZ;
#endif /* NXPBUILD__PHAC_DISCLOOP_I18000P3M3_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TARGET
    pDataParams->sTargetParams.bRetryCount = 0;
#endif /* NXPBUILD__PHAC_DISCLOOP_TARGET */

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);
}

phStatus_t phacDiscLoop_Sw_Run(
                               phacDiscLoop_Sw_DataParams_t * pDataParams,
                               uint8_t bEntryPoint
                               )
{
    phStatus_t PH_MEMLOC_REM wDiscloopStatus;

    /* Disable Emd Check for Emvco */
    if(pDataParams->bOpeMode == RD_LIB_MODE_EMVCO)
    {
        PH_CHECK_SUCCESS_FCT(wDiscloopStatus, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_SET_EMD, PH_OFF));
    }

    switch (bEntryPoint)
    {
    case PHAC_DISCLOOP_ENTRY_POINT_POLL:

        /* Perform LPCD if Enabled. */
        if ((pDataParams->bLpcdEnabled)
            && (pDataParams->bPollState == PHAC_DISCLOOP_POLL_STATE_DETECTION))
        {
            wDiscloopStatus = phhalHw_Lpcd(pDataParams->pHalDataParams);
            if ((wDiscloopStatus & PH_ERR_MASK) != PH_ERR_SUCCESS)
            {
                if ((wDiscloopStatus & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT)
                {
                    return PH_ADD_COMPCODE(PHAC_DISCLOOP_NO_TECH_DETECTED, PH_COMP_AC_DISCLOOP); /* No card presence. */
                }

                return wDiscloopStatus; /* Other error. */
            }
        }

        /* Check for active poll configuration */
        if((pDataParams->bActPollTechCfg)
           && (pDataParams->bPollState == PHAC_DISCLOOP_POLL_STATE_DETECTION))
        {
            wDiscloopStatus = phacDiscLoop_Sw_Int_ActivePollMode(pDataParams);
            /* Continue with passive polling, if no peer detected */
            if((wDiscloopStatus & PH_ERR_MASK) != PHAC_DISCLOOP_NO_TECH_DETECTED)
            {
                if(((wDiscloopStatus & PH_ERR_MASK) != PHAC_DISCLOOP_ACTIVE_TARGET_ACTIVATED) &&
                    ((wDiscloopStatus & PH_ERR_MASK) != PHAC_DISCLOOP_EXTERNAL_RFON))
                {
                    pDataParams->wErrorCode = wDiscloopStatus;
                    return PH_ADD_COMPCODE(PHAC_DISCLOOP_FAILURE, PH_COMP_AC_DISCLOOP);
                }
                return wDiscloopStatus;
            }
            /* Field OFF after Active polling and wait for recovery time. */
            PH_CHECK_SUCCESS_FCT(wDiscloopStatus, phhalHw_FieldOff(pDataParams->pHalDataParams));
            PH_CHECK_SUCCESS_FCT(wDiscloopStatus, phhalHw_Wait(pDataParams->pHalDataParams, PHHAL_HW_TIME_MICROSECONDS, pDataParams->wActPollGTimeUs));
        }

        /* Check for passive poll configuration */
        if(pDataParams->bPasPollTechCfg)
        {
            /* Perform the Poll operation and store the Status code. */
            wDiscloopStatus = phacDiscLoop_Sw_Int_PollMode(pDataParams);

            /* If error is from below layer, store it and return failure */
            if((wDiscloopStatus & PH_COMP_MASK) != PH_COMP_AC_DISCLOOP)
            {
                pDataParams->wErrorCode = wDiscloopStatus;
                return PH_ADD_COMPCODE(PHAC_DISCLOOP_FAILURE, PH_COMP_AC_DISCLOOP);
            }
            return wDiscloopStatus;
        }
        else
        {
            pDataParams->bDetectedTechs = 0x00;
            pDataParams->bNumOfCards = 0x00;
            return PH_ADD_COMPCODE(PHAC_DISCLOOP_NO_TECH_DETECTED, PH_COMP_AC_DISCLOOP);
        }

    case PHAC_DISCLOOP_ENTRY_POINT_LISTEN:
        /* Check for listen configurations */
        if ((pDataParams->bPasLisTechCfg) || (pDataParams->bActLisTechCfg))
        {
            /* Turn OFF RF field if already on. */
            PH_CHECK_SUCCESS_FCT(wDiscloopStatus, phhalHw_FieldOff(pDataParams->pHalDataParams));

            /* Perform listen operation and store the Status code. */
            wDiscloopStatus = phacDiscLoop_Sw_Int_ListenMode(pDataParams);

            /* If error is from below layer, store it and return failure */
            if((wDiscloopStatus & PH_COMP_MASK) != PH_COMP_AC_DISCLOOP)
            {
                pDataParams->wErrorCode = wDiscloopStatus;
                return PH_ADD_COMPCODE(PHAC_DISCLOOP_FAILURE, PH_COMP_AC_DISCLOOP);
            }
            return wDiscloopStatus;
        }
        else
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_AC_DISCLOOP);
        }
        break;

    default:
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_AC_DISCLOOP);
    }
}

phStatus_t phacDiscLoop_Sw_SetConfig(
                                     phacDiscLoop_Sw_DataParams_t * pDataParams,
                                     uint16_t wConfig,
                                     uint16_t wValue
                                     )
{
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;

    switch(wConfig)
    {
    case PHAC_DISCLOOP_CONFIG_ACT_LIS_TECH_CFG:
        pDataParams->bActLisTechCfg = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_ACT_POLL_TECH_CFG:
        pDataParams->bActPollTechCfg = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_PAS_LIS_TECH_CFG:
        pDataParams->bPasLisTechCfg = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_COLLISION_PENDING:
        pDataParams->bCollPend = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_NEXT_POLL_STATE:
        if(!(((wValue & 0xFF) == PHAC_DISCLOOP_POLL_STATE_DETECTION) ||
            ((wValue & 0xFF) == PHAC_DISCLOOP_POLL_STATE_COLLISION_RESOLUTION) ||
            ((wValue & 0xFF) == PHAC_DISCLOOP_POLL_STATE_REMOVAL)))
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->bPollState = (phacDiscLoop_Sw_PollState_t)(wValue & 0xFF);
        break;

    case PHAC_DISCLOOP_CONFIG_PAS_POLL_TECH_CFG:
        pDataParams->bPasPollTechCfg = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_ANTI_COLL:
        pDataParams->bUseAntiColl = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_ACTIVITY_VERSION:
        if((wValue != PHAC_DISCLOOP_NFC_ACTIVITY_VERSION_1_0)
           && (wValue != PHAC_DISCLOOP_NFC_ACTIVITY_VERSION_1_1))
        {
            return PH_ADD_COMPCODE(
                 PH_ERR_INVALID_PARAMETER,
                 PH_COMP_AC_DISCLOOP);
        }
        pDataParams->bNfcActivityVersion = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_GTA_VALUE_US:
        pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_A] = wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_GTB_VALUE_US:
        pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_B] = wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_GTFB_VALUE_US:
        if(! (wValue >= PHAC_DISCLOOP_TYPEF_DEFAULT_GT))
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_F212] = wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_GTBF_VALUE_US:
        if(! (wValue >= PHAC_DISCLOOP_B_TO_F_DEFAULT_GT))
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP
                );
        }
        pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_F424] = wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_GTV_VALUE_US:
        pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_V] = wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_GT18000P3M3_VALUE_US:
        pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_18000P3M3] = wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_GT_ACTIVE_US:
        pDataParams->wActPollGTimeUs = wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_BAIL_OUT:
        if (wValue & ~(PHAC_DISCLOOP_POS_BIT_MASK_A |  PHAC_DISCLOOP_POS_BIT_MASK_B
            | PHAC_DISCLOOP_POS_BIT_MASK_F424 | PHAC_DISCLOOP_POS_BIT_MASK_F212
            | PHAC_DISCLOOP_POS_BIT_MASK_V))
        {
            /* Invalid bail out option set */
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }

        if ((wValue & PHAC_DISCLOOP_POS_BIT_MASK_A) &&
            !(pDataParams->bPasPollTechCfg & PHAC_DISCLOOP_POS_BIT_MASK_A))
        {
            /* Invalid to bail out of Type A if detection for Type A is
               not set */
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }

        if ((wValue & PHAC_DISCLOOP_POS_BIT_MASK_B) &&
            !(pDataParams->bPasPollTechCfg & PHAC_DISCLOOP_POS_BIT_MASK_B))
        {
            /* Invalid to bail out of Type B if detection for Type B is
               not set */
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }

        if ((wValue & PHAC_DISCLOOP_POS_BIT_MASK_F424) &&
            !(pDataParams->bPasPollTechCfg & PHAC_DISCLOOP_POS_BIT_MASK_F424))
        {
            /* Invalid to bail out of Type F if detection for Type F424 is
               not set */
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }

        if ((wValue & PHAC_DISCLOOP_POS_BIT_MASK_F212) &&
            !(pDataParams->bPasPollTechCfg & PHAC_DISCLOOP_POS_BIT_MASK_F212))
        {
            /* Invalid to bail out of Type F if detection for Type F212 is
               not set */
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }

        if ((wValue & PHAC_DISCLOOP_POS_BIT_MASK_V) &&
            !(pDataParams->bPasPollTechCfg & PHAC_DISCLOOP_POS_BIT_MASK_V))
        {
            /* Invalid to bail out of Type V if detection for Type V is
               not set */
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }

        pDataParams->bPasPollBailOut = (uint8_t) wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_OPE_MODE:
        if(!((wValue == RD_LIB_MODE_NFC) ||
            (wValue == RD_LIB_MODE_EMVCO) ||
            (wValue == RD_LIB_MODE_ISO)))
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->bOpeMode = (uint8_t)wValue;
        PH_CHECK_SUCCESS_FCT(status, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_OPE_MODE, wValue));
#ifdef NXPBUILD__PHPAL_I14443P4_SW
        PH_CHECK_SUCCESS_FCT(status, phpalI14443p4_SetConfig(pDataParams->pPal14443p4DataParams, PHPAL_I14443P4_CONFIG_OPE_MODE, wValue));
#endif /* NXPBUILD__PHPAL_I14443P4_SW */

#ifdef NXPBUILD__PHPAL_I14443P3B_SW
        PH_CHECK_SUCCESS_FCT(status, phpalI14443p3b_SetConfig(pDataParams->pPal1443p3bDataParams, PHPAL_I14443P3B_CONFIG_OPE_MODE, wValue));
#endif /* NXPBUILD__PHPAL_I14443P3B_SW */

#ifdef NXPBUILD__PHPAL_I14443P3A_SW
        PH_CHECK_SUCCESS_FCT(status, phpalI14443p3a_SetConfig(pDataParams->pPal1443p3aDataParams, PHPAL_I14443P3A_CONFIG_OPE_MODE, wValue));
#endif /* NXPBUILD__PHPAL_I14443P3A_SW */

#ifdef NXPBUILD__PHPAL_I14443P4A_SW
        PH_CHECK_SUCCESS_FCT(status, phpalI14443p4a_SetConfig(pDataParams->pPal1443p4aDataParams, PHPAL_I14443P4A_CONFIG_OPE_MODE, wValue));
#endif /* NXPBUILD__PHPAL_I14443P4A_SW */
        break;

    case PHAC_DISCLOOP_CONFIG_ENABLE_LPCD:
        pDataParams->bLpcdEnabled = (uint8_t)wValue;
        break;

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS
    case PHAC_DISCLOOP_CONFIG_TYPEA_DEVICE_LIMIT:
        if((! (pDataParams->bPasPollTechCfg & PHAC_DISCLOOP_POS_BIT_MASK_A)) ||
           ((wValue & 0xFF) > PHAC_DISCLOOP_CFG_MAX_CARDS_SUPPORTED))
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_A] = (uint8_t)wValue;
        break;

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS
    case PHAC_DISCLOOP_CONFIG_TYPEA_I3P4_FSDI:
            if (wValue > 8)
            {
                return PH_ADD_COMPCODE(
                    PH_ERR_INVALID_PARAMETER,
                    PH_COMP_AC_DISCLOOP);
            }
            pDataParams->sTypeATargetInfo.sTypeA_I3P4.bFsdi = (uint8_t)wValue;
            break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_I3P4_CID:
        if (wValue > PHAC_DISCLOOP_SW_I3P4_MAX_CID)
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sTypeATargetInfo.sTypeA_I3P4.bCid = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_I3P4_DRI:
        if (wValue > PHAC_DISCLOOP_SW_I3P4_MAX_DRI)
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sTypeATargetInfo.sTypeA_I3P4.bDri = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_I3P4_DSI:
        if (wValue > PHAC_DISCLOOP_SW_I3P4_MAX_DSI)
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sTypeATargetInfo.sTypeA_I3P4.bDsi = (uint8_t)wValue;
        break;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS
    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_DID:
        if (wValue > 14)
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sTypeATargetInfo.sTypeA_P2P.bDid = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_LRI:
        if (wValue > 3)
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sTypeATargetInfo.sTypeA_P2P.bLri = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_NAD_ENABLE:
        if (wValue == 0)
        {
            pDataParams->sTypeATargetInfo.sTypeA_P2P.bNadEnable = PH_OFF;
        }
        else
        {
            pDataParams->sTypeATargetInfo.sTypeA_P2P.bNadEnable = PH_ON;
        }
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_NAD:
        if (wValue > 255)
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sTypeATargetInfo.sTypeA_P2P.bNad = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_GI_LEN:
        if (wValue > 255)
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sTypeATargetInfo.sTypeA_P2P.bGiLength = (uint8_t)wValue;
        break;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS */
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS
    case PHAC_DISCLOOP_CONFIG_TYPEB_DEVICE_LIMIT:
    if((! (pDataParams->bPasPollTechCfg & PHAC_DISCLOOP_POS_BIT_MASK_B))||
       ((wValue & 0xFF) > PHAC_DISCLOOP_CFG_MAX_CARDS_SUPPORTED))
    {
        return PH_ADD_COMPCODE(
            PH_ERR_INVALID_PARAMETER,
            PH_COMP_AC_DISCLOOP);
    }
    pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_B] = (uint8_t)wValue;
    break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_AFI_REQ:
      pDataParams->sTypeBTargetInfo.bAfiReq = (uint8_t)wValue;
      break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_EXTATQB:
      if (wValue == 0)
      {
          pDataParams->sTypeBTargetInfo.bExtendedAtqBbit = 0;
      }
      else
      {
          pDataParams->sTypeBTargetInfo.bExtendedAtqBbit = 1;
      }
      break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_FSDI:
      if (wValue > 0xff)
      {
          return PH_ADD_COMPCODE(
              PH_ERR_INVALID_PARAMETER,
              PH_COMP_AC_DISCLOOP);
      }
      pDataParams->sTypeBTargetInfo.bFsdi = (uint8_t)wValue;
      break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_CID:
      if (wValue > PHAC_DISCLOOP_SW_I3P4_MAX_CID)
      {
          return PH_ADD_COMPCODE(
              PH_ERR_INVALID_PARAMETER,
              PH_COMP_AC_DISCLOOP);
      }
      pDataParams->sTypeBTargetInfo.bCid = (uint8_t)wValue;
      break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_DRI:
      if (wValue > PHAC_DISCLOOP_SW_I3P4_MAX_DRI)
      {
          return PH_ADD_COMPCODE(
              PH_ERR_INVALID_PARAMETER,
              PH_COMP_AC_DISCLOOP);
      }
      pDataParams->sTypeBTargetInfo.bDri = (uint8_t)wValue;
      break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_DSI:
      if (wValue > PHAC_DISCLOOP_SW_I3P4_MAX_DSI)
      {
          return PH_ADD_COMPCODE(
              PH_ERR_INVALID_PARAMETER,
              PH_COMP_AC_DISCLOOP);
      }
      pDataParams->sTypeBTargetInfo.bDsi = (uint8_t)wValue;
      break;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS
    case PHAC_DISCLOOP_CONFIG_TYPEF_CUR_BAUD:
        pDataParams->bFelicaBaud = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_DEVICE_LIMIT:
        if((! (pDataParams->bPasPollTechCfg & (PHAC_DISCLOOP_POS_BIT_MASK_F212 | PHAC_DISCLOOP_POS_BIT_MASK_F424)))||
             ((wValue & 0xFF) > PHAC_DISCLOOP_CFG_MAX_CARDS_SUPPORTED))
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_F212] = (uint8_t)wValue;
        break;

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS
    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_DID:
        if (wValue > 14)
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }

        pDataParams->sTypeFTargetInfo.sTypeF_P2P.bDid = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_LRI:
        if (wValue > 3)
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sTypeFTargetInfo.sTypeF_P2P.bLri = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_NAD_ENABLE:
        pDataParams->sTypeFTargetInfo.sTypeF_P2P.bNadEnable = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_NAD:
        if (wValue > 255)
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sTypeFTargetInfo.sTypeF_P2P.bNad = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_GI_LEN:
        if (wValue > 255)
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sTypeFTargetInfo.sTypeF_P2P.bGiLength = (uint8_t)wValue;
        break;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS */
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS
    case PHAC_DISCLOOP_CONFIG_TYPEV_DEVICE_LIMIT:
        if((!(pDataParams->bPasPollTechCfg & PHAC_DISCLOOP_POS_BIT_MASK_V)) ||
           ((wValue & 0xFF) > PHAC_DISCLOOP_CFG_MAX_CARDS_SUPPORTED))
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_V] = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEV_RX_DATA_RATE:
        if((wValue != PHHAL_HW_RF_RX_DATARATE_HIGH)
           && (wValue != PHHAL_HW_RF_RX_DATARATE_LOW))
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }

        /* Set request flag to indicate high data rate */
        if(wValue == PHHAL_HW_RF_RX_DATARATE_HIGH)
        {
            pDataParams->sTypeVTargetInfo.bFlag = PHPAL_SLI15693_FLAG_DATA_RATE;
        }
        else
        {
            pDataParams->sTypeVTargetInfo.bFlag = 0;
        }
        break;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_I18000P3M3_TAGS
    case PHAC_DISCLOOP_CONFIG_18000P3M3_DEVICE_LIMIT:
        if((!(pDataParams->bPasPollTechCfg & PHAC_DISCLOOP_POS_BIT_MASK_18000P3M3)) ||
           ((wValue & 0xFF) > PHAC_DISCLOOP_CFG_MAX_CARDS_SUPPORTED))
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_18000P3M3] = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_I18000P3M3_MODULATION:
        if((wValue != PHPAL_I18000P3M3_M_MANCHESTER_2)
           && (wValue != PHPAL_I18000P3M3_M_MANCHESTER_4))
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sI18000p3m3TargetInfo.bM = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_I18000P3M3_FREQUENCY:
        if((wValue != PHPAL_I18000P3M3_LF_423KHZ)
           && (wValue != PHPAL_I18000P3M3_LF_847KHZ))
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sI18000p3m3TargetInfo.bDr = (uint8_t)wValue;
        break;
#endif /* NXPBUILD__PHAC_DISCLOOP_I18000P3M3_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TARGET
    case PHAC_DISCLOOP_CONFIG_TARGET_RETRY_COUNT:
        if (wValue > 255)
        {
            return PH_ADD_COMPCODE(
                PH_ERR_INVALID_PARAMETER,
                PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sTargetParams.bRetryCount = (uint8_t)wValue;
        break;
#endif /* NXPBUILD__PHAC_DISCLOOP_TARGET */

    default:
        status = PH_ADD_COMPCODE(
            PH_ERR_INVALID_PARAMETER,
            PH_COMP_AC_DISCLOOP);
    }

    return status;
}

phStatus_t phacDiscLoop_Sw_GetConfig(
                                     phacDiscLoop_Sw_DataParams_t * pDataParams,
                                     uint16_t wConfig,
                                     uint16_t * pValue
                                     )
{
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;

    switch(wConfig)
    {
    case PHAC_DISCLOOP_CONFIG_PAS_LIS_TECH_CFG:
        *pValue = pDataParams->bPasLisTechCfg;
        break;

    case PHAC_DISCLOOP_CONFIG_ACT_LIS_TECH_CFG:
        *pValue = pDataParams->bActLisTechCfg;
        break;

    case PHAC_DISCLOOP_CONFIG_ACT_POLL_TECH_CFG:
        *pValue = pDataParams->bActPollTechCfg;
        break;

    case PHAC_DISCLOOP_CONFIG_COLLISION_PENDING:
        *pValue = pDataParams->bCollPend;
        break;

    case PHAC_DISCLOOP_CONFIG_NEXT_POLL_STATE:
        *pValue = (uint8_t)pDataParams->bPollState;
        break;

    case PHAC_DISCLOOP_CONFIG_ANTI_COLL:
        *pValue = pDataParams->bUseAntiColl;
        break;

    case PHAC_DISCLOOP_CONFIG_TECH_DETECTED:
        *pValue = pDataParams->bDetectedTechs;
        break;

    case PHAC_DISCLOOP_CONFIG_NR_TAGS_FOUND:
        *pValue = pDataParams->bNumOfCards;
         break;

    case PHAC_DISCLOOP_CONFIG_BAIL_OUT:
        *pValue = pDataParams->bPasPollBailOut;
        break;

    case PHAC_DISCLOOP_CONFIG_PAS_POLL_TECH_CFG:
        *pValue = pDataParams->bPasPollTechCfg;
        break;

    case PHAC_DISCLOOP_CONFIG_ENABLE_LPCD:
        *pValue = pDataParams->bLpcdEnabled;
        break;

    case PHAC_DISCLOOP_CONFIG_ACTIVITY_VERSION:
        *pValue = pDataParams->bNfcActivityVersion;
        break;

    case PHAC_DISCLOOP_CONFIG_ADDITIONAL_INFO:
        *pValue = pDataParams->wErrorCode;
        break;

    case PHAC_DISCLOOP_CONFIG_GT_ACTIVE_US:
        *pValue = pDataParams->wActPollGTimeUs;
        break;

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS
    case PHAC_DISCLOOP_CONFIG_GTA_VALUE_US:
        *pValue = pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_A];
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_TAGS_FOUND:
        *pValue = pDataParams->sTypeATargetInfo.bTotalTagsFound;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_DEVICE_LIMIT:
        *pValue = pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_A];
        break;

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS
    case PHAC_DISCLOOP_CONFIG_TYPEA_I3P4_FSDI:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_I3P4.bFsdi;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_I3P4_CID:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_I3P4.bCid;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_I3P4_DSI:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_I3P4.bDsi;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_I3P4_DRI:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_I3P4.bDri;
        break;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS
    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_DID:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_P2P.bDid;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_LRI:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_P2P.bLri;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_NAD_ENABLE:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_P2P.bNadEnable;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_NAD:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_P2P.bNad;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_GI_LEN:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_P2P.bGiLength;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_ATR_RES_LEN:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_P2P.bAtrResLength;
        break;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS */
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS
    case PHAC_DISCLOOP_CONFIG_GTB_VALUE_US:
        *pValue = pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_B];
        break;

     case PHAC_DISCLOOP_CONFIG_TYPEB_TAGS_FOUND:
        *pValue = pDataParams->sTypeBTargetInfo.bTotalTagsFound;
        break;

     case PHAC_DISCLOOP_CONFIG_TYPEB_DEVICE_LIMIT:
        *pValue = pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_B];
        break;
 #endif /* NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS
     case PHAC_DISCLOOP_CONFIG_GTFB_VALUE_US:
         *pValue = pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_F212];
         break;

     case PHAC_DISCLOOP_CONFIG_GTBF_VALUE_US:
         *pValue = pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_F424];
         break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_TAGS_FOUND:
        *pValue = pDataParams->sTypeFTargetInfo.bTotalTagsFound;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_DEVICE_LIMIT:
        *pValue = pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_F212];
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_CUR_BAUD:
         *pValue = pDataParams->bFelicaBaud;
         break;

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS
    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_DID:
        *pValue = pDataParams->sTypeFTargetInfo.sTypeF_P2P.bDid;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_LRI:
        *pValue = pDataParams->sTypeFTargetInfo.sTypeF_P2P.bLri;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_NAD_ENABLE:
        *pValue = pDataParams->sTypeFTargetInfo.sTypeF_P2P.bNadEnable;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_NAD:
        *pValue = pDataParams->sTypeFTargetInfo.sTypeF_P2P.bNad;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_GI_LEN:
        *pValue = pDataParams->sTypeFTargetInfo.sTypeF_P2P.bGiLength;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_ATR_RES_LEN:
        *pValue = pDataParams->sTypeFTargetInfo.sTypeF_P2P.bAtrResLength;
        break;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS */
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS
    case PHAC_DISCLOOP_CONFIG_GTV_VALUE_US:
        *pValue = pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_V];
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEV_DEVICE_LIMIT:
        *pValue = pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_V];
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEV_TAGS_FOUND:
        *pValue = pDataParams->sTypeVTargetInfo.bTotalTagsFound;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEV_RX_DATA_RATE:
        if(pDataParams->sTypeVTargetInfo.bFlag == PHPAL_SLI15693_FLAG_DATA_RATE)
        {
            *pValue = PHHAL_HW_RF_RX_DATARATE_HIGH;
        }
        else
        {
            *pValue = PHHAL_HW_RF_RX_DATARATE_LOW;
        }
        break;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_I18000P3M3_TAGS
    case PHAC_DISCLOOP_CONFIG_GT18000P3M3_VALUE_US:
        *pValue = pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_18000P3M3];
        break;

    case PHAC_DISCLOOP_CONFIG_18000P3M3_DEVICE_LIMIT:
        *pValue = pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_18000P3M3];
        break;

    case PHAC_DISCLOOP_CONFIG_18000P3M3_TAGS_FOUND:
        *pValue = pDataParams->sI18000p3m3TargetInfo.bTotalTagsFound;
        break;

    case PHAC_DISCLOOP_CONFIG_I18000P3M3_MODULATION:
        *pValue = pDataParams->sI18000p3m3TargetInfo.bM;
        break;

    case PHAC_DISCLOOP_CONFIG_I18000P3M3_FREQUENCY:
        *pValue = pDataParams->sI18000p3m3TargetInfo.bDr;
        break;
#endif /* NXPBUILD__PHAC_DISCLOOP_I18000P3M3_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TARGET
    case PHAC_DISCLOOP_CONFIG_TARGET_RETRY_COUNT:
        *pValue = pDataParams->sTargetParams.bRetryCount;
        break;
#endif /* NXPBUILD__PHAC_DISCLOOP_TARGET */

    default:
        status = PH_ADD_COMPCODE(
            PH_ERR_INVALID_PARAMETER,
            PH_COMP_AC_DISCLOOP);
    }

    return status;
}

phStatus_t phacDiscLoop_Sw_ActivateCard(
                                        phacDiscLoop_Sw_DataParams_t * pDataParams,
                                        uint8_t bTechType,
                                        uint8_t bTagIndex
                                        )
{
    return phacDiscLoop_Sw_Int_ActivateDevice(pDataParams, bTechType, bTagIndex);
}
#endif  /* NXPBUILD__PHAC_DISCLOOP_SW */
