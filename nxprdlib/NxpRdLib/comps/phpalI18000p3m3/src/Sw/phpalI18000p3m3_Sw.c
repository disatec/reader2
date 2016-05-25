/*
*         Copyright (c), NXP Semiconductors Gratkorn / Austria
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
* Software ISO18000-3 Mode3 Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 3350 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  MWt: Generated 1. February 2010
*
*/

#include <ph_Status.h>
#include <phTools.h>
#include <phhalHw.h>
#include <phpalI18000p3m3.h>
#include <ph_RefDefs.h>

#ifdef NXPBUILD__PHPAL_I18000P3M3_SW

#include "phpalI18000p3m3_Sw.h"
#include "phpalI18000p3m3_Sw_Int.h"

phStatus_t phpalI18000p3m3_Sw_Init(
                                   phpalI18000p3m3_Sw_DataParams_t * pDataParams,
                                   uint16_t wSizeOfDataParams,
                                   void * pHalDataParams
                                   )
{
    if (sizeof(phpalI18000p3m3_Sw_DataParams_t) != wSizeOfDataParams)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_DATA_PARAMS, PH_COMP_PAL_I18000P3M3);
    }
    PH_ASSERT_NULL (pDataParams);
    PH_ASSERT_NULL (pHalDataParams);

    pDataParams->wId                = PH_COMP_PAL_I18000P3M3 | PHPAL_I18000P3M3_SW_ID;
    pDataParams->pHalDataParams     = pHalDataParams;
    pDataParams->bSession           = PHPAL_I18000P3M3_SESSION_S0;
    pDataParams->bStoredCRCValid    = PH_OFF;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_I18000P3M3);
}

phStatus_t phpalI18000p3m3_Sw_Exchange(
                                       phpalI18000p3m3_Sw_DataParams_t * pDataParams,
                                       uint16_t wOption,
                                       uint8_t * pTxBuffer,
                                       uint16_t wTxLength,
                                       uint8_t bTxLastBits,
                                       uint8_t ** ppRxBuffer,
                                       uint16_t * pRxLength,
                                       uint8_t * pRxLastBits
                                       )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    phStatus_t  PH_MEMLOC_REM status;
    uint16_t    PH_MEMLOC_REM wValue;

    /* TxLastBits != 0 is invalid for buffered operation */
    if ((wOption & PH_EXCHANGE_BUFFERED_BIT) && (bTxLastBits != 0))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_I18000P3M3);
    }

    /* TxLastBits > 7 is invalid  */
    if (bTxLastBits > 7)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_I18000P3M3);
    }

    /* Reset RxLastBits */
    if (pRxLastBits != NULL)
    {
        *pRxLastBits = 0;
    }

    /*  last Tx-Byte. */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXLASTBITS, bTxLastBits));

    /* Perform exchange */
    status = phhalHw_Exchange(
        pDataParams->pHalDataParams,
        wOption,
        pTxBuffer,
        wTxLength,
        ppRxBuffer,
        pRxLength);

    /* Ignore incomplete byte status, bail out on error */
    if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS_INCOMPLETE_BYTE)
    {
        /* Get RxLastBits */
        if (pRxLastBits != NULL)
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(
                pDataParams->pHalDataParams,
                PHHAL_HW_CONFIG_RXLASTBITS,
                &wValue));

            *pRxLastBits = (uint8_t)wValue;
        }

        /* Map to success status */
        status = PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_I18000P3M3);
    }

    return status;
}


