/*
 *                    Copyright (c), NXP Semiconductors
 *
 *                       (C) NXP Semiconductors 2014
 *
 *         All rights are reserved. Reproduction in whole or in part is
 *        prohibited without the written consent of the copyright owner.
 *    NXP reserves the right to make changes without notice at any time.
 *   NXP makes no warranty, expressed, implied or statutory, including but
 *   not limited to any implied warranty of merchantability or fitness for any
 *  particular purpose, or that the use will not infringe any third party patent,
 *   copyright or trademark. NXP must not be liable for any loss or damage
 *                            arising from its use.
 */

/** \file
* Discovery Loop Activities for Type A polling.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 4078 ))
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
#include <phpalI18092mPI.h>
#include <phpalI14443p3a.h>
#include <phpalI14443p4a.h>
#include <phalT1T.h>

#ifdef NXPBUILD__PHAC_DISCLOOP_SW
#include "phacDiscLoop_Sw_Int.h"
#include "phacDiscLoop_Sw_Int_A.h"

/* *****************************************************************************************************************
 * Global and Static Variables
 * ***************************************************************************************************************** */
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P3_TAGS
static const uint8_t aCascadeCodes[] = {
                                        PHPAL_I14443P3A_CASCADE_LEVEL_1,
                                        PHPAL_I14443P3A_CASCADE_LEVEL_2,
                                        PHPAL_I14443P3A_CASCADE_LEVEL_3
                                        };
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P3_TAGS */
/* *****************************************************************************************************************
 * Internal Definitions
 * ***************************************************************************************************************** */
phStatus_t phacDiscLoop_Sw_DetTechTypeA(
                                        phacDiscLoop_Sw_DataParams_t *pDataParams
                                        )
{
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P3_TAGS
    phStatus_t PH_MEMLOC_REM wStatus;

    /* Reset total tags found */
    pDataParams->sTypeATargetInfo.bTotalTagsFound = 0;
    /* Reset Collision Bit for Type A*/
    pDataParams->bCollPend &= (uint8_t)~(uint8_t)PHAC_DISCLOOP_POS_BIT_MASK_A;

    /* Apply Guard time. */
    PH_CHECK_SUCCESS_FCT(wStatus, phhalHw_Wait(
        pDataParams->pHalDataParams,
        PHHAL_HW_TIME_MICROSECONDS,
        pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_A])
        );

    /* sending the WakeUpA */
    wStatus = phpalI14443p3a_WakeUpA(
        pDataParams->pPal1443p3aDataParams,
        pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aAtqa);

    if(phacDiscLoop_Sw_Int_IsValidPollStatus(wStatus))
    {
        if((wStatus & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR)
        {
            pDataParams->bCollPend |= PHAC_DISCLOOP_POS_BIT_MASK_A;
        }

        (void)phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXWAIT_US, 500);

        /* Send HALT, if we support NFC Activity 1.1 or if not in NFC mode */
        if(((pDataParams->bNfcActivityVersion == PHAC_DISCLOOP_NFC_ACTIVITY_VERSION_1_1)
           && (pDataParams->bOpeMode == RD_LIB_MODE_NFC))
           || (pDataParams->bOpeMode != RD_LIB_MODE_NFC))
        {
            /* Halt the detected cards. */
            (void)phpalI14443p3a_HaltA(pDataParams->pPal1443p3aDataParams);
        }
        pDataParams->sTypeATargetInfo.bTotalTagsFound = 1;

    }
    else
    {
        return wStatus;
    }

    return PH_ADD_COMPCODE(PHAC_DISCLOOP_TECH_DETECTED, PH_COMP_AC_DISCLOOP);
#else
    return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_AC_DISCLOOP);
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P3_TAGS */
}

