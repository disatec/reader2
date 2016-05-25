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
 * $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 4078 ))
 * $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
 */

#include <ph_TypeDefs.h>
#include <ph_Status.h>
#include <ph_RefDefs.h>
#include <phalI15693.h>
#include <phpalSli15693.h>
#include <phalTop.h>

#ifdef NXPBUILD__PHAL_TOP_SW

#ifdef NXPBUILD__PHAL_I15693_SW

#include "phalTop_Sw_Int_T5T.h"

phStatus_t phalTop_Sw_Int_LockBlock(
                                    phalTop_T5T_t * pT5T,
                                    uint16_t wBlockNum
                                    )
{
    phStatus_t PH_MEMLOC_REM status;

    if ( (pT5T->bExtendedCommandSupport == 0 ) && (wBlockNum > PHAL_TOP_T5T_DEFAULT_BLOCK_NUMBER))
    {
       return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_AL_TOP);
    }

    if(wBlockNum <= PHAL_TOP_T5T_DEFAULT_BLOCK_NUMBER)
    {
        PH_CHECK_SUCCESS_FCT(status, phalI15693_LockBlock(
            pT5T->pAlI15693DataParams,
            PHPAL_SLI15693_ACTIVATE_DEFAULT,
            (uint8_t)wBlockNum));
    }
    else
    {
        PH_CHECK_SUCCESS_FCT(status, phalI15693_ExtendedLockBlock(
            pT5T->pAlI15693DataParams,
            PHPAL_SLI15693_ACTIVATE_DEFAULT,
            wBlockNum));
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T5T_Read(
                                   phalTop_T5T_t * pT5T,
                                   uint16_t wBlockNum,
                                   uint16_t wNumberOfBlocks,
                                   uint8_t ** pRxBuffer,
                                   uint16_t * wLength
                                   )
{
    phStatus_t PH_MEMLOC_REM status;
    uint8_t    PH_MEMLOC_REM bOptionFlag = PHAL_I15693_OPTION_OFF;

    if(pT5T->bSplFrm == 1)
    {
        bOptionFlag = PHAL_I15693_OPTION_ON;
    }

    if(wNumberOfBlocks == PHAL_TOP_T5T_MIMIMUM_NO_OF_BLOCK)
    {
        if(wBlockNum <= PHAL_TOP_T5T_DEFAULT_BLOCK_NUMBER)
        {
            PH_CHECK_SUCCESS_FCT(status, phalI15693_ReadSingleBlock(
                pT5T->pAlI15693DataParams,
                bOptionFlag,
                (uint8_t)(wBlockNum),
                pRxBuffer,
                wLength));

        }
        else
        {
            PH_CHECK_SUCCESS_FCT(status, phalI15693_ExtendedReadSingleBlock(
                pT5T->pAlI15693DataParams,
                bOptionFlag,
                wBlockNum,
                pRxBuffer,
                wLength));
        }

        /* Check if Special Frame of Tag and the Option Flag during Transmit is
         * enabled. Then Discard the Block security Status Byte */
        if ((pT5T->bSplFrm == 1) && (pT5T->bOptionFlag == PH_ON ))
        {
            (*pRxBuffer)++;
            *wLength = *wLength -1;
        }
    }
    else if(pT5T->bMbRead == 1)
    {
        if(wBlockNum+wNumberOfBlocks <= PHAL_TOP_T5T_DEFAULT_BLOCK_NUMBER)
        {
            PH_CHECK_SUCCESS_FCT(status, phalI15693_ReadMultipleBlocks(
                pT5T->pAlI15693DataParams,
                bOptionFlag,
                (uint8_t)(wBlockNum),
                wNumberOfBlocks,
                pRxBuffer,
                wLength));
        }
        else
        {
            PH_CHECK_SUCCESS_FCT(status, phalI15693_ExtendedReadMultipleBlocks(
                pT5T->pAlI15693DataParams,
                bOptionFlag,
                wBlockNum,
                wNumberOfBlocks,
                pRxBuffer,
                wLength));
        }

        /*Check if Special Frame of Tag and the Option Flag during Transmit is enabled. Then discard the Block security Status Bytes depending on the number of blocks*/
        if ((pT5T->bSplFrm == 1) && (pT5T->bOptionFlag == PH_ON ))
        {
            wLength = wLength -wBlockNum;
        }
    }
    else
    {
        return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_AL_TOP);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T5T_Write(
                                    phalTop_T5T_t * pT5T,
                                    uint16_t wBlockNum,
                                    uint8_t * pRxBuffer,
                                    uint16_t wLength
                                    )
{
    phStatus_t PH_MEMLOC_REM status;

    if(wBlockNum <= PHAL_TOP_T5T_DEFAULT_BLOCK_NUMBER)
    {
        PH_CHECK_SUCCESS_FCT(status, phalI15693_WriteSingleBlock(
            pT5T->pAlI15693DataParams,
            PHPAL_SLI15693_ACTIVATE_DEFAULT,
            (uint8_t)(wBlockNum),
            pRxBuffer,
            wLength));
    }
    else
    {
        PH_CHECK_SUCCESS_FCT(status, phalI15693_ExtendedWriteSingleBlock(
            pT5T->pAlI15693DataParams,
            PHPAL_SLI15693_ACTIVATE_DEFAULT,
            wBlockNum,
            pRxBuffer,
            wLength));
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T5T_ClearState(
                                         phalTop_Sw_DataParams_t * pDataParams,
                                         phalTop_T5T_t * pT5T
                                         )
{
    /* Reset parameters */
    pDataParams->bTagState = 0;
    pT5T->bTerminatorTlvPresence = 0;
    pDataParams->wMaxNdefLength = 0;
    pT5T->wNdefHeaderAddr = 0;
    pDataParams->wNdefLength = 0;
    pT5T->wNdefMsgAddr = 0;
    pT5T->bBlockSize = 0;
    pT5T->bExtendedCommandSupport = 0;

    return PH_ERR_SUCCESS;
}

phStatus_t phalTop_Sw_Int_T5T_SetReadOnly(
                                          phalTop_Sw_DataParams_t * pDataParams
                                          )
{
    phStatus_t    PH_MEMLOC_REM   status;
    uint8_t       PH_MEMLOC_BUF   aData[PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE] = {0};
    uint8_t       PH_MEMLOC_REM   * pRxBuffer;
    uint16_t      PH_MEMLOC_REM   wRxLength = 0;
    uint16_t      PH_MEMLOC_COUNT wIndex;
    phalTop_T5T_t PH_MEMLOC_REM * pT5T = pDataParams->pT5T;

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

    /* Read CC */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
        pT5T,
        PHAL_TOP_T5T_CC_BLOCK,
        1,
        &pRxBuffer,
        &wRxLength));

    memcpy(aData, pRxBuffer, wRxLength);    /* PRQA S 3200 */

    /* Update R/W access byte of CC to RO */
    aData[1] |= PHAL_TOP_T5T_READ_WRITE_ACCESS_BYTE;

    /* Write back CC */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Write(
        pT5T,
        PHAL_TOP_T5T_CC_BLOCK,
        aData,
        wRxLength));

    if(pT5T->bLockBlock == 1)
    {
        for(wIndex = 0; wIndex < pT5T->bMaxBlockNum; wIndex++)
        {
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_LockBlock(pT5T, wIndex));
        }
    }

    /* Update state */
    pT5T->bRwa = PHAL_TOP_T5T_CC_RWA_RO;
    pDataParams->bTagState = PHAL_TOP_STATE_READONLY;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T5T_DetectTlvBlocks(
                                              phalTop_Sw_DataParams_t * pDataParams,
                                              phalTop_T5T_t * pT5T
                                              )
{
    phStatus_t PH_MEMLOC_REM   status;
    uint8_t    PH_MEMLOC_REM   bIndexInit = 0;
    uint8_t    PH_MEMLOC_REM   aLength[3] = {0};
    uint8_t    PH_MEMLOC_REM   bExitLoop = 0;
    uint8_t    PH_MEMLOC_REM   * pRxBuffer;
    uint8_t    PH_MEMLOC_BUF   aData[40] = { 0 };
    uint16_t   PH_MEMLOC_REM   wRxLength = 0;
    uint16_t   PH_MEMLOC_REM   wBlockNum = 0;
    uint16_t   PH_MEMLOC_COUNT wIndex = 0;

    /* Read the whole memory of the tag and search for all the TLVs */
    for(wBlockNum = 0;
        wBlockNum <= pT5T->bMaxBlockNum;
        wBlockNum += (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE / pT5T->bBlockSize))
    {
        /* Check if Multiple Blocks need to be read 32 Bytes of data a time ,
         * else read the next block to get the data*/
        if(wIndex < PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE)
        {
            if(pT5T->bMbRead)
            {
                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
                    pT5T,
                    wBlockNum,
                    (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE / pT5T->bBlockSize),
                    &pRxBuffer,
                    &wRxLength));
                memcpy(aData, pRxBuffer, wRxLength);    /* PRQA S 3200 */
            }
            /* Loop to read the multiple blocks and read 32 bytes of data at a time */
            else
            {
                for(wIndex = 0;
                    wIndex < (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE / pT5T->bBlockSize);
                    wIndex++)
                {
                    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
                        pT5T,
                        (wIndex + wBlockNum),
                        1,
                        &pRxBuffer,
                        &wRxLength));
                    memcpy(&aData[wIndex * wRxLength], pRxBuffer, wRxLength);   /* PRQA S 3200 */
                }
            }

            wIndex = bIndexInit;

            /* Traverse through these 32 bytes to search for 'T' value in the
             * TLVs bytes */
            while(wIndex < PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE)
            {
                switch(aData[wIndex])
                {
                case PHAL_TOP_T5T_PROPRIETARY_TLV:
                    /* Get the length of the respective TLV */
                    status = phalTop_Sw_Int_T5T_CalculateTLVLength(pT5T, wIndex, aLength,aData);
                    if(aLength[0] == 0xFF)
                    {
                        wIndex += (((uint16_t)aLength[1] << 8) | aLength[2]) + 3;
                    }
                    else
                    {
                        wIndex += aLength[0] + 1;
                    }
                    break;

                case PHAL_TOP_T5T_NDEF_TLV:
                     /* Initializing the NdefHeaderAddr */
                    pT5T->wNdefHeaderAddr = wIndex + (wBlockNum * pT5T->bBlockSize);

                    /* get the length of the respective TLV */
                    status = phalTop_Sw_Int_T5T_CalculateTLVLength(
                        pT5T,
                        wIndex,
                        aLength,
                        aData);

                    /* Updating the T5T NdefLength and NdefMsgAddr parameters */
                    if(aLength[0] == 0xFF)
                    {
                        pDataParams->wNdefLength = ((uint16_t)aLength[1] << 8) | aLength[2];
                        pT5T->wNdefMsgAddr = pT5T->wNdefHeaderAddr + 4;
                    }
                    else
                    {
                        pDataParams->wNdefLength = aLength[0];
                        pT5T->wNdefMsgAddr = pT5T->wNdefHeaderAddr + 2;
                    }

                    wIndex = pT5T->wNdefMsgAddr + pDataParams->wNdefLength - 1;
                    pDataParams->wMaxNdefLength = ((pT5T->wMlen * 8) - pT5T->wNdefMsgAddr + 1);

                    /* This indicates that next position is having Terminator
                     * TLV (If Present) or else simply exit */
                    bExitLoop = 1;
                    break;

                case PHAL_TOP_T5T_TERMINATOR_TLV:
                    /* If Terminator TLV exits, if sets the T5T Parameter */
                    pT5T->bTerminatorTlvPresence = PH_ON;
                    bExitLoop = 1;
                    break;

                default:
                    break;
                }

                wIndex++;
            }
        }
        else
        {
            wIndex -= PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE;
        }

        if(bExitLoop == 1)
        {
          /* Stop reading the rest of the Data Memory and exit */
            break;
        }
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T5T_CheckNdef(
                                        phalTop_Sw_DataParams_t * pDataParams,
                                        uint8_t * pTagState
                                        )
{
    phStatus_t    PH_MEMLOC_REM status;
    uint8_t       PH_MEMLOC_REM * pRxBuffer;
    uint16_t      PH_MEMLOC_REM wRxLength;
    phalTop_T5T_t PH_MEMLOC_REM * pT5T = pDataParams->pT5T;

    /* Reset tag state */
    *pTagState = PHAL_TOP_STATE_NONE;

    /* Clear values from previous detection, if any */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_ClearState(pDataParams, pT5T));

    /* Read CC Block */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
        pT5T,
        PHAL_TOP_T5T_CC_BLOCK,
        1,
        &pRxBuffer,
        &wRxLength));

    /* Update block size */
    pT5T->bBlockSize = wRxLength;

    /* Check for NMN */
    if((pRxBuffer[0] != PHAL_TOP_T5T_NDEF_MAGIC_NUMBER) &&
       (pRxBuffer[0] != PHAL_TOP_T5T_NDEF_MAGIC_NUMBER_2))
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_NON_NDEF_TAG, PH_COMP_AL_TOP);
    }

    /* Check for extended support */
    if(*pRxBuffer == PHAL_TOP_T5T_NDEF_MAGIC_NUMBER_2)
    {
        pT5T->bExtendedCommandSupport = 1;
    }

    /* Version Check */
    pDataParams->bVno = (pRxBuffer[1] >> 4);

    /* Validate version number */
    if((pDataParams->bVno & 0x0F) != (uint8_t)PHAL_TOP_T5T_NDEF_SUPPORTED_VNO)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_UNSUPPORTED_VERSION, PH_COMP_AL_TOP);
    }

    /* Update RW access */
    pT5T->bRwa = (pRxBuffer[1] & 0x0F);

    /* Validate read write access */
    if((pT5T->bRwa != PHAL_TOP_T5T_CC_RWA_RW) && (pT5T->bRwa != PHAL_TOP_T5T_CC_RWA_RO))
    {
        /* RFU/Proprietary options; Not supported */
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_UNSUPPORTED_TAG, PH_COMP_AL_TOP);
    }

    /* Update Read multiple block support */
    pT5T->bMbRead = (pRxBuffer[3] & 0x01);

    /* Update Lock block support */
    pT5T->bLockBlock = ((pRxBuffer[3] >> 3) & 0x01);

    /* Update special frame support */
    pT5T->bSplFrm = ((pRxBuffer[3] >> 4) & 0x01);

    /* Update MLEN depending upon the CC Bytes value */
    if(pRxBuffer[2] != 0x00)
    {
        pT5T->wMlen = pRxBuffer[2];
    }
    else if((pRxBuffer[2] == 0x00) && (pT5T->bBlockSize > 4))
    {
        pT5T->wMlen = (((uint16_t)pRxBuffer[6] << 8) | (pRxBuffer[7]));
    }
    else
    {
        /* Read next block */
        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
            pT5T,
            (PHAL_TOP_T5T_CC_BLOCK + 1),
            1,
            &pRxBuffer,
            &wRxLength));

        pT5T->wMlen = (((uint16_t)pRxBuffer[2] << 8) | (pRxBuffer[3]));
    }

    /* Set Maximum Blocks count */
    pT5T->bMaxBlockNum = (((pT5T->wMlen * 8) / pT5T->bBlockSize) - 1);

    /* Detect TLVs */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_DetectTlvBlocks(pDataParams, pT5T));

    if((pT5T->wNdefHeaderAddr) && (pDataParams->wNdefLength != 0))
    {
        if(pT5T->bRwa == PHAL_TOP_T5T_CC_RWA_RW)
        {
            pDataParams->bTagState = PHAL_TOP_STATE_READWRITE;
        }
        else
        {
            pDataParams->bTagState = PHAL_TOP_STATE_READONLY;
        }
    }
    else if((pDataParams->wNdefLength == 0) && (pT5T->wNdefHeaderAddr))
    {
        if(pT5T->bRwa == PHAL_TOP_T5T_CC_RWA_RW)
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
    phalTop_Sw_Int_T5T_CalculateMaxNdefSize(pDataParams, pT5T);

    /* Update state in out parameter */
    *pTagState = pDataParams->bTagState;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T5T_EraseNdef(
                                        phalTop_Sw_DataParams_t * pDataParams
                                        )
{
    phStatus_t    PH_MEMLOC_REM status;
    uint8_t       PH_MEMLOC_REM * pRxBuffer;
    uint16_t      PH_MEMLOC_REM wRxLength;
    phalTop_T5T_t PH_MEMLOC_REM * pT5T = pDataParams->pT5T;

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

    /* Read the NDEF length block */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
        pT5T,
        ((pT5T->wNdefHeaderAddr + 1) / pT5T->bBlockSize),
        1,
        &pRxBuffer,
        &wRxLength));

    /* Set NDEF length as '0' */
    pRxBuffer[(pT5T->wNdefHeaderAddr + 1) % pT5T->bBlockSize] = 0x00;

    /* Write NDEF length as '0' */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Write(
        pT5T,
        ((pT5T->wNdefHeaderAddr + 1) / pT5T->bBlockSize),
        pRxBuffer,
        pT5T->bBlockSize));

    /* Update tag state */
    pDataParams->bTagState = PHAL_TOP_STATE_INITIALIZED;
    pDataParams->wNdefLength = 0;
    pT5T->wNdefMsgAddr = pT5T->wNdefHeaderAddr + 2;
    pDataParams->wMaxNdefLength = ((pT5T->wMlen * 8) - pT5T->wNdefMsgAddr + 1);

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T5T_FormatNdef(
                                         phalTop_Sw_DataParams_t * pDataParams
                                         )
{
    phStatus_t    PH_MEMLOC_REM   status;
    uint8_t       PH_MEMLOC_BUF   aData[PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE] = {0};
    uint8_t       PH_MEMLOC_REM   * pRxBuffer;
    uint16_t      PH_MEMLOC_REM   wRxLength;
    uint16_t      PH_MEMLOC_COUNT wIndex = 0;
    uint16_t      PH_MEMLOC_COUNT wCount = 0;
    phalTop_T5T_t PH_MEMLOC_REM * pT5T = pDataParams->pT5T;
    uint8_t       PH_MEMLOC_BUF   aNdefData[20] = {0x03, 0x10, 0xD1, 0x01, 0x0C, 0x55,
                                                0x00, 0x77, 0x77, 0x77, 0x2E, 0x6E,
                                                0x78, 0x70, 0x2E, 0x63, 0x6F, 0x6D,
                                                0xFE, 0x00};

    /* Check for NDEF presence */
    if(pDataParams->bTagState != PHAL_TOP_STATE_NONE)
    {
        return PH_ADD_COMPCODE(PHAL_TOP_ERR_FORMATTED_TAG, PH_COMP_AL_TOP);
    }

    /* Check Tag memory */
    if(pT5T->wMlen < 0x05)
    {
        /* Set default (size of ICODE SLI-L) */
        pT5T->wMlen = 8;
    }

    /* Perform a read to get actual block size */
    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
        pT5T,
        PHAL_TOP_T5T_CC_BLOCK,
        1,
        &pRxBuffer,
        &wRxLength));

    /* Update block size */
    pT5T->bBlockSize = wRxLength;

    /* Configuring the CC Bytes */
    if(((pT5T->wMlen * 8) / pT5T->bBlockSize) <= PHAL_TOP_T5T_DEFAULT_BLOCK_NUMBER)
    {
        /* 255 blocks of data read by 1-byte block addressing */
        aData[0] = 0xE1;
        pT5T->bExtendedCommandSupport = 0;
        pT5T->wNdefHeaderAddr = 4;
        aData[2] = (uint8_t)pT5T->wMlen;
    }
    else
    {
        /* More than 255 blocks of data read by 2-byte block addressing */
        aData[0] = 0xE2;
        pT5T->bExtendedCommandSupport = 1;
        pT5T->wNdefHeaderAddr = 8;
        aData[2] = 0x00;
        aData[6] = (uint8_t)(pT5T->wMlen << 8);
        aData[7] = (uint8_t)(pT5T->wMlen);
    }

    /* Load the Default Version No. */
    pDataParams->bVno = PHAL_TOP_T5T_NDEF_SUPPORTED_VNO;

    /* Set r/w access */
    pT5T->bRwa = PHAL_TOP_T5T_CC_RWA_RW;

    /* Version 1.0 and Read-Write Access */
    aData[1] = (uint8_t)(((pDataParams->bVno << 4) & 0xF0) | pT5T->bRwa);

    /* Additional information byte */
    aData[3] = (uint8_t)((pT5T->bMbRead & 0x01) | ((pT5T->bLockBlock << 3) & 0x08) | ((pT5T->bSplFrm << 4) & 0x10));

    /* RFU bytes */
    aData[4] = 0x00;
    aData[5] = 0x00;

    /* Update NDEF length */
    pDataParams->wNdefLength = 0x10;
    pT5T->wNdefMsgAddr = (pT5T->wNdefHeaderAddr + 2);

    /* Writing the Data For Block size = 4 */
    if(pT5T->bBlockSize == PHAL_TOP_T5T_BYTES_PER_BLOCK_DEFAULT)
    {
        for(wIndex = 0; wIndex < (pT5T->wNdefHeaderAddr / pT5T->bBlockSize); wIndex++)
        {
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Write (
                pT5T,
                PHAL_TOP_T5T_CC_BLOCK + wIndex,
                &aData[((wIndex)*(pT5T->bBlockSize))],
                pT5T->bBlockSize));

            wCount += pT5T->bBlockSize;
        }

        /* Write default NDEF Message (www.nxp.com) */
        for(wIndex = (uint8_t)(pT5T->wNdefHeaderAddr / pT5T->bBlockSize);
            wIndex <= ((pDataParams->wNdefLength + pT5T->wNdefMsgAddr) / pT5T->bBlockSize);
            wIndex++)
        {
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Write (
                pT5T,
                wIndex,
                (&aNdefData[((wIndex-1)*(pT5T->bBlockSize))]),
                pT5T->bBlockSize));
        }
    }
    /* Writing the Data For Block size > 4 */
    else
    {
        /* Padding of data bytes if CC length is less than the Block Size */
        if(pT5T->bExtendedCommandSupport == 0)
        {
            for(wCount = 0; wCount < (pT5T->bBlockSize - 4); wCount++)
            {
                aData[4 + wCount] = aNdefData[wCount];
            }
        }
        else
        {
            for(wCount = 0; wCount < (pT5T->bBlockSize - 8); wCount++)
            {
                aData[8 + wCount] = aNdefData[wCount];
            }
        }

        /* Write CC block Separately */
        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Write (
            pT5T,
            PHAL_TOP_T5T_CC_BLOCK,
            aData,
            pT5T->bBlockSize));

        /* Write remaining NDEF Message */
        for(wIndex = 1; wIndex < ((pDataParams->wNdefLength - wCount) / pT5T->bBlockSize); wIndex++)
        {
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Write (
                pT5T,
                PHAL_TOP_T5T_CC_BLOCK + wIndex,
                (&aNdefData[((wIndex-1)*(pT5T->bBlockSize))]),
                pT5T->bBlockSize));

            wCount += pT5T->bBlockSize;
        }

        /* Checking for remaining data, where number of bytes left is less than block size */
        if(wCount < pDataParams->wNdefLength)
        {
            for(wIndex = 0; wIndex < pDataParams->wNdefLength - wCount; wIndex++)
            {
                aData[wIndex] = aNdefData[wCount + wIndex];
            }

            /* Padding of zeros */
            while(wIndex < pT5T->bBlockSize)
            {
                aData[wIndex] = 0x00;
                wIndex++;
            }

            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Write (
                pT5T,
                (wCount / pT5T->bBlockSize)+1,
                aData,
                pT5T->bBlockSize));
        }
    }

    /* Update tag state */
    pDataParams->bTagState = PHAL_TOP_STATE_READWRITE;
    pDataParams->wMaxNdefLength = ((pT5T->wMlen * 8) - pT5T->wNdefMsgAddr + 1);

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T5T_ReadNdef(
                                       phalTop_Sw_DataParams_t * pDataParams,
                                       uint8_t * pData,
                                       uint16_t * pLength
                                       )
{
    phStatus_t    PH_MEMLOC_REM   status;
    uint8_t       PH_MEMLOC_BUF   aTemp[PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE] = {0};
    uint8_t       PH_MEMLOC_REM   * pRxBuffer;
    uint16_t      PH_MEMLOC_COUNT wIndex;
    uint16_t      PH_MEMLOC_COUNT wCount;
    uint16_t      PH_MEMLOC_REM   wBlockNum;
    uint16_t      PH_MEMLOC_REM   wRxLength = 0;
    phalTop_T5T_t PH_MEMLOC_REM * pT5T = pDataParams->pT5T;

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

    /* Managing the first 32 (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE) Bytes, starting from wNdefHeaderAddr */
    if(pT5T->bMbRead == 0)
    {
        for(wIndex = 0;
            ((wIndex < (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE / pT5T->bBlockSize))
            && (((pT5T->wNdefHeaderAddr / pT5T->bBlockSize) + wIndex) < pT5T->wMlen));
            wIndex++)
        {
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
                pT5T,
                ((pT5T->wNdefHeaderAddr / pT5T->bBlockSize) + wIndex),
                1,
                &pRxBuffer,
                &wRxLength));
            memcpy(&aTemp[wIndex * wRxLength], pRxBuffer, wRxLength);   /* PRQA S 3200 */
        }

       wRxLength = wIndex * wRxLength;
    }
    else
    {
        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
            pT5T,
            (pT5T->wNdefHeaderAddr / pT5T->bBlockSize),
            (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE / pT5T->bBlockSize),
            &pRxBuffer,
            &wRxLength));
        memcpy(aTemp, pRxBuffer, wRxLength);    /* PRQA S 3200 */
    }

    wCount = wRxLength - ((pT5T->wNdefHeaderAddr % pT5T->bBlockSize) +
             (pT5T->wNdefMsgAddr - pT5T->wNdefHeaderAddr));

    wBlockNum = (pT5T->wNdefHeaderAddr / pT5T->bBlockSize) +
                (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE / pT5T->bBlockSize);

    /* Check if the read bytes are more than the NdefLength, if yes, then modify
     * the count to ndeflength for copying to the buffer */
    if(wCount > pDataParams->wNdefLength)
    {
        wCount = pDataParams->wNdefLength;
    }
    /* Copy the data into the buffer provided by the application */
    memcpy(pData,&aTemp[(pT5T->wNdefHeaderAddr % pT5T->bBlockSize) + (pT5T->wNdefMsgAddr - pT5T->wNdefHeaderAddr)], wCount); /* PRQA S 3200 */

    /* Reading the rest of the Ndef Message */
    while(wCount < pDataParams->wNdefLength)
    {    /* Condition to check if the number of bytes in the ndef message to be
          * read is less than the maximum bock size, ie. 32 */
        if((pDataParams->wNdefLength - wCount) < PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE)
        {    /* if the left bytes are less than the block size */
            if(((pDataParams->wNdefLength - wCount) / pT5T->bBlockSize) == 0)
            {
                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
                    pT5T,
                    wBlockNum,
                    1,
                    &pRxBuffer,
                    &wRxLength));
                memcpy(&pData[wCount], pRxBuffer, (pDataParams->wNdefLength - wCount));    /* PRQA S 3200 */
                wCount += (pDataParams->wNdefLength - wCount);
                wBlockNum += 1;
            }
            /* else we read the number of blocks equal to the multiples of
             * block size and update the buffer and count value */
            else
            {  /* Check for multiple read command supported or not */
                if(pT5T->bMbRead == 0)
                {
                    for(wIndex = 0;
                        wIndex < ((pDataParams->wNdefLength - wCount) / pT5T->bBlockSize);
                        wIndex++)
                    {
                        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
                            pT5T,
                            wBlockNum + wIndex,
                            1,
                            &pRxBuffer,
                            &wRxLength));
                        memcpy(&pData[wCount + wIndex * wRxLength], pRxBuffer, wRxLength);  /* PRQA S 3200 */
                    }
                }
                else
                {
                    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
                        pT5T,
                        wBlockNum,
                        ((pDataParams->wNdefLength - wCount) / pT5T->bBlockSize),
                        &pRxBuffer,
                        &wRxLength));
                    memcpy(&pData[wCount], pRxBuffer, wRxLength);   /* PRQA S 3200 */
                }
                wCount += ((pDataParams->wNdefLength - wCount) / pT5T->bBlockSize) * pT5T->bBlockSize;
                wBlockNum = ((pT5T->wNdefMsgAddr + wCount) / pT5T->bBlockSize);
            }
        }
        /* the number of bytes left to read are more than the maximum bock size */
        else
        {   /*Check for multiple read command supported or not */
            if(pT5T->bMbRead == 0)
            {
                for(wIndex = 0;
                    wIndex < (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE / pT5T->bBlockSize);
                    wIndex++)
                {
                    PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
                        pT5T,
                        (wBlockNum + wIndex),
                        1,
                        &pRxBuffer,
                        &wRxLength));
                    memcpy(&pData[wCount + (wIndex * wRxLength)], pRxBuffer, wRxLength);    /* PRQA S 3200 */
                }
            }
            else
            {
                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
                    pT5T,
                    wBlockNum,
                    (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE / pT5T->bBlockSize),
                    &pRxBuffer,
                    &wRxLength));
                memcpy(&pData[wCount], pRxBuffer, wRxLength);    /* PRQA S 3200 */
            }
            wCount += (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE / pT5T->bBlockSize) * pT5T->bBlockSize;
            wBlockNum = ((pT5T->wNdefMsgAddr + wCount) / pT5T->bBlockSize);
        }
    }

    /* Check if all the bytes of the NDEF message are read  */
    if(wCount == pDataParams->wNdefLength)
    {
        /* Return the value of the length of NDEF message after success
         * read operation */
        *pLength = pDataParams->wNdefLength;
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

phStatus_t phalTop_Sw_Int_T5T_WriteNdef(
                                        phalTop_Sw_DataParams_t * pDataParams,
                                        uint8_t * pData,
                                        uint16_t wLength
                                        )
{
    phStatus_t    PH_MEMLOC_REM   status;
    uint8_t       PH_MEMLOC_REM   * pRxBuffer;
    uint8_t       PH_MEMLOC_BUF   aTemp[PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE] = {0};
    uint16_t      PH_MEMLOC_REM   wBlockNum;
    uint16_t      PH_MEMLOC_REM   wByteNum;
    uint16_t      PH_MEMLOC_COUNT wIndex;
    uint16_t      PH_MEMLOC_REM   wRxLength = 0;
    uint16_t      PH_MEMLOC_REM   wTempLength = 0;
    phalTop_T5T_t PH_MEMLOC_REM * pT5T = pDataParams->pT5T;

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

    /* Reading first 32 (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE ) Byte chunk starting
     * from NDEF TLV */
    if(pT5T->bMbRead == 0)
    {
        for(wIndex = 0;
            wIndex < (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE / pT5T->bBlockSize);
            wIndex++)
        {
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
                pT5T,
                ((pT5T->wNdefHeaderAddr / pT5T->bBlockSize) + wIndex),
                1,
                &pRxBuffer,
                &wRxLength));
            memcpy(&aTemp[wIndex * wRxLength], pRxBuffer, wRxLength);   /* PRQA S 3200 */
        }
    }
    else
    {
        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
            pT5T,
            (pT5T->wNdefHeaderAddr / pT5T->bBlockSize),
            (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE / pT5T->bBlockSize),
            &pRxBuffer,
            &wRxLength));
        memcpy(aTemp, pRxBuffer, wRxLength);    /* PRQA S 3200 */
    }

    /* Updating the Length in NDEF TLV to 1 byte long and value zero */
    aTemp[(pT5T->wNdefHeaderAddr % pT5T->bBlockSize) + 1] = 0x00;

    /* Appending message bytes after the length field of Ndef TLV */
    /* TLV length is of 3 byte */
    if(wLength > 0xFE)
    {
        wIndex = ((pT5T->wNdefHeaderAddr % pT5T->bBlockSize) + 4);
        memcpy(&aTemp[wIndex],&pData[wIndex - ((pT5T->wNdefHeaderAddr % pT5T->bBlockSize) + 4)],(PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE - ((pT5T->wNdefHeaderAddr % pT5T->bBlockSize) + 4)));   /* PRQA S 3200 */
        wByteNum = (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE - ((pT5T->wNdefHeaderAddr % pT5T->bBlockSize) + 4));
    }
    /* TLV length is of 1 bytes */
    else
    {
        wIndex = ((pT5T->wNdefHeaderAddr % pT5T->bBlockSize) + 2);
        if(wLength < (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE - ((pT5T->wNdefHeaderAddr % pT5T->bBlockSize) + 2)))
        {
            memcpy(&aTemp[wIndex], &pData[wIndex - ((pT5T->wNdefHeaderAddr % pT5T->bBlockSize) + 2)], wLength);  /* PRQA S 3200 */
            wByteNum = wLength;
        }
        else
        {
            memcpy(&aTemp[wIndex], &pData[wIndex - ((pT5T->wNdefHeaderAddr % pT5T->bBlockSize) + 2)],(PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE - ((pT5T->wNdefHeaderAddr % pT5T->bBlockSize) + 2))); /* PRQA S 3200 */
            wByteNum = (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE - ((pT5T->wNdefHeaderAddr % pT5T->bBlockSize) + 2));
        }
    }

    /* Update the ndef length to zero */
    pDataParams->wNdefLength = 0;

    /* Write the first 32 bytes to the ndef tlv */
    for(wIndex = 0;
        wIndex < (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE / pT5T->bBlockSize);
        wIndex++)
    {
        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Write(
            pT5T,
            ((pT5T->wNdefHeaderAddr / pT5T->bBlockSize) + wIndex),
            &aTemp[wIndex * pT5T->bBlockSize],
            pT5T->bBlockSize));
    }

    /* Update the block number */
    wBlockNum = (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE / pT5T->bBlockSize) + 1;

    /* Keep writing while all the data is written in the NDEF TLV  */
    while((wLength - wByteNum) > pT5T->bBlockSize)
    {
        memcpy(aTemp, &pData[wByteNum], pT5T->bBlockSize);  /* PRQA S 3200 */
        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Write(
            pT5T,
            wBlockNum,
            aTemp,
            pT5T->bBlockSize));
        wBlockNum += 1;
        wByteNum += pT5T->bBlockSize;
    }

    /* check if there are any bytes, which would be less than the block size, are left to be written */
    if(wLength > wByteNum)
    {   wTempLength = wLength - wByteNum;
        memcpy(aTemp, &pData[wByteNum], wTempLength);  /* PRQA S 3200 */

        if(((wTempLength) < pT5T->bBlockSize) && (wBlockNum < pT5T->bMaxBlockNum))
        {
            aTemp[wTempLength] = 0xFE; /* Appending Terminator TVL */
            for(wIndex = ((wTempLength) + 1);
                wIndex < pT5T->bBlockSize;
                wIndex++)
            {
                aTemp[wIndex] = 0x00;
            }
            pT5T->bTerminatorTlvPresence = 1; /* Updating Terminator TVL dataparam */
        }

        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Write(
            pT5T,
            wBlockNum,
            aTemp,
            pT5T->bBlockSize));

        wBlockNum += 1;
        wByteNum = wLength;

        if((pT5T->bTerminatorTlvPresence != 1) && (wBlockNum < pT5T->bMaxBlockNum))
        {
            aTemp[0] = 0xFE; /* Appending Terminator TVL */

            for(wIndex = 1; wIndex < pT5T->bBlockSize; wIndex++)
            {
                aTemp[wIndex] = 0x00;
            }

            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Write(
                pT5T,
                wBlockNum,
                aTemp,
                pT5T->bBlockSize));
            pT5T->bTerminatorTlvPresence = 1; /* Updating Terminator TVL dataparam */
        }
    }
    /* No bytes are left to be written in the ndef tlv, directly append the Terminator tlv */
    else
    {
        if((pT5T->bTerminatorTlvPresence != 1) && (wBlockNum < pT5T->bMaxBlockNum))
        {
            aTemp[0] = 0xFE; /* Appending Terminator TVL */

            for(wIndex = 1; wIndex < pT5T->bBlockSize; wIndex++)
            {
                aTemp[wIndex] = 0x00;
            }

            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Write(
                pT5T,
                wBlockNum,
                aTemp,
                pT5T->bBlockSize));

            pT5T->bTerminatorTlvPresence = 1; /* Updating Terminator TVL data parameter */
        }
    }

    /* Updating the length Field after completing the write process */
    if(wByteNum == wLength)
    {
        if(pT5T->bMbRead == 0)
        {
            for(wIndex = 0; wIndex < 2; wIndex++)
            {
                PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
                    pT5T,
                    ((pT5T->wNdefHeaderAddr / pT5T->bBlockSize) + wIndex),
                    1,
                    &pRxBuffer,
                    &wRxLength));
                memcpy(&aTemp[wIndex * wRxLength], pRxBuffer, wRxLength);   /* PRQA S 3200 */
            }
        }
        else
        {
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
                pT5T,
                (pT5T->wNdefHeaderAddr / pT5T->bBlockSize),
                2,
                &pRxBuffer,
                &wRxLength));
            memcpy(aTemp, pRxBuffer, wRxLength);    /* PRQA S 3200 */
        }

        if(wLength < 0xFF)
        {
            aTemp[(pT5T->wNdefHeaderAddr % pT5T->bBlockSize) + 1] = (uint8_t)wLength;
        }
        else
        {
            aTemp[(pT5T->wNdefHeaderAddr % pT5T->bBlockSize) + 1] = 0xFF;
            aTemp[(pT5T->wNdefHeaderAddr % pT5T->bBlockSize) + 2] = (uint8_t)(wLength >> 8);
            aTemp[(pT5T->wNdefHeaderAddr % pT5T->bBlockSize) + 3] = (uint8_t)(wLength);
        }

        for(wIndex = 0; wIndex < 2; wIndex++)
        {
            PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Write(
                pT5T,
                ((pT5T->wNdefHeaderAddr / pT5T->bBlockSize) + wIndex),
                &aTemp[wIndex * pT5T->bBlockSize],
                pT5T->bBlockSize));
        }

        pDataParams->wNdefLength = wLength;
    }

    /* Update tag state */
    pDataParams->bTagState = PHAL_TOP_STATE_READWRITE;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

