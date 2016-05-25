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
* Internal functions for Tag Operation Application Layer Component of
* Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 3529 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*/
#include <ph_TypeDefs.h>
#include <ph_Status.h>
#include <ph_RefDefs.h>
#include <phacDiscLoop.h>
#include <phalMful.h>
#include <phalTop.h>

#ifdef NXPBUILD__PHAL_TOP_SW
#ifdef NXPBUILD__PHAL_MFUL_SW

#include "phalTop_Sw_Int_T2T.h"

phStatus_t phalTop_Sw_Int_T2T_SetReadOnly(
                                          phalTop_Sw_DataParams_t * pDataParams
                                          )
{
    phStatus_t    PH_MEMLOC_REM   status;
    uint8_t       PH_MEMLOC_BUF   aData[16];
    uint16_t      PH_MEMLOC_REM   wBlockNo;
    uint16_t      PH_MEMLOC_COUNT wIndex;
    uint16_t      PH_MEMLOC_REM   wTempIndex;
    uint16_t      PH_MEMLOC_REM   wOffset;
    uint16_t      PH_MEMLOC_COUNT wLockBytesCount = 0;
    uint16_t      PH_MEMLOC_REM   wDefaultLockAddress;
    uint16_t      PH_MEMLOC_REM   wDefaultLockLength;
    phalTop_T2T_t PH_MEMLOC_REM * pT2T = pDataParams->pT2T;

    /* Check if tag is in valid state */
    if(pDataParams->bTagState == PHAL_TOP_STATE_NONE)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_INVALID_STATE, PH_COMP_AL_TOP);
    }

    /* Check for NDEF length > 0 (in initialized state NDEF length is 0) */
    if(pDataParams->bTagState == PHAL_TOP_STATE_INITIALIZED)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_EMPTY_NDEF, PH_COMP_AL_TOP);
    }

    /* Check if tag is already in read-only state */
    if(pDataParams->bTagState == PHAL_TOP_STATE_READONLY)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_READONLY_TAG, PH_COMP_AL_TOP);
    }

    /* Read static lock bytes and CC */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Read(
        pT2T,
        PHAL_TOP_T2T_STATIC_LOCK_BLOCK * PHAL_TOP_T2T_BYTES_PER_BLOCK,
        aData));

    /* Update R/W access byte of CC to RO */
    pT2T->bRwa = PHAL_TOP_T2T_CC_RWA_RO;
    aData[7] = PHAL_TOP_T2T_CC_RWA_RO;

    /* Write back CC */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Write(
        pT2T,
        PHAL_TOP_T2T_CC_BLOCK * PHAL_TOP_T2T_BYTES_PER_BLOCK,
        &aData[4]));

    /* Set static lock bytes */
    aData[2] = 0xFF;
    aData[3] = 0xFF;

    /* Write back static lock bytes */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Write(
        pT2T,
        PHAL_TOP_T2T_STATIC_LOCK_BLOCK * PHAL_TOP_T2T_BYTES_PER_BLOCK,
        aData));

    /* For static tags only static lock bits are present, so exit */
    if(pT2T->bTagMemoryType == PHAL_TOP_T2T_TAG_MEMORY_TYPE_STATIC)
    {
        /* Update state */
        pDataParams->bTagState = PHAL_TOP_STATE_READONLY;

        return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
    }

    /* Update dynamic lock bytes */
    if(pT2T->bLockTlvCount)
    {
        for(wIndex = 0; wIndex < pT2T->bLockTlvCount; wIndex++)
        {
            wBlockNo = (pT2T->asLockCtrlTlv[wIndex].wByteAddr / PHAL_TOP_T2T_BYTES_PER_BLOCK);
            for(wLockBytesCount = 0;
                wLockBytesCount < (pT2T->asLockCtrlTlv[wIndex].bSizeInBits / 8);
                wBlockNo += 4)
            {
                /* Read dynamic lock bytes */
                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Read(
                    pT2T,
                    (wBlockNo * PHAL_TOP_T2T_BYTES_PER_BLOCK),
                    aData));

                if(wBlockNo == (pT2T->asLockCtrlTlv[wIndex].wByteAddr / PHAL_TOP_T2T_BYTES_PER_BLOCK))
                {
                    wOffset = pT2T->asLockCtrlTlv[wIndex].wByteAddr % PHAL_TOP_T2T_BYTES_PER_BLOCK;
                }
                else
                {
                    wOffset = 0;
                }

                for(wTempIndex = wOffset;
                    ((wLockBytesCount < (pT2T->asLockCtrlTlv[wIndex].bSizeInBits / 8)) && (wTempIndex <= 16));
                    wTempIndex++, wLockBytesCount++)
                {
                    if(wTempIndex != 16)
                    {
                        aData[wTempIndex] = 0xFF;
                    }

                    /* If one block is updated with lock bytes, write that block */
                    if((wTempIndex >= 4) && ((wTempIndex % PHAL_TOP_T2T_BYTES_PER_BLOCK) == 0))
                    {
                        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Write(
                            pT2T,
                            ((wBlockNo + ((wTempIndex / PHAL_TOP_T2T_BYTES_PER_BLOCK) - 1)) * PHAL_TOP_T2T_BYTES_PER_BLOCK),
                            &aData[(((wTempIndex / PHAL_TOP_T2T_BYTES_PER_BLOCK) - 1) * 4)]));
                    }
                }

                wTempIndex--;

                /* Write partially filled lock bytes */
                if((wTempIndex <= 16) && ((wTempIndex % PHAL_TOP_T2T_BYTES_PER_BLOCK) != 0))
                {
                    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Write(
                        pT2T,
                        ((wBlockNo + (wTempIndex / PHAL_TOP_T2T_BYTES_PER_BLOCK)) * PHAL_TOP_T2T_BYTES_PER_BLOCK),
                        &aData[((wTempIndex / PHAL_TOP_T2T_BYTES_PER_BLOCK) * 4)]));
                }
            }
        }
    }
    /* If lock TLVs are not present, set default dynamic lock bits */
    else
    {
        /* Calculate default dynamic lock bytes starting address (data area +
         * first 4 blocks of sector 0) */
        wDefaultLockAddress = (pT2T->bTms * 8) + 16;

        /* Calculate default dynamic lock length (in bits) */
        wDefaultLockLength = ((pT2T->bTms * 8) - 48) / 8;

        /* If there are one or more block(s) of dynamic lock bytes, set it */
        for(wIndex = 0; wIndex < (wDefaultLockLength / 8) / PHAL_TOP_T2T_BYTES_PER_BLOCK; wIndex++)
        {
            memset(aData, 0xFF, 4); /* PRQA S 3200 */
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Write(
                pT2T,
                wDefaultLockAddress,
                aData));
            wDefaultLockAddress += 4;
        }

        /* Check for partial dynamic lock block/byte */
        if(((wDefaultLockLength / 8) % PHAL_TOP_T2T_BYTES_PER_BLOCK) || (wDefaultLockLength % 8))
        {
            memset(aData, 0x00, 4); /* PRQA S 3200 */
            wIndex = 0;

            /* Calculate partial lock bytes (less than one block) */
            if((wDefaultLockLength / 8) % PHAL_TOP_T2T_BYTES_PER_BLOCK)
            {
                for(wIndex = 0; wIndex < (wDefaultLockLength / 8) % PHAL_TOP_T2T_BYTES_PER_BLOCK; wIndex++)
                {
                    aData[wIndex] = 0xFF;
                }
            }

            /* Calculate partial lock bits (less than one byte) */
            if(wDefaultLockLength % 8)
            {
                aData[wIndex] = (uint8_t)((1 << (wDefaultLockLength % 8)) - 1);
            }

            /* Set partial dynamic lock block/byte */
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Write(
                pT2T,
                wDefaultLockAddress,
                aData));
        }
    }

    /* Update state */
    pDataParams->bTagState = PHAL_TOP_STATE_READONLY;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T2T_ClearState(
                                         phalTop_Sw_DataParams_t * pDataParams,
                                         phalTop_T2T_t * pT2T
                                         )
{
    /* Reset parameters */
    pT2T->bLockTlvCount = 0;
    pT2T->bMemoryTlvCount = 0;
    pT2T->bTagMemoryType = 0;
    pDataParams->bTagState = 0;
    pDataParams->wMaxNdefLength = 0;
    pT2T->wNdefHeaderAddr = 0;
    pDataParams->wNdefLength = 0;
    pT2T->wNdefMsgAddr = 0;
    pT2T->sSector.bAddress = 0;
    pT2T->sSector.bBlockAddress = 0;
    pT2T->sSector.bValidity = PH_OFF;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

/* Finds the count and position of memory, lock and NDEF TLV */
phStatus_t phalTop_Sw_Int_T2T_DetectTlvBlocks(
                                              phalTop_Sw_DataParams_t * pDataParams,
                                              phalTop_T2T_t * pT2T
                                              )
{
    phStatus_t PH_MEMLOC_REM   status;
    uint16_t   PH_MEMLOC_COUNT wIndex;
    uint16_t   PH_MEMLOC_REM   wIndexInit;
    uint8_t    PH_MEMLOC_REM   bPageAddr;
    uint8_t    PH_MEMLOC_REM   bByteOffset;
    uint8_t    PH_MEMLOC_REM   bBytesPerPage;
    uint8_t    PH_MEMLOC_BUF   aLength[3];
    uint8_t    PH_MEMLOC_REM   bExitLoop = 0;
    uint8_t    PH_MEMLOC_REM   bBlockNum;
    uint8_t    PH_MEMLOC_BUF   aData[20];
    uint8_t    PH_MEMLOC_REM   bDataLen;

    /* Set maximum data length for looping */
    bDataLen = 16;

    /* Set initial data start index */
    wIndexInit = 4;

    /* Check for TLVs only in first sector */
    for(bBlockNum = 4;
        ((bBlockNum < (((pT2T->bTms * 8) / PHAL_TOP_T2T_BYTES_PER_BLOCK) + 4)) && (bBlockNum <= 252));
        bBlockNum += 4)
    {
        /* Read data only if we don't have to skip */
        if(wIndexInit < bDataLen)
        {
            /* Read data and store it from aData[4] - aData[19] */
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Read(
                pT2T,
                bBlockNum * PHAL_TOP_T2T_BYTES_PER_BLOCK,
                &aData[PHAL_TOP_T2T_BYTES_PER_BLOCK]));

            /* Set data starting index for loop */
            wIndex = wIndexInit;

            /* Check for TLVs in read data */
            while((wIndex < bDataLen) && (!bExitLoop))
            {
                switch(aData[wIndex])
                {
                case PHAL_TOP_T2T_NULL_TLV:
                    break;

                case PHAL_TOP_T2T_LOCK_CTRL_TLV:
                    if(pT2T->bLockTlvCount >= PHAL_TOP_T2T_MAX_LOCK_CTRL_TLV)
                    {
                        return PH_ADD_COMPCODE(PHAL_TOP_ERR_UNSUPPORTED_TAG, PH_COMP_AL_TOP);
                    }

                    if(pT2T->bTagMemoryType != PHAL_TOP_T2T_TAG_MEMORY_TYPE_STATIC)
                    {
                        pT2T->asLockCtrlTlv[pT2T->bLockTlvCount].wOffset =
                            (bBlockNum * PHAL_TOP_T2T_BYTES_PER_BLOCK) + wIndex - PHAL_TOP_T2T_BYTES_PER_BLOCK;

                        bPageAddr = (uint8_t)((aData[wIndex + 2] & 0xF0) >> 4);
                        bByteOffset = aData[wIndex + 2] & 0x0F;

                        pT2T->asLockCtrlTlv[pT2T->bLockTlvCount].bSizeInBits = aData[wIndex + 3];

                        bBytesPerPage = aData[wIndex + 4] & 0x0F;
                        pT2T->asLockCtrlTlv[pT2T->bLockTlvCount].bBytesPerPage = aData[wIndex + 4] & 0x0F;

                        pT2T->asLockCtrlTlv[pT2T->bLockTlvCount].bBytesLockedPerBit =
                            (uint8_t)((aData[wIndex + 4] & 0xF0) >> 4);
                        pT2T->asLockCtrlTlv[pT2T->bLockTlvCount].wByteAddr =
                            (bPageAddr * (1 << bBytesPerPage)) + bByteOffset;

                        pT2T->bLockTlvCount++;
                        wIndex = wIndex + 4;

                        /* Update reserved/lock bytes status */
                        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_UpdateLockReservedOtp(
                            pT2T,
                            0x00));
                    }
                    break;

                case PHAL_TOP_T2T_MEMORY_CTRL_TLV:
                    if(pT2T->bMemoryTlvCount >= PHAL_TOP_T2T_MAX_MEM_CTRL_TLV)
                    {
                        return PH_ADD_COMPCODE(PHAL_TOP_ERR_UNSUPPORTED_TAG, PH_COMP_AL_TOP);
                    }

                    if(pT2T->bTagMemoryType != PHAL_TOP_T2T_TAG_MEMORY_TYPE_STATIC)
                    {
                        pT2T->asMemCtrlTlv[pT2T->bMemoryTlvCount].wOffset =
                            (bBlockNum * PHAL_TOP_T2T_BYTES_PER_BLOCK) + wIndex - PHAL_TOP_T2T_BYTES_PER_BLOCK;
                        bPageAddr = (uint8_t)((aData[wIndex + 2] & 0xF0) >> 4);
                        bByteOffset = aData[wIndex + 2] & 0x0F;
                        pT2T->asMemCtrlTlv[pT2T->bMemoryTlvCount].bSizeInBytes = aData[wIndex + 3];

                        bBytesPerPage = aData[wIndex + 4] & 0x0F;
                        pT2T->asMemCtrlTlv[pT2T->bMemoryTlvCount].bBytesPerPage = aData[wIndex + 4] & 0x0F;

                        pT2T->asMemCtrlTlv[pT2T->bMemoryTlvCount].wByteAddr =
                            (bPageAddr * (1 << bBytesPerPage)) + bByteOffset;
                        pT2T->bMemoryTlvCount++;
                        wIndex = wIndex + 4;

                        /* Update reserved/lock bytes status */
                        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_UpdateLockReservedOtp(
                            pT2T,
                            0x00));
                    }
                    break;

                case PHAL_TOP_T2T_NDEF_TLV:
                    pT2T->wNdefHeaderAddr = (bBlockNum * PHAL_TOP_T2T_BYTES_PER_BLOCK) + wIndex - PHAL_TOP_T2T_BYTES_PER_BLOCK;

                    aLength[0] = aData[wIndex + 1];

                    if(aLength[0] == 0xFF)
                    {
                        aLength[1] = aData[wIndex + 2];
                        aLength[2] = aData[wIndex + 3];
                        pDataParams->wNdefLength = ((uint16_t)aLength[1] << 8) | aLength[2];
                        pT2T->wNdefMsgAddr = pT2T->wNdefHeaderAddr + 4;
                        wIndex = (uint8_t)(wIndex + pDataParams->wNdefLength + 3);
                    }
                    else
                    {
                        pDataParams->wNdefLength = aLength[0];
                        pT2T->wNdefMsgAddr = pT2T->wNdefHeaderAddr + 2;
                        wIndex = (uint8_t)(wIndex + pDataParams->wNdefLength + 1);
                    }

                    /* Only one NDEF TLV is supported, exit loop */
                    bExitLoop = 1;
                    break;

                case PHAL_TOP_T2T_PROPRIETARY_TLV:
                    aLength[0] = aData[wIndex + 1];

                    if(aLength[0] == 0xFF)
                    {
                        aLength[1] = aData[wIndex + 2];
                        aLength[2] = aData[wIndex + 3];

                        wIndex = (uint8_t)(wIndex + (((uint16_t)aLength[1] << 8) | aLength[2]) + 3);
                    }
                    else
                    {
                        wIndex = wIndex + aLength[0] + 1;
                    }
                    break;

                case PHAL_TOP_T2T_TERMINATOR_TLV:
                    /* Exit loop after when terminator TLV is detected */
                    bExitLoop = 1;
                    break;

                default:
                    /* Invalid TLV, exit loop */
                    bExitLoop = 1;
                    break;
                }
                wIndex++;
            }

            if(!bExitLoop)
            {
                /* Copy last block of data as first block for next loop */
                memcpy(aData, &aData[bDataLen], 4);         /* PRQA S 3200 */

                /* Update starting index of data for next loop */
                if(wIndex >= bDataLen)
                {
                    wIndexInit = wIndex - bDataLen;
                }
            }
        }
        else
        {
            wIndexInit -= bDataLen;
        }

        /* Exit condition set, exit loop */
        if(bExitLoop)
        {
            break;
        }
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T2T_CheckNdef(
                                        phalTop_Sw_DataParams_t * pDataParams,
                                        uint8_t * pTagState
                                        )
{
    phStatus_t    PH_MEMLOC_REM status;
    uint8_t       PH_MEMLOC_BUF aData[16];
    phalTop_T2T_t PH_MEMLOC_REM * pT2T = pDataParams->pT2T;

    /* Reset tag state */
    *pTagState = PHAL_TOP_STATE_NONE;

    /* Clear values from previous detection, if any */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_ClearState(pDataParams, pT2T));

    /* Read CC */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Read(
        pT2T,
        PHAL_TOP_T2T_CC_BLOCK * PHAL_TOP_T2T_BYTES_PER_BLOCK,
        aData));

    /* Check for NDEF Magic Number */
    if(aData[0] != PHAL_TOP_T2T_NDEF_MAGIC_NUMBER)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_NON_NDEF_TAG, PH_COMP_AL_TOP);
    }

    /* Update version number */
    pDataParams->bVno = aData[1];

    /* Check for supported version number */
    if((uint8_t)(pDataParams->bVno & 0xF0) > (uint8_t)(PHAL_TOP_T2T_NDEF_SUPPORTED_VNO & 0xF0))
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_UNSUPPORTED_VERSION, PH_COMP_AL_TOP);
    }

    /* Update tag memory size */
    pT2T->bTms = aData[2];

    /* Validate tag memory size */
    if((pT2T->bTms * 8) == 48)
    {
        pT2T->bTagMemoryType = PHAL_TOP_T2T_TAG_MEMORY_TYPE_STATIC;
    }
    else if((pT2T->bTms * 8) > 48)
    {
        pT2T->bTagMemoryType = PHAL_TOP_T2T_TAG_MEMORY_TYPE_DYNAMIC;
    }
    else
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_MISCONFIGURED_TAG, PH_COMP_AL_TOP);
    }

    /* Update read/write access */
    pT2T->bRwa = aData[3];

    /* Check for proprietary and RFU values of R/W access */
    if((pT2T->bRwa != PHAL_TOP_T2T_CC_RWA_RW) && (pT2T->bRwa != PHAL_TOP_T2T_CC_RWA_RO))
    {
        /* Proprietary options; Not supported */
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_UNSUPPORTED_TAG, PH_COMP_AL_TOP);
    }

    /* Check for TLVs */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_DetectTlvBlocks(pDataParams, pT2T));

    /* Update tag state */
    if((pT2T->wNdefHeaderAddr) && (pDataParams->wNdefLength != 0))
    {
        if(pT2T->bRwa == PHAL_TOP_T2T_CC_RWA_RW)
        {
            pDataParams->bTagState = PHAL_TOP_STATE_READWRITE;
        }
        else
        {
            pDataParams->bTagState = PHAL_TOP_STATE_READONLY;
        }
    }
    else if((pT2T->wNdefHeaderAddr) && (pDataParams->wNdefLength == 0))
    {
        if(pT2T->bRwa == PHAL_TOP_T2T_CC_RWA_RW)
        {
            pDataParams->bTagState = PHAL_TOP_STATE_INITIALIZED;
        }
        else
        {
            return PH_ADD_COMPCODE(PHAL_TOP_ERR_MISCONFIGURED_TAG, PH_COMP_AL_TOP);
        }
    }
    else
    {
        /* No NDEF TLV detected */
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_MISCONFIGURED_TAG, PH_COMP_AL_TOP);
    }

    /* Update max. NDEF size */
    phalTop_Sw_Int_T2T_CalculateMaxNdefSize(pDataParams, pT2T);

    /* Update reserved/lock bytes status for first sector */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_UpdateLockReservedOtp(
        pT2T,
        0x00));

    /* Update state in out parameter */
    *pTagState = pDataParams->bTagState;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T2T_EraseNdef(
                                        phalTop_Sw_DataParams_t * pDataParams
                                        )
{
    phStatus_t    PH_MEMLOC_REM status;
    uint8_t       PH_MEMLOC_BUF aData[16];
    uint16_t      PH_MEMLOC_REM wOffset;
    phalTop_T2T_t PH_MEMLOC_REM * pT2T = pDataParams->pT2T;

    /* Check if tag is in valid state */
    if(pDataParams->bTagState == PHAL_TOP_STATE_NONE)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_INVALID_STATE, PH_COMP_AL_TOP);
    }

    /* Check if tag is read-only */
    if(pDataParams->bTagState == PHAL_TOP_STATE_READONLY)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_READONLY_TAG, PH_COMP_AL_TOP);
    }

    /* Check if tag is already in initialized state */
    if(pDataParams->bTagState == PHAL_TOP_STATE_INITIALIZED)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_EMPTY_NDEF, PH_COMP_AL_TOP);
    }

    /* NDEF length offset */
    wOffset = pT2T->wNdefHeaderAddr + 1;

    /* Read Block with NDEF length */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Read(
        pT2T,
        (pT2T->wNdefHeaderAddr + 1),
        aData));

    /* Set NDEF length as zero */
    aData[wOffset % PHAL_TOP_T2T_BYTES_PER_BLOCK] = 0x00;

    /* Write NDEF length */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Write(
        pT2T,
        (pT2T->wNdefHeaderAddr + 1),
        aData));

    /* Reset state variables */
    pDataParams->bTagState = PHAL_TOP_STATE_INITIALIZED;
    pDataParams->wNdefLength = 0x00;
    pT2T->wNdefMsgAddr = pT2T->wNdefHeaderAddr + 2;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T2T_FormatNdef(
                                         phalTop_Sw_DataParams_t * pDataParams
                                         )
{
    phStatus_t    PH_MEMLOC_REM   status;
    uint8_t       PH_MEMLOC_BUF   aData[16];
    uint16_t      PH_MEMLOC_COUNT wIndex;
    uint16_t      PH_MEMLOC_COUNT wWriteIndex;
    uint16_t      PH_MEMLOC_REM   wTlvOffset;
    uint8_t       PH_MEMLOC_BUF   aTlv[8];
    phalTop_T2T_t PH_MEMLOC_REM * pT2T = pDataParams->pT2T;
    uint8_t       PH_MEMLOC_BUF   aNdefData[16] = {0xD1, 0x01, 0x0C, 0x55, 0x00, 0x77,
                                              0x77, 0x77, 0x2E, 0x6E, 0x78, 0x70,
                                              0x2E, 0x63, 0x6F, 0x6D};

    /* Check for NDEF presence */
    if(pDataParams->bTagState != PHAL_TOP_STATE_NONE)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_FORMATTED_TAG, PH_COMP_AL_TOP);
    }

    /* Set NMN */
    aData[0] = 0xE1;

    /* If version is not set, set default version number */
    pDataParams->bVno = PHAL_TOP_T2T_NDEF_SUPPORTED_VNO;
    aData[1] = pDataParams->bVno;

    /* If TMS is not set/wrong, set TMS of static tag  */
    if(pT2T->bTms < 0x06)
    {
        pT2T->bTms = 0x06;
        pT2T->bTagMemoryType = PHAL_TOP_T2T_TAG_MEMORY_TYPE_STATIC;
        aData[2] = pT2T->bTms;
    }
    else
    {
        pT2T->bTagMemoryType = PHAL_TOP_T2T_TAG_MEMORY_TYPE_DYNAMIC;
        aData[2] = pT2T->bTms;
    }

    /* Set R/W access */
    pT2T->bRwa = 0x00;
    aData[3] = pT2T->bRwa;

    /* Write CC (CC is OTP) */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Write(
        pT2T,
        PHAL_TOP_T2T_CC_BLOCK * PHAL_TOP_T2T_BYTES_PER_BLOCK,
        aData));

    /* Read back CC */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Read(
        pT2T,
        PHAL_TOP_T2T_CC_BLOCK * PHAL_TOP_T2T_BYTES_PER_BLOCK,
        aData));

    /* Check if CC write was successful (CC is OTP, configuring CC can fail) */
    if((aData[0] != 0xE1) || (aData[1] != pDataParams->bVno) ||
       (aData[2] != pT2T->bTms) || (aData[3] != pT2T->bRwa))
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_UNSUPPORTED_TAG, PH_COMP_AL_TOP);
    }

    /* TLV offset (if present) */
    wTlvOffset = 4;

    /* Reset TLV data */
    memset(aTlv, 0, 8);  /* PRQA S 3200 */

    /* Write lock control TLVs (if specified) */
    for(wIndex = 0; ((wIndex < pT2T->bLockTlvCount) && (wIndex < PHAL_TOP_T2T_MAX_LOCK_CTRL_TLV)); wIndex++)
    {
        /* TLV offset from block start */
        pT2T->asLockCtrlTlv[wIndex].wOffset =  wTlvOffset;

        /* Create lock TLVs */
        aTlv[(wTlvOffset++) % 8] = 0x01;
        aTlv[(wTlvOffset++) % 8] = 0x03;
        aTlv[(wTlvOffset++) % 8] = (uint8_t)((uint16_t)(pT2T->asLockCtrlTlv[wIndex].wByteAddr /
                                   (1 << pT2T->asLockCtrlTlv[wIndex].bBytesPerPage)) << 4) |
                                   (uint8_t)(pT2T->asLockCtrlTlv[wIndex].wByteAddr %
                                   (1 << pT2T->asLockCtrlTlv[wIndex].bBytesPerPage));

        aTlv[(wTlvOffset++) % 8] = pT2T->asLockCtrlTlv[wIndex].bSizeInBits;
        aTlv[(wTlvOffset++) % 8] = (pT2T->asLockCtrlTlv[wIndex].bBytesLockedPerBit << 4) |
                                   pT2T->asLockCtrlTlv[wIndex].bBytesPerPage;

        /* Write lock TLVs */
        for(wWriteIndex = 0; wWriteIndex < 2; wWriteIndex++)
        {
            /* Write byte block */
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Write(
                pT2T,
                (pT2T->asLockCtrlTlv[wIndex].wOffset + (wWriteIndex * 4)),
                &aTlv[wWriteIndex * 4]));
        }

        /* Copy bytes present is last block to beginning */
        memcpy(aTlv, &aTlv[7 - (wTlvOffset % 4)], (wTlvOffset % 4));  /* PRQA S 3200 */
    }

    /* Write memory control TLVs (if specified) */
    for(wIndex = 0; ((wIndex < pT2T->bMemoryTlvCount) && (wIndex < PHAL_TOP_T2T_MAX_MEM_CTRL_TLV)); wIndex++)
    {
        /* TLV offset from block start */
        pT2T->asMemCtrlTlv[wIndex].wOffset =  wTlvOffset;

        /* Create memory TLV */
        aTlv[(wTlvOffset++) % 8] = 0x02;
        aTlv[(wTlvOffset++) % 8] = 0x03;
        aTlv[(wTlvOffset++) % 8] = (uint8_t)((uint16_t)(pT2T->asMemCtrlTlv[wIndex].wByteAddr /
                                   (1 << pT2T->asMemCtrlTlv[wIndex].bBytesPerPage)) << 4) |
                                   (uint8_t)(pT2T->asMemCtrlTlv[wIndex].wByteAddr %
                                   (1 << pT2T->asMemCtrlTlv[wIndex].bBytesPerPage));

        aTlv[(wTlvOffset++) % 8] = pT2T->asMemCtrlTlv[wIndex].bSizeInBytes;
        aTlv[(wTlvOffset++) % 8] = pT2T->asMemCtrlTlv[wIndex].bBytesPerPage;

        /* Write memory TLV */
        for(wWriteIndex = 0; wWriteIndex < 2; wWriteIndex++)
        {
            /* Write block */
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Write(
                pT2T,
                (pT2T->asMemCtrlTlv[wIndex].wOffset + (wWriteIndex * 4)),
                &aTlv[wWriteIndex * 4]));
        }
    }

    /* Update NDEF header address */
    if((pT2T->bLockTlvCount !=0) && (pT2T->bMemoryTlvCount != 0))
    {
        pT2T->wNdefHeaderAddr =
            (pT2T->asLockCtrlTlv[pT2T->bLockTlvCount - 1].wOffset >
            pT2T->asMemCtrlTlv[pT2T->bMemoryTlvCount - 1].wOffset?
            pT2T->asLockCtrlTlv[pT2T->bLockTlvCount - 1].wOffset:
        pT2T->asMemCtrlTlv[pT2T->bMemoryTlvCount - 1].wOffset) + 5;
    }
    else if(pT2T->bLockTlvCount !=0)
    {
        pT2T->wNdefHeaderAddr = pT2T->asLockCtrlTlv[pT2T->bLockTlvCount - 1].wOffset + 5;
    }
    else if(pT2T->bMemoryTlvCount != 0)
    {
        pT2T->wNdefHeaderAddr = pT2T->asMemCtrlTlv[pT2T->bMemoryTlvCount - 1].wOffset + 5;
    }
    else
    {
        pT2T->wNdefHeaderAddr = 16;
    }

    /* Update the values for using WriteNdef API for writing default NDEF */
    pDataParams->bTagState = PHAL_TOP_STATE_INITIALIZED;
    pDataParams->wNdefLength = 0;

    /* Update max. NDEF size */
    phalTop_Sw_Int_T2T_CalculateMaxNdefSize(pDataParams, pT2T);

    /* Write default NDEF Message (www.nxp.com) */
    status = phalTop_Sw_Int_T2T_WriteNdef(pDataParams, aNdefData, 16);

    if((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
    {
        /* Reset state on error */
        pDataParams->bTagState = PHAL_TOP_STATE_NONE;
        pT2T->wNdefHeaderAddr = 0x00;

        return status;
    }

    /* Update state and NDEF presence */
    pDataParams->bTagState = PHAL_TOP_STATE_READWRITE;
    pT2T->wNdefMsgAddr = pT2T->wNdefHeaderAddr + 2;
    pDataParams->wNdefLength = 0x10;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T2T_Read(
                                   phalTop_T2T_t * pT2T,
                                   uint16_t wOffset,
                                   uint8_t * pData
                                   )
{
    phStatus_t PH_MEMLOC_REM status;

    /* Check if read offset is in current sector */
    if(pT2T->sSector.bAddress != (uint8_t)(wOffset / PHAL_TOP_T2T_SECTOR_SIZE))
    {
        PH_CHECK_SUCCESS_FCT(status, phalMful_SectorSelect(
            pT2T->pAlT2TDataParams,
            (uint8_t)(wOffset / PHAL_TOP_T2T_SECTOR_SIZE)));

        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_UpdateLockReservedOtp(
            pT2T,
            (uint8_t)(wOffset / PHAL_TOP_T2T_SECTOR_SIZE)));
    }

    /* Read data */
    PH_CHECK_SUCCESS_FCT(status, phalMful_Read(
        pT2T->pAlT2TDataParams,
        (uint8_t)((wOffset % PHAL_TOP_T2T_SECTOR_SIZE) / PHAL_TOP_T2T_BYTES_PER_BLOCK),
        pData));

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T2T_Write(
                                    phalTop_T2T_t * pT2T,
                                    uint16_t wOffset,
                                    uint8_t * pData
                                    )
{
    phStatus_t PH_MEMLOC_REM status;

    /* Check if write offset is in current sector */
    if(pT2T->sSector.bAddress != (uint8_t)(wOffset / PHAL_TOP_T2T_SECTOR_SIZE))
    {
        PH_CHECK_SUCCESS_FCT(status, phalMful_SectorSelect(
            pT2T->pAlT2TDataParams,
            (uint8_t)(wOffset / PHAL_TOP_T2T_SECTOR_SIZE)));

        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_UpdateLockReservedOtp(
            pT2T,
            (uint8_t)(wOffset / PHAL_TOP_T2T_SECTOR_SIZE)));
    }

    /* Write data */
    PH_CHECK_SUCCESS_FCT(status, phalMful_Write(
        pT2T->pAlT2TDataParams,
        (uint8_t)((wOffset % PHAL_TOP_T2T_SECTOR_SIZE) / PHAL_TOP_T2T_BYTES_PER_BLOCK),
        pData));

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T2T_ReadNdef(
                                       phalTop_Sw_DataParams_t * pDataParams,
                                       uint8_t * pData,
                                       uint16_t * pLength
                                       )
{
    phStatus_t    PH_MEMLOC_REM   status;
    uint16_t      PH_MEMLOC_COUNT wIndex;
    uint16_t      PH_MEMLOC_COUNT wTempIndex;
    uint16_t      PH_MEMLOC_COUNT wCount;
    uint8_t       PH_MEMLOC_BUF   aData[16];
    uint16_t      PH_MEMLOC_REM   wIndexInit = 0;
    phalTop_T2T_t PH_MEMLOC_REM * pT2T = pDataParams->pT2T;

    /* Reset NDEF length */
    *pLength = 0;

    /* Check if tag is in valid state */
    if(pDataParams->bTagState == PHAL_TOP_STATE_NONE)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_INVALID_STATE, PH_COMP_AL_TOP);
    }

    /* Check for NDEF length > 0 (in initialized state NDEF length is 0) */
    if(pDataParams->bTagState == PHAL_TOP_STATE_INITIALIZED)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_EMPTY_NDEF, PH_COMP_AL_TOP);
    }

    /* Update lock/reserved/OTP block positions */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_UpdateLockReservedOtp(
        pT2T,
        (uint8_t)(pT2T->wNdefMsgAddr / PHAL_TOP_T2T_SECTOR_SIZE)));

    /* Block to start read */
    wIndexInit = pT2T->wNdefMsgAddr % PHAL_TOP_T2T_BYTES_PER_BLOCK;

    /* Read NDEF data */
    for(wIndex = pT2T->wNdefMsgAddr, wTempIndex = 0;
        wTempIndex < pDataParams->wNdefLength;
        wIndex += 16)
    {
        /* Read 16 bytes */
        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Read(
            pT2T,
            wIndex,
            aData));

        /* Copy valid data bytes */
        for(wCount = wIndexInit; ((wCount < 16) && (wTempIndex < pDataParams->wNdefLength)); wCount++)
        {
            if(phalTop_Sw_Int_T2T_CheckLockReservedOtp(pT2T, ((wIndex + wCount) / 4)))
            {
                pData[wTempIndex] = aData[wCount];
                wTempIndex++;
            }
        }

        wIndexInit = 0;
    }

    /* Update NDEF length */
    *pLength = pDataParams->wNdefLength;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T2T_WriteNdef(
                                        phalTop_Sw_DataParams_t * pDataParams,
                                        uint8_t * pData,
                                        uint16_t wLength
                                        )
{
    phStatus_t    PH_MEMLOC_REM   status;
    uint16_t      PH_MEMLOC_COUNT wIndex = 0;
    uint16_t      PH_MEMLOC_COUNT wTempIndex;
    uint8_t       PH_MEMLOC_BUF   aTempData[4];
    uint8_t       PH_MEMLOC_BUF   aData[16];
    uint16_t      PH_MEMLOC_COUNT wCount = 0;
    uint16_t      PH_MEMLOC_REM   wOffset;
    uint16_t      PH_MEMLOC_REM   wTernimatorTlvLen = 0;
    phalTop_T2T_t PH_MEMLOC_REM * pT2T = pDataParams->pT2T;

    /* Check if tag is in valid state */
    if(pDataParams->bTagState == PHAL_TOP_STATE_NONE)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_INVALID_STATE, PH_COMP_AL_TOP);
    }

    /* Check if tag is read-only */
    if(pDataParams->bTagState == PHAL_TOP_STATE_READONLY)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_READONLY_TAG, PH_COMP_AL_TOP);
    }

    /* Validate input parameters */
    if((wLength > pDataParams->wMaxNdefLength) || (wLength == 0x00))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_AL_TOP);
    }

    /* Read first block containing NDEF TLV */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Read(
        pT2T,
        pT2T->wNdefHeaderAddr,
        aData));

    /* NDEF TLV offset */
    wOffset = pT2T->wNdefHeaderAddr % PHAL_TOP_T2T_BYTES_PER_BLOCK;

    /* Set NDEF TLV length as 0 */
    aData[wOffset++] = PHAL_TOP_T2T_NDEF_TLV;
    aData[wOffset++] = 0x00;

    /* Calculate NDEF message address */
    if(wLength > 0xFE)
    {
        pT2T->wNdefMsgAddr = pT2T->wNdefHeaderAddr + 4;
        wOffset += 2;
    }
    else
    {
        pT2T->wNdefMsgAddr = pT2T->wNdefHeaderAddr + 2;
    }

    /* Update part of NDEF message in first block */
    for(wCount = 0; ((wOffset % PHAL_TOP_T2T_BYTES_PER_BLOCK) != 0); wCount++, wOffset++)
    {
        aData[wOffset] = pData[wCount];
    }

    /* Write the first block with NDEF TLV */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Write(
        pT2T,
        pT2T->wNdefHeaderAddr,
        aData));

    /* Write second block */
    if(wOffset > PHAL_TOP_T2T_BYTES_PER_BLOCK)
    {
        for(wIndex = (pT2T->wNdefHeaderAddr + PHAL_TOP_T2T_BYTES_PER_BLOCK);
            wIndex < pDataParams->wMaxNdefLength;
            wIndex += 4)
        {
            /* Check for reserved/lock blocks */
            if(phalTop_Sw_Int_T2T_CheckLockReservedOtp(pT2T, (wIndex / 4)))
            {
                /* Write the next block */
                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Write(
                    pT2T->pAlT2TDataParams,
                    wIndex,
                    &aData[4]));
                break;
            }
        }
    }

    /* Check if terminator TLV is needed/possible */
    if(wLength < pDataParams->wMaxNdefLength)
    {
        wTernimatorTlvLen = 1;
    }

    /* Write NDEF data */
    for(wIndex = (pT2T->wNdefMsgAddr + wCount), wTempIndex = wCount;
        wTempIndex < (wLength + wTernimatorTlvLen);
        wIndex += 4)
    {
        /* Check if write offset is in current sector */
        if(pT2T->sSector.bAddress != (uint8_t)(wIndex / PHAL_TOP_T2T_SECTOR_SIZE))
        {
            PH_CHECK_SUCCESS_FCT(status, phalMful_SectorSelect(
                pT2T->pAlT2TDataParams,
                (uint8_t)(wIndex / PHAL_TOP_T2T_SECTOR_SIZE)));

            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_UpdateLockReservedOtp(
                pT2T,
                (uint8_t)(wIndex / PHAL_TOP_T2T_SECTOR_SIZE)));
        }

        if(phalTop_Sw_Int_T2T_CheckLockReservedOtp(pT2T, (wIndex / 4)))
        {
            if((wLength - wTempIndex) >= PHAL_TOP_T2T_BYTES_PER_BLOCK)
            {
                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Write(
                    pT2T,
                    wIndex,
                    &pData[wTempIndex]));

                wTempIndex += 4;
            }
            /* Write last partially filled block */
            else
            {
                memset(aTempData, 0, 4);    /* PRQA S 3200 */

                for(wOffset = 0; wLength > wTempIndex; wOffset++, wTempIndex++)
                {
                    aTempData[wOffset] = pData[wTempIndex];
                }

                /* Set terminator TLV */
                aTempData[wOffset] = 0xFE;

                /* Write last partially filled block */
                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Write(
                    pT2T,
                    wIndex,
                    aTempData));

                wTempIndex++;
            }
        }
    }

    wOffset = pT2T->wNdefHeaderAddr % PHAL_TOP_T2T_BYTES_PER_BLOCK;

    /* If length more than 254, use 3 byte length format */
    if(wLength > 0xFE)
    {
        aData[wOffset] = PHAL_TOP_T2T_NDEF_TLV;
        aData[++wOffset] = 0xFF;
        aData[++wOffset] = (uint8_t)(wLength >> 8);
        aData[++wOffset] = (uint8_t)wLength;
    }
    else
    {
        aData[wOffset] = PHAL_TOP_T2T_NDEF_TLV;
        aData[++wOffset] = (uint8_t)wLength;
    }

    /* Write the first block with NDEF TLV */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Write(
        pT2T,
        pT2T->wNdefHeaderAddr,
        aData));

    /* Write second block */
    if(wOffset > PHAL_TOP_T2T_BYTES_PER_BLOCK)
    {
        for(wIndex = (pT2T->wNdefHeaderAddr + PHAL_TOP_T2T_BYTES_PER_BLOCK);
            wIndex < pDataParams->wMaxNdefLength;
            wIndex += 4)
        {
            /* Check for reserved/lock blocks */
            if(phalTop_Sw_Int_T2T_CheckLockReservedOtp(pT2T, (wIndex / 4)))
            {
                /* Write the next block */
                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T2T_Write(
                    pT2T->pAlT2TDataParams,
                    wIndex,
                    &aData[4]));
                break;
            }
        }
    }

    /* Update NDEF Length */
    pDataParams->wNdefLength = wLength;
    pDataParams->bTagState = PHAL_TOP_STATE_READWRITE;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

