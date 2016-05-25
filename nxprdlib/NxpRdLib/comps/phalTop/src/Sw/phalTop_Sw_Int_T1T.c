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
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 3467 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*/

#include <ph_TypeDefs.h>
#include <ph_Status.h>
#include <ph_RefDefs.h>
#include <phacDiscLoop.h>
#include <phalT1T.h>
#include <phalTop.h>

#ifdef NXPBUILD__PHAL_TOP_SW
#ifdef NXPBUILD__PHAL_T1T_SW

#include "phalTop_Sw_Int_T1T.h"

phStatus_t phalTop_Sw_Int_T1T_SetReadOnly(
                                          phalTop_Sw_DataParams_t * pDataParams
                                          )
{
    phStatus_t    PH_MEMLOC_REM   status;
    uint8_t       PH_MEMLOC_BUF   aRxData[11];
    uint16_t      PH_MEMLOC_REM   wRxLength;
    uint16_t      PH_MEMLOC_COUNT wIndex;
    uint16_t      PH_MEMLOC_COUNT wTempIndex;
    phalTop_T1T_t PH_MEMLOC_REM   * pT1T = pDataParams->pT1T;

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

    /* Update RW access */
    pT1T->bRwa = PHAL_TOP_T1T_CC_RWA_RO;

    /* Write Read/Write access byte*/
    PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
        pT1T->pAlT1TDataParams,
        pT1T->bUid,
        PHAL_TOP_T1T_BLOCK_RWA,
        pT1T->bRwa,
        aRxData,
        &wRxLength));

    /* Update Lock-0 */
    PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteNoEraseByte(
        pT1T->pAlT1TDataParams,
        pT1T->bUid,
        PHAL_TOP_T1T_BLOCK_LOCK0,
        0xFF,
        aRxData,
        &wRxLength));

    /* Update Lock-1 */
    PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteNoEraseByte(
        pT1T->pAlT1TDataParams,
        pT1T->bUid,
        PHAL_TOP_T1T_BLOCK_LOCK1,
        0xFF,
        aRxData,
        &wRxLength));

    /* Update dynamic lock bytes */
    for(wIndex = 0; wIndex < pT1T->bLockTlvCount; wIndex++)
    {
        for(wTempIndex = 0; wTempIndex < (pT1T->asLockCtrlTlv[wIndex].bSizeInBits / 8); wTempIndex++)
        {
            PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteNoEraseByte(
                pT1T->pAlT1TDataParams,
                pT1T->bUid,
                (uint8_t)(pT1T->asLockCtrlTlv[wIndex].wByteAddr + wTempIndex),
                0xFF,
                aRxData,
                &wRxLength));
        }

        if(pT1T->asLockCtrlTlv[wIndex].bSizeInBits % 8)
        {
            PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteNoEraseByte(
                pT1T->pAlT1TDataParams,
                pT1T->bUid,
                (uint8_t)(pT1T->asLockCtrlTlv[wIndex].wByteAddr + wTempIndex),
                0xFF,
                aRxData,
                &wRxLength));
        }
    }

    pDataParams->bTagState = PHAL_TOP_STATE_READONLY;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