void phalTop_Sw_Int_T5T_CalculateMaxNdefSize(
                                             phalTop_Sw_DataParams_t * pDataParams,
                                             phalTop_T5T_t * pT5T
                                             )
{
    pDataParams->wMaxNdefLength = (((pT5T->wMlen) * 8) - pT5T->wNdefMsgAddr);
}

phStatus_t phalTop_Sw_Int_T5T_CalculateTLVLength(
                                                 phalTop_T5T_t * pT5T,
                                                 uint16_t wIndex,
                                                 uint8_t *pLength,
                                                 uint8_t *pData
                                                 )
{
   phStatus_t PH_MEMLOC_REM status;
   uint8_t    PH_MEMLOC_REM * pRxBuffer;
   uint16_t   PH_MEMLOC_REM wRxLength = 0;

    /* Get the length of the respective TLV if the tag field is within PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE -1
       and length is less than 0xFF */
    if((wIndex < (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE - 1)) && (pData[wIndex + 1] != 0xFF))
    {
          pLength[0] = pData[wIndex + 1];
    }
    /* Get the length of the respective TLV if the tag field is at PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE-2
       and length is greater than 0xFF */
    else if ((wIndex == (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE - 2)) && (pData[wIndex + 1] == 0xFF))
    {
        pLength[0] = pData[wIndex + 1];
        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
            pT5T,
            ((wIndex / pT5T->bBlockSize) + 1),
            1,
            &pRxBuffer,
            &wRxLength));

        pLength[1] = pRxBuffer[0];
        pLength[2] = pRxBuffer[1];
    }
    /* Get the length of the respective TLV if the tag field is at PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE-3
       and length is greater than 0xFF */
    else if ((wIndex == (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE - 3)) && (pData[wIndex + 1] == 0xFF))
    {
        pLength[0] = pData[wIndex + 1];
        pLength[1] = pData[wIndex + 2];

        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
            pT5T,
            ((wIndex / pT5T->bBlockSize) + 1),
            1,
            &pRxBuffer,
            &wRxLength));

        pLength[2] = pRxBuffer[0];
    }
   /* Get the length of the respective TLV if the tag field is within PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE-3
       and length is greater than 0xFF */
    else if((wIndex < (PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE - 3)) && (pData[wIndex + 1] == 0xFF))
    {
        pLength[0] = pData[wIndex + 1];
        pLength[1] = pData[wIndex + 2];
        pLength[2] = pData[wIndex + 3];
    }
    /* Get the length of the respective TLV if the tag field is at PHAL_TOP_T5T_MAXIMUM_BLOCK_SIZE-1 */
    else
    {
        PH_CHECK_SUCCESS_FCT(status, phalTop_Sw_Int_T5T_Read(
            pT5T,
            ((wIndex / pT5T->bBlockSize) + 1),
            1,
            &pRxBuffer,
            &wRxLength));

        pLength[0] = pRxBuffer[0];

        if(pLength[0] == 0xFF)
        {
            pLength[1] = pRxBuffer[1];
            pLength[2] = pRxBuffer[2];
        }
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AL_TOP);
}

#endif /* NXPBUILD__PHAL_I15693_SW */
#endif  /* NXPBUILD__PHAL_TOP_SW */