phStatus_t phpalI18000p3m3_Sw_CreateSelectCmd(
                                         phpalI18000p3m3_Sw_DataParams_t * pDataParams,
                                         uint8_t bTarget,
                                         uint8_t bAction,
                                         uint8_t bMemBank,
                                         uint8_t * pPointer,
                                         uint8_t bPointerLength,
                                         uint8_t * pMask,
                                         uint8_t bMaskBitLength,
                                         uint8_t bTruncate,
                                         uint8_t * pSelectCmd,
                                         uint8_t * bLen,
                                         uint8_t * bValidBits
                                        )
{
    uint8_t     PH_MEMLOC_COUNT bIndex;
    uint8_t     PH_MEMLOC_REM bCmdLength;
    uint8_t     PH_MEMLOC_REM bTmp;
    uint8_t     PH_MEMLOC_REM bMaskByteLength;
    uint16_t    PH_MEMLOC_REM wCmdBitLength;
    PH_UNUSED_VARIABLE(pDataParams);
    /* Parameter check */
    if ((bTarget > 0x07) || (bAction > 0x07) || (bMemBank > 0x03) || (bPointerLength > 3) || (bTruncate > 1))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_I18000P3M3);
    }

    /* Build Command frame */
    bCmdLength = 0;
    *(pSelectCmd + bCmdLength++) = (PHPAL_I18000P3M3_CMD_SELECT << 4) | (bTarget << 1) | (bAction >> 2);  /* 0 bits left */
    *(pSelectCmd + bCmdLength)   = (bAction << 6) | (bMemBank <<  4) | (bPointerLength << 2);             /* 2 bits left */

    /* append pointer */
    for (bIndex = 0; bIndex < (bPointerLength + 1); ++bIndex)
    {
        *(pSelectCmd + bCmdLength++) |= (pPointer[bIndex]) >> 6;
        *(pSelectCmd + bCmdLength)    = (pPointer[bIndex]) << 2;
    }
    /* 2 bits left */

    *(pSelectCmd + bCmdLength++) |= bMaskBitLength >> 6;
    *(pSelectCmd + bCmdLength)    = bMaskBitLength << 2;  /* 2 bits left */

    /* calculate intermediate bitlength */
    wCmdBitLength = ((uint16_t)bCmdLength << 3) + 6;

    /* get byte-length of mask */
    bMaskByteLength = bMaskBitLength >> 3;
    if (bMaskBitLength & 0x07)
    {
        ++bMaskByteLength;
    }

    /* append mask value */
    for (bIndex = 0; bIndex < bMaskByteLength; ++bIndex)
    {
        bTmp = pMask[bIndex];

        /* Mask out invalid bits of last byte */
        if ((bIndex == (bMaskByteLength - 1)) && (bMaskBitLength & 0x07) > 0)
        {
            bTmp &= (uint8_t)(0xFF << (8 - (bMaskBitLength & 0x07)));
        }

        /* Append mask value */
        *(pSelectCmd + bCmdLength++) |= bTmp >> 6;
        *(pSelectCmd + bCmdLength)    = bTmp << 2;
    }
    /* calculate new bit length */
    wCmdBitLength = wCmdBitLength + bMaskBitLength;

    /* decrement length if we incremented too much */
    if ((wCmdBitLength >> 3) != bCmdLength)
    {
        --bCmdLength;
    }

    /* append truncate bit */
    if (bTruncate != PH_OFF)
    {
        *(pSelectCmd + bCmdLength) |= (uint8_t)(0x80 >> (wCmdBitLength & 0x07));
    }
    ++bCmdLength;
    ++wCmdBitLength;
    *bLen = bCmdLength;
    *bValidBits = (uint8_t)(wCmdBitLength & 0x07);

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_I18000P3M3);
}

phStatus_t phpalI18000p3m3_Sw_CreateBeginRoundCmd(
    phpalI18000p3m3_Sw_DataParams_t * pDataParams,
    uint8_t bDr,
    uint8_t bM,
    uint8_t bTRext,
    uint8_t bSel,
    uint8_t bSession,
    uint8_t bRfu,
    uint8_t bQ,
    uint8_t * pBeginRnd
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t PH_MEMLOC_REM wValue = 0;
    /* Parameter check */
    if ((bDr > 0x01) || (bM < 0x02) || (bM > 0x03) || (bTRext > 1) || (bSel > 0x03) || (bSession > 0x03) || (bRfu > 0x01) || (bQ > 0x0F))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_I18000P3M3);
    }

    /* Store new session parameter */
   pDataParams->bSession = bSession;

    /* Map TRext to 0 or 1 */
    if (bTRext != PH_OFF)
    {
        bTRext = 1;
    }
    else
    {
        bTRext = 0;
    }

    /* Build Command frame */
    *pBeginRnd = (PHPAL_I18000P3M3_CMD_BEGIN_ROUND << 4) | (bDr << 3) | (bM << 1) | (bTRext);  /* 0 bits left */
    *(pBeginRnd+1) = (bSel << 6) | (bSession << 4) | (bRfu << 3) | (bQ >> 1);                      /* 0 bits left */
    *(pBeginRnd+2) = (bQ << 7);                                                                    /* 7 bits left */

    if((bDr == 0)&&(bM == 2)){
        wValue = PHHAL_HW_RX_I18000P3M3_FL_423_MAN2;
    }
    else if((bDr == 0)&&(bM == 3)){
        wValue = PHHAL_HW_RX_I18000P3M3_FL_423_MAN4;
    }
    else if((bDr == 1)&&(bM == 2)){
        wValue = PHHAL_HW_RX_I18000P3M3_FL_847_MAN2;
    }
    else if((bDr == 1)&&(bM == 3)){
        wValue = PHHAL_HW_RX_I18000P3M3_FL_847_MAN4;
    }

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
                pDataParams->pHalDataParams,
                PHHAL_HW_CONFIG_RXDATARATE_FRAMING,
                wValue));

    /* Set Subcarrier */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
        pDataParams->pHalDataParams,
        PHHAL_HW_CONFIG_SUBCARRIER,
        (bDr == 0) ? PHHAL_HW_SUBCARRIER_DUAL : PHHAL_HW_SUBCARRIER_QUAD));

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_I18000P3M3);
}