phStatus_t phacDiscLoop_Sw_Int_CollisionResolutionA(
                                                    phacDiscLoop_Sw_DataParams_t * pDataParams
                                                    )
{
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P3_TAGS
    phStatus_t PH_MEMLOC_REM   status = PH_ERR_SUCCESS;
    uint8_t    PH_MEMLOC_REM   bTypeANvbUid;
    uint8_t    PH_MEMLOC_COUNT bDeviceCount;
    uint8_t    PH_MEMLOC_COUNT bTypeATagIdx;
    uint8_t    PH_MEMLOC_COUNT bCascadeCodeIdx;
    uint8_t    PH_MEMLOC_BUF   aTypeAUid[7] = {0};

    bDeviceCount = 0;
    bTypeATagIdx = 0;

    /* Collision_Pending = 1 and Device limit  = 0 */
    if((pDataParams->bCollPend & PHAC_DISCLOOP_POS_BIT_MASK_A) && ((pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_A] == 0x00)))
    {
        pDataParams->sTypeATargetInfo.bTotalTagsFound = 0;
        return PH_ADD_COMPCODE(PHAC_DISCLOOP_NO_DEVICE_RESOLVED, PH_COMP_AC_DISCLOOP);
    }

    /* If supporting Activity 1.0, update status based on detection phase */
    if((pDataParams->bOpeMode == RD_LIB_MODE_NFC)
       && (pDataParams->bNfcActivityVersion == PHAC_DISCLOOP_NFC_ACTIVITY_VERSION_1_0))
    {
        if(pDataParams->sTypeATargetInfo.bTotalTagsFound)
        {
            status = PH_ERR_SUCCESS;
        }

        if(pDataParams->bCollPend & PHAC_DISCLOOP_POS_BIT_MASK_A)
        {
            status = PH_ERR_COLLISION_ERROR;
        }
    }
    /*Symbol 0*/
    else
    {
        /* Apply Guard time. */
        PH_CHECK_SUCCESS_FCT(status, phhalHw_Wait(
            pDataParams->pHalDataParams,
            PHHAL_HW_TIME_MICROSECONDS,
            pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_A]));

        /* Send WakeUpA */
        status = phpalI14443p3a_WakeUpA(
            pDataParams->pPal1443p3aDataParams,
            pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aAtqa);
    }

    /* Reset card detected count */
    pDataParams->sTypeATargetInfo.bT1TFlag = 0;
    pDataParams->sTypeATargetInfo.bTotalTagsFound = 0;

    /*Symbol 1*/
    if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
    {
        /*Symbol 2*/
        /* Check for T1T Tag*/
        if(pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aAtqa[0] == 0x00)
        {
            /* Enable Jewel Mode */
            PH_CHECK_SUCCESS_FCT(status, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_JEWEL_MODE, PH_ON));

            /*Symbol 23*/
            status = phalT1T_ReadUID(
                pDataParams->pAlT1TDataParams,
                pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aUid,
                (uint16_t *)&pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].bUidSize);

            if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
            {
                /*Symbol 27*/
                /* indicates T1T card*/
                pDataParams->sTypeATargetInfo.bT1TFlag = 1;
                pDataParams->sTypeATargetInfo.bTotalTagsFound = 1;
                pDataParams->bNumOfCards = 1;
                /* Symbol 26 */
                pDataParams->bCollPend &= (uint8_t)~(uint8_t)PHAC_DISCLOOP_POS_BIT_MASK_A;

                return PH_ADD_COMPCODE(PH_COMP_AC_DISCLOOP, PH_ERR_SUCCESS);
            }
            else
            {
                /*Symbol 24*/
                if ((status & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR)
                {
                    /* Symbol 25 */
                    pDataParams->bCollPend |= PHAC_DISCLOOP_POS_BIT_MASK_A;
                    return PH_ADD_COMPCODE(PHAC_DISCLOOP_COLLISION_PENDING, PH_COMP_AC_DISCLOOP);
                }
                return status;
            }
        }
    }
    else
    {
        /* Device count = 0 and any Error */
        if(((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
          && (pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_A] == 0x00))
        {
            pDataParams->sTypeATargetInfo.bTotalTagsFound = 0;
            pDataParams->bCollPend |= PHAC_DISCLOOP_POS_BIT_MASK_A;
            return status;
        }

        /* Emvco: case_id TA311_X EMVCo Req 9.6.1.3 */
        if((pDataParams->bOpeMode == RD_LIB_MODE_EMVCO))
        {
            if(PH_ERR_IO_TIMEOUT == (status & PH_ERR_MASK))
            {
                status = phpalI14443p3a_WakeUpA(
                    pDataParams->pPal1443p3aDataParams,
                    pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aAtqa);
            }

            if (PH_ERR_IO_TIMEOUT == (status & PH_ERR_MASK))
            {
                status = phpalI14443p3a_WakeUpA(
                    pDataParams->pPal1443p3aDataParams,
                    pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aAtqa);
            }

            /* Collision error may happen */
            pDataParams->bCollPend &= (uint8_t)~(uint8_t)PHAC_DISCLOOP_POS_BIT_MASK_A;

            /* Some error that can't be handled */
            PH_CHECK_SUCCESS(status);
        }
        else
        {
            /*Symbol 1*/
            if ((status & PH_ERR_MASK) != PH_ERR_COLLISION_ERROR)
            {
                pDataParams->bCollPend &= (uint8_t)~(uint8_t)PHAC_DISCLOOP_POS_BIT_MASK_A;
                return status;
            }
        }
    }

    /*Symbol 3*/
    if(!pDataParams->bUseAntiColl)
    {
        /*Symbol 25*/
        pDataParams->bCollPend |= (uint8_t)PHAC_DISCLOOP_POS_BIT_MASK_A;
        return PH_ADD_COMPCODE(PHAC_DISCLOOP_COLLISION_PENDING, PH_COMP_AC_DISCLOOP);
    }

    while (bDeviceCount <= pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_A])
    {
        if (bTypeATagIdx <= pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_A])
        {
            /*Symbol 5*/
            pDataParams->bCollPend &= (uint8_t)~(uint8_t)PHAC_DISCLOOP_POS_BIT_MASK_A;

            /*Symbol 5, 17*/
            for (bCascadeCodeIdx=0; bCascadeCodeIdx<3; bCascadeCodeIdx++)
            {
                bTypeANvbUid = 0;

                /* Anti-collision loop */
                while (bTypeANvbUid != 0x40)
                {
                    /*Symbol 6,7,8*/
                    status = phpalI14443p3a_Anticollision(
                        pDataParams->pPal1443p3aDataParams,
                        aCascadeCodes[bCascadeCodeIdx],      /* Cascade code */
                        aTypeAUid,                           /* Input UID */
                        bTypeANvbUid,                        /* UID len = 0 */
                        aTypeAUid,                           /* UID out */
                        &bTypeANvbUid);                      /* UID out size */

                    /* Emvco: case_id TA310 */
                    if((pDataParams->bOpeMode == RD_LIB_MODE_EMVCO) && ((status & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT))
                    {
                        status = phpalI14443p3a_Anticollision(
                            pDataParams->pPal1443p3aDataParams,
                            aCascadeCodes[bCascadeCodeIdx],      /* Cascade code */
                            aTypeAUid,                           /* Input UID */
                            bTypeANvbUid,                        /* UID len = 0 */
                            aTypeAUid,                           /* UID out */
                            &bTypeANvbUid);                      /* UID out size */
                    }

                    /*Symbol 9, 14*/
                    if ((status & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR)
                    {
                        /* Emvco: case_id TA302_00 */
                        if(pDataParams->bOpeMode == RD_LIB_MODE_EMVCO)
                        {
                            /* Report Error to Application and Application will perform PICC Reset */
                            return PH_ADD_COMPCODE(PHAC_DISCLOOP_COLLISION_PENDING, PH_COMP_AC_DISCLOOP);
                        }

                        /*Symbol 10*/
                        pDataParams->bCollPend |= PHAC_DISCLOOP_POS_BIT_MASK_A;
                        if(bTypeATagIdx >= pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_A])
                        {
                            /*Symbol 11*/
                            return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);
                        }

                        if ((bTypeANvbUid & 0x07) < 7)
                        {
                            bTypeANvbUid++;
                        }
                        else
                        {
                            bTypeANvbUid = (uint8_t)((((bTypeANvbUid & 0xF0) >> 4) + 1) << 4);
                        }
                    }
                    else
                    {
                        /* Check success */
                        /* Some error that can't be handled */
                        PH_CHECK_SUCCESS(status); /* TA302_01*/
                    }
                } /*End of while (bTypeANvbUid != 0x40)*/

                /* EMVCO: case_id 302_10 */
                /*
                * Digital Proto: Article 4.7.2 and 4.7.2.4
                * ISO14443-3 Article 6.5.2.1
                * EMVCo article 5.4.2
                * So For Single UID Size The NFC Forum Device MUST set nfcid10 of a single-size NFCID1 and nfcid13
                * of a double-size NFCID1 to a value different from 88h.
                */
                if((!(pDataParams->bCollPend & PHAC_DISCLOOP_POS_BIT_MASK_A))
                   && ((((pDataParams->sTypeATargetInfo.aTypeA_I3P3[bTypeATagIdx].aAtqa[0] & 0xC0) == 0x00)
                   && (aCascadeCodes[bCascadeCodeIdx] == PHPAL_I14443P3A_CASCADE_LEVEL_1))
                   || (((pDataParams->sTypeATargetInfo.aTypeA_I3P3[bTypeATagIdx].aAtqa[0] & 0xC0) == 0x40)
                   && (aCascadeCodes[bCascadeCodeIdx] == PHPAL_I14443P3A_CASCADE_LEVEL_2)))
                   && (aTypeAUid[0] == 0x88))
                {
                    return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_AC_DISCLOOP);
                }

                /*symbol 15*/
                status = phpalI14443p3a_Select(
                    pDataParams->pPal1443p3aDataParams,
                    aCascadeCodes[bCascadeCodeIdx],
                    aTypeAUid,
                    &pDataParams->sTypeATargetInfo.aTypeA_I3P3[bTypeATagIdx].aSak);

                /* Emvco:  case_id TA312 */
                if((pDataParams->bOpeMode == RD_LIB_MODE_EMVCO) && ((status & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT))
                {
                    status = phpalI14443p3a_Select(
                        pDataParams->pPal1443p3aDataParams,
                        aCascadeCodes[bCascadeCodeIdx],
                        aTypeAUid,
                        &pDataParams->sTypeATargetInfo.aTypeA_I3P3[bTypeATagIdx].aSak);
                }

                if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
                {
                    /*Symbol 16*/
                    if (!(pDataParams->sTypeATargetInfo.aTypeA_I3P3[bTypeATagIdx].aSak & 0x04))
                    {
                        /*UID Complete*/
                        break;
                    }
                }
                else
                {
                    return status;
                }

            } /*for()*/

            /* If select was successful */
            if (status == PH_ERR_SUCCESS)
            {
                /*Symbol 19*/
                status = phpalI14443p3a_GetSerialNo(
                    pDataParams->pPal1443p3aDataParams,
                    &pDataParams->sTypeATargetInfo.aTypeA_I3P3[bTypeATagIdx].aUid[0],
                    &pDataParams->sTypeATargetInfo.aTypeA_I3P3[bTypeATagIdx].bUidSize);

                PH_CHECK_SUCCESS(status);

                /*Symbol 18*/
                bTypeATagIdx++;

                /*Symbol 20*/
                if((pDataParams->bCollPend & PHAC_DISCLOOP_POS_BIT_MASK_A) && (bTypeATagIdx < pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_A]))
                {
                    /* Symbol 21 */
                    status = phpalI14443p3a_HaltA(pDataParams->pPal1443p3aDataParams);
                    if (status != PH_ERR_SUCCESS)
                    {
                        /* Halt A has failed, can't count this as detected tag */
                        return status;
                    }

                    /* Symbol 22 */
                    status = phpalI14443p3a_RequestA(
                        pDataParams->pPal1443p3aDataParams,
                        pDataParams->sTypeATargetInfo.aTypeA_I3P3[bTypeATagIdx].aAtqa);

                    if((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
                    {
                        /*Symbol 4*/
                        if(pDataParams->sTypeATargetInfo.aTypeA_I3P3[bTypeATagIdx].aAtqa[0] == 0x00)
                        {
                            /*Symbol 25*/
                            pDataParams->bCollPend |= PHAC_DISCLOOP_POS_BIT_MASK_A;
                            return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);
                        }
                    }
                    else if((status & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR)
                    {
                        /* More device to resolve */
                        pDataParams->bCollPend |= PHAC_DISCLOOP_POS_BIT_MASK_A;
                    }
                    else
                    {
                        return status;
                    }
                }
                else
                {
                    break;
                }
            }
        }
        bDeviceCount++;
    } /* End of while(bDeviceCount < pDataParams->baPasConDevLim[0]) */

    pDataParams->sTypeATargetInfo.bTotalTagsFound = bTypeATagIdx;
    pDataParams->bNumOfCards = bTypeATagIdx;

    if(pDataParams->sTypeATargetInfo.bTotalTagsFound == 0)
    {
        return PH_ADD_COMPCODE(PHAC_DISCLOOP_NO_DEVICE_RESOLVED, PH_COMP_AC_DISCLOOP);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);
