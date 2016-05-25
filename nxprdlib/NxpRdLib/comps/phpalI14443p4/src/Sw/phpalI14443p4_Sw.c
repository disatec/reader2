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
* Software ISO14443-4 Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 4166 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  CHu: Generated 29. May 2009
*
*/

#include <ph_Status.h>
#include <phhalHw.h>
#include <phpalI14443p4.h>
#include <ph_RefDefs.h>

#ifdef NXPBUILD__PHPAL_I14443P4_SW

#include "phpalI14443p4_Sw_Int.h"
#include "phpalI14443p4_Sw.h"

static const uint16_t PH_MEMLOC_CONST_ROM bI14443p4_FsTable[13] = {16, 24, 32,
                                                                   40, 48, 64,
                                                                   96, 128, 256,
                                                                   512, 1024,
                                                                   2048, 4096};

#define PHPAL_I14443P4_SW_IS_BLOCKNR_EQUAL(bPcb)                                    \
    (                                                                               \
    ((((bPcb) & PHPAL_I14443P4_SW_PCB_BLOCKNR) ^ pDataParams->bPcbBlockNum) == 0)   \
    ? 1 : 0                                                                         \
    )

#define PHPAL_I14443P4_SW_IS_ACK(bPcb)                                  \
    (                                                                   \
    (((bPcb) & PHPAL_I14443P4_SW_PCB_NAK) == PHPAL_I14443P4_SW_PCB_ACK) \
    ? 1 : 0                                                             \
    )

#define PHPAL_I14443P4_SW_IS_CHAINING(bPcb)                                         \
    (                                                                               \
    (((bPcb) & PHPAL_I14443P4_SW_PCB_CHAINING) == PHPAL_I14443P4_SW_PCB_CHAINING)   \
    ? 1 : 0                                                                         \
    )

#define PHPAL_I14443P4_SW_IS_WTX(bPcb)                                  \
    (                                                                   \
    (((bPcb) & PHPAL_I14443P4_SW_PCB_WTX) == PHPAL_I14443P4_SW_PCB_WTX) \
    ? 1 : 0                                                             \
    )

#define PHPAL_I14443P4_SW_IS_DESELECT(bPcb)                                     \
    (                                                                           \
    (((bPcb) & PHPAL_I14443P4_SW_PCB_WTX) == PHPAL_I14443P4_SW_PCB_DESELECT)    \
    ? 1 : 0                                                                     \
    )

#define PHPAL_I14443P4_SW_IS_I_BLOCK(bPcb)                                  \
    (                                                                       \
    (((bPcb) & PHPAL_I14443P4_SW_BLOCK_MASK) == PHPAL_I14443P4_SW_I_BLOCK)  \
    ? 1 : 0                                                                 \
    )

#define PHPAL_I14443P4_SW_IS_R_BLOCK(bPcb)                                  \
    (                                                                       \
    (((bPcb) & PHPAL_I14443P4_SW_BLOCK_MASK) == PHPAL_I14443P4_SW_R_BLOCK)  \
    ? 1 : 0                                                                 \
    )

#define PHPAL_I14443P4_SW_IS_S_BLOCK(bPcb)                                  \
    (                                                                       \
    (((bPcb) & PHPAL_I14443P4_SW_BLOCK_MASK) == PHPAL_I14443P4_SW_S_BLOCK)  \
    ? 1 : 0                                                                 \
    )

/* Emvco: case_id TA415_4 */
#define PHPAL_I14443P4_SW_EMVCO_IS_INVALID_BLOCK_STATUS(bStatus)            \
    ((                                                                      \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT) ||                     \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_INTEGRITY_ERROR) ||                \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_NOISE_ERROR) ||                    \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_FRAMING_ERROR) ||                  \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR) ||                \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_PROTOCOL_ERROR))                   \
    ? 1 : 0                                                                 \
    )

#define PHPAL_I14443P4_SW_IS_INVALID_BLOCK_STATUS(bStatus)                  \
    ((                                                                      \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT) ||                     \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_INTEGRITY_ERROR) ||                \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_FRAMING_ERROR) ||                  \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR))                  \
    ? 1 : 0                                                                 \
    )

#define PHPAL_I14443P4_SW_DESELECT_IS_INVALID_BLOCK_STATUS(bStatus)         \
    ((                                                                      \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT) ||                     \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_INTEGRITY_ERROR) ||                \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_FRAMING_ERROR) ||                  \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_SUCCESS_INCOMPLETE_BYTE) ||        \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_PROTOCOL_ERROR) ||                 \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR))                  \
    ? 1 : 0                                                                 \
    )

phStatus_t phpalI14443p4_Sw_Init(
                                 phpalI14443p4_Sw_DataParams_t * pDataParams,
                                 uint16_t wSizeOfDataParams,
                                 void * pHalDataParams
                                 )
{
    if (sizeof(phpalI14443p4_Sw_DataParams_t) != wSizeOfDataParams)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_DATA_PARAMS, PH_COMP_PAL_ISO14443P4);
    }
    PH_ASSERT_NULL (pDataParams);
    PH_ASSERT_NULL (pHalDataParams);

    /* Init private data */
    pDataParams->wId            = PH_COMP_PAL_ISO14443P4 | PHPAL_I14443P4_SW_ID;
    pDataParams->pHalDataParams = pHalDataParams;
    pDataParams->bOpeMode       = RD_LIB_MODE_NFC;

    /* Reset protocol to defaults */
    return phpalI14443p4_Sw_ResetProtocol(pDataParams);
}