void phalTop_Sw_Int_T1T_CalculateMaxNdefSize(
                                             phalTop_Sw_DataParams_t * pDataParams,
                                             phalTop_T1T_t * pT1T
                                             )
{
    uint16_t   PH_MEMLOC_COUNT wIndex;
    uint16_t   PH_MEMLOC_REM   wTagSize;

    if(pT1T->bTagMemoryType == PHAL_TOP_T1T_TAG_MEMORY_TYPE_STATIC)
    {
        pDataParams->wMaxNdefLength = 90;
    }
    else
    {
        /* Reset max. NDEF length */
        pDataParams->wMaxNdefLength = 0;

        /* Calculate total tag size including lock and reserved bytes */
        wTagSize = (pT1T->bTms + 1) * 8;

        /* Calculate NDEF Header Address */
        if((pT1T->bLockTlvCount !=0) && (pT1T->bMemoryTlvCount != 0))
        {
            pT1T->wNdefHeaderAddr = (pT1T->asLockCtrlTlv[pT1T->bLockTlvCount - 1].wOffset >
                                    pT1T->asMemCtrlTlv[pT1T->bMemoryTlvCount - 1].wOffset?
                                    pT1T->asLockCtrlTlv[pT1T->bLockTlvCount - 1].wOffset:
                                    (pT1T->asMemCtrlTlv[pT1T->bMemoryTlvCount - 1].wOffset) + 5);
        }
        else if(pT1T->bLockTlvCount !=0)
        {
            pT1T->wNdefHeaderAddr = pT1T->asLockCtrlTlv[pT1T->bLockTlvCount - 1].wOffset + 5;
        }
        else if(pT1T->bMemoryTlvCount != 0)
        {
            pT1T->wNdefHeaderAddr = pT1T->asMemCtrlTlv[pT1T->bMemoryTlvCount - 1].wOffset + 5;
        }
        else
        {
            pT1T->wNdefHeaderAddr = 12;
        }

        /* Calculate max. NDEF TLV length */
        for(wIndex = pT1T->wNdefHeaderAddr; wIndex < wTagSize; wIndex++)
        {
            /* Update Lock/Reserved/OTP block status when segment changes */
            if((wIndex % PHAL_TOP_T1T_SEGMENT_SIZE) == 0)
            {
                pT1T->sSegment.bAddress = (uint8_t)(wIndex / PHAL_TOP_T1T_SEGMENT_SIZE);
                (void)phalTop_Sw_Int_T1T_UpdateLockReservedOtp(pT1T);
            }

            /* Increment max. NDEF length if not lock/OTP/reserved byte */
            if(phalTop_Sw_Int_T1T_CheckLockReservedOtp(pT1T, wIndex))
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

phStatus_t phalTop_Sw_Int_T1T_UpdateLockReservedOtp(
                                                    phalTop_T1T_t * pT1T
                                                    )
{
    uint16_t PH_MEMLOC_COUNT wOffset;
    uint16_t PH_MEMLOC_COUNT wIndex;
    uint16_t PH_MEMLOC_REM   wSegmentOffset;

    memset(pT1T->sSegment.bLockReservedOtp, 0, 16);     /* PRQA S 3200 */

    if(pT1T->sSegment.bAddress == 0)
    {
        pT1T->sSegment.bLockReservedOtp[0x00] = 0xFF;
        pT1T->sSegment.bLockReservedOtp[0x0D] = 0xFF;
        pT1T->sSegment.bLockReservedOtp[0x0E] = 0xFF;
        pT1T->sSegment.bLockReservedOtp[0x0F] = 0xFF;
    }
    else
    {
        /* Update Lock Bytes */
        for(wIndex = 0; wIndex < pT1T->bLockTlvCount; wIndex++)
        {
            if((pT1T->asLockCtrlTlv[wIndex].wByteAddr / PHAL_TOP_T1T_SEGMENT_SIZE) == pT1T->sSegment.bAddress)
            {
                wSegmentOffset = pT1T->asLockCtrlTlv[wIndex].wByteAddr % PHAL_TOP_T1T_SEGMENT_SIZE;

                for(wOffset = wSegmentOffset;
                    wOffset < (wSegmentOffset + (pT1T->asLockCtrlTlv[wIndex].bSizeInBits / 8));
                    wOffset++)
                {
                    pT1T->sSegment.bLockReservedOtp[wOffset / 8] |=
                        (uint8_t)(1 << (wOffset % 8));
                }
            }
        }

        /* Update Memory Bytes */
        for(wIndex = 0; wIndex < pT1T->bMemoryTlvCount; wIndex++)
        {
            if(pT1T->asMemCtrlTlv[wIndex].wByteAddr / PHAL_TOP_T1T_SEGMENT_SIZE == pT1T->sSegment.bAddress)
            {
                wSegmentOffset = pT1T->asMemCtrlTlv[wIndex].wByteAddr % PHAL_TOP_T1T_SEGMENT_SIZE;

                for(wOffset = wSegmentOffset;
                    wOffset < (wSegmentOffset + (pT1T->asMemCtrlTlv[wIndex].bSizeInBytes));
                    wOffset++)
                {
                    pT1T->sSegment.bLockReservedOtp[wOffset / 8] |= (uint8_t)(1 << (wOffset % 8));
                }
            }
        }
    }
    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

uint8_t phalTop_Sw_Int_T1T_CheckLockReservedOtp(
                                                phalTop_T1T_t * pT1T,
                                                uint16_t wIndex
                                                )
{
    if((pT1T->sSegment.bLockReservedOtp[(wIndex % PHAL_TOP_T1T_SEGMENT_SIZE) / 8]) && (1 << (wIndex % 8)))
    {
        /* Lock/Reserved/OTP block */
        return false;
    }
    else
    {
        /* Data block */
        return true;
    }
}

phStatus_t phalTop_Sw_Int_T1T_Read(
                                   phalTop_T1T_t * pT1T,
                                   uint16_t wAddress,
                                   uint8_t * pData,
                                   uint8_t bFreshRead
                                   )
{
    phStatus_t PH_MEMLOC_REM status;
    uint16_t   PH_MEMLOC_REM wLength;
    uint8_t    PH_MEMLOC_REM * pRxData;

    if(pT1T->bTagMemoryType != PHAL_TOP_T1T_TAG_MEMORY_TYPE_STATIC)
    {
        if(bFreshRead == PH_ON)
        {
            /* Get segment address */
            pT1T->sSegment.bAddress = (uint8_t)(wAddress / PHAL_TOP_T1T_SEGMENT_SIZE);

            /* Read segment */
            PH_CHECK_SUCCESS_FCT(status, phalT1T_ReadSegment(
                pT1T->pAlT1TDataParams,
                pT1T->bUid,
                (uint8_t)((pT1T->sSegment.bAddress << 4) & 0xF0),
                &pRxData,
                &wLength));

            /* Remove ADDS */
            pT1T->sSegment.pData = pRxData + 1;
        }

        *pData = pT1T->sSegment.pData[wAddress % PHAL_TOP_T1T_SEGMENT_SIZE];
    }
    else
    {
        if(bFreshRead == PH_ON)
        {
            /* First segment */
            pT1T->sSegment.bAddress = 0;

            /* Read first segment */
            PH_CHECK_SUCCESS_FCT(status, phalT1T_ReadAll(
                pT1T->pAlT1TDataParams,
                pT1T->bUid,
                &pRxData,
                &wLength));

            /* Remove HR0 and HR1 */
            pT1T->sSegment.pData = pRxData + 2;
        }

        *pData = pT1T->sSegment.pData[wAddress];
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T1T_Write(
                                    phalTop_Sw_DataParams_t * pDataParams,
                                    phalTop_T1T_t * pT1T,
                                    uint16_t wAddress,
                                    uint8_t * pData,
                                    uint16_t pDataIndex
                                    )
{
    phStatus_t PH_MEMLOC_REM   status;
    uint8_t    PH_MEMLOC_BUF   aRxData[11];
    uint8_t    PH_MEMLOC_BUF   aTempData[11];
    uint16_t   PH_MEMLOC_REM   wRxLength;
    uint8_t    PH_MEMLOC_REM   bFirstBlock = 0;
    uint8_t    PH_MEMLOC_REM   bLastBlock = 0;
    uint16_t   PH_MEMLOC_COUNT wIndex = 0;

    if(pT1T->bTagMemoryType != PHAL_TOP_T1T_TAG_MEMORY_TYPE_STATIC)
    {
        if((wAddress % 8) == 0)
        {
            if((pDataIndex + 8) <= pDataParams->wNdefLength)
            {
                PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseBlock(
                    pT1T->pAlT1TDataParams,
                    pT1T->bUid,
                    (uint8_t)(wAddress / 8),
                    &pData[pDataIndex],
                    aRxData,
                    &wRxLength));
            }
            else
            {
                /* If Last block of NDEF message is less than 8 bytes, write
                 * byte by byte */
                bLastBlock = 1;
            }
        }

        if((pT1T->wNdefMsgAddr % 8) && (pT1T->wNdefMsgAddr == wAddress))
        {
            /* If First block of NDEF message is not starting from start of a
             * block, write byte by byte. */
            bFirstBlock = 1;
        }
    }
    else
    {
        PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
            pT1T->pAlT1TDataParams,
            pT1T->bUid,
            (uint8_t)wAddress,
            pData[pDataIndex],
            aRxData,
            &wRxLength));

        /* Write Terminator TLV, if enabled and space is available */
        if((pT1T->bTerminatorTlvPresence == PH_ON) && ((pDataIndex + 1) == pDataParams->wNdefLength))
        {
            /* Loop is needed, if current byte address is Lock, Reserved Memory
             * or OTP. Loop till we find data byte. */
            for(wIndex = (wAddress + 1); wIndex < ((pT1T->bTms + 1) * 8); wIndex++)
            {
                if(phalTop_Sw_Int_T1T_CheckLockReservedOtp(pT1T, wIndex))
                {
                    PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
                        pT1T->pAlT1TDataParams,
                        pT1T->bUid,
                        (uint8_t)wIndex,
                        0xFE,
                        aRxData,
                        &wRxLength));

                    break;
                }
            }
        }
    }

    /* Write partially filled first block */
    if(bFirstBlock)
    {
        PH_CHECK_SUCCESS_FCT(status, phalT1T_ReadBlock(
            pT1T->pAlT1TDataParams,
            pT1T->bUid,
            (uint8_t)(wAddress / 8),
            aTempData,
            &wRxLength));

        memcpy(&aTempData[(wAddress % 8) + 1], &pData[pDataIndex], (8 - (wAddress % 8)));   /* PRQA S 3200 */

        PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseBlock(
            pT1T->pAlT1TDataParams,
            pT1T->bUid,
            (uint8_t)(wAddress / 8),
            &aTempData[1],
            aRxData,
            &wRxLength));
    }
    /* Write partially filled last block */
    else if(bLastBlock)
    {
        memset(aTempData, 0, 8);    /* PRQA S 3200 */
        memcpy(aTempData, &pData[pDataIndex], (pDataParams->wNdefLength - pDataIndex));    /* PRQA S 3200 */

        /* Add terminator TLV, if enabled */
        if(((pDataParams->wNdefLength - pDataIndex) < 8) && (pT1T->bTerminatorTlvPresence == PH_ON))
        {
            aTempData[(pDataParams->wNdefLength - pDataIndex)] = 0xFE;
        }

        PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseBlock(
            pT1T->pAlT1TDataParams,
            pT1T->bUid,
            (uint8_t)(wAddress / 8),
            aTempData,
            aRxData,
            &wRxLength));

        /* Write terminator TLV, if enabled and space is available */
        if(((pDataParams->wNdefLength - pDataIndex) == 8) && (pT1T->bTerminatorTlvPresence == PH_ON))
        {
            memset(aTempData, 0, 8);    /* PRQA S 3200 */
            aTempData[0] = 0xFE;

            /* Loop is needed, if current byte address is Lock, Reserved Memory
             * or OTP. Loop till we find data byte. */
            for(wIndex = ((wAddress / 8) + 1); (wIndex * 8) < ((pT1T->bTms + 1) * 8); wIndex++)
            {
                if(phalTop_Sw_Int_T1T_CheckLockReservedOtp(pT1T, (wIndex * 8)))
                {
                    PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseBlock(
                        pT1T->pAlT1TDataParams,
                        pT1T->bUid,
                        (uint8_t)wIndex,
                        aTempData,
                        aRxData,
                        &wRxLength));
                }
            }
        }
    }
    else
    {
        /* Do Nothing */
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T1T_ClearState(
                                         phalTop_Sw_DataParams_t * pDataParams,
                                         phalTop_T1T_t * pT1T
                                         )
{
    /* Reset parameters */
    pT1T->bLockTlvCount = 0;
    pT1T->bRwa = 0;
    pT1T->bTms = 0;
    pDataParams->bVno = 0;
    pT1T->bMemoryTlvCount = 0;
    pT1T->bTagMemoryType = 0;
    pDataParams->bTagState = 0;
    pT1T->bTerminatorTlvPresence = 0;
    pDataParams->wMaxNdefLength = 0;
    pT1T->wNdefHeaderAddr = 0;
    pDataParams->wNdefLength = 0;
    pT1T->wNdefMsgAddr = 0;
    pT1T->sSegment.bAddress = 0;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

/* Finds the count and position of memory, lock and NDEF TLV */
phStatus_t phalTop_Sw_Int_T1T_DetectTlvBlocks(
                                              phalTop_Sw_DataParams_t * pDataParams,
                                              phalTop_T1T_t * pT1T
                                              )
{
    phStatus_t PH_MEMLOC_REM   status;
    uint16_t   PH_MEMLOC_COUNT wIndex;
    uint8_t    PH_MEMLOC_REM   bPageAddr;
    uint8_t    PH_MEMLOC_REM   bByteOffset;
    uint8_t    PH_MEMLOC_REM   bBytesPerPage;
    uint8_t    PH_MEMLOC_BUF   aLength[3];
    uint8_t    PH_MEMLOC_REM   bExitLoop = 0;
    uint8_t    PH_MEMLOC_BUF   aData[1];

    /* Check for NDEF TLV is done only in first segment */
    wIndex = PHAL_TOP_T1T_SEGMENT_START_INDEX;

    /* Loop through first segment */
    while(wIndex < PHAL_TOP_T1T_SEGMENT_END_INDEX)
    {
        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
            pT1T,
            wIndex,
            aData,
            PH_OFF));

        switch(aData[0])
        {
        case PHAL_TOP_T1T_NULL_TLV:
            break;

        case PHAL_TOP_T1T_LOCK_CTRL_TLV:
            if(pT1T->bLockTlvCount >= PHAL_TOP_T1T_MAX_LOCK_CTRL_TLV)
            {
                return PH_ADD_COMPCODE(PHAL_TOP_ERR_UNSUPPORTED_TAG, PH_COMP_AL_TOP);
            }

            if(pT1T->bTagMemoryType != PHAL_TOP_T1T_TAG_MEMORY_TYPE_STATIC)
            {
                /* Read TLV length */
                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
                    pT1T,
                    (wIndex + 1),
                    aData,
                    PH_OFF));

                /* Validate length */
                if(aData[0] != 3)
                {
                    bExitLoop = 1;
                    break;
                }

                pT1T->asLockCtrlTlv[pT1T->bLockTlvCount].wOffset = wIndex;

                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
                    pT1T,
                    (wIndex + 2),
                    aData,
                    PH_OFF));

                bPageAddr = (uint8_t)((aData[0] & 0xF0) >> 4);
                bByteOffset = aData[0] & 0x0F;

                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
                    pT1T,
                    (wIndex + 3),
                    aData,
                    PH_OFF));

                pT1T->asLockCtrlTlv[pT1T->bLockTlvCount].bSizeInBits = aData[0];

                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
                    pT1T,
                    (wIndex + 4),
                    aData,
                    PH_OFF));

                bBytesPerPage = aData[0] & 0x0F;
                pT1T->asLockCtrlTlv[pT1T->bLockTlvCount].bBytesPerPage = aData[0] & 0x0F;
                pT1T->asLockCtrlTlv[pT1T->bLockTlvCount].bBytesLockedPerBit = (uint8_t)((aData[0] & 0xF0) >> 4);
                pT1T->asLockCtrlTlv[pT1T->bLockTlvCount].wByteAddr = (bPageAddr * (1 << bBytesPerPage)) + bByteOffset;
                pT1T->bLockTlvCount++;

                wIndex = wIndex + 4;
            }
            break;

        case PHAL_TOP_T1T_MEMORY_CTRL_TLV:
            if(pT1T->bMemoryTlvCount >= PHAL_TOP_T1T_MAX_MEM_CTRL_TLV)
            {
                return PH_ADD_COMPCODE(PHAL_TOP_ERR_UNSUPPORTED_TAG, PH_COMP_AL_TOP);
            }

            if(pT1T->bTagMemoryType != PHAL_TOP_T1T_TAG_MEMORY_TYPE_STATIC)
            {
                /* Read TLV length */
                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
                    pT1T,
                    (wIndex + 1),
                    aData,
                    PH_OFF));

                /* Validate length */
                if(aData[0] != 3)
                {
                    bExitLoop = 1;
                    break;
                }

                pT1T->asMemCtrlTlv[pT1T->bMemoryTlvCount].wOffset = wIndex;

                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
                    pT1T,
                    (wIndex + 2),
                    aData,
                    PH_OFF));

                bPageAddr = (uint8_t)((aData[0] & 0xF0) >> 4);
                bByteOffset = (uint8_t)(aData[0] & 0x0F);

                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
                    pT1T,
                    (wIndex + 3),
                    aData,
                    PH_OFF));

                pT1T->asMemCtrlTlv[pT1T->bMemoryTlvCount].bSizeInBytes = aData[0];

                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
                    pT1T,
                    (wIndex + 4),
                    aData,
                    PH_OFF));

                bBytesPerPage = aData[0] & 0x0F;
                pT1T->asMemCtrlTlv[pT1T->bMemoryTlvCount].bBytesPerPage = aData[0] & 0x0F;

                pT1T->asMemCtrlTlv[pT1T->bMemoryTlvCount].wByteAddr = (bPageAddr * (1 << bBytesPerPage)) + bByteOffset;
                pT1T->bMemoryTlvCount++;

                wIndex = wIndex + 4;
            }
            break;

        case PHAL_TOP_T1T_NDEF_TLV:
            pT1T->wNdefHeaderAddr = wIndex;

            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
                pT1T,
                (wIndex + 1),
                aData,
                PH_OFF));

            aLength[0] = aData[0];

            if(aLength[0] == 0xFF)
            {
                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
                    pT1T,
                    (wIndex + 2),
                    aData,
                    PH_OFF));

                aLength[1] = aData[0];

                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
                    pT1T,
                    (wIndex + 3),
                    aData,
                    PH_OFF));

                aLength[2] = aData[0];
                pDataParams->wNdefLength = ((uint16_t)aLength[1] << 8) | aLength[2];
                pT1T->wNdefMsgAddr = pT1T->wNdefHeaderAddr + 4;

                wIndex = wIndex + pDataParams->wNdefLength + 3;
            }
            else
            {
                pDataParams->wNdefLength = aLength[0];
                pT1T->wNdefMsgAddr = pT1T->wNdefHeaderAddr + 2;

                wIndex = wIndex + pDataParams->wNdefLength + 1;
            }

            bExitLoop = 1;
            break;

        case PHAL_TOP_T1T_PROPRIETARY_TLV:
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
                pT1T,
                (wIndex + 1),
                aData,
                PH_OFF));

            aLength[0] = aData[0];

            if(aLength[0] == 0xFF)
            {
                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
                    pT1T,
                    (wIndex + 2),
                    aData,
                    PH_OFF));

                aLength[1] = aData[0];

                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
                    pT1T,
                    (wIndex + 3),
                    aData, PH_OFF));

                aLength[2] = aData[0];

                wIndex = wIndex + (((uint16_t)aLength[1] << 8) | aLength[2]) + 3;
            }
            else
            {
                wIndex = wIndex + aLength[0] + 1;
            }
            break;

        case PHAL_TOP_T1T_TERMINATOR_TLV:
            pT1T->bTerminatorTlvPresence = PH_ON;
            bExitLoop = 1;
            break;

        default:
            bExitLoop = 1;
            break;
        }

        if (bExitLoop)
        {
            break;
        }

        wIndex++;
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T1T_CheckNdef(
                                        phalTop_Sw_DataParams_t * pDataParams,
                                        uint8_t * pTagState
                                        )
{
    phStatus_t    PH_MEMLOC_REM status;
    uint8_t       PH_MEMLOC_REM aData[1];
    phalTop_T1T_t PH_MEMLOC_REM * pT1T = pDataParams->pT1T;

    /* Update UID */
    memcpy(pT1T->bUid, ((phalT1T_Sw_DataParams_t *)(pT1T->pAlT1TDataParams))->abUid, 4);   /* PRQA S 3200 */

    /* Reset tag state */
    *pTagState = PHAL_TOP_STATE_NONE;

    /* Clear values from previous detection, if any */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_ClearState(pDataParams, pT1T));

    /* Check for NDEF support */
    if((((phalT1T_Sw_DataParams_t *)(pT1T->pAlT1TDataParams))->abHR[0] & 0x10) != 0x10)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_NON_NDEF_TAG, PH_COMP_AL_TOP);
    }

    /* Check for memory type */
    if((((phalT1T_Sw_DataParams_t *)(pT1T->pAlT1TDataParams))->abHR[0] & 0x01) == 0x01)
    {
        pT1T->bTagMemoryType = PHAL_TOP_T1T_TAG_MEMORY_TYPE_STATIC;
    }
    else
    {
        pT1T->bTagMemoryType = PHAL_TOP_T1T_TAG_MEMORY_TYPE_DYNAMIC;
    }

    /* Read NDEF Magic Number (NMN) */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
        pT1T,
        PHAL_TOP_T1T_CC_NMN_ADDR,
        aData,
        PH_ON));

    /* Validate NMN */
    if(aData[0] != PHAL_TOP_T1T_NDEF_NMN)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_NON_NDEF_TAG, PH_COMP_AL_TOP);
    }

    /* Read version number */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
        pT1T,
        PHAL_TOP_T1T_CC_VNO_ADDR,
        aData,
        PH_OFF));

    /* Update version */
    pDataParams->bVno = aData[0];

    /* Validate if version is supported */
    if((uint8_t)(pDataParams->bVno & 0xF0) > (uint8_t)(PHAL_TOP_T1T_NDEF_SUPPORTED_VNO & 0xF0))
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_UNSUPPORTED_VERSION, PH_COMP_AL_TOP);
    }

    /* Read TMS */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
        pT1T,
        PHAL_TOP_T1T_CC_TMS_ADDR,
        aData,
        PH_OFF));

    /* Update TMS */
    pT1T->bTms = aData[0];

    /* Validate TMS */
    if(pT1T->bTms < 0x0E)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_MISCONFIGURED_TAG, PH_COMP_AL_TOP);
    }

    /* Validate TMS and Tag Memory Type */
    if(((pT1T->bTms == 0x0E) && (pT1T->bTagMemoryType != PHAL_TOP_T1T_TAG_MEMORY_TYPE_STATIC)) ||
       ((pT1T->bTms > 0x0E) && (pT1T->bTagMemoryType != PHAL_TOP_T1T_TAG_MEMORY_TYPE_DYNAMIC)))
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_MISCONFIGURED_TAG, PH_COMP_AL_TOP);
    }

    /* Read R/W access */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
        pT1T,
        PHAL_TOP_T1T_CC_RWA_ADDR,
        aData,
        PH_OFF));

    /* Update read/write access */
    pT1T->bRwa = aData[0];

    /* Validate read/write access */
    if((pT1T->bRwa != PHAL_TOP_T1T_CC_RWA_RW) && (pT1T->bRwa != PHAL_TOP_T1T_CC_RWA_RO))
    {
        /* RFU/Proprietary options; Not supported */
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_UNSUPPORTED_TAG, PH_COMP_AL_TOP);
    }

    /* Detect TLVs */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_DetectTlvBlocks(pDataParams, pT1T));

    if((pT1T->wNdefHeaderAddr) && (pDataParams->wNdefLength != 0))
    {
        if(pT1T->bRwa == PHAL_TOP_T1T_CC_RWA_RW)
        {
            pDataParams->bTagState = PHAL_TOP_STATE_READWRITE;
        }
        else
        {
            /* NOTE: Lock bytes status are not checked. */
            pDataParams->bTagState = PHAL_TOP_STATE_READONLY;
        }
    }
    else
    {
        /* If dynamic memory; check for lock and memory TLVs */
        if((pT1T->bTagMemoryType == PHAL_TOP_T1T_TAG_MEMORY_TYPE_DYNAMIC) &&
          ((pT1T->bLockTlvCount == 0) || (pT1T->bMemoryTlvCount == 0)))
        {
            return PH_ADD_COMPCODE(PHAL_TOP_ERR_MISCONFIGURED_TAG, PH_COMP_AL_TOP);
        }

        pDataParams->bTagState = PHAL_TOP_STATE_INITIALIZED;
    }

    /* Update max. NDEF size */
    phalTop_Sw_Int_T1T_CalculateMaxNdefSize(pDataParams, pT1T);

    /* Update reserved/lock bytes status for first sector */
    pT1T->sSegment.bAddress = 0;
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_UpdateLockReservedOtp(pT1T));

    /* Update state in out parameter */
    *pTagState = pDataParams->bTagState;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T1T_FormatNdef(
                                         phalTop_Sw_DataParams_t * pDataParams
                                         )
{
    phStatus_t    PH_MEMLOC_REM   status;
    uint8_t       PH_MEMLOC_BUF   aRxData[11];
    uint16_t      PH_MEMLOC_REM   wRxLength;
    uint16_t      PH_MEMLOC_COUNT wIndex;
    uint16_t      PH_MEMLOC_COUNT wWriteIndex;
    uint8_t       PH_MEMLOC_BUF   aTlv[5];
    phalTop_T1T_t PH_MEMLOC_REM * pT1T = pDataParams->pT1T;
    uint8_t       PH_MEMLOC_BUF   aNdefData[16] = {0xD1, 0x01, 0x0C, 0x55, 0x00, 0x77,
                                                0x77, 0x77, 0x2E, 0x6E, 0x78, 0x70,
                                                0x2E, 0x63, 0x6F, 0x6D};

    /* Check for NDEF support */
    if((((phalT1T_Sw_DataParams_t *)(pT1T->pAlT1TDataParams))->abHR[0] & 0x10) != 0x10)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_UNSUPPORTED_TAG, PH_COMP_AL_TOP);
    }

    /* Check for NDEF presence */
    if(pDataParams->bTagState != PHAL_TOP_STATE_NONE)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_FORMATTED_TAG, PH_COMP_AL_TOP);
    }

    /* If TMS is not configured, use default configuration */
    if(pT1T->bTms < 0x0E)
    {
        /* Check for memory type */
        if((((phalT1T_Sw_DataParams_t *)(pT1T->pAlT1TDataParams))->abHR[0] & 0x01) == 0x01)
        {
            /* Topaz96 */
            pT1T->bTms = 0x0E;
        }
        else
        {
            /* Topaz512 */
            pT1T->bTms = 0x3F;
        }
    }

    /* Set R/W access */
    pT1T->bRwa = PHAL_TOP_T1T_CC_RWA_RW;

    /* Set version number */
    pDataParams->bVno = PHAL_TOP_T1T_NDEF_SUPPORTED_VNO;

    /* Write NDEF Magic Number */
    PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
        pT1T->pAlT1TDataParams,
        pT1T->bUid,
        PHAL_TOP_T1T_BLOCK_NMN,
        PHAL_TOP_T1T_NDEF_NMN,
        aRxData,
        &wRxLength));

    /* Write NDEF version number */
    PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
        pT1T->pAlT1TDataParams,
        pT1T->bUid,
        PHAL_TOP_T1T_BLOCK_VNO,
        pDataParams->bVno,
        aRxData,
        &wRxLength));

    /* Write TMS byte*/
    PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
        pT1T->pAlT1TDataParams,
        pT1T->bUid,
        PHAL_TOP_T1T_BLOCK_TMS,
        pT1T->bTms,
        aRxData,
        &wRxLength));

    /* Write Read/Write access byte*/
    PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
        pT1T->pAlT1TDataParams,
        pT1T->bUid,
        PHAL_TOP_T1T_BLOCK_RWA,
        pT1T->bRwa,
        aRxData,
        &wRxLength));

    /* Update memory type */
    if((((phalT1T_Sw_DataParams_t *)(pT1T->pAlT1TDataParams))->abHR[0] & 0x01) == 0x01)
    {
        pT1T->bTagMemoryType = PHAL_TOP_T1T_TAG_MEMORY_TYPE_STATIC;
    }
    else
    {
        pT1T->bTagMemoryType = PHAL_TOP_T1T_TAG_MEMORY_TYPE_DYNAMIC;

        /* Update default Lock control TLVs (for Topaz512) */
        if(pT1T->bLockTlvCount == 0)
        {
            /* Update Lock TLVs (default for Topaz512) */
            pT1T->bLockTlvCount = 1;

            pT1T->asLockCtrlTlv[0].wOffset = 0x0C;
            pT1T->asLockCtrlTlv[0].wByteAddr = 0x7A;
            pT1T->asLockCtrlTlv[0].bBytesLockedPerBit = 0x03;
            pT1T->asLockCtrlTlv[0].bBytesPerPage = 0x03;
            pT1T->asLockCtrlTlv[0].bSizeInBits = 0x30;
        }

        /* Update Memory control TLVs (for Topaz512) */
        if(pT1T->bMemoryTlvCount == 0)
        {
            /* Update Memory TLVs (default for Topaz512) */
            pT1T->bMemoryTlvCount = 1;

            pT1T->asMemCtrlTlv[0].wOffset = 0x11;
            pT1T->asMemCtrlTlv[0].wByteAddr = 0x78;
            pT1T->asMemCtrlTlv[0].bBytesPerPage = 0x03;
            pT1T->asMemCtrlTlv[0].bSizeInBytes = 0x02;
        }
    }

    /* Write lock control TLVs */
    for(wIndex = 0;
       ((wIndex < pT1T->bLockTlvCount) && (wIndex < PHAL_TOP_T1T_MAX_LOCK_CTRL_TLV));
       wIndex++)
    {
        /* Create lock TLVs */
        aTlv[0] = 0x01;
        aTlv[1] = 0x03;
        aTlv[2] = (uint8_t)((uint16_t)(pT1T->asLockCtrlTlv[wIndex].wByteAddr / (1 << pT1T->asLockCtrlTlv[wIndex].bBytesPerPage)) << 4) |
                  (uint8_t)(pT1T->asLockCtrlTlv[wIndex].wByteAddr % (1 << pT1T->asLockCtrlTlv[wIndex].bBytesPerPage));
        aTlv[3] = pT1T->asLockCtrlTlv[wIndex].bSizeInBits;
        aTlv[4] = (pT1T->asLockCtrlTlv[wIndex].bBytesLockedPerBit << 4) | pT1T->asLockCtrlTlv[wIndex].bBytesPerPage;

        /* Write lock TLVs */
        for(wWriteIndex = 0; wWriteIndex < 5; wWriteIndex++)
        {
            /* Write byte */
            PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
                pT1T->pAlT1TDataParams,
                pT1T->bUid,
                (uint8_t)(pT1T->asLockCtrlTlv[wIndex].wOffset + wWriteIndex),
                aTlv[wWriteIndex],
                aRxData,
                &wRxLength));
        }
    }

    /* Write memory control TLVs */
    for(wIndex = 0;
       ((wIndex < pT1T->bMemoryTlvCount) && (wIndex < PHAL_TOP_T1T_MAX_MEM_CTRL_TLV));
       wIndex++)
    {
        /* Create memory TLV */
        aTlv[0] = 0x02;
        aTlv[1] = 0x03;
        aTlv[2] = (uint8_t)((uint16_t)(pT1T->asMemCtrlTlv[wIndex].wByteAddr / (1 << pT1T->asMemCtrlTlv[wIndex].bBytesPerPage)) << 4) |
                  (uint8_t)(pT1T->asMemCtrlTlv[wIndex].wByteAddr % (1 << pT1T->asMemCtrlTlv[wIndex].bBytesPerPage));
        aTlv[3] = pT1T->asMemCtrlTlv[wIndex].bSizeInBytes;
        aTlv[4] = pT1T->asMemCtrlTlv[wIndex].bBytesPerPage;

        /* Write memory TLV */
        for(wWriteIndex = 0; wWriteIndex < 5; wWriteIndex++)
        {
            /* Write byte */
            PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
                pT1T->pAlT1TDataParams,
                pT1T->bUid,
                (uint8_t)(pT1T->asMemCtrlTlv[wIndex].wOffset + wWriteIndex),
                aTlv[wWriteIndex],
                aRxData,
                &wRxLength));
        }
    }

    /* Calculate NDEF header address */
    if((pT1T->bLockTlvCount !=0) && (pT1T->bMemoryTlvCount != 0))
    {
        pT1T->wNdefHeaderAddr =
            (pT1T->asLockCtrlTlv[pT1T->bLockTlvCount - 1].wOffset >
            pT1T->asMemCtrlTlv[pT1T->bMemoryTlvCount - 1].wOffset ?
            pT1T->asLockCtrlTlv[pT1T->bLockTlvCount - 1].wOffset :
            pT1T->asMemCtrlTlv[pT1T->bMemoryTlvCount - 1].wOffset) + 5;
    }
    else if(pT1T->bLockTlvCount != 0)
    {
        pT1T->wNdefHeaderAddr = pT1T->asLockCtrlTlv[pT1T->bLockTlvCount - 1].wOffset + 5;
    }
    else if(pT1T->bMemoryTlvCount != 0)
    {
        pT1T->wNdefHeaderAddr = pT1T->asMemCtrlTlv[pT1T->bMemoryTlvCount - 1].wOffset + 5;
    }
    else
    {
        pT1T->wNdefHeaderAddr = 12;
    }

    pDataParams->wNdefLength = 0;
    pDataParams->bTagState = PHAL_TOP_STATE_INITIALIZED;

    /* Update max. NDEF size */
    phalTop_Sw_Int_T1T_CalculateMaxNdefSize(pDataParams, pT1T);

    /* Write default NDEF Message (www.nxp.com) */
    status = phalTop_Sw_Int_T1T_WriteNdef(pDataParams, aNdefData, 16);

    if((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
    {
        /* Reset state on error */
        pDataParams->bTagState = PHAL_TOP_STATE_NONE;
        pT1T->wNdefHeaderAddr = 0x00;

        return status;
    }

    /* Update state and NDEF presence */
    pDataParams->bTagState = PHAL_TOP_STATE_READWRITE;
    pT1T->wNdefMsgAddr = pT1T->wNdefHeaderAddr + 2;
    pDataParams->wNdefLength = 0x10;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T1T_EraseNdef(
                                        phalTop_Sw_DataParams_t * pDataParams
                                        )
{
    phStatus_t    PH_MEMLOC_REM status;
    uint8_t       PH_MEMLOC_REM aRxData[11];
    uint16_t      PH_MEMLOC_REM wRxLength;
    phalTop_T1T_t PH_MEMLOC_REM * pT1T = pDataParams->pT1T;

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

    /* Make NDEF length '0' */
    PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
        pT1T->pAlT1TDataParams,
        pT1T->bUid,
        (uint8_t)(pT1T->wNdefHeaderAddr + 1),
        0x00,
        aRxData,
        &wRxLength));

    /* Update state */
    pDataParams->bTagState = PHAL_TOP_STATE_INITIALIZED;
    pDataParams->wNdefLength = 0;
    pT1T->wNdefMsgAddr = pT1T->wNdefHeaderAddr + 2;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T1T_ReadNdef(
                                       phalTop_Sw_DataParams_t * pDataParams,
                                       uint8_t * pData,
                                       uint16_t * pLength
                                       )
{
    phStatus_t    PH_MEMLOC_REM   status;
    uint16_t      PH_MEMLOC_COUNT wIndex;
    uint16_t      PH_MEMLOC_COUNT wTempIndex;
    uint8_t       PH_MEMLOC_BUF   aData[1];
    uint8_t       PH_MEMLOC_REM   bFreshRead;
    phalTop_T1T_t PH_MEMLOC_REM * pT1T = pDataParams->pT1T;

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

    pT1T->sSegment.bAddress = (uint8_t)(pT1T->wNdefMsgAddr / PHAL_TOP_T1T_SEGMENT_SIZE);
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_UpdateLockReservedOtp(pT1T));

    /* Do a fresh read of segment for one time */
    bFreshRead = PH_ON;

    for(wIndex = pT1T->wNdefMsgAddr, wTempIndex = 0; wTempIndex < pDataParams->wNdefLength; wIndex++)
    {
        if(((wIndex / PHAL_TOP_T1T_SEGMENT_SIZE) != pT1T->sSegment.bAddress))
        {
            pT1T->sSegment.bAddress = (uint8_t)(wIndex / PHAL_TOP_T1T_SEGMENT_SIZE);
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_UpdateLockReservedOtp(pT1T));

            /* Do a fresh read of segment for one time */
            bFreshRead = PH_ON;
        }

        if(phalTop_Sw_Int_T1T_CheckLockReservedOtp(pT1T, wIndex))
        {
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Read(
                pT1T,
                wIndex,
                aData,
                bFreshRead));

            bFreshRead = PH_OFF;

            pData[wTempIndex] = aData[0];
            wTempIndex++;
        }
    }

    /* Update NDEF length */
    *pLength = pDataParams->wNdefLength;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T1T_WriteNdef(
                                        phalTop_Sw_DataParams_t * pDataParams,
                                        uint8_t * pData,
                                        uint16_t wLength
                                        )
{
    phStatus_t    PH_MEMLOC_REM   status;
    uint8_t       PH_MEMLOC_BUF   aRxData[11];
    uint16_t      PH_MEMLOC_REM   wRxLength;
    uint16_t      PH_MEMLOC_COUNT wIndex;
    uint16_t      PH_MEMLOC_COUNT wTempIndex;
    phalTop_T1T_t PH_MEMLOC_REM * pT1T = pDataParams->pT1T;

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

    /* Check NDEF length */
    if((wLength > pDataParams->wMaxNdefLength) || (wLength == 0))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_AL_TOP);
    }

    /* Make NDEF Magic Number value '0' */
    PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
        pT1T->pAlT1TDataParams,
        pT1T->bUid,
        PHAL_TOP_T1T_BLOCK_NMN,
        0x00,
        aRxData,
        &wRxLength));

    /* Write NDEF version number */
    PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
        pT1T->pAlT1TDataParams,
        pT1T->bUid,
        PHAL_TOP_T1T_BLOCK_VNO,
        PHAL_TOP_T1T_NDEF_SUPPORTED_VNO,
        aRxData,
        &wRxLength));

    /* Write Read/Write access byte*/
    PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
        pT1T->pAlT1TDataParams,
        pT1T->bUid,
        PHAL_TOP_T1T_BLOCK_RWA,
        pT1T->bRwa,
        aRxData,
        &wRxLength));

    /* Calculate NDEF Header Address */
    if((pT1T->bLockTlvCount !=0) && (pT1T->bMemoryTlvCount != 0))
    {
        pT1T->wNdefHeaderAddr = (pT1T->asLockCtrlTlv[pT1T->bLockTlvCount - 1].wOffset >
                                pT1T->asMemCtrlTlv[pT1T->bMemoryTlvCount - 1].wOffset?
                                pT1T->asLockCtrlTlv[pT1T->bLockTlvCount - 1].wOffset:
                                (pT1T->asMemCtrlTlv[pT1T->bMemoryTlvCount - 1].wOffset) + 5);
    }
    else if(pT1T->bLockTlvCount !=0)
    {
        pT1T->wNdefHeaderAddr = pT1T->asLockCtrlTlv[pT1T->bLockTlvCount - 1].wOffset + 5;
    }
    else if(pT1T->bMemoryTlvCount != 0)
    {
        pT1T->wNdefHeaderAddr = pT1T->asMemCtrlTlv[pT1T->bMemoryTlvCount - 1].wOffset + 5;
    }
    else
    {
        pT1T->wNdefHeaderAddr = 12;
    }

    /* Write NDEF TLV */
    PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
        pT1T->pAlT1TDataParams,
        pT1T->bUid,
        (uint8_t)pT1T->wNdefHeaderAddr,
        PHAL_TOP_T1T_NDEF_TLV,
        aRxData,
        &wRxLength));

    /* Write NDEF length */
    if(wLength > 0xFE)
    {
        pT1T->wNdefMsgAddr = pT1T->wNdefHeaderAddr + 4;

        PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
            pT1T->pAlT1TDataParams,
            pT1T->bUid,
            (uint8_t)(pT1T->wNdefHeaderAddr + 1),
            0xFF,
            aRxData,
            &wRxLength));

        PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
            pT1T->pAlT1TDataParams,
            pT1T->bUid,
            (uint8_t)(pT1T->wNdefHeaderAddr + 2),
            (uint8_t)(wLength >> 8),
            aRxData,
            &wRxLength));

        PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
            pT1T->pAlT1TDataParams,
            pT1T->bUid,
            (uint8_t)(pT1T->wNdefHeaderAddr + 3),
            (uint8_t)wLength,
            aRxData,
            &wRxLength));
    }
    else
    {
        pT1T->wNdefMsgAddr = pT1T->wNdefHeaderAddr + 2;

        PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
            pT1T->pAlT1TDataParams,
            pT1T->bUid,
            (uint8_t)(pT1T->wNdefHeaderAddr + 1),
            (uint8_t)wLength,
            aRxData,
            &wRxLength));
    }

    /* Update NDEF Length */
    pDataParams->wNdefLength = wLength;

    /* Update Lock/Reserved/OTP presence */
    pT1T->sSegment.bAddress = (uint8_t)(pT1T->wNdefMsgAddr / PHAL_TOP_T1T_SEGMENT_SIZE);
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_UpdateLockReservedOtp(pT1T));

    /* Write NDEF data */
    for(wIndex = pT1T->wNdefMsgAddr, wTempIndex = 0; wTempIndex < (pDataParams->wNdefLength); wIndex++)
    {
        if((wIndex/PHAL_TOP_T1T_SEGMENT_SIZE) != pT1T->sSegment.bAddress)
        {
            pT1T->sSegment.bAddress = (uint8_t)(wIndex / PHAL_TOP_T1T_SEGMENT_SIZE);
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_UpdateLockReservedOtp(pT1T));
        }

        if(phalTop_Sw_Int_T1T_CheckLockReservedOtp(pT1T, wIndex))
        {
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T1T_Write(
                pDataParams,
                pT1T,
                wIndex,
                pData,
                wTempIndex));

            wTempIndex++;
        }
    }

    /* Write NDEF Magic Number */
    PH_CHECK_SUCCESS_FCT(status, phalT1T_WriteEraseByte(
        pT1T->pAlT1TDataParams,
        pT1T->bUid,
        PHAL_TOP_T1T_BLOCK_NMN,
        PHAL_TOP_T1T_NDEF_NMN,
        aRxData,
        &wRxLength));

    pDataParams->bTagState = PHAL_TOP_STATE_READWRITE;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

#endif /* NXPBUILD__PHAL_T1T_SW */
#endif /* NXPBUILD__PHAL_TOP_SW */