#else
    return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_AC_DISCLOOP);
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P3_TAGS */
}

phStatus_t phacDiscLoop_Sw_Int_ActivateA(
                                         phacDiscLoop_Sw_DataParams_t * pDataParams,
                                         uint8_t bTypeATagIdx
                                         )
{
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P3_TAGS
    phStatus_t   PH_MEMLOC_REM status = PH_ERR_SUCCESS;
    uint8_t      PH_MEMLOC_BUF aUid[10];
    uint8_t      PH_MEMLOC_REM bUidLen;
    uint8_t      PH_MEMLOC_REM bMoreCardsAvailable;
    uint8_t      PH_MEMLOC_REM bTagType;

    if (bTypeATagIdx > pDataParams->sTypeATargetInfo.bTotalTagsFound)
    {
        /* Out of range or no such card found yet */
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_AC_DISCLOOP);
    }

    /* If it is Single device or selected device is at last position, In both case device is already active */
    if((pDataParams->sTypeATargetInfo.bTotalTagsFound > 1) && ((bTypeATagIdx + 1) < pDataParams->sTypeATargetInfo.bTotalTagsFound))
    {
        /* Activate the card in sleep with given UID */
        PH_CHECK_SUCCESS_FCT(status, phpalI14443p3a_ActivateCard(
            pDataParams->pPal1443p3aDataParams,
            pDataParams->sTypeATargetInfo.aTypeA_I3P3[bTypeATagIdx].aUid,
            pDataParams->sTypeATargetInfo.aTypeA_I3P3[bTypeATagIdx].bUidSize,
            aUid,
            &bUidLen,
            &pDataParams->sTypeATargetInfo.aTypeA_I3P3[bTypeATagIdx].aSak,
            &bMoreCardsAvailable));
    }
    if(bTypeATagIdx >= PHAC_DISCLOOP_CFG_MAX_CARDS_SUPPORTED)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_AC_DISCLOOP);
    }
    else
    {
    bTagType = (pDataParams->sTypeATargetInfo.aTypeA_I3P3[bTypeATagIdx].aSak & 0x60);
    bTagType = bTagType >> 5;
    }

