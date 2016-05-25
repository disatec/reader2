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
* Discovery Loop Activities for Type B polling.
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
#include <phpalI14443p3b.h>

#ifdef NXPBUILD__PHAC_DISCLOOP_SW
#include "phacDiscLoop_Sw_Int.h"
#include "phacDiscLoop_Sw_Int_B.h"

/* *****************************************************************************************************************
 * Internal Definitions
 * ***************************************************************************************************************** */
phStatus_t phacDiscLoop_Sw_DetTechTypeB(
                                        phacDiscLoop_Sw_DataParams_t *pDataParams
                                        )
{
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEB_I3P3B_TAGS
    phStatus_t PH_MEMLOC_REM wStatus;

    pDataParams->sTypeBTargetInfo.bAfiReq = 0x00;
    pDataParams->sTypeBTargetInfo.bTotalTagsFound = 0;
    pDataParams->bCollPend &= (uint8_t)~(uint8_t)PHAC_DISCLOOP_POS_BIT_MASK_B;

    /* Apply Guard time. */
    PH_CHECK_SUCCESS_FCT(wStatus, phhalHw_Wait(
        pDataParams->pHalDataParams,
        PHHAL_HW_TIME_MICROSECONDS,
        pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_B]));

    /* WakeupB with number of slot as 0 */
    wStatus = phpalI14443p3b_WakeUpB(
        pDataParams->pPal1443p3bDataParams,
        0,
        pDataParams->sTypeBTargetInfo.bAfiReq,
        pDataParams->sTypeBTargetInfo.bExtendedAtqBbit,
        pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].aAtqB,
        &pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].bAtqBLength);

    if(phacDiscLoop_Sw_Int_IsValidPollStatus(wStatus))
    {
        if((wStatus & PH_ERR_MASK) != PH_ERR_SUCCESS)
        {
            pDataParams->bCollPend |= PHAC_DISCLOOP_POS_BIT_MASK_B;
        }
        pDataParams->sTypeBTargetInfo.bTotalTagsFound++;
    }
    else
    {
        return wStatus;
    }

    return PH_ADD_COMPCODE(PHAC_DISCLOOP_TECH_DETECTED, PH_COMP_AC_DISCLOOP);
#else
    return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_AC_DISCLOOP);
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEB_I3P3B_TAGS */
}