phStatus_t phpalI18000p3m3_Sw_Ack(
                                  phpalI18000p3m3_Sw_DataParams_t * pDataParams,
                                  uint8_t bOption,
                                  uint8_t * pHandle,
                                  uint8_t ** ppRxBuffer,
                                  uint16_t * pRxBitLength
                                  )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_BUF aCmd[3];
    uint16_t    PH_MEMLOC_REM wCmdBitLength;
    uint8_t *   PH_MEMLOC_REM pData;
    uint16_t    PH_MEMLOC_REM wRxLength;
    uint8_t     PH_MEMLOC_REM bRxLastBits;

    /* Send given handle */
    if (bOption == PHPAL_I18000P3M3_ACK_USE_HANDLE)
    {
        pData = pHandle;
    }
    /* Send StoredCRC */
    else if (bOption == PHPAL_I18000P3M3_ACK_USE_CRC)
    {
        /* StoredCRC should be valid */
        if (pDataParams->bStoredCRCValid == PH_OFF)
        {
            return PH_ADD_COMPCODE(PH_ERR_USE_CONDITION, PH_COMP_PAL_I18000P3M3);
        }

        pData = pDataParams->abStoredCRC;
    }
    /* Invalid parameter */
    else
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_I18000P3M3);
    }

    /* Build Command frame */
    aCmd[0] = (PHPAL_I18000P3M3_CMD_ACK << 6) | (pData[0] >> 2);    /* 0 bits left */
    aCmd[1] = (pData[0] << 6) | (pData[1] >> 2);                    /* 0 bits left */
    aCmd[2] = (pData[1] << 6);                                      /* 6 bits left */

    /* Calculate BitLength */
    wCmdBitLength = 18;

    /* Set frame-sync as start symbol */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
        pDataParams->pHalDataParams,
        PHHAL_HW_CONFIG_SYMBOL_START,
        PHHAL_HW_SYMBOL_I18000P3M3_FSYNC));

    /* Set TxWait to T2 */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
        pDataParams->pHalDataParams,
        PHHAL_HW_CONFIG_TXWAIT_US,
        PHPAL_I18000P3M3_SW_T2_MIN_US));

    /* Set T1+T3 Timeout */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
        pDataParams->pHalDataParams,
        PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,
        PHPAL_I18000P3M3_SW_T1_MAX_US + PHPAL_I18000P3M3_SW_T3_MIN_US + PHPAL_I18000P3M3_SW_DELTA_MIN_US));

    /* Switch off HAL-CRC */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXCRC, PH_OFF));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXCRC, PH_OFF));

    /* Exchange command */
    status = phpalI18000p3m3_Sw_Exchange(
        pDataParams,
        PH_EXCHANGE_DEFAULT,
        aCmd,
        3,
        (uint8_t)(wCmdBitLength & 0x07),
        ppRxBuffer,
        &wRxLength,
        &bRxLastBits);

    /* Switch on HAL-CRC from now on */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXCRC, PH_ON));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXCRC, PH_ON));

    /* set Tx-CRC as CRC16  */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_CRCTYPE, PHHAL_HW_CRCTYPE_CRC16|PHHAL_HW_CRCOPTION_INVERTED));

    /* Incomplete byte is allowed */
    if ((status & PH_ERR_MASK) != PH_ERR_SUCCESS_INCOMPLETE_BYTE)
    {
        PH_CHECK_SUCCESS(status);
    }

    /* Get bit length */
    if (bRxLastBits & 0x07)
    {
        --wRxLength;
    }
    *pRxBitLength = (uint16_t)((wRxLength << 3) + bRxLastBits);

    /* Check response length */
    if ((*pRxBitLength < 5) || (*pRxBitLength > 528))
    {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_I18000P3M3);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_I18000P3M3);
}