uint8_t phalTop_Sw_Int_T2T_CheckLockReservedOtp(
                                                phalTop_T2T_t * pT2T,
                                                uint16_t wIndex
                                                )
{
    if((pT2T->sSector.bLockReservedOtp[wIndex / 8]) & (uint8_t)(1 << (wIndex % 8)))
    {
        /* Lock/Reserved/OTP block */
        return 0x00;
    }
    else
    {
        /* Data block */
        return 0x01;
    }
}

void phalTop_Sw_Int_T2T_CalculateMaxNdefSize(
                                             phalTop_Sw_DataParams_t * pDataParams,
                                             phalTop_T2T_t * pT2T
                                             )
{
    uint16_t PH_MEMLOC_COUNT wIndex;
    uint16_t PH_MEMLOC_REM   wTagSize;

    if(pT2T->bTagMemoryType == PHAL_TOP_T2T_TAG_MEMORY_TYPE_STATIC)
    {
        pDataParams->wMaxNdefLength = 64 - pT2T->wNdefMsgAddr;
    }
    else
    {
        /* Reset max. NDEF length */
        pDataParams->wMaxNdefLength = 0;

        /* Calculate total tag size including lock and reserved bytes */
        wTagSize = ((pT2T->bTms) * 8) + 16;

        /* Add lock TLV length */
        for(wIndex = 0; wIndex < pT2T->bLockTlvCount; wIndex++)
        {
            wTagSize += (pT2T->asLockCtrlTlv[wIndex].bSizeInBits / 8);

            if((pT2T->asLockCtrlTlv[wIndex].bSizeInBits % 8))
            {
                wTagSize++;
            }
        }

        /* Add memory TLV length */
        for(wIndex = 0; wIndex < pT2T->bMemoryTlvCount; wIndex++)
        {
            wTagSize += pT2T->asMemCtrlTlv[wIndex].bSizeInBytes;
        }

        /* Calculate max. NDEF TLV length */
        for(wIndex = pT2T->wNdefHeaderAddr; wIndex < wTagSize; wIndex++)
        {
            /* Update Lock/Reserved/OTP block status when sector changes */
            if((wIndex % PHAL_TOP_T2T_SECTOR_SIZE) == 0)
            {
                (void)phalTop_Sw_Int_T2T_UpdateLockReservedOtp(
                    pT2T,
                    (uint8_t)(wIndex / PHAL_TOP_T2T_SECTOR_SIZE));
            }

            /* Increment max. NDEF length if not lock/OTP/reserved byte */
            if(phalTop_Sw_Int_T2T_CheckLockReservedOtp(pT2T, (wIndex / 4)))
            {
                pDataParams->wMaxNdefLength++;
            }
        }

        /* Subtract T and L field length to get NDEF message (V field) length */
        if(pDataParams->wMaxNdefLength < 0xFF)
        {
            pDataParams->wMaxNdefLength -= 2;
        }
        else
        {
            pDataParams->wMaxNdefLength -= 4;
        }
    }
}