#ifdef  NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS
    /* Type 4A */
    if(bTagType == PHAC_DISCLOOP_TYPEA_TYPE4A_TAG_CONFIG_MASK)
    {
        /* Enable Emd Check for Emvco */
        if(pDataParams->bOpeMode == RD_LIB_MODE_EMVCO)
        {
            PH_CHECK_SUCCESS_FCT(status, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_SET_EMD, PH_ON));
        }

        /* Enable Emd Check for ISO */
        if(pDataParams->bOpeMode == RD_LIB_MODE_ISO)
        {
             PH_CHECK_SUCCESS_FCT(status, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_SET_EMD, PH_ON));
        }

        PH_CHECK_SUCCESS_FCT(status, phpalI14443p4a_ActivateCard(
            pDataParams->pPal1443p4aDataParams,
            pDataParams->sTypeATargetInfo.sTypeA_I3P4.bFsdi,
            pDataParams->sTypeATargetInfo.sTypeA_I3P4.bCid,
            pDataParams->sTypeATargetInfo.sTypeA_I3P4.bDri,
            pDataParams->sTypeATargetInfo.sTypeA_I3P4.bDsi,
            pDataParams->sTypeATargetInfo.sTypeA_I3P4.pAts));
    }
#endif /*  NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS
    if (bTagType == PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TAG_CONFIG_MASK)
    {
        /* Send ATR_REQ and PSL_REQ */
        PH_CHECK_SUCCESS_FCT(status, phpalI18092mPI_Atr(
            pDataParams->pPal18092mPIDataParams,
            pDataParams->sTypeATargetInfo.aTypeA_I3P3[bTypeATagIdx].aUid,
            pDataParams->sTypeATargetInfo.sTypeA_P2P.bDid,
            pDataParams->sTypeATargetInfo.sTypeA_P2P.bLri,
            pDataParams->sTypeATargetInfo.sTypeA_P2P.bNadEnable,
            pDataParams->sTypeATargetInfo.sTypeA_P2P.bNad,
            pDataParams->sTypeATargetInfo.sTypeA_P2P.pGi,
            pDataParams->sTypeATargetInfo.sTypeA_P2P.bGiLength,
            pDataParams->sTypeATargetInfo.sTypeA_P2P.pAtrRes,
            &(pDataParams->sTypeATargetInfo.sTypeA_P2P.bAtrResLength)));

        return PH_ADD_COMPCODE(PHAC_DISCLOOP_PASSIVE_TARGET_ACTIVATED, PH_COMP_AC_DISCLOOP);
    }
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS */

    /* Support for both Type 4A and NFC-DEP (Merged SAK) */
    if (bTagType == PHAC_DISCLOOP_TYPEA_TYPE_NFC_DEP_TYPE4A_TAG_CONFIG_MASK)
    {
        if(pDataParams->bOpeMode == RD_LIB_MODE_EMVCO)
        {
            PH_CHECK_SUCCESS_FCT(status, phpalI14443p4a_ActivateCard(
                pDataParams->pPal1443p4aDataParams,
                pDataParams->sTypeATargetInfo.sTypeA_I3P4.bFsdi,
                pDataParams->sTypeATargetInfo.sTypeA_I3P4.bCid,
                pDataParams->sTypeATargetInfo.sTypeA_I3P4.bDri,
                pDataParams->sTypeATargetInfo.sTypeA_I3P4.bDsi,
                pDataParams->sTypeATargetInfo.sTypeA_I3P4.pAts));
        }
        else
        {
            /* Return to application; application can decide what to send RATS or ATR */
            return PH_ADD_COMPCODE(PHAC_DISCLOOP_MERGED_SEL_RES_FOUND, PH_COMP_AC_DISCLOOP);
        }
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);
#else
    return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_AC_DISCLOOP);
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P3_TAGS */
}
#endif /* NXPBUILD__PHAC_DISCLOOP_SW */
