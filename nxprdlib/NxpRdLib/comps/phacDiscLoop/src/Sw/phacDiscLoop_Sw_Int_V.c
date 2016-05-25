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
* Discovery Loop Activities for Type V polling.
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
#include <phpalSli15693.h>

#ifdef NXPBUILD__PHAC_DISCLOOP_SW
#include "phacDiscLoop_Sw_Int.h"
#include "phacDiscLoop_Sw_Int_V.h"
/* *****************************************************************************************************************
 * Private Functions
 * ***************************************************************************************************************** */
phStatus_t phacDiscLoop_Sw_DetTechTypeV(
                                        phacDiscLoop_Sw_DataParams_t *pDataParams
                                        )
{
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS
    phStatus_t PH_MEMLOC_REM status = PH_ERR_INTERNAL_ERROR;

    /* Set the Guard times */
    PH_CHECK_SUCCESS_FCT(status, phhalHw_Wait(
        pDataParams->pHalDataParams,
        PHHAL_HW_TIME_MICROSECONDS,
        pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_V]));

    /* Inventory request with one slot */
    status = phpalSli15693_Inventory(
        pDataParams->pPalSli15693DataParams,
        pDataParams->sTypeVTargetInfo.bFlag | PHPAL_SLI15693_FLAG_NBSLOTS,
        0,
        NULL,
        0,
        &pDataParams->sTypeVTargetInfo.aTypeV[0].bDsfid,
        pDataParams->sTypeVTargetInfo.aTypeV[0].aUid
        );

    if(phacDiscLoop_Sw_Int_IsValidPollStatus(status))
    {
        return PH_ADD_COMPCODE(PHAC_DISCLOOP_TECH_DETECTED, PH_COMP_AC_DISCLOOP);
    }
    else
    {
        return status;
    }
#else
    return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_AC_DISCLOOP);
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS */
}