phStatus_t phpalI18000p3m3_Sw_Nak(
                                  phpalI18000p3m3_Sw_DataParams_t * pDataParams
                                  )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_BUF aCmd[1];
    uint8_t *   PH_MEMLOC_REM pResponse;
    uint16_t    PH_MEMLOC_REM wRxLength;
    uint8_t     PH_MEMLOC_REM bRxLastBits;

    /* Build Command frame */
    aCmd[0] = PHPAL_I18000P3M3_CMD_NAK; /* 0 bits left */

    /* Set frame-sync as start symbol */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
        pDataParams->pHalDataParams,
        PHHAL_HW_CONFIG_SYMBOL_START,
        PHHAL_HW_SYMBOL_I18000P3M3_FSYNC));

    /* Disable TxWait */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
        pDataParams->pHalDataParams,
        PHHAL_HW_CONFIG_TXWAIT_US,
        0));

    /* Set minimum Timeout */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
        pDataParams->pHalDataParams,
        PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,
        1));

    /* Switch off HAL-CRC */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXCRC, PH_OFF));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXCRC, PH_OFF));

    /* Exchange command */
    status = phpalI18000p3m3_Sw_Exchange(
        pDataParams,
        PH_EXCHANGE_DEFAULT,
        aCmd,
        1,
        0,
        &pResponse,
        &wRxLength,
        &bRxLastBits);

    /* Timeout is the correct behavior */
    if ((status & PH_ERR_MASK) != PH_ERR_IO_TIMEOUT)
    {
        if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
        {
            /* if data was received return an error */
            return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_I18000P3M3);
        }
        else
        {
            /* Return the received error */
            return status;
        }
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_I18000P3M3);
}