phStatus_t phpalI14443p4_Sw_SetProtocol(
                                        phpalI14443p4_Sw_DataParams_t * pDataParams,
                                        uint8_t   bCidEnable,
                                        uint8_t   bCid,
                                        uint8_t   bNadEnable,
                                        uint8_t   bNad,
                                        uint8_t   bFwi,
                                        uint8_t   bFsdi,
                                        uint8_t   bFsci
                                        )
{
    /* Rule A, ISO/IEC 14443-4:2008(E), init. Blocknumber to zero */
    pDataParams->bPcbBlockNum = 0;

    /* Check parameters */
    if ((bCid > PHPAL_I14443P4_CID_MAX) ||
        (bFwi > PHPAL_I14443P4_FWI_MAX) ||
        (bFsdi > PHPAL_I14443P4_FRAMESIZE_MAX) ||
        (bFsci > PHPAL_I14443P4_FRAMESIZE_MAX))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P4);
    }

    /* Apply parameters */
    pDataParams->bCidEnabled = bCidEnable;
    pDataParams->bCid = bCid;
    pDataParams->bNadEnabled = bNadEnable;
    pDataParams->bNad = bNad;
    pDataParams->bFwi = bFwi;
    pDataParams->bFsdi = bFsdi;
    pDataParams->bFsci = bFsci;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_ResetProtocol(
    phpalI14443p4_Sw_DataParams_t * pDataParams
    )
{
    /* Set default state */
    pDataParams->bStateNow = PHPAL_I14443P4_SW_STATE_I_BLOCK_TX;

    /* Rule A, ISO/IEC 14443-4:2008(E), init. Blocknumber to zero */
    pDataParams->bPcbBlockNum = 0;

    /* Apply default parameters */
    pDataParams->bCidEnabled        = 0x00;
    pDataParams->bCid               = 0x00;
    pDataParams->bNadEnabled        = 0x00;
    pDataParams->bNad               = 0x00;
    pDataParams->bFwi               = PHPAL_I14443P4_SW_FWI_DEFAULT;
    pDataParams->bFsdi              = 0x00;
    pDataParams->bFsci              = PHPAL_I14443P4_SW_FSCI_DEFAULT;
    pDataParams->bMaxRetryCount     = PHPAL_I14443P4_SW_MAX_RETRIES_DEFAULT;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_Deselect(
                                     phpalI14443p4_Sw_DataParams_t * pDataParams
                                     )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bIsoFrame[3];
    uint16_t    PH_MEMLOC_REM wIsoFrameLen;
    uint8_t *   PH_MEMLOC_REM pResp;
    uint16_t    PH_MEMLOC_REM wRespLen;
    uint8_t     PH_MEMLOC_REM bInvalidBlock;
    uint8_t     PH_MEMLOC_REM bResponseReceived;
    uint16_t    PH_MEMLOC_REM wRetries;

    /* Build S(DESELECT) frame */
    PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildSBlock(
        pDataParams->bCidEnabled,
        pDataParams->bCid,
        0,
        0,
        bIsoFrame,
        &wIsoFrameLen));

    /* Set DESELECT timeout */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
        pDataParams->pHalDataParams,
        PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,
        PHPAL_I14443P4_SW_FWT_DESELECT_US + PHPAL_I14443P4_EXT_TIME_US));

    /* Retrieve max. retry count */
    wRetries = (uint16_t)pDataParams->bMaxRetryCount + 1;

    /* Reset response received flag */
    bResponseReceived = 0;

    /* Do as long as invalid responses are received
    and the retry counter has not reached zero.*/
    do
    {
        /* Send the S(DESELECT) request */
        status = phhalHw_Exchange(
            pDataParams->pHalDataParams,
            PH_EXCHANGE_DEFAULT,
            bIsoFrame,
            wIsoFrameLen,
            &pResp,
            &wRespLen);

        /* Status --> InvalidBlock mapping */
        bInvalidBlock = (uint8_t)PHPAL_I14443P4_SW_DESELECT_IS_INVALID_BLOCK_STATUS(status);

        if (!bInvalidBlock)
        {
            /* Check for other errors */
            PH_CHECK_SUCCESS(status);

            /* Signal that we've received something */
            bResponseReceived = 1;

            /* Rule 8, ISO/IEC 14443-4:2008(E), "...the S(DESELECT) request may be re-transmitted..." */
            if (wRespLen == wIsoFrameLen)
            {
                /* Mask out Power-Level-Indication */
                if (pDataParams->bCidEnabled)
                {
                    pResp[1] &= 0x3F;
                }

                /* Verify S(DESELECT) Response */
                if (memcmp(bIsoFrame, pResp, wRespLen) != 0)
                {
                    status = PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
                }
            }
            else
            {
                status = PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
            }
        }
    }
    /* Retry as long as neccessary */
    while ((bInvalidBlock) && (--wRetries));

    /* Operation not successful */
    if ((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
    {
        /* Return ERR_RECOVERY_FAILED if some response has been received before (bMaxRetryCount = 0 suppresses the retry behaviour) */
        if ((pDataParams->bMaxRetryCount > 0) && (bResponseReceived))
        {
            status = PH_ADD_COMPCODE(PHPAL_I14443P4_ERR_RECOVERY_FAILED, PH_COMP_PAL_ISO14443P4);
        }
    }

    /* Apply default parameters */
    (void) phpalI14443p4_Sw_ResetProtocol(pDataParams);

    return status;
}

phStatus_t phpalI14443p4_Sw_PresCheck(
                                      phpalI14443p4_Sw_DataParams_t * pDataParams
                                      )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bIsoFrame[3];
    uint8_t *   PH_MEMLOC_REM pIsoFrameResp;
    uint16_t    PH_MEMLOC_REM wIsoFrameLen;

    /* Build R(NAK) frame */
    PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildRBlock(
        pDataParams->bCidEnabled,
        pDataParams->bCid,
        pDataParams->bPcbBlockNum,
        0,
        bIsoFrame,
        &wIsoFrameLen));

    /* Send frame */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Exchange(
        pDataParams->pHalDataParams,
        PH_EXCHANGE_DEFAULT,
        bIsoFrame,
        wIsoFrameLen,
        &pIsoFrameResp,
        &wIsoFrameLen));

    /* Check if we got a valid R(ACK) response */
    /* */
    if (PHPAL_I14443P4_SW_IS_R_BLOCK(pIsoFrameResp[PHPAL_I14443P4_SW_PCB_POS]) &&
        PHPAL_I14443P4_SW_IS_ACK(pIsoFrameResp[PHPAL_I14443P4_SW_PCB_POS]))
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_IsValidRBlock(
            pDataParams->bCidEnabled,
            pDataParams->bCid,
            pIsoFrameResp,
            wIsoFrameLen));
    }
    else
    {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_Exchange(
                                     phpalI14443p4_Sw_DataParams_t * pDataParams,
                                     uint16_t wOption,
                                     uint8_t * pTxBuffer,
                                     uint16_t wTxLength,
                                     uint8_t ** ppRxBuffer,
                                     uint16_t * pRxLength
                                     )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bBufferOverflow;

    /* Used to build I/R/S block frames */
    uint8_t     PH_MEMLOC_REM bIsoFrame[3];
    uint16_t    PH_MEMLOC_REM wIsoFrameLen;
    uint8_t     PH_MEMLOC_REM bRxOverlapped[3];
    uint16_t    PH_MEMLOC_REM wRxOverlappedLen;
    uint16_t    PH_MEMLOC_REM wRxStartPos;
    uint8_t     PH_MEMLOC_REM bUseNad = 0;
    uint8_t     PH_MEMLOC_REM bForceSend;

    /* Used for Transmission */
    uint16_t    PH_MEMLOC_REM wRxBufferSize;
    uint16_t    PH_MEMLOC_REM wTxBufferSize;
    uint16_t    PH_MEMLOC_REM wTxBufferLen;
    uint16_t    PH_MEMLOC_REM wInfLength;
    uint16_t    PH_MEMLOC_REM wMaxPcdFrameSize;
    uint16_t    PH_MEMLOC_REM wMaxCardFrameSize;
    uint16_t    PH_MEMLOC_REM wPcb = 0;
    uint8_t     PH_MEMLOC_REM bRetryCountRetransmit;

    /* Used for Reception */
    uint16_t    PH_MEMLOC_REM RxLength;
    uint8_t *   PH_MEMLOC_REM pRxBuffer;

    /* Option parameter check */
    if (wOption &  (uint16_t)~(uint16_t)
        (
        PH_EXCHANGE_BUFFERED_BIT | PH_EXCHANGE_LEAVE_BUFFER_BIT |
        PH_EXCHANGE_TXCHAINING | PH_EXCHANGE_RXCHAINING | PH_EXCHANGE_RXCHAINING_BUFSIZE
        ))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P4);
    }

    /* Check if caller has provided valid RxBuffer */
    if (ppRxBuffer == NULL)
    {
        ppRxBuffer = &pRxBuffer;
    }
    if (pRxLength == NULL)
    {
        pRxLength = &RxLength;
    }

    /* Retrieve HAL buffer sizes */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXBUFFER_BUFSIZE, &wRxBufferSize));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXBUFFER_BUFSIZE, &wTxBufferSize));

    /* Retrieve maximum frame sizes */
    wMaxPcdFrameSize = bI14443p4_FsTable[pDataParams->bFsdi];
    wMaxCardFrameSize = bI14443p4_FsTable[pDataParams->bFsci]-2;

    /* R(ACK) transmission in case of Rx-Chaining */
    if (((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_RXCHAINING) ||
        ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_RXCHAINING_BUFSIZE))
    {
        pDataParams->bStateNow = PHPAL_I14443P4_SW_STATE_I_BLOCK_RX | PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
    }
    /* I-Block transmission */
    else
    {
        /* Reset to default state if not in Tx-Mode */
        if ((pDataParams->bStateNow & PH_EXCHANGE_MODE_MASK) != PHPAL_I14443P4_SW_STATE_I_BLOCK_TX)
        {
            pDataParams->bStateNow = PHPAL_I14443P4_SW_STATE_I_BLOCK_TX;
        }
    }

    /* Reset receive length */
    *pRxLength = 0;

    /* Reset RetryCount */
    bRetryCountRetransmit = 0;

    /* Reset BufferOverflow flag */
    bBufferOverflow = 0;

    /* ******************************** */
    /*     I-BLOCK TRANSMISSION LOOP    */
    /* ******************************** */
    do
    {
        /* Reset Preloaded bytes and ForceSend */
        wTxBufferLen = 0;
        bForceSend = 0;

        switch (pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_MASK)
        {
        case PHPAL_I14443P4_SW_STATE_I_BLOCK_TX:

            /* Retrieve Number of preloaded bytes */
            if (wOption & PH_EXCHANGE_LEAVE_BUFFER_BIT)
            {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(
                    pDataParams->pHalDataParams,
                    PHHAL_HW_CONFIG_TXBUFFER_LENGTH,
                    &wTxBufferLen));
            }

            /* Set initial INF length to (remaining) input data length */
            wInfLength = wTxLength;

            /* Frame has already been preloaded -> IsoFrameLen is zero */
            if (wTxBufferLen > 0)
            {
                /* do not generate the iso frame */
                wIsoFrameLen = 0;
            }
            /* Else evaluate IsoFrameLen*/
            else
            {
                /* 7.1.1.3 c), ISO/IEC 14443-4:2008(E), "During chaining the NAD shall only be transmitted in the first block of chain." */
                if (!(pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_CHAINING_BIT))
                {
                    bUseNad = pDataParams->bNadEnabled;
                }
                else
                {
                    bUseNad = 0;
                }

                /* Evaluate frame overhead */
                wIsoFrameLen = 1;
                if (bUseNad)
                {
                    ++wIsoFrameLen;
                }
                if (pDataParams->bCidEnabled)
                {
                    ++wIsoFrameLen;
                }
            }

            /* Check if chaining is intended or not */
            if ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_TXCHAINING)
            {
                pDataParams->bStateNow |= PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
            }
            else
            {
                pDataParams->bStateNow &= (uint8_t)~(uint8_t)PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
            }

            /* Force frame exchange if
            a) the maximum frame size of the card has been reached;
            */
            if ((wTxBufferLen + wIsoFrameLen + wInfLength) > wMaxCardFrameSize)
            {
                /* force frame exchange */
                bForceSend = 1;

                /* force chaining */
                pDataParams->bStateNow |= PHPAL_I14443P4_SW_STATE_CHAINING_BIT;

                /* limit number of bytes to send */
                wInfLength = wMaxCardFrameSize - wTxBufferLen - wIsoFrameLen;
            }

            /* Force frame exchange if
            b) the TxBuffer is full;
            */
            if ((wOption & PH_EXCHANGE_BUFFERED_BIT) &&
                ((wTxBufferLen + wIsoFrameLen + wInfLength) >= wTxBufferSize))
            {
                /* force frame exchange */
                bForceSend = 1;

                /* force chaining */
                pDataParams->bStateNow |= PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
            }

            /* Generate / Buffer ISO frame */
            if (wIsoFrameLen > 0)
            {
                /* Generate I-Block frame header */
                PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildIBlock(
                    pDataParams->bCidEnabled,
                    pDataParams->bCid,
                    bUseNad,
                    pDataParams->bNad,
                    pDataParams->bPcbBlockNum,
                    (pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_CHAINING_BIT) ? 1 : 0,
                    bIsoFrame,
                    &wIsoFrameLen));

                /* Write Frame to HAL TxBuffer but do not preform Exchange */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Exchange(
                    pDataParams->pHalDataParams,
                    PH_EXCHANGE_BUFFER_FIRST,
                    bIsoFrame,
                    wIsoFrameLen,
                    NULL,
                    NULL));

                /* Retain the preloaded bytes from now on */
                wOption |= PH_EXCHANGE_LEAVE_BUFFER_BIT;
            }

            /* Tx-Buffering mode (and no forced exchange) */
            if ((!bForceSend) && (wOption & PH_EXCHANGE_BUFFERED_BIT))
            {
                /* Preload the data into the TxBuffer */
                return phhalHw_Exchange(
                    pDataParams->pHalDataParams,
                    (wOption & (uint16_t)~(uint16_t)PH_EXCHANGE_MODE_MASK),
                    pTxBuffer,
                    wInfLength,
                    NULL,
                    NULL);
            }

            /* Content has been buffered before */
            if (wTxBufferLen > 0)
            {
                /* retrieve PCB byte */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXBUFFER_OFFSET, 0x00));
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXBUFFER, &wPcb));

                /* Preloaded Data or ForceSend -> Modify PCB byte if neccessary */
                if ((bForceSend) || ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_TXCHAINING))
                {
                    /* modify PCB byte */
                    wPcb |= PHPAL_I14443P4_SW_PCB_CHAINING;
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXBUFFER, wPcb));
                }
            }
            break;

        case PHPAL_I14443P4_SW_STATE_I_BLOCK_RX:

            /* Build R(ACK) frame */
            PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildRBlock(
                pDataParams->bCidEnabled,
                pDataParams->bCid,
                pDataParams->bPcbBlockNum,
                1,
                bIsoFrame,
                &wIsoFrameLen));

            /* Write Frame to HAL TxBuffer but do not preform Exchange */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Exchange(
                pDataParams->pHalDataParams,
                PH_EXCHANGE_BUFFERED_BIT,
                bIsoFrame,
                wIsoFrameLen,
                NULL,
                NULL));

            /* Retain the preloaded bytes from now on */
            wOption |= PH_EXCHANGE_LEAVE_BUFFER_BIT;

            /* do not append any data */
            wInfLength = 0;
            break;

            /* Should NEVER happen! */
        default:
            return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_PAL_ISO14443P4);
        }

        /* Perform Exchange using complete ISO handling */
        status = phpalI14443p4_Sw_IsoHandling(
            pDataParams,
            wOption & (uint16_t)~(uint16_t)PH_EXCHANGE_BUFFERED_BIT,
            bRetryCountRetransmit,
            pTxBuffer,
            wInfLength,
            ppRxBuffer,
            pRxLength);

        /* Complete chaining if buffer is full */
        if (((status & PH_ERR_MASK) == PH_ERR_BUFFER_OVERFLOW) &&
            ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_RXCHAINING_BUFSIZE))
        {
            /* Indicate Buffer Overflow */
            bBufferOverflow = 1;

            /* Toggle Blocknumber */
            pDataParams->bPcbBlockNum ^= PHPAL_I14443P4_SW_PCB_BLOCKNR;
        }
        /* Else bail out on error */
        else
        {
            PH_CHECK_SUCCESS(status);
        }

        /* Retransmission in progress */
        if (pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_RETRANSMIT_BIT)
        {
            /* Increment Retransmit RetryCount */
            ++bRetryCountRetransmit;

            /* Restore internal TxBuffer. */
            /* Neccessary in case RxBuffer and TxBuffer are the same. */
            if (wTxBufferLen > 0)
            {
                /* restore PCB byte */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXBUFFER_OFFSET, 0x00));
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXBUFFER, wPcb));

                /* restore TxBufferLen */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
                    pDataParams->pHalDataParams,
                    PHHAL_HW_CONFIG_TXBUFFER_LENGTH,
                    wTxBufferLen));
            }

            /* Clear retransmission bit */
            pDataParams->bStateNow &= (uint8_t)~(uint8_t)PHPAL_I14443P4_SW_STATE_RETRANSMIT_BIT;
        }
        /* No retransmission in progress */
        else
        {
            /* Reset Retransmit RetryCount */
            bRetryCountRetransmit = 0;

            /* Chaining is active */
            if (pDataParams->bStateNow == (PHPAL_I14443P4_SW_STATE_I_BLOCK_TX | PHPAL_I14443P4_SW_STATE_CHAINING_BIT))
            {
                /* Bytes to send cannot be less than sent bytes */
                if (wTxLength < wInfLength)
                {
                    return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_PAL_ISO14443P4);
                }

                /* Remove sent bytes from TxBuffer */
                pTxBuffer += wInfLength;

                /* below if condition is added to make QAC compliant  */
                if(wTxLength > 0)
                {
                    wTxLength = wTxLength - wInfLength;
                }
            }

            /* Buffered / TxChaining mode -> finished after sending */
            if ((wTxLength == 0) &&
                (
                (wOption & PH_EXCHANGE_BUFFERED_BIT) ||
                ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_TXCHAINING)
                ))
            {
                return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
            }
        }
    }
    /* Loop as long as the state does not transit to RX mode */
    while ((pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_MASK) != PHPAL_I14443P4_SW_STATE_I_BLOCK_RX);

    /* Overlap PCB */
    wRxOverlappedLen = 1;

    /* Overlap CID */
    if ((*ppRxBuffer)[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_CID_FOLLOWING)
    {
        wRxOverlappedLen++;
    }

    /* Overlap NAD */
    if ((*ppRxBuffer)[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_NAD_FOLLOWING)
    {
        wRxOverlappedLen++;
    }

    /* Reset RxStartPos */
    wRxStartPos = 0;

    /* ******************************** */
    /*      I-BLOCK RECEPTION LOOP      */
    /* ******************************** */
    do
    {
        /* Only allow receive state at this point */
        if ((pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_MASK) != PHPAL_I14443P4_SW_STATE_I_BLOCK_RX)
        {
            return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_PAL_ISO14443P4);
        }

        /* Rule 2, ISO/IEC 14443-4:2008(E), PICC chaining */
        if (pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_CHAINING_BIT)
        {
            /* Skip overlapping / SUCCESS_CHAINING checks in case of BufferOverflow */
            if (!bBufferOverflow)
            {
                /* This is first chained response */
                if (wRxStartPos == 0)
                {
                    /* Special NAD chaining handling */
                    /* 7.1.1.3 c), ISO/IEC 14443-4:2008(E), "During chaining the NAD shall only be transmitted in the first block of chain." */
                    if (pDataParams->bNadEnabled)
                    {
                        --wRxOverlappedLen;
                    }
                }

                /* Backup overlapped bytes */
                memcpy(bRxOverlapped, &(*ppRxBuffer)[((*pRxLength) - wRxOverlappedLen)], wRxOverlappedLen);  /* PRQA S 3200 */

                /* Calculate RxBuffer Start Position */
                wRxStartPos = (*pRxLength) - wRxOverlappedLen;

                /* Skip SUCCESS_CHAINING check for RXCHAINING_BUFSIZE mode */
                if ((wOption & PH_EXCHANGE_MODE_MASK) != PH_EXCHANGE_RXCHAINING_BUFSIZE)
                {
                    /* Return with chaining status if the next chain may not fit into our buffer */
                    if ((*pRxLength + wMaxPcdFrameSize) > wRxBufferSize)
                    {
                        /* Adjust RxBuffer position */
                        (*ppRxBuffer) += wRxOverlappedLen;
                        *pRxLength = *pRxLength - wRxOverlappedLen;

                        return PH_ADD_COMPCODE(PH_ERR_SUCCESS_CHAINING, PH_COMP_PAL_ISO14443P4);
                    }
                }
            }

            /* Set RxBuffer Start Position */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
                pDataParams->pHalDataParams,
                PHHAL_HW_CONFIG_RXBUFFER_STARTPOS,
                wRxStartPos));

            /* Prepare R(ACK) frame */
            PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildRBlock(
                pDataParams->bCidEnabled,
                pDataParams->bCid,
                pDataParams->bPcbBlockNum,
                1,
                bIsoFrame,
                &wIsoFrameLen));

            /* Perform Exchange using complete ISO handling */
            status = phpalI14443p4_Sw_IsoHandling(
                pDataParams,
                PH_EXCHANGE_DEFAULT,
                0,
                bIsoFrame,
                wIsoFrameLen,
                ppRxBuffer,
                pRxLength);

            /* Complete chaining if buffer is full */
            if (((status & PH_ERR_MASK) == PH_ERR_BUFFER_OVERFLOW) &&
                ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_RXCHAINING_BUFSIZE))
            {
                /* Reset wRxStartPos */
                wRxStartPos = 0;

                /* Indicate Buffer Overflow */
                bBufferOverflow = 1;

                /* Toggle Blocknumber */
                pDataParams->bPcbBlockNum ^= PHPAL_I14443P4_SW_PCB_BLOCKNR;
            }
            /* Default behaviour */
            else
            {
                /* Emvco: case_id: TA411_04 and Tb411_04 */
                if(pDataParams->bOpeMode == RD_LIB_MODE_EMVCO)
                {
                    if((status & PH_ERR_MASK) == PH_ERR_BUFFER_OVERFLOW)
                    {
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXBUFFER_STARTPOS, 0));
                    }
                }
                /* Bail out on error */
                PH_CHECK_SUCCESS(status);

                /* Restore overlapped INF bytes */
                memcpy(&(*ppRxBuffer)[wRxStartPos], bRxOverlapped, wRxOverlappedLen);  /* PRQA S 3200 */ /* PRQA S 3354 */
            }
        }
        /* No chaining -> reception finished */
        else
        {
            /* Return data */
            if (!bBufferOverflow)
            {
                /* Special NAD chaining handling */
                /* 7.1.1.3 c), ISO/IEC 14443-4:2008(E), "During chaining the NAD shall only be transmitted in the first block of chain." */
                if ((wRxStartPos > 0) && (pDataParams->bNadEnabled))
                {
                    ++wRxOverlappedLen;
                }

                /* Do not return protocol bytes, advance to INF field */
                (*ppRxBuffer) += wRxOverlappedLen;
                *pRxLength = *pRxLength - wRxOverlappedLen;
                /* Reception successful */
                status = PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
            }
            /* do not return any data in case of Buffer Overflow */
            else
            {
                *pRxLength = 0;
                status = PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_PAL_ISO14443P4);
            }

            /* Reception finished */
            pDataParams->bStateNow = PHPAL_I14443P4_SW_STATE_FINISHED;
        }
    }
    while (pDataParams->bStateNow != PHPAL_I14443P4_SW_STATE_FINISHED);

    return status;
}