phStatus_t phacDiscLoop_Sw_Int_CollisionResolutionB(
                                                    phacDiscLoop_Sw_DataParams_t * pDataParams
                                                    )
{
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEB_I3P3B_TAGS
    phStatus_t PH_MEMLOC_REM   status = PH_ERR_SUCCESS;
    uint8_t    PH_MEMLOC_REM   bCurrentSlotNum;
    uint8_t    PH_MEMLOC_REM   bCurrentDeviceCount;
    uint8_t    PH_MEMLOC_REM   bLastSlotReached;
    uint8_t    PH_MEMLOC_COUNT bNumOfSlots;

    /* Collision_Pending = 1 and Device limit  = 0 */
    if((pDataParams->bCollPend & PHAC_DISCLOOP_POS_BIT_MASK_B) && ((pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_B] == 0x00)))
    {
        pDataParams->sTypeBTargetInfo.bTotalTagsFound = 0;
        return PH_ADD_COMPCODE(PHAC_DISCLOOP_NO_DEVICE_RESOLVED, PH_COMP_AC_DISCLOOP);
    }

    /* Symbol 0 */
    bNumOfSlots = 0;

    /* If supporting Activity 1.0, update status based on detection phase */
    if((pDataParams->bOpeMode == RD_LIB_MODE_NFC)
       && (pDataParams->bNfcActivityVersion == PHAC_DISCLOOP_NFC_ACTIVITY_VERSION_1_0))
    {
        if(pDataParams->sTypeBTargetInfo.bTotalTagsFound)
        {
            status = PH_ERR_SUCCESS;
        }

        if(pDataParams->bCollPend & PHAC_DISCLOOP_POS_BIT_MASK_B)
        {
            status = PH_ERR_COLLISION_ERROR;
        }
    }
    /* Send WUPB if we support NFC Activity 1.1 or EMVCo mode */
    else
    {
        /* Apply Guard time. */
        PH_CHECK_SUCCESS_FCT(status, phhalHw_Wait(
            pDataParams->pHalDataParams,
            PHHAL_HW_TIME_MICROSECONDS,
            pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_B]));

        /* WakeupB with number of slot as 0 */
        status = phpalI14443p3b_WakeUpB(
            pDataParams->pPal1443p3bDataParams,
            bNumOfSlots,
            pDataParams->sTypeBTargetInfo.bAfiReq,
            pDataParams->sTypeBTargetInfo.bExtendedAtqBbit,
            pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].aAtqB,
            &pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].bAtqBLength);
    }

    if(PH_ERR_SUCCESS != (status & PH_ERR_MASK))
    {
        /* EMVCO:  case_id 311_X Req. 9.6.1.3 */
        if((pDataParams->bOpeMode == RD_LIB_MODE_EMVCO))
        {
            if(PH_ERR_IO_TIMEOUT == (status & PH_ERR_MASK))
            {
            status = phpalI14443p3b_WakeUpB(pDataParams->pPal1443p3bDataParams,
                bNumOfSlots,
                pDataParams->sTypeBTargetInfo.bAfiReq,
                pDataParams->sTypeBTargetInfo.bExtendedAtqBbit,
                pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].aAtqB,
                &pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].bAtqBLength);
            }

            if (PH_ERR_IO_TIMEOUT == (status & PH_ERR_MASK))
            {
                PH_CHECK_SUCCESS_FCT(status, phpalI14443p3b_WakeUpB(pDataParams->pPal1443p3bDataParams,
                    bNumOfSlots,
                    pDataParams->sTypeBTargetInfo.bAfiReq,
                    pDataParams->sTypeBTargetInfo.bExtendedAtqBbit,
                    pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].aAtqB,
                    &pDataParams->sTypeBTargetInfo.aTypeB_I3P3[0].bAtqBLength));
            }

            pDataParams->bCollPend &= (uint8_t)~(uint8_t)PHAC_DISCLOOP_POS_BIT_MASK_B;
            /* Some error that can't be handled */
            PH_CHECK_SUCCESS(status);
        }
        else
        {
            /* Symbol 2 */
            /* No Response */
            if((status & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT)
            {
                return status;
            }
            else
            { /* Symbol 3 */
                if((pDataParams->baPasConDevLim[1] == 0x00))
                {
                    pDataParams->sTypeBTargetInfo.bTotalTagsFound = 0;
                    /* Symbol 4 */
                    pDataParams->bCollPend |= PHAC_DISCLOOP_POS_BIT_MASK_B;
                    return status;
                }
            }
        }
    }

    while(bNumOfSlots <= PHAC_DISCLOOP_TYPEB_MAX_SLOT_NUM)
    {
        /* Symbol 5 */
        bCurrentSlotNum = 0;
        bCurrentDeviceCount = 0;
        bLastSlotReached = 0;
        pDataParams->sTypeBTargetInfo.bTotalTagsFound = 0;
        pDataParams->bCollPend &= (uint8_t)~(uint8_t)PHAC_DISCLOOP_POS_BIT_MASK_B;

        while(!bLastSlotReached)
        {
            /* Symbol 6: Slot is Empty */
            if((status & PH_ERR_MASK) != PH_ERR_IO_TIMEOUT)
            {
                /* Symbol 7: Validate SENSB_RES */
                if (status == PH_ERR_SUCCESS)
                {
                    /* Symbol 9 */
                    if((pDataParams->bNfcActivityVersion == PHAC_DISCLOOP_NFC_ACTIVITY_VERSION_1_1)
                       && (bCurrentDeviceCount > 0))
                    {
                        /* Symbol 10 */
                        status = phpalI14443p3b_SetSerialNo(
                                    pDataParams->pPal1443p3bDataParams,
                                    pDataParams->sTypeBTargetInfo.aTypeB_I3P3[pDataParams->sTypeBTargetInfo.bTotalTagsFound - 1].aPupi
                                    );

                        status = phpalI14443p3b_HaltB(pDataParams->pPal1443p3bDataParams);
                    }

                    /* Symbol 12 */
                    memcpy(pDataParams->sTypeBTargetInfo.aTypeB_I3P3[pDataParams->sTypeBTargetInfo.bTotalTagsFound].aPupi,  /* PRQA S 3200 */
                        &pDataParams->sTypeBTargetInfo.aTypeB_I3P3[pDataParams->sTypeBTargetInfo.bTotalTagsFound].aAtqB[1],
                        PHAC_DISCLOOP_I3P3B_PUPI_LENGTH);

                    /* Symbol 11 */
                    pDataParams->sTypeBTargetInfo.bTotalTagsFound++;
                    bCurrentDeviceCount++;

                    /* Symbol 13 */
                    if (pDataParams->sTypeBTargetInfo.bTotalTagsFound >= pDataParams->baPasConDevLim[1])
                    {
                        pDataParams->bNumOfCards = pDataParams->sTypeBTargetInfo.bTotalTagsFound;

                        return PH_ADD_COMPCODE(
                            PH_ERR_SUCCESS,
                            PH_COMP_AC_DISCLOOP);
                    }

                    if(pDataParams->bNfcActivityVersion == PHAC_DISCLOOP_NFC_ACTIVITY_VERSION_1_0)
                    {
                        status = phpalI14443p3b_HaltB(pDataParams->pPal1443p3bDataParams);
                    }
                }
                else
                {
                    /* Symbol 8 CollisionPend: 1*/
                    pDataParams->bCollPend |= PHAC_DISCLOOP_POS_BIT_MASK_B;
                }
            }

            /* Symbol 14 */
            bCurrentSlotNum++;

            /* Symbol 15 */
            if(bCurrentSlotNum < (1 << bNumOfSlots))
            {
                /* Symbol 25 */
                status = phpalI14443p3b_SlotMarker(
                    pDataParams->pPal1443p3bDataParams,
                    (bCurrentSlotNum + 1),
                    pDataParams->sTypeBTargetInfo.aTypeB_I3P3[pDataParams->sTypeBTargetInfo.bTotalTagsFound].aAtqB,
                    &pDataParams->sTypeBTargetInfo.aTypeB_I3P3[pDataParams->sTypeBTargetInfo.bTotalTagsFound].bAtqBLength);
            }
            else
            {
                bLastSlotReached = 1;
                /* Symbol 16 */
                if (pDataParams->bCollPend & PHAC_DISCLOOP_POS_BIT_MASK_B)
                {
                    /* Symbol 17 */
                    if (pDataParams->sTypeBTargetInfo.bTotalTagsFound > 0)
                    {
                        /* Symbol 20 */
                        if (pDataParams->sTypeBTargetInfo.bTotalTagsFound >= pDataParams->baPasConDevLim[1])
                        {
                            pDataParams->bNumOfCards = pDataParams->sTypeBTargetInfo.bTotalTagsFound;

                            return PH_ADD_COMPCODE(
                                PH_ERR_SUCCESS,
                                PH_COMP_AC_DISCLOOP);
                        }
                    }
                    else
                    {
                        /* Symbol 18 */
                        if (bNumOfSlots == PHAC_DISCLOOP_TYPEB_MAX_SLOT_NUM)
                        {
                            pDataParams->bNumOfCards = pDataParams->sTypeBTargetInfo.bTotalTagsFound;

                            if(pDataParams->sTypeBTargetInfo.bTotalTagsFound == 0)
                            {
                                return PH_ADD_COMPCODE(PHAC_DISCLOOP_NO_DEVICE_RESOLVED, PH_COMP_AC_DISCLOOP);
                            }

                            return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);
                        }

                        /* Symbol 19 */
                        bNumOfSlots++;
                    }

                    /* Symbol 21 */
                    if(bCurrentDeviceCount > 0)
                    {
                        /* Symbol 22 */
                        status = phpalI14443p3b_HaltB(pDataParams->pPal1443p3bDataParams);
                    }

                    /* Symbol 23 */
                    status = phpalI14443p3b_RequestB(
                        pDataParams->pPal1443p3bDataParams,
                        bNumOfSlots,
                        pDataParams->sTypeBTargetInfo.bAfiReq,
                        pDataParams->sTypeBTargetInfo.bExtendedAtqBbit,
                        pDataParams->sTypeBTargetInfo.aTypeB_I3P3[pDataParams->sTypeBTargetInfo.bTotalTagsFound].aAtqB,
                        &pDataParams->sTypeBTargetInfo.aTypeB_I3P3[pDataParams->sTypeBTargetInfo.bTotalTagsFound].bAtqBLength);
                }
                else
                {
                    pDataParams->bNumOfCards = pDataParams->sTypeBTargetInfo.bTotalTagsFound;

                    if(pDataParams->sTypeBTargetInfo.bTotalTagsFound == 0)
                    {
                        return (PHAC_DISCLOOP_NO_DEVICE_RESOLVED | PH_COMP_AC_DISCLOOP);
                    }
                    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);
                }
            }
        } /* while(!bLastSlotReached) */
    }

    pDataParams->bNumOfCards = pDataParams->sTypeBTargetInfo.bTotalTagsFound;

    if(pDataParams->sTypeBTargetInfo.bTotalTagsFound == 0)
    {
        return (PHAC_DISCLOOP_NO_DEVICE_RESOLVED | PH_COMP_AC_DISCLOOP);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);