phStatus_t phacDiscLoop_Sw_Int_CollisionResolutionV(
                                                    phacDiscLoop_Sw_DataParams_t *pDataParams
                                                    )
{
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS
    phStatus_t PH_MEMLOC_REM   status = PH_ERR_INTERNAL_ERROR;
    uint8_t    PH_MEMLOC_BUF   aMaskBuffer[PHPAL_SLI15693_UID_LENGTH];
    uint8_t    PH_MEMLOC_REM   bMaskBitLength = 0;
    uint8_t    PH_MEMLOC_REM   bUidLengthDummy;
    uint8_t    PH_MEMLOC_REM   bDataDummy[1];
    uint8_t    PH_MEMLOC_REM   bNextSlot = 1;
    uint16_t   PH_MEMLOC_REM   wDataLengthDummy;
    uint8_t    PH_MEMLOC_COUNT bSlot;

    /* Symbol 0 */
    pDataParams->sTypeVTargetInfo.bTotalTagsFound = 0;
    pDataParams->bCollPend &= (uint8_t)~(uint8_t)(PHAC_DISCLOOP_POS_BIT_MASK_V);

    /* Set the Guard times */
    PH_CHECK_SUCCESS_FCT(status, phhalHw_Wait(
        pDataParams->pHalDataParams,
        PHHAL_HW_TIME_MICROSECONDS,
        pDataParams->waPasPollGTimeUs[PHAC_DISCLOOP_TECH_TYPE_V]));

    /* Symbol 0 */
    status = phpalSli15693_Inventory(
        pDataParams->pPalSli15693DataParams,
        pDataParams->sTypeVTargetInfo.bFlag | PHPAL_SLI15693_FLAG_NBSLOTS,
        0,
        aMaskBuffer,
        bMaskBitLength,
        &pDataParams->sTypeVTargetInfo.aTypeV[0].bDsfid,
        pDataParams->sTypeVTargetInfo.aTypeV[0].aUid);

    /* Symbol 1 */
    if((status & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT)
    {
        return status;
    }
    else
    {
        /* Symbol 2 */
        if((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
        {
            /* Symbol 5 */
            pDataParams->sTypeVTargetInfo.bTotalTagsFound++;
            pDataParams->bNumOfCards++;
            return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);
        }
        else if(((status & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR)
                || ((status & PH_ERR_MASK) == PH_ERR_INTEGRITY_ERROR))
        {
            /* Symbol 3 */
            pDataParams->bCollPend |= PHAC_DISCLOOP_POS_BIT_MASK_V;

            /* Symbol 4 */
            if(pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_V] == 0x00)
            {
                return PH_ADD_COMPCODE(PHAC_DISCLOOP_COLLISION_PENDING, PH_COMP_AC_DISCLOOP);
            }

            while(pDataParams->sTypeVTargetInfo.bTotalTagsFound < pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_V])
            {
                /* Symbol 6 */
                bSlot = 0;

                /* Symbol 7 */
                pDataParams->bCollPend &= (uint8_t)~(uint8_t)(PHAC_DISCLOOP_POS_BIT_MASK_V);

                /* Symbol 8 */
                status = phpalSli15693_Inventory(
                    pDataParams->pPalSli15693DataParams,
                    pDataParams->sTypeVTargetInfo.bFlag,
                    0,
                    aMaskBuffer,
                    bMaskBitLength,
                    &pDataParams->sTypeVTargetInfo.aTypeV[pDataParams->sTypeVTargetInfo.bTotalTagsFound].bDsfid,
                    pDataParams->sTypeVTargetInfo.aTypeV[pDataParams->sTypeVTargetInfo.bTotalTagsFound].aUid);

                /* Loop till 16 slots or collision error */
                do
                {
                    /* Symbol 9 */
                    if((status & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT)
                    {
                        /* Continue with next slot */
                    }
                    /* Symbol 10 */
                    else if(((status & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR)
                            || ((status & PH_ERR_MASK) == PH_ERR_INTEGRITY_ERROR))
                    {
                        /* Symbol 16 */
                        pDataParams->bCollPend |= PHAC_DISCLOOP_POS_BIT_MASK_V;

                        /* Get bit-length of last valid byte */
                        bUidLengthDummy = bMaskBitLength % 8;

                        /* Whole byte is valid -> append slot number to next byte */
                        if (bUidLengthDummy == 0)
                        {
                            aMaskBuffer[bMaskBitLength >> 3] = bSlot;
                        }
                        /* Partial byte is valid */
                        else
                        {
                            /* Fill the invalid bits of the incomplete byte with the 4 bits slot number */
                            aMaskBuffer[bMaskBitLength >> 3] &= (uint8_t)(0xFF >> (8 - bUidLengthDummy));
                            aMaskBuffer[bMaskBitLength >> 3] |= (uint8_t)(bSlot << bUidLengthDummy);

                            /* If not all 4 bits of the Slot number fit in the incomplete byte, put the rest in the next byte */
                            if (bUidLengthDummy > 4)
                            {
                                aMaskBuffer[(bMaskBitLength >> 3) + 1] = (uint8_t)(bSlot >> bUidLengthDummy);
                            }
                        }

                        /* Increment the bit length by the 4 bits slot number */
                        bMaskBitLength = bMaskBitLength + 4;
                        break;
                    }
                    else if((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
                    {
                        /* Symbol 11 */
                        pDataParams->sTypeVTargetInfo.bTotalTagsFound++;
                        pDataParams->bNumOfCards++;

                        /* Symbol 12 */
                        if(pDataParams->sTypeVTargetInfo.bTotalTagsFound >= pDataParams->baPasConDevLim[PHAC_DISCLOOP_TECH_TYPE_V])
                        {
                            return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);
                        }
                    }
                    else
                    {
                        /* For all other error types, return error */
                        return status;
                    }

                    /* Symbol 13, 14 */
                    if(++bSlot == 16)
                    {
                        bNextSlot = 0;
                        bMaskBitLength = 0;
                    }

                    if(bNextSlot)
                    {
                        /* Symbol 15 */
                        status = phpalSli15693_SendEof(
                            pDataParams->pPalSli15693DataParams,
                            PHPAL_SLI15693_EOF_NEXT_SLOT,
                            &pDataParams->sTypeVTargetInfo.aTypeV[pDataParams->sTypeVTargetInfo.bTotalTagsFound].bDsfid,
                            pDataParams->sTypeVTargetInfo.aTypeV[pDataParams->sTypeVTargetInfo.bTotalTagsFound].aUid,
                            &bUidLengthDummy,
                            bDataDummy,
                            &wDataLengthDummy);
                    }
                }
                while(bNextSlot);

                /* Symbol 17 */
                if(!(pDataParams->bCollPend & PHAC_DISCLOOP_POS_BIT_MASK_V))
                {
                    break;
                }
            }
        }
        else
        {
            /* For all other error types, return error */
            return status;
        }
    }

    if(pDataParams->sTypeVTargetInfo.bTotalTagsFound)
    {
        return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);
    }

    return PH_ADD_COMPCODE(PHAC_DISCLOOP_NO_DEVICE_RESOLVED, PH_COMP_AC_DISCLOOP);
#else
    return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_AC_DISCLOOP);
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS */
}

phStatus_t phacDiscLoop_Sw_Int_ActivateV(
                                         phacDiscLoop_Sw_DataParams_t * pDataParams,
                                         uint8_t bTypeVTagIdx
                                         )
{
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS
    phStatus_t PH_MEMLOC_REM status = PH_ERR_INTERNAL_ERROR;

    if (bTypeVTagIdx >= pDataParams->sTypeVTargetInfo.bTotalTagsFound)
    {
        /* Out of range */
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_AC_DISCLOOP);
    }

    /* Set the UID to be selected and the UID length into PAL */
    PH_CHECK_SUCCESS_FCT(status, phpalSli15693_SetSerialNo(
        pDataParams->pPalSli15693DataParams,
        pDataParams->sTypeVTargetInfo.aTypeV[bTypeVTagIdx].aUid,
        PHPAL_SLI15693_UID_LENGTH));

    /* Move the tag to selected state */
    PH_CHECK_SUCCESS_FCT(status, phpalSli15693_Select(pDataParams->pPalSli15693DataParams));

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);
#else
    return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_AC_DISCLOOP);
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS */
}
#endif /* NXPBUILD__PHAC_DISCLOOP_SW */