phStatus_t phpalI14443p4_Sw_IsoHandling(
                                        phpalI14443p4_Sw_DataParams_t * pDataParams,
                                        uint16_t wOption,
                                        uint8_t bRetryCount,
                                        uint8_t * pTxBuffer,
                                        uint16_t wTxLength,
                                        uint8_t ** ppRxBuffer,
                                        uint16_t * pRxLength
                                        )
{
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;
    phStatus_t  PH_MEMLOC_REM statusBkUp = PH_ERR_SUCCESS;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bIsoFrame[3];
    uint8_t *   PH_MEMLOC_REM pResp;
    uint16_t    PH_MEMLOC_REM wRespLen;
    uint16_t    PH_MEMLOC_REM wRxBufferStartPos;
    uint8_t     PH_MEMLOC_REM bInvalidBlock;
    uint8_t     PH_MEMLOC_REM bResponseReceived;
    uint8_t     PH_MEMLOC_REM bWtxm = 0;
    uint8_t     PH_MEMLOC_REM bCheckNad;
    uint32_t    PH_MEMLOC_REM dwTimeout;
    uint16_t    PH_MEMLOC_REM wTimeoutPrev = 0;
    uint8_t     PH_MEMLOC_REM bTimeoutInMs = 0;
    uint8_t     bNAKCount = 0;
    uint8_t     bACKCount = 0;

    /* Buffered Exchange is not allowed here */
    if (wOption & PH_EXCHANGE_BUFFERED_BIT)
    {
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    /* Ignore custom modes */
    wOption &= (uint16_t)~(uint16_t)PH_EXCHANGE_MODE_MASK;

    /* Reset response received flag */
    bResponseReceived = 0;

    /* Loops in case of an error */
    do
    {
        /* WTX Timeout - set temporary FWT */
        if (bWtxm > 0)
        {
            /* Retrieve current timeout */
            status = phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, &wTimeoutPrev);

            /* Timeout is out of range, retrieve it in milliseconds */
            if ((status & PH_ERR_MASK) == PH_ERR_PARAMETER_OVERFLOW)
            {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS, &wTimeoutPrev));
                bTimeoutInMs = 1;
            }
            else
            {
                PH_CHECK_SUCCESS(status);
                bTimeoutInMs = 0;
            }

            /* Calculate temporary WTX timeout */
            /* Emvco: case_id TA204/TB110_X/TB004_X/TA202_x/TB204 */
            if(pDataParams->bOpeMode != RD_LIB_MODE_EMVCO)
            {
                dwTimeout = (uint32_t)(((PHPAL_I14443P4_SW_FWT_MIN_US * (1 << pDataParams->bFwi)) * bWtxm) + PHPAL_I14443P4_EXT_TIME_US);
            }
            else
            {
                dwTimeout = (uint32_t)((((PHPAL_I14443P4_SW_FWT_MIN_US) * (1 << pDataParams->bFwi) * bWtxm) + PHPAL_I14443P4_SW_DELTA_FWT_US + PHPAL_I14443P4_SW_DELTA_TPCD_US));
            }

            /* Limit timeout to FWT max */
            if (dwTimeout > PHPAL_I14443P4_SW_FWT_MAX_US)
            {
                dwTimeout = PHPAL_I14443P4_SW_FWT_MAX_US;
            }

            /* Set temporary WTX timeout */
            if (dwTimeout > 0xFFFF)
            {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS, (uint16_t)((dwTimeout / 1000) + 1)));
            }
            else
            {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, (uint16_t)dwTimeout));
            }
        }

        /* Call HAL exchange function */
        status = phhalHw_Exchange(
            pDataParams->pHalDataParams,
            wOption,
            pTxBuffer,
            wTxLength,
            ppRxBuffer,
            pRxLength);
        statusBkUp = status;

        /* Reset Exchange Option */
        wOption = PH_EXCHANGE_DEFAULT;

        /* Reset transmission length */
        wTxLength = 0;

        /* WTX Timeout - restore previous value */
        if (bWtxm > 0)
        {
            /* Restore previous state and timeout after */
            if (!bTimeoutInMs)
            {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, wTimeoutPrev));
            }
            else
            {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS, wTimeoutPrev));
            }
        }

        /* Status --> InvalidBlock mapping */
        if(pDataParams->bOpeMode == RD_LIB_MODE_EMVCO)
        {
            bInvalidBlock = (uint8_t)PHPAL_I14443P4_SW_EMVCO_IS_INVALID_BLOCK_STATUS(status);
        }
        else
        {
           bInvalidBlock = (uint8_t)PHPAL_I14443P4_SW_IS_INVALID_BLOCK_STATUS(status);
        }
        if (!bInvalidBlock)
        {
            /* MIFARE compliancy: force protocol error on NAK */
            if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS_INCOMPLETE_BYTE)
            {
                *pRxLength = 0;
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXBUFFER_STARTPOS, 0));
                return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
            }
            else
            {
                PH_CHECK_SUCCESS(status);
            }
        }

        /* Reset defaults */
        bWtxm = 0;

        /* Emvco: case_id TA404_04, TA411_04, TB404_04, TB411_04 */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXBUFFER_STARTPOS, &wRxBufferStartPos));
        /* Check for FSD */
        if(((*pRxLength) - wRxBufferStartPos) >= bI14443p4_FsTable[pDataParams->bFsdi])
         {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXBUFFER_STARTPOS, 0));
            status = PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
            *pRxLength = 0;
            return status;
         }
        /* Do not enter if we received an invalid block */
        if (!bInvalidBlock)
        {
            /* Signal that we've received something */
            bResponseReceived = 1;

            /* Map receive buffer pointer to current data */
            pResp = *ppRxBuffer + wRxBufferStartPos;
            wRespLen = *pRxLength - wRxBufferStartPos;

            /* I-Block handling */
            if (PHPAL_I14443P4_SW_IS_I_BLOCK(pResp[PHPAL_I14443P4_SW_PCB_POS]))
            {
                /* Rule 2/10, ISO/IEC 14443-4:2008(E) */
                if (pDataParams->bStateNow == (PHPAL_I14443P4_SW_STATE_I_BLOCK_TX | PHPAL_I14443P4_SW_STATE_CHAINING_BIT))
                {
                    /* Protocol violation */
                    bInvalidBlock = 1;
                }
                else
                {
                    /* Evaluate if NAD should be present */
                    /* 7.1.1.3 c), ISO/IEC 14443-4:2008(E), "During chaining the NAD shall only be transmitted in the first block of chain." */
                    if ((pDataParams->bNadEnabled) && !(pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_CHAINING_BIT))
                    {
                        bCheckNad = 1;
                    }
                    else
                    {
                        bCheckNad = 0;
                    }

                    /* Check if I-Block is valid */
                    status = phpalI14443p4_Sw_IsValidIBlock(
                        pDataParams->bCidEnabled,
                        pDataParams->bCid,
                        bCheckNad,
                        pDataParams->bNad,
                        pResp,
                        wRespLen);

                    /* Blocknumber is equal, I-Block Rx mode is active */
                    if (((status & PH_ERR_MASK) == PH_ERR_SUCCESS) && (PHPAL_I14443P4_SW_IS_BLOCKNR_EQUAL(pResp[PHPAL_I14443P4_SW_PCB_POS])))
                    {
                        /* Switch from Tx-Mode to Rx-Mode */
                        if ((pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_MASK) == PHPAL_I14443P4_SW_STATE_I_BLOCK_TX)
                        {
                            pDataParams->bStateNow = PHPAL_I14443P4_SW_STATE_I_BLOCK_RX;
                        }

                        /* Rule B, ISO/IEC 14443-4:2008(E), toggle Blocknumber */
                        pDataParams->bPcbBlockNum ^= PHPAL_I14443P4_SW_PCB_BLOCKNR;

                        /* Set chaining bit if card is chaining */
                        if (PHPAL_I14443P4_SW_IS_CHAINING(pResp[PHPAL_I14443P4_SW_PCB_POS]))
                        {
                            pDataParams->bStateNow |= PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
                        }
                        /* Clear chaining bit otherwise */
                        else
                        {
                            pDataParams->bStateNow &= (uint8_t)~(uint8_t)PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
                        }
                    }
                    /* We received an invalid block */
                    else
                    {
                        /* Protocol violation */
                        bInvalidBlock = 1;
                    }
                }
            }
            /* R(ACK) handling */
            else if (PHPAL_I14443P4_SW_IS_R_BLOCK(pResp[PHPAL_I14443P4_SW_PCB_POS]) && PHPAL_I14443P4_SW_IS_ACK(pResp[PHPAL_I14443P4_SW_PCB_POS]))
            {
                /* Check if R-Block is valid */
                status = phpalI14443p4_Sw_IsValidRBlock(
                    pDataParams->bCidEnabled,
                    pDataParams->bCid,
                    pResp,
                    wRespLen);

                /* R-Block invalid or not in transmission mode */
                if ((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
                {
                    /* Protocol violation */
                    bInvalidBlock = 1;
                }
                else
                {
                    /* Blocknumber is equal */
                    if (PHPAL_I14443P4_SW_IS_BLOCKNR_EQUAL(pResp[PHPAL_I14443P4_SW_PCB_POS]))
                    {
                        /* Continue TX chaining */
                        if (pDataParams->bStateNow == (PHPAL_I14443P4_SW_STATE_I_BLOCK_TX | PHPAL_I14443P4_SW_STATE_CHAINING_BIT))
                        {
                            /* Rule B, ISO/IEC 14443-4:2008(E), toggle Blocknumber */
                            pDataParams->bPcbBlockNum ^= PHPAL_I14443P4_SW_PCB_BLOCKNR;
                        }
                        /* Not in chaining mode */
                        else
                        {
                            /* Protocol violation */
                            bInvalidBlock = 1;
                        }
                    }
                    /* Rule 6, ISO/IEC 14443-4:2008(E), unequal block number */
                    else
                    {
                        /* Limit this behaviour with bMaxRetryCount, bRetry count is set by the caller in this case */
                        /* Note: pDataParams->bMaxRetryCount = 0 behaviour does NOT suppress this behaviour since it is mandatory. */
                        if (bRetryCount > pDataParams->bMaxRetryCount)
                        {
                            /* Protocol violation */
                            bInvalidBlock = 1;
                        }
                        /* Send last I-Block again */
                        else
                        {
                            pDataParams->bStateNow |= PHPAL_I14443P4_SW_STATE_RETRANSMIT_BIT;
                        }
                    }
                }
            }
            /* S(WTX) handling */
            else if (PHPAL_I14443P4_SW_IS_S_BLOCK(pResp[PHPAL_I14443P4_SW_PCB_POS]) && PHPAL_I14443P4_SW_IS_WTX(pResp[PHPAL_I14443P4_SW_PCB_POS]))
            {
                /* Check if S-Block is valid */
                status = phpalI14443p4_Sw_IsValidSBlock(
                    pDataParams->bCidEnabled,
                    pDataParams->bCid,
                    pResp,
                    wRespLen);

                /* Rule 3, ISO/IEC 14443-4:2008(E), S(WTX) handling */
                if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
                {
                    /* Retrieve WTXM */
                    bWtxm = pResp[wRespLen-1];

                    /* EMV 2.5 */
                    if ((bWtxm & PHPAL_I14443P4_SW_S_BLOCK_INF_PLI_MASK) && (pDataParams->bOpeMode == RD_LIB_MODE_EMVCO))
                    {
                        status =  PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
                        break;
                    }

                    /* Ignore and clear the Power Level Indication */
                    bWtxm &= 0x3F;

                    /* Treat invalid WTXM value as protocol error, do not perform error correction. */
                    if (bWtxm == 0 || bWtxm > 59)
                    {
                        status = PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
                        break;
                    }

                    /* Generate S(WTX) frame */
                    PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildSBlock(
                        pDataParams->bCidEnabled,
                        pDataParams->bCid,
                        1,
                        bWtxm,
                        bIsoFrame,
                        &wTxLength));
                }
                /* We received an invalid block */
                else
                {
                    /* Protocol violation */
                    bInvalidBlock = 1;
                }
            }
            /* We received an invalid block */
            else
            {
                /* Protocol violation */
                bInvalidBlock = 1;
            }

            /* Protocol violation */
            if (bInvalidBlock)
            {
                /* Emvco:  case_id TA404_XY and TA401_15 */
                /* bMaxRetryCount = 0 suppresses the S(DESELECT) behaviour */
                if((pDataParams->bMaxRetryCount > 0) && (pDataParams->bOpeMode != RD_LIB_MODE_EMVCO))
                {
                    /* send S(DESELECT) (ignore return code) */
                    statusTmp = phpalI14443p4_Sw_Deselect(pDataParams);
                }

                /* bail out with protocol error */
                status = PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
                break;
            }
            /* Reset retry counter on no error */
            else
            {
                bRetryCount = 0;
            }
        }
        /* Invalid Block received */
        else
        {
            /* Only perform error correction if the max. retry count is not reached */
            /* For ISO (7.5.6.1 Errors detected by the PCD)
             * a) Transmission error (Frame error or EDC error) or FWT time-out
             *      The PCD shall attempt error recovery by the following techniques in the order shown:
             *   - Application of PCD rules (see 7.5.4.2),
             *   - Optionally apply PCD rules (see 7.5.4.2) once more,
             *   - Use of S(DESELECT) request,
             *
             *   ISO 10373-6 Scenario H.29
             */

            /* DTA: Scenario 110 case: TC_POL_NFCA_T4AT_BI_31_xy, Article(13.2.5.4). Page 210
             * and For EMVCo: TA401_15 Article(10.3.4.4)
             * Says, After 2nd failed retry of I-block exchange. Send NAk
             */

            /* This is added to restrict retry count less than 1 for ISO mode */
            if( (bRetryCount >= pDataParams->bMaxRetryCount) && (pDataParams->bOpeMode == RD_LIB_MODE_ISO))
            {
                if (pDataParams->bMaxRetryCount > 0)
                {
                    /* backup retry count */
                    bRetryCount = pDataParams->bMaxRetryCount;

                    /* set retry count to zero to send only one S(DESELECT) */
                    pDataParams->bMaxRetryCount = 0;

                    /* send deselect (ignore return code) */
                    statusTmp = phpalI14443p4_Sw_Deselect(pDataParams);

                    /* restore retry count setting */
                    pDataParams->bMaxRetryCount = bRetryCount;

                    /* Return ERR_RECOVERY_FAILED if some response has been received before */
                    if (bResponseReceived)
                    {
                        status = PH_ADD_COMPCODE(PHPAL_I14443P4_ERR_RECOVERY_FAILED, PH_COMP_PAL_ISO14443P4);
                    }
                }
                break;
            }

            else if (bRetryCount <= pDataParams->bMaxRetryCount)
            {
                /* Rule 5, ISO/IEC 14443-4:2008(E) */
                if (pDataParams->bStateNow == (PHPAL_I14443P4_SW_STATE_I_BLOCK_RX | PHPAL_I14443P4_SW_STATE_CHAINING_BIT))
                {
                    /* Generate R(ACK) frame */
                    PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildRBlock(
                        pDataParams->bCidEnabled,
                        pDataParams->bCid,
                        pDataParams->bPcbBlockNum,
                        1,
                        bIsoFrame,
                        &wTxLength));

                    bNAKCount = 0;

                    /* Emvco: case_id TA410 */
                    if(bACKCount >= pDataParams->bMaxRetryCount)
                    {
                        break;
                    }

                    if((statusBkUp & PH_ERR_MASK) != PH_ERR_SUCCESS )
                    {
                        bACKCount++;
                    }
                    else
                    {
                        bACKCount = 0;
                    }
                }
                /* Rule 4, ISO/IEC 14443-4:2008(E) */
                else
                {
                    /* Generate R(NAK) frame */
                    PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildRBlock(
                        pDataParams->bCidEnabled,
                        pDataParams->bCid,
                        pDataParams->bPcbBlockNum,
                        0,
                        bIsoFrame,
                        &wTxLength));
                    bACKCount = 0;

                    /* Emvco: case_id TA402 TA403 */
                    if(bNAKCount >= pDataParams->bMaxRetryCount)
                    {
                        break;
                    }
                    /* Emvco: case_id: TA415_X */
                    if((statusBkUp & PH_ERR_MASK) != PH_ERR_SUCCESS )
                    {
                        bNAKCount++;
                    }
                    else
                    {
                        bNAKCount = 0;
                    }
                }
            }
            /* Bail out if the max. retry count is reached */
            else
            {
                /* Deselect card if behaviour is enabled */
                if (pDataParams->bMaxRetryCount > 0)
                {
                    /* backup retry count */
                    bRetryCount = pDataParams->bMaxRetryCount;

                    /* set retry count to zero to send only one S(DESELECT) */
                    pDataParams->bMaxRetryCount = 0;

                    /* Emvco Doesnot expect DeSelect Command*/
                    if(pDataParams->bOpeMode != RD_LIB_MODE_EMVCO)
                    {
                        /* send deselect (ignore return code) */
                        statusTmp = phpalI14443p4_Sw_Deselect(pDataParams);
                    }
                    /* restore retry count setting */
                    pDataParams->bMaxRetryCount = bRetryCount;

                    /* Return ERR_RECOVERY_FAILED if some response has been received before */
                    if (bResponseReceived) {
                        status = PH_ADD_COMPCODE(PHPAL_I14443P4_ERR_RECOVERY_FAILED, PH_COMP_PAL_ISO14443P4);
                    }
                }
                break;
            }

            /* Increment retry count */
            ++bRetryCount;
        }

        /* Map TxBuffer to ISO Frame buffer for next loop */
        pTxBuffer = bIsoFrame;
    }
    /* Loop as long as we generate transmissions */
    while (wTxLength);

    /* Reset RxBuffer Start Position */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXBUFFER_STARTPOS, 0));

    return status;
}