phStatus_t phpalI18000p3m3_Sw_ActivateCard(
    phpalI18000p3m3_Sw_DataParams_t * pDataParams,
    uint8_t * pSelCmd,
    uint8_t bSelCmdLen,
    uint8_t bNumValidBitsinLastByte,
    uint8_t * pBeginRndCmd,
    uint8_t bTSprocessing,
    uint8_t ** ppRxBuffer,
    uint16_t * pRxBufferLen,
    uint8_t * pMoreCardsAvailable
    )
{
    phStatus_t PH_MEMLOC_REM status;
    uint8_t    PH_MEMLOC_COUNT bQ = 1;
    uint8_t    PH_MEMLOC_REM *pRxBuffer;
    uint8_t    PH_MEMLOC_REM bCollisionFlag;
    uint16_t   PH_MEMLOC_REM wIndex;
    uint16_t   PH_MEMLOC_REM wTimeoutCount;
    uint8_t    PH_MEMLOC_REM bPcLength;
    uint8_t    PH_MEMLOC_REM bPacketCrcLen;
    uint8_t    PH_MEMLOC_REM bUiiStartAddress = 0x20;
    uint8_t    PH_MEMLOC_REM aUii[58] = {0};
    uint8_t    PH_MEMLOC_REM bCardFound = 0;
    uint8_t    PH_MEMLOC_REM bUiiLength = 0;
    uint8_t    PH_MEMLOC_REM mask_bByte1, mask_bByte2;
    uint8_t    PH_MEMLOC_REM bBuff1, bBuff2;
    uint8_t    PH_MEMLOC_REM pSelCmd_Int[39];
    uint8_t    PH_MEMLOC_REM bSelCmdLen_Int;

    PH_UNUSED_VARIABLE(bTSprocessing);

    pRxBuffer = *ppRxBuffer;

    /* Extract bQ value from pBeginRndCmd */
    bBuff1 = *(pBeginRndCmd + 1) & (uint8_t)0x7;
    bBuff2 = *(pBeginRndCmd + 2) & (uint8_t)0x80;

    bQ = (bBuff1 << 1) | (bBuff2 >> 7);

    /* Inventory 2^bQ slots */
    status = phhalHw_I18000p3m3Inventory(
        pDataParams->pHalDataParams,
        pSelCmd,
        (uint8_t)bSelCmdLen,
        bNumValidBitsinLastByte,
        pBeginRndCmd,
        PHHAL_HW_I18000P3M3_GET_MAX_RESPS,
        &pRxBuffer,
        pRxBufferLen);
    do
    {
        bCardFound = 0;

        /* Reset collision flag */
        bCollisionFlag = PH_OFF;

        /* Reset receive buffer index */
        wIndex = 0;

        /* Number of slots with timeout error */
        wTimeoutCount = 0;

        /* Return on timeout */
        if((status & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT)
        {
            return status;
        }
        else if((status & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR)
        {
            /* Update more tags available on collision */
            bCollisionFlag = PH_ON;
        }
        else if(((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
                || ((status & PH_ERR_MASK) == PH_ERR_SUCCESS_CHAINING))
        {
            while(wIndex < *pRxBufferLen)
            {
                if(pRxBuffer[wIndex] == 0)
                {
                    if(!bCardFound)
                    {
                        /* Partial bits shall be 0 as truncation is not set */
                        if(pRxBuffer[wIndex + 2] != 0)
                        {
                            return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_PAL_I18000P3M3);
                        }

                        /* StoredPC/PacketPC length */
                        bPcLength = 2;

                        /* PacketCRC length */
                        bPacketCrcLen = 0;

                        /* Check for XPC Indicator (XI) */
                        if(pRxBuffer[wIndex + 3] & 0x02)
                        {
                            /* XPC_W1 length */
                            bPcLength += 2;

                            /* PacketCRC length */
                            bPacketCrcLen = 2;

                            /* Check for XPC Extension Bit (XEB) */
                            if(pRxBuffer[wIndex + 5] & 0x80)
                            {
                                /* XPC_W2 length */
                                bPcLength += 2;
                            }
                        }

                        /* Copy UII */
                        (void)memcpy(
                            aUii,
                            &pRxBuffer[wIndex + 3 + bPcLength],
                            (pRxBuffer[wIndex + 1] - (bPcLength + bPacketCrcLen)));

                        bUiiLength = pRxBuffer[wIndex + 1] - (bPcLength + bPacketCrcLen);
                        bCardFound = 1;

                        /* Increment index to next slot */
                        wIndex =  (uint16_t)(wIndex + pRxBuffer[wIndex + 1] + 3);
                    }
                    else
                    {
                        *pMoreCardsAvailable = 1;
                        break;
                    }
                }
                else if(pRxBuffer[wIndex] == 1)
                {
                    /* This should not come as ReqRN is not send */
                    return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_PAL_I18000P3M3);
                }
                else if(pRxBuffer[wIndex] == 2)
                {
                    /* Timeout */
                    wIndex = wIndex + 3;
                    wTimeoutCount++;
                }
                else
                {
                    /* Collision */
                    *pMoreCardsAvailable = 1;

                    if(bCardFound)
                    {
                        break;
                    }
                    else
                    {
                        wIndex = wIndex + 3;
                        bCollisionFlag = PH_ON;
                    }
                }
            }
        }

        if(!bCardFound)
        {
            if(wTimeoutCount == (1 << bQ))
            {
                /* Timeout */
                return PH_ADD_COMPCODE(PH_ERR_IO_TIMEOUT, PH_COMP_PAL_I18000P3M3);
            }

            if((status & PH_ERR_MASK) == PH_ERR_SUCCESS_CHAINING)
            {
                status = phhalHw_I18000p3m3ResumeInventory(
                    pDataParams->pHalDataParams,
                    &pRxBuffer,
                    pRxBufferLen);
            }
            else
            {
                /* Increment number of slots */
                bQ++;

                /* update pBeginRndCmd buffer with new bQ value */
                mask_bByte2 = (bQ & 0x1) << 7; /* bQ == 4bit MNOP */
                mask_bByte1 = (bQ & 0xE) >> 1; /* mask_bByte1 last 3 bits == MNO and mask_bByte2 msb bit == P */

                *(pBeginRndCmd + 1) &= (uint8_t)~0x07; /* Clear the bQ field from Cmd */
                *(pBeginRndCmd +2) &= (uint8_t)~0x80; /* Write the new bQ field */

                *(pBeginRndCmd +1)|= mask_bByte1; /* Write the update value to pBeginRndCmd */
                *(pBeginRndCmd + 2)|= mask_bByte2;

                /* Inventory */
                status = phhalHw_I18000p3m3Inventory(
                    pDataParams->pHalDataParams,
                    pSelCmd,
                    (uint8_t)bSelCmdLen,
                    bNumValidBitsinLastByte,
                    pBeginRndCmd,
                    PHHAL_HW_I18000P3M3_GET_MAX_RESPS,
                    &pRxBuffer,
                    pRxBufferLen);
            }
        }
    }
    while((bCollisionFlag != PH_OFF) && (bQ <= 0x0F) && (bCardFound != 1));

    /* Maximum mask length possible is 255 */
    if((bUiiLength * 8) > 255)
    {
        bUiiLength = 255;
    }
    else
    {
        bUiiLength = (uint8_t)(bUiiLength * 8);
    }

    /* Create select command frame (with selected UII in mask) */
    PH_CHECK_SUCCESS_FCT(status, phpalI18000p3m3_CreateSelectCmd(
        pDataParams,
        PHPAL_I18000P3M3_TARGET_INVENTORIED_S0,
        0,
        PHPAL_I18000P3M3_MEMBANK_UII,
        &bUiiStartAddress,
        0,
        aUii,
        bUiiLength,
        0,
        pSelCmd_Int,
        &bSelCmdLen_Int,
        &bNumValidBitsinLastByte));

    /* Update pBeginRndCmd buffer with new bQ as '0' */
    pBeginRndCmd[1] &= (uint8_t)0xF8;
    pBeginRndCmd[2] &= (uint8_t)0x00;

    /* Activate tag with specified UII */
    PH_CHECK_SUCCESS_FCT(status, phhalHw_I18000p3m3Inventory(
        pDataParams->pHalDataParams,
        pSelCmd_Int,
        (uint8_t)bSelCmdLen_Int,
        bNumValidBitsinLastByte,
        pBeginRndCmd,
        PHHAL_HW_I18000P3M3_GET_TAG_HANDLE,
        &pRxBuffer,
        pRxBufferLen));

    *ppRxBuffer = &pRxBuffer[*pRxBufferLen - 2];
    *pRxBufferLen = 2;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_I18000P3M3);
}

phStatus_t phpalI18000p3m3_Sw_SetConfig(
                                        phpalI18000p3m3_Sw_DataParams_t * pDataParams,
                                        uint16_t wConfig,
                                        uint16_t wValue
                                        )
{
    phStatus_t PH_MEMLOC_REM status;
    uint8_t    PH_MEMLOC_REM bVal;

    switch (wConfig)
    {
    case PHPAL_I18000P3M3_CONFIG_TXLASTBITS:

        if (wValue > PHPAL_I18000P3M3_TXLASTBITS_MAX)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_I18000P3M3);
        }

        if (wValue > PHPAL_I18000P3M3_VALID_TXLASTBITS)
        {
            bVal = PH_OFF;
        }
        else
        {
            bVal = PH_ON;
        }
        PH_CHECK_SUCCESS_FCT(status, phhalHw_SetConfig(
            pDataParams->pHalDataParams,
            PHHAL_HW_CONFIG_SET_SYMBOL_SEND,
            (uint16_t)bVal));

        status = phhalHw_SetConfig(
            pDataParams->pHalDataParams,
            PHHAL_HW_CONFIG_TXLASTBITS,
            (wValue & PHPAL_I18000P3M3_VALID_TXLASTBITS));
        break;

    case PHPAL_I18000P3M3_CONFIG_TIMEOUT_VALUE_MS:
        status = phhalHw_SetConfig(
            pDataParams->pHalDataParams,
            PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,
            wValue);
        break;

    default:
        return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_PARAMETER, PH_COMP_PAL_I18000P3M3);
    }
    return status;
}

#endif /* NXPBUILD__PHPAL_I18000P3M3_SW */