#else
    return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_AC_DISCLOOP);
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEB_I3P3B_TAGS */
}

phStatus_t phacDiscLoop_Sw_Int_ActivateB(
                                         phacDiscLoop_Sw_DataParams_t * pDataParams,
                                         uint8_t bTypeBTagIdx
                                         )
{
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEB_I3P3B_TAGS
    phStatus_t PH_MEMLOC_REM status = PH_ERR_SUCCESS;
    uint8_t    PH_MEMLOC_REM bAtqbLen;
    uint8_t    PH_MEMLOC_BUF aAtqb[13];

    /* Only deviation form Device Activation Activity is
     * Device sleep state is not been check and always send WakeUpB Command
     */
    if (bTypeBTagIdx >= pDataParams->sTypeBTargetInfo.bTotalTagsFound)
    {
        /* Out of range or no such card found yet */
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_AC_DISCLOOP);
    }

    /* Send WUPB for tags in sleep state (except the last detected tag all are
     * in sleep state) */
    if(bTypeBTagIdx < (pDataParams->sTypeBTargetInfo.bTotalTagsFound - 1))
    {
        status = phpalI14443p3b_WakeUpB(
            pDataParams->pPal1443p3bDataParams,
            0x00,
            pDataParams->sTypeBTargetInfo.bAfiReq,
            pDataParams->sTypeBTargetInfo.bExtendedAtqBbit,
            aAtqb,
            &bAtqbLen);
    }

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEB_I3P4B_TAGS
    /* AttriB: activate PICC */
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

    status = phpalI14443p3b_Attrib(
        pDataParams->pPal1443p3bDataParams,
        pDataParams->sTypeBTargetInfo.aTypeB_I3P3[bTypeBTagIdx].aAtqB,
        pDataParams->sTypeBTargetInfo.aTypeB_I3P3[bTypeBTagIdx].bAtqBLength,
        pDataParams->sTypeBTargetInfo.bFsdi,
        pDataParams->sTypeBTargetInfo.bCid,
        pDataParams->sTypeBTargetInfo.bDri,
        pDataParams->sTypeBTargetInfo.bDsi,
        &pDataParams->sTypeBTargetInfo.sTypeB_I3P4.bMbli);

#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEB_I3P4B_TAGS */
    return status;
#else
    return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_AC_DISCLOOP);
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEB_I3P3B_TAGS */
}
#endif /* NXPBUILD__PHAC_DISCLOOP_SW */