phStatus_t phpalI14443p4_Sw_IsValidIBlock(
    uint8_t bCheckCid,
    uint8_t bCid,
    uint8_t bCheckNad,
    uint8_t bNad,
    uint8_t * pRxBuffer,
    uint16_t wRxLength
    )
{
    uint16_t PH_MEMLOC_REM wExpRxLength;

    /* Check RFU bits */
    if ((pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_I_BLOCK_RFU_MASK) != PHPAL_I14443P4_SW_I_BLOCK_RFU_BITS)
    {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    /* We always expect the PCB byte to be present */
    wExpRxLength = 1;

    /* Enable CID checking if neccessary */
    if (bCheckCid)
    {
        wExpRxLength++;
    }

    /* Enable NAD checking if neccessary */
    if (bCheckNad)
    {
        wExpRxLength++;
    }

    /* The frame should have the minimum frame length */
    if (wRxLength < wExpRxLength)
    {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    /* CID presence check */
    if ((bCheckCid) &&
        (pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_CID_FOLLOWING) &&
        ((pRxBuffer[PHPAL_I14443P4_SW_PCB_POS+1] & PHPAL_I14443P4_SW_CID_MASK) == bCid)
        )
    {
        /* CHECK SUCCEEDED */
    }
    /* CID absence check */
    else if (!(bCheckCid) && !(pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_CID_FOLLOWING))
    {
        /* CHECK SUCCEEDED */
    }
    /* CID protocol error */
    else
    {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    /* NAD presence check */
    if ((bCheckNad) &&
        (pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_NAD_FOLLOWING) &&
        (pRxBuffer[wExpRxLength-1] == bNad)
        )
    {
        /* CHECK SUCCEEDED */
    }
    /* NAD absence check */
    else if (!(bCheckNad) && !(pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_NAD_FOLLOWING))
    {
        /* CHECK SUCCEEDED */
    }
    /* NAD protocol error */
    else
    {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_IsValidRBlock(
    uint8_t bCheckCid,
    uint8_t bCid,
    uint8_t * pRxBuffer,
    uint16_t wRxLength
    )
{
    uint16_t PH_MEMLOC_REM wExpRxLength;

    /* Check RFU bits */
    if ((pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_R_BLOCK_RFU_MASK) != PHPAL_I14443P4_SW_R_BLOCK_RFU_BITS)
    {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    /* We always expect the PCB byte to be present */
    wExpRxLength = 1;

    /* If CID is enabled, we always expect it */
    if (bCheckCid)
    {
        wExpRxLength++;
    }

    /* The frame should have the exact frame length */
    if (wRxLength != wExpRxLength)
    {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    /* CID presence check */
    if ((bCheckCid) &&
        (pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_CID_FOLLOWING) &&
        ((pRxBuffer[PHPAL_I14443P4_SW_PCB_POS+1] & PHPAL_I14443P4_SW_CID_MASK) == bCid))
    {
        /* CHECK SUCCEEDED */
    }
    /* CID absence check */
    else if (!(bCheckCid) && !(pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_CID_FOLLOWING))
    {
        /* CHECK SUCCEEDED */
    }
    /* CID protocol error */
    else
    {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_IsValidSBlock(
    uint8_t bCheckCid,
    uint8_t bCid,
    uint8_t * pRxBuffer,
    uint16_t wRxLength
    )
{
    uint16_t PH_MEMLOC_REM wExpRxLength;

    /* Check RFU bits */
    if ((pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_S_BLOCK_RFU_MASK) != PHPAL_I14443P4_SW_S_BLOCK_RFU_BITS)
    {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    /* We always expect the PCB byte to be present */
    wExpRxLength = 1;

    /* If CID is enabled, we always expect it */
    if (bCheckCid)
    {
        wExpRxLength++;
    }

    /* If this is a WTX request, we expect an additional INF byte */
    if (PHPAL_I14443P4_SW_IS_WTX(pRxBuffer[PHPAL_I14443P4_SW_PCB_POS]))
    {
        wExpRxLength++;
    }

    /* The frame should have the exact frame length */
    if (wRxLength != wExpRxLength)
    {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    /* CID presence check */
    if ((bCheckCid) &&
        (pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_CID_FOLLOWING) &&
        ((pRxBuffer[PHPAL_I14443P4_SW_PCB_POS+1] & PHPAL_I14443P4_SW_CID_MASK) == bCid))
    {
        /* CHECK SUCCEEDED */
    }
    /* CID absence check */
    else if (!(bCheckCid) && !(pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_CID_FOLLOWING))
    {
        /* CHECK SUCCEEDED */
    }
    /* CID protocol error */
    else
    {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_BuildIBlock(
                                        uint8_t bCidEnabled,
                                        uint8_t bCid,
                                        uint8_t bNadEnabled,
                                        uint8_t bNad,
                                        uint8_t bPcbBlockNum,
                                        uint8_t bChaining,
                                        uint8_t * pTxBuffer,
                                        uint16_t * pTxLength
                                        )
{
    /* I-Block PCB */
    pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] = PHPAL_I14443P4_SW_I_BLOCK | PHPAL_I14443P4_SW_I_BLOCK_RFU_BITS | bPcbBlockNum;
    *pTxLength = 1;

    /* Check if chaining is neccessary */
    if (bChaining)
    {
        pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] |= PHPAL_I14443P4_SW_PCB_CHAINING;
    }

    /* Append CID if neccessary */
    if (bCidEnabled)
    {
        pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] |= PHPAL_I14443P4_SW_PCB_CID_FOLLOWING;
        pTxBuffer[(*pTxLength)++] = bCid;
    }

    /* Append NAD if neccessary */
    if (bNadEnabled)
    {
        pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] |= PHPAL_I14443P4_SW_PCB_NAD_FOLLOWING;
        pTxBuffer[(*pTxLength)++] = bNad;
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_BuildRBlock(
                                        uint8_t bCidEnabled,
                                        uint8_t bCid,
                                        uint8_t bPcbBlockNum,
                                        uint8_t bIsAck,
                                        uint8_t * pTxBuffer,
                                        uint16_t * pTxLength
                                        )
{
    /* R(ACK) PCB */
    pTxBuffer[PHPAL_I14443P4_SW_PCB_POS]  = PHPAL_I14443P4_SW_R_BLOCK | PHPAL_I14443P4_SW_R_BLOCK_RFU_BITS | bPcbBlockNum;
    *pTxLength = 1;

    /* Switch to R(NAK) if neccessary */
    if (!bIsAck)
    {
        pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] |= PHPAL_I14443P4_SW_PCB_NAK;
    }

    /* Append CID if supported */
    if (bCidEnabled != 0)
    {
        pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] |= PHPAL_I14443P4_SW_PCB_CID_FOLLOWING;
        pTxBuffer[(*pTxLength)++] = bCid;
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_BuildSBlock(
                                        uint8_t bCidEnabled,
                                        uint8_t bCid,
                                        uint8_t bIsWtx,
                                        uint8_t bWtxm,
                                        uint8_t * pTxBuffer,
                                        uint16_t * pTxLength
                                        )
{
    /* S-Block PCB */
    pTxBuffer[PHPAL_I14443P4_SW_PCB_POS]  = PHPAL_I14443P4_SW_S_BLOCK | PHPAL_I14443P4_SW_S_BLOCK_RFU_BITS;
    *pTxLength = 1;

    /* Append CID if supported */
    if (bCidEnabled != 0)
    {
        pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] |= PHPAL_I14443P4_SW_PCB_CID_FOLLOWING;
        pTxBuffer[(*pTxLength)++] = bCid;
    }

    /* Set WTX bits and append WTXM if neccessary */
    if (bIsWtx)
    {
        pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] |= PHPAL_I14443P4_SW_PCB_WTX;
        pTxBuffer[(*pTxLength)++] = bWtxm;
    }
    /* Set DESELECT bits if neccessary */
    else
    {
        pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] |= PHPAL_I14443P4_SW_PCB_DESELECT;
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_SetConfig(
                                      phpalI14443p4_Sw_DataParams_t * pDataParams,
                                      uint16_t wConfig,
                                      uint16_t wValue
                                      )
{
    switch (wConfig)
    {
    case PHPAL_I14443P4_CONFIG_OPE_MODE:
        {
            pDataParams->bOpeMode = (uint8_t)wValue;
            break;
        }
    case PHPAL_I14443P4_CONFIG_BLOCKNO:
        {
            if (wValue == 0)
            {
                pDataParams->bPcbBlockNum = 0;
            }
            else
            {
                pDataParams->bPcbBlockNum = 1;
            }

            break;
        }
    case PHPAL_I14443P4_CONFIG_CID:
        {
            if (wValue & 0xFF00)
            {
                if ((wValue & 0x00FF) > PHPAL_I14443P4_CID_MAX)
                {
                    return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P4);
                }
                pDataParams->bCidEnabled = 1;
                pDataParams->bCid = (uint8_t)(wValue);
            }
            else
            {
                pDataParams->bCidEnabled = 0;
                pDataParams->bCid = 0;
            }

            break;
        }
    case PHPAL_I14443P4_CONFIG_NAD:
        {
            if (wValue & 0xFF00)
            {
                pDataParams->bNadEnabled = 1;
                pDataParams->bNad = (uint8_t)(wValue);
            }
            else
            {
                pDataParams->bNadEnabled = 0;
                pDataParams->bNad = 0;
            }

            break;
        }
    case PHPAL_I14443P4_CONFIG_FWI:
        {
            if (wValue > PHPAL_I14443P4_FWI_MAX)
            {
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P4);
            }
            pDataParams->bFwi = (uint8_t)(wValue);
            break;
        }
    case PHPAL_I14443P4_CONFIG_FSI:
        {
            if (((wValue >> 8) > PHPAL_I14443P4_FRAMESIZE_MAX) ||
                ((wValue & 0xFF) > PHPAL_I14443P4_FRAMESIZE_MAX))
            {
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P4);
            }

            pDataParams->bFsdi = (uint8_t)(wValue >> 8);
            pDataParams->bFsci = (uint8_t)(wValue);
            break;
        }
    case PHPAL_I14443P4_CONFIG_MAXRETRYCOUNT:
        {
            pDataParams->bMaxRetryCount = (uint8_t)(wValue);
            break;
        }
    default:
        {
            return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_PARAMETER, PH_COMP_PAL_ISO14443P4);
        }
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_GetConfig(
                                      phpalI14443p4_Sw_DataParams_t * pDataParams,
                                      uint16_t wConfig,
                                      uint16_t * pValue
                                      )
{
    switch (wConfig)
    {
    case PHPAL_I14443P4_CONFIG_BLOCKNO:
        {
            if (pDataParams->bPcbBlockNum == 0)
            {
                *pValue = 0;
            }
            else
            {
                *pValue = 1;
            }

            break;
        }
    case PHPAL_I14443P4_CONFIG_CID:
        {
            if (pDataParams->bCidEnabled)
            {
                *pValue = (uint16_t)(0x0100 | pDataParams->bCid);
            }
            else
            {
                *pValue = (uint16_t)(pDataParams->bCid);
            }

            break;
        }
    case PHPAL_I14443P4_CONFIG_NAD:
        {
            if (pDataParams->bNadEnabled)
            {
                *pValue = (uint16_t)(0x0100 | pDataParams->bNad);
            }
            else
            {
                *pValue = (uint16_t)(pDataParams->bNad);
            }

            break;
        }
    case PHPAL_I14443P4_CONFIG_FWI:
        {
            *pValue = (uint16_t)pDataParams->bFwi;
            break;
        }
    case PHPAL_I14443P4_CONFIG_FSI:
        {
            *pValue  = (uint16_t)((uint16_t)(pDataParams->bFsdi) << 8);
            *pValue |= (uint16_t)(pDataParams->bFsci);
            break;
        }
    case PHPAL_I14443P4_CONFIG_MAXRETRYCOUNT:
        {
            *pValue = (uint16_t)pDataParams->bMaxRetryCount;
            break;
        }
    default:
        {
            return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_PARAMETER, PH_COMP_PAL_ISO14443P4);
        }
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

#endif /* NXPBUILD__PHPAL_I14443P4_SW */