phStatus_t phalTop_Sw_Int_T2T_UpdateLockReservedOtp(
                                                    phalTop_T2T_t * pT2T,
                                                    uint8_t bSector
                                                    )
{
    uint16_t PH_MEMLOC_REM   wOffset;
    uint16_t PH_MEMLOC_REM   wSectorOffset;
    uint16_t PH_MEMLOC_REM   wBlockOffset;
    uint16_t PH_MEMLOC_COUNT wIndex;

    /* Update sector */
    pT2T->sSector.bAddress = bSector;

    if(pT2T->sSector.bAddress == 0)
    {
        /* First 3 blocks are internal/lock for sector 0 */
        pT2T->sSector.bLockReservedOtp[0x00] = 0x07;
    }

    if(pT2T->bTagMemoryType == PHAL_TOP_T2T_TAG_MEMORY_TYPE_DYNAMIC)
    {
        /* Update Lock Bytes */
        for(wIndex = 0; wIndex < pT2T->bLockTlvCount; wIndex++)
        {
            if((pT2T->asLockCtrlTlv[wIndex].wByteAddr / PHAL_TOP_T2T_SECTOR_SIZE) == pT2T->sSector.bAddress)
            {
                wSectorOffset = (pT2T->asLockCtrlTlv[wIndex].wByteAddr %
                                PHAL_TOP_T2T_SECTOR_SIZE) / PHAL_TOP_T2T_BYTES_PER_BLOCK;

                if((pT2T->asLockCtrlTlv[wIndex].bSizeInBits / 8) % PHAL_TOP_T2T_BYTES_PER_BLOCK)
                {
                    wBlockOffset = 1;
                }
                else
                {
                    wBlockOffset = 0;
                }

                for(wOffset = wSectorOffset;
                    wOffset < (wSectorOffset +
                    ((pT2T->asLockCtrlTlv[wIndex].bSizeInBits / 8) / PHAL_TOP_T2T_BYTES_PER_BLOCK) +
                    wBlockOffset);
                    wOffset++)
                {
                    pT2T->sSector.bLockReservedOtp[wOffset / 8] |= (uint8_t)(1 << (wOffset % 8));
                }
            }
        }
        /* Update Memory Bytes */
        for(wIndex = 0; wIndex < pT2T->bMemoryTlvCount; wIndex++)
        {
            if(pT2T->asMemCtrlTlv[wIndex].wByteAddr / PHAL_TOP_T2T_SECTOR_SIZE == pT2T->sSector.bAddress)
            {
                wSectorOffset = (pT2T->asMemCtrlTlv[wIndex].wByteAddr % PHAL_TOP_T2T_SECTOR_SIZE) /
                                PHAL_TOP_T2T_BYTES_PER_BLOCK;

                if((pT2T->asMemCtrlTlv[wIndex].bSizeInBytes) / PHAL_TOP_T2T_BYTES_PER_BLOCK)
                {
                    wBlockOffset = 1;
                }
                else
                {
                    wBlockOffset = 0;
                }

                for(wOffset = wSectorOffset;
                    wOffset < (wSectorOffset +
                    ((pT2T->asMemCtrlTlv[wIndex].bSizeInBytes) / PHAL_TOP_T2T_BYTES_PER_BLOCK) +
                    wBlockOffset);
                    wOffset++)
                {
                    pT2T->sSector.bLockReservedOtp[wOffset / 8] |= (uint8_t)(1 << (wOffset % 8));
                }
            }
        }
    }
    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

#endif /* NXPBUILD__PHAL_MFUL_SW */
#endif /* NXPBUILD__PHAL_TOP_SW */
