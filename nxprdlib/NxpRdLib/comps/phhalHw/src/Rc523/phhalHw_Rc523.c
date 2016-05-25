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
* RC523 specific HAL-Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4245 $ (NFCRDLIB_V4.010.01.001603 : 4093 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  CHu: Generated 19. May 2009
*
*/

#include <ph_Status.h>
#include <phbalReg.h>
#include <phhalHw.h>
#include <ph_RefDefs.h>
#include <phOsal.h>

#ifdef NXPBUILD__PH_KEYSTORE
#include <phKeyStore.h>
#endif

#ifdef NXPBUILD__PHHAL_HW_RC523

#include "phhalHw_Rc523.h"
#include "phhalHw_Rc523_Int.h"
#include <phhalHw_Rc523_Reg.h>
#include "phhalHw_Rc523_Config.h"
#include <phTools.h>

/* Default shadow for ISO14443-3A Mode */
static const uint16_t PH_MEMLOC_CONST_ROM wRc523_DefaultShadow_I14443a[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_ON},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_OFF},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_OFF},
    {PHHAL_HW_CONFIG_TXWAIT_US,             500},
    {PHHAL_HW_CONFIG_TXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_106 | PHHAL_HW_RF_TYPE_A_FRAMING},
    {PHHAL_HW_CONFIG_RXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_106 | PHHAL_HW_RF_TYPE_A_FRAMING},
    {PHHAL_HW_CONFIG_RXWAIT_US,             76},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_RC523_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_MODINDEX,              0x3FU},
    {PHHAL_HW_CONFIG_ASK100,                PH_ON}
};

/* Default shadow for ISO14443-3B Mode */
static const uint16_t PH_MEMLOC_CONST_ROM wRc523_DefaultShadow_I14443b[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_ON},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_TXWAIT_US,             500},
    {PHHAL_HW_CONFIG_TXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_106 | PHHAL_HW_RF_TYPE_B_FRAMING},
    {PHHAL_HW_CONFIG_RXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_106 | PHHAL_HW_RF_TYPE_B_FRAMING},
    {PHHAL_HW_CONFIG_RXWAIT_US,             76},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_RC523_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_MODINDEX,              PHHAL_HW_RC523_MODINDEX_I14443B},
    {PHHAL_HW_CONFIG_ASK100,                PH_OFF}
};

/* Default shadow for FeliCa Mode */
static const uint16_t PH_MEMLOC_CONST_ROM wRc523_DefaultShadow_Felica[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_OFF},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_TXWAIT_US,             730},
    {PHHAL_HW_CONFIG_TXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_212 | PHHAL_HW_RF_TYPE_F_FRAMING},
    {PHHAL_HW_CONFIG_RXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_212 | PHHAL_HW_RF_TYPE_F_FRAMING},
    {PHHAL_HW_CONFIG_RXWAIT_US,             15},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_RC523_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_MODINDEX,              PHHAL_HW_RC523_MODINDEX_FELICA},
    {PHHAL_HW_CONFIG_ASK100,                PH_OFF}
};

/* Default shadow for FeliCa Mode at 424 baud rate */
static const uint16_t PH_MEMLOC_CONST_ROM wRc523_DefaultShadow_Felica_424[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_OFF},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_TXWAIT_US,             615},
    {PHHAL_HW_CONFIG_TXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_424 | PHHAL_HW_RF_TYPE_F_FRAMING},
    {PHHAL_HW_CONFIG_RXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_424 | PHHAL_HW_RF_TYPE_F_FRAMING},
    {PHHAL_HW_CONFIG_RXWAIT_US,             8},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_RC523_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_MODINDEX,              PHHAL_HW_RC523_MODINDEX_FELICA},
    {PHHAL_HW_CONFIG_ASK100,                PH_OFF}
};

/* Default shadow for ISO18092 Active Initiator Mode at 106kbps. */
static const uint16_t PH_MEMLOC_CONST_ROM wRc523_DefShadow_I18092m_Active[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_ON},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_TXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_106 | PHHAL_HW_RF_TYPE_ACTIVE_FRAMING},
    {PHHAL_HW_CONFIG_RXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_106 | PHHAL_HW_RF_TYPE_ACTIVE_FRAMING},
    {PHHAL_HW_CONFIG_RXWAIT_US,             29},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_RC523_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_MODINDEX,              PHHAL_HW_RC523_MODINDEX_FELICA},
    {PHHAL_HW_CONFIG_ASK100,                PH_ON}
};

/* Default shadow for ISO18092 Active Initiator Mode at 212kbps. */
static const uint16_t PH_MEMLOC_CONST_ROM wRc523_DefShadow_I18092m_212_Active[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_OFF},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_TXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_212 | PHHAL_HW_RF_TYPE_ACTIVE_FRAMING},
    {PHHAL_HW_CONFIG_RXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_212 | PHHAL_HW_RF_TYPE_ACTIVE_FRAMING},
    {PHHAL_HW_CONFIG_RXWAIT_US,             15},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_RC523_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_MODINDEX,              PHHAL_HW_RC523_MODINDEX_FELICA},
    {PHHAL_HW_CONFIG_ASK100,                PH_OFF}
};

/* Default shadow for ISO18092 Active Initiator Mode at 424kbps. */
static const uint16_t PH_MEMLOC_CONST_ROM wRc523_DefShadow_I18092m_Active_424[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_OFF},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_TXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_424 | PHHAL_HW_RF_TYPE_ACTIVE_FRAMING},
    {PHHAL_HW_CONFIG_RXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_424 | PHHAL_HW_RF_TYPE_ACTIVE_FRAMING},
    {PHHAL_HW_CONFIG_RXWAIT_US,             8},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_RC523_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_MODINDEX,              PHHAL_HW_RC523_MODINDEX_FELICA},
    {PHHAL_HW_CONFIG_ASK100,                PH_OFF}
};

/* Shadow that can be used for ISO18092 Target Mode. */
static const uint16_t PH_MEMLOC_CONST_ROM wRc523_DefaultShadow_I18092mPT[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_OFF},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXWAIT_US,             76},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_RC523_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      PHHAL_HW_RC523_DEFAULT_TIMEOUT_MILLI},
    {PHHAL_HW_CONFIG_ASK100,                PH_OFF}
};

static const uint16_t PH_MEMLOC_CONST_ROM wRc523_DepShadow_I14443a[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_ON},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_RC523_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_MODINDEX,              0x3FU},
    {PHHAL_HW_CONFIG_ASK100,                PH_ON}
};

phStatus_t phhalHw_Rc523_Init(
                              phhalHw_Rc523_DataParams_t * pDataParams,
                              uint16_t wSizeOfDataParams,
                              void * pBalDataParams,
                              void * pKeyStoreDataParams,
                              uint8_t * pTxBuffer,
                              uint16_t wTxBufSize,
                              uint8_t * pRxBuffer,
                              uint16_t wRxBufSize
                              )
{
    if (sizeof(phhalHw_Rc523_DataParams_t) != wSizeOfDataParams)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_DATA_PARAMS, PH_COMP_HAL);
    }
    if (wTxBufSize == 0 || wRxBufSize == 0)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }
    PH_ASSERT_NULL (pDataParams);
    PH_ASSERT_NULL (pBalDataParams);
    PH_ASSERT_NULL (pTxBuffer);
    PH_ASSERT_NULL (pRxBuffer);

    /* We need to reserve 1 byte at the beginning of the buffers for SPI operation */
    ++pTxBuffer;
    --wTxBufSize;
    ++pRxBuffer;
    --wRxBufSize;

    /* Init. private data */
    pDataParams->wId                    = PH_COMP_HAL | PHHAL_HW_RC523_ID;
    pDataParams->pBalDataParams         = pBalDataParams;
    pDataParams->pKeyStoreDataParams    = pKeyStoreDataParams;
    pDataParams->pTxBuffer              = pTxBuffer;
    pDataParams->wTxBufSize             = wTxBufSize;
    pDataParams->wTxBufLen              = 0;
    pDataParams->pRxBuffer              = pRxBuffer;
    pDataParams->wRxBufSize             = wRxBufSize;
    pDataParams->wRxBufLen              = 0;
    pDataParams->wRxBufStartPos         = 0;
    pDataParams->bCardType              = PHHAL_HW_CARDTYPE_ISO14443A;
    pDataParams->wTimingMode            = PHHAL_HW_TIMING_MODE_OFF;
    pDataParams->bTimeoutUnit           = PHHAL_HW_TIME_MICROSECONDS;
    pDataParams->dwTimingUs             = 0;
    pDataParams->wFieldOffTime          = PHHAL_HW_FIELD_OFF_DEFAULT;
    pDataParams->wFieldRecoveryTime     = PHHAL_HW_FIELD_RECOVERY_DEFAULT;
    pDataParams->wMaxPrecachedBytes     = PHHAL_HW_RC523_PRECACHED_BYTES;
    pDataParams->wAdditionalInfo        = 0;
#ifndef NXPBUILD__PHBAL_REG_SERIALWIN
    pDataParams->bBalConnectionType     = PHHAL_HW_BAL_CONNECTION_SPI;
#endif /* NXPBUILD__PHBAL_REG_SERIALWIN */
    pDataParams->bRfResetAfterTo        = PH_OFF;
    pDataParams->bRxMultiple            = PH_OFF;
    pDataParams->bActiveMode            = PH_OFF;
    pDataParams->bRfca                  = PH_ON;
    pDataParams->wTargetMode            = PH_OFF;
    pDataParams->bOpeMode               = RD_LIB_MODE_NFC;
    pDataParams->bNfcipMode             = PH_OFF;
    pDataParams->wTxWaitReloadVal       = 0;
    pDataParams->wTimeoutReloadVal      = 0;
    pDataParams->bTxWaitPrescaler       = 0;
    pDataParams->bTimeoutPrescaler      = 0;
    pDataParams->bSkipSend              = PH_OFF;
    pDataParams->bCheckEmdErr           = PH_OFF;

#ifndef NXPBUILD__PHBAL_REG_SERIALWIN
    return phbalReg_SetConfig(pDataParams->pBalDataParams, PHBAL_REG_CONFIG_HAL_HW_TYPE, PHBAL_REG_HAL_HW_RC523);
#endif /* NXPBUILD__PHBAL_REG_SERIALWIN */
}

/* Used to create delay in microseconds using clif timers it does not affect the clif state machine. */
static phStatus_t phhalHw_Rc523_Delay(
                                      phhalHw_Rc523_DataParams_t * pDataParams,
                                      uint16_t wDelay)
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bRegister;
    uint8_t     PH_MEMLOC_REM bIrq0WaitFor;
    uint8_t     PH_MEMLOC_REM bIrq1WaitFor;

    /* Configure the timers. */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetFdt(pDataParams, PHHAL_HW_TIME_MICROSECONDS, wDelay));

    /* retrieve content of Control register */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_CONTROL, &bRegister));

    /* clear all irq flags */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIRQ, (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_SET));

    /* start timer */
    bRegister |= PHHAL_HW_RC523_BIT_TSTARTNOW;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_CONTROL, bRegister));

    /* wait for timer interrupt */
    bIrq0WaitFor = PHHAL_HW_RC523_BIT_ERRI | PHHAL_HW_RC523_BIT_TIMERI;
    bIrq1WaitFor = 0x00;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WaitIrq(pDataParams, bIrq0WaitFor, bIrq1WaitFor, &bRegister, &bRegister));

    /* clear all irq flags */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIRQ, (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_SET));

    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Rc523_Exchange(
                                  phhalHw_Rc523_DataParams_t * pDataParams,
                                  uint16_t wOption,
                                  uint8_t * pTxBuffer,
                                  uint16_t wTxLength,
                                  uint8_t ** ppRxBuffer,
                                  uint16_t * pRxLength
                                  )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint16_t    PH_MEMLOC_REM wTmpBufferSize;
    uint16_t    PH_MEMLOC_REM wloopCnt;
    uint8_t     PH_MEMLOC_REM aCrc[2];
    uint8_t     PH_MEMLOC_REM bFraming;
    uint8_t     PH_MEMLOC_REM bRegister;
    uint8_t     PH_MEMLOC_REM bBitlen ;

    /* assign the address of Tx buffer received. This will be used in case of jewel mode
    *  if any other Hardware is used*/
    pTmpBuffer    = pTxBuffer;
    wTmpBufferLen = 0x00;

    if (pDataParams->wTargetMode != PH_OFF)
    {
        /* If wTargetMode is changed then Target is activated and Exchange should not be used to perform Tx Operation. */
        return PH_ADD_COMPCODE(PH_ERR_USE_CONDITION, PH_COMP_HAL);
    }

    /* Check options */
    if (wOption & (uint16_t)~(uint16_t)(PH_EXCHANGE_BUFFERED_BIT | PH_EXCHANGE_LEAVE_BUFFER_BIT))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* clear internal buffer if requested */
    if (!(wOption & PH_EXCHANGE_LEAVE_BUFFER_BIT))
    {
        pDataParams->wTxBufLen = 0;
    }

    /* set the receive length */
    if (pRxLength != NULL)
    {
        *pRxLength = 0;
    }

    /* Prepend Start Byte (0xF0) in Passive 106kbps during NFCIP exchange. */
    if ((pDataParams->bNfcipMode == PH_ON) &&
        (!(wOption & PH_EXCHANGE_LEAVE_BUFFER_BIT)))
    {
        /* Read the TxMode Register to read the BaudRate */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, &bRegister));
        bFraming = bRegister & PHHAL_HW_RC523_MASK_FRAMING;
        bRegister = bRegister & PHHAL_HW_RC523_MASK_SPEED;
        if ((bRegister == PHHAL_HW_RC523_BIT_106KBPS) && (bFraming == PHHAL_HW_RC523_BIT_MIFARE))
        {
            /* retrieve transmit buffer */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetTxBuffer(pDataParams, PH_ON, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

            /* Prepend Start Byte 0xF0 in case of Passive Communication at 106kbps BaudRate */
            memset(&pTmpBuffer[wTmpBufferLen], 0xF0, 0x01);  /* PRQA S 3200 */
            pDataParams->wTxBufLen++;
        }
    }

    /* Fill the global TxBuffer */
    /* Note: We always need to buffer for SPI, else the input buffer would get overwritten! */
    if ((wOption & PH_EXCHANGE_BUFFERED_BIT) ||
        (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_SPI) ||
        (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_I2C))
    {
        /* retrieve transmit buffer */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetTxBuffer(pDataParams, PH_ON, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

        if (wTxLength != 0)
        {
            /* TxBuffer overflow check */
            if (wTxLength > (wTmpBufferSize - wTmpBufferLen))
            {
                pDataParams->wTxBufLen = 0;
                return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
            }

            /* copy data */
            memcpy(&pTmpBuffer[wTmpBufferLen], pTxBuffer, wTxLength);  /* PRQA S 3200 */
            pDataParams->wTxBufLen = pDataParams->wTxBufLen + wTxLength;
        }

        /* Buffer operation -> finished */
        if (wOption & PH_EXCHANGE_BUFFERED_BIT)
        {
            return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
        }
        /* Else reset the input length for SPI and continue */
        else
        {
            wTxLength = 0;
        }
    }

    /* Terminate a possibly running command */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_IDLE));

    if(pDataParams->bJewelActivated == PH_ON)
    {
        if((pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_SPI) ||
           (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_I2C))
        {
            wTxLength = pDataParams->wTxBufLen;
        }
        wOption = PH_EXCHANGE_DEFAULT;
        /*Compute Crc_B for the data to be transmitted */
        PH_CHECK_SUCCESS_FCT(statusTmp, phTools_ComputeCrc_B(pTmpBuffer, wTxLength, aCrc));

        /* Turn OFF the parity, by default shadow it is ON */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_PARITY, PH_OFF));

        /* Make sure CRC is OFF */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXCRC, PH_OFF));
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_RXCRC, PH_OFF));

        /* Only 7 bits are valid*/
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXLASTBITS, 7));

        /*transmit the  command*/
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_Transmit(pDataParams,wOption, &pTmpBuffer[0], 1));

        /* All 8 bits are valid for last byte*/
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXLASTBITS, 0));

        /*transmit add-data-0 except last byte*/
        for( wloopCnt = 1; wloopCnt < wTxLength; wloopCnt++ )
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_Transmit(pDataParams, wOption, &pTmpBuffer[wloopCnt], 1));
        }
        /*transmit first byte of the Crc_B*/
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_Transmit( pDataParams, wOption, &aCrc[0], 1));

        /*Hw_exchange() of last byte of Crc_B*/
        pTxBuffer[0] = aCrc[1];
        wTxLength = 1;
    }

    /* Flush FiFo */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FlushFifo(pDataParams));

    /* clear all IRQ0 flags */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_COMMIRQ,
        (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_SET));

    /* clear all IRQ1 flags */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_DIVIRQ,
        (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_SET));

    /* perform transmission */
    status = phhalHw_Rc523_ExchangeTransmit(pDataParams, PHHAL_HW_RC523_CMD_TRANSCEIVE, pTxBuffer, wTxLength);

    /* perform receive operation */
    if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS)
    {
        /* Clear HIALERT IRQ flag */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
            pDataParams,
            PHHAL_HW_RC523_REG_COMMIRQ,
            PHHAL_HW_RC523_BIT_HIALERTI));

        status = phhalHw_Rc523_ExchangeReceive(pDataParams, ppRxBuffer, pRxLength);
    }

    if(pDataParams->bJewelActivated == PH_ON && (status == PH_ERR_SUCCESS || ( status & PH_ERR_MASK) == PH_ERR_SUCCESS_INCOMPLETE_BYTE))
    {
        /* QAC Fix */
        if((NULL == ppRxBuffer) || (NULL == pRxLength ))
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        /* Retrieve RxBits */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_CONTROL, &bRegister));
        /* Mask out valid bits of last byte */
        bRegister &= PHHAL_HW_RC523_MASK_RXBITS;
        /*remove parity of the received data-as harware's parity is off*/
        PH_CHECK_SUCCESS_FCT(
            statusTmp,
            phTools_DecodeParity(
            PH_TOOLS_PARITY_OPTION_ODD ,
            *ppRxBuffer,
            *pRxLength,
            bRegister,
            *pRxLength,
            *ppRxBuffer,
            &wTmpBufferLen,
            &bBitlen));

        /* Check for response length
         * For Type 1 tag, valid minimum response length is 4 */
        if(wTmpBufferLen >= 4)
        {
        /*verify Crc_B for the received data*/
            PH_CHECK_SUCCESS_FCT(statusTmp,phTools_ComputeCrc_B( *ppRxBuffer, wTmpBufferLen-2, aCrc));
            if(!(aCrc[0] == ((*ppRxBuffer)[wTmpBufferLen - 2]) && aCrc[1] ==  ((*ppRxBuffer)[wTmpBufferLen - 1])))
            {
                status = PH_ERR_INTEGRITY_ERROR;
            }
            /* subtract CRC from Rx buffer */
            *pRxLength =wTmpBufferLen - 2;
            status = PH_ERR_SUCCESS;
        }
        else
        {
            return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_HAL);
        }
    }
    return status;
}

phStatus_t phhalHw_Rc523_Transmit(
                                  phhalHw_Rc523_DataParams_t * pDataParams,
                                  uint16_t wOption,
                                  uint8_t * pTxBuffer,
                                  uint16_t wTxLength
                                  )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bFraming;
    uint8_t     PH_MEMLOC_REM bRegister;
    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint16_t    PH_MEMLOC_REM wTmpBufferSize;

    wTmpBufferLen = 0x00;

    /* Check options */
    if (wOption & (uint16_t)~(uint16_t)(PH_TRANSMIT_BUFFERED_BIT | PH_TRANSMIT_LEAVE_BUFFER_BIT | PH_TRANSMIT_PREV_FRAME))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Re-send option should not be combined with buffer options. */
    if ((wOption & PH_TRANSMIT_PREV_FRAME) && (wOption & (uint16_t)~(uint16_t)(PH_TRANSMIT_PREV_FRAME)))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    if (wOption == PH_TRANSMIT_PREV_FRAME)
    {
        /* Length to be transmitted should not be zero. */
        if (wTxLength == 0x00)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        /* Retrieve TxBuffer parameters */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetTxBuffer(pDataParams, PH_ON, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

        /* Check parameter, must not exceed TxBufferSize */
        if (wTxLength > wTmpBufferSize)
        {
            return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
        }

        /* Set buffer length */
        pDataParams->wTxBufLen = wTxLength;
        wTxLength = 0;
    }

    /* If Target Mode is not activated then terminate the running command. */
    if (pDataParams->wTargetMode == PH_OFF)
    {
        /* Terminate a possibly running command */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_IDLE));
    }

    /* Flush FiFo */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FlushFifo(pDataParams));

    /* clear all IRQ0 flags */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_COMMIRQ,
        (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_SET));

    /* clear all IRQ1 flags */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_DIVIRQ,
        (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_SET));

    /* When Transmit is called in Initiator mode based on wOption, handle Timer. */
    if(pDataParams->wTargetMode != PH_OFF)
    {
        /* In Passive Target Mode if External RF is OFF then Transmit should return External RF Error,
        * instead of going further and performing Transmit. */
        if (pDataParams->bActiveMode == PH_OFF)
        {
            /* Read the External RF Field Detection Bit from Status Register */
            PH_CHECK_SUCCESS_FCT(status, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS1, &bRegister));
            if(!(bRegister & PHHAL_HW_RC523_BIT_RFON))
            {
                /* External RF Field is OFF. Transmit should not be performed. */
                return PH_ADD_COMPCODE(PH_ERR_EXT_RF_ERROR, PH_COMP_HAL);
            }
        }
    }

    /* clear internal buffer if requested */
    if (!(wOption & (PH_TRANSMIT_LEAVE_BUFFER_BIT | PH_TRANSMIT_PREV_FRAME)))
    {
        pDataParams->wTxBufLen = 0;
    }

    /* Prepend Start Byte (0xF0) in Passive 106kbps during NFCIP exchange. */
    if ((pDataParams->bNfcipMode == PH_ON) &&
        (!(wOption & PH_TRANSMIT_LEAVE_BUFFER_BIT)))
    {
        if (!(wOption & PH_TRANSMIT_PREV_FRAME))
        {
            /* Read the TxMode Register to read the BaudRate */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, &bRegister));
            bFraming = bRegister & PHHAL_HW_RC523_MASK_FRAMING;
            bRegister = bRegister & PHHAL_HW_RC523_MASK_SPEED;
            if ((bRegister == PHHAL_HW_RC523_BIT_106KBPS) && (bFraming == PHHAL_HW_RC523_BIT_MIFARE))
            {
                /* retrieve transmit buffer */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetTxBuffer(pDataParams, PH_ON, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

                /* Prepend Start Byte 0xF0 in case of Passive Communication at 106kbps BaudRate */
                memset(&pTmpBuffer[wTmpBufferLen], 0xF0, 0x01);  /* PRQA S 3200 */
                pDataParams->wTxBufLen++;
            }
        }
        else
        {
            pDataParams->wTxBufLen++;
        }
    }

    /* Fill the global TxBuffer */
    /* Note: We always need to buffer for SPI, else the input buffer would get overwritten! */
    if ((wOption & PH_TRANSMIT_BUFFERED_BIT) ||
        (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_SPI) ||
        (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_I2C))
    {
        /* In Target Mode it's not allowed to have same buffers for transmission and reception
        *  since the data to transmit would get overwritten by received data. */
        if ((pDataParams->wTargetMode != PH_OFF) &&
            (pDataParams->pTxBuffer == pDataParams->pRxBuffer))
        {
            return PH_ADD_COMPCODE(PH_ERR_USE_CONDITION, PH_COMP_HAL);
        }

        /* retrieve transmit buffer */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetTxBuffer(pDataParams, PH_ON, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

        if (wTxLength != 0)
        {
            /* TxBuffer overflow check */
            if (wTxLength > (wTmpBufferSize - wTmpBufferLen))
            {
                pDataParams->wTxBufLen = 0;
                return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
            }

            /* copy data */
            memcpy(&pTmpBuffer[wTmpBufferLen], pTxBuffer, wTxLength);  /* PRQA S 3200 */
            pDataParams->wTxBufLen = pDataParams->wTxBufLen + wTxLength;
        }

        /* Buffer operation -> finished */
        if (wOption & PH_TRANSMIT_BUFFERED_BIT)
        {
            return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
        }
        /* Else reset the input length for SPI and continue */
        else
        {
            wTxLength = 0;
        }
    }

    /* Perform transmission */
    status = phhalHw_Rc523_ExchangeTransmit(pDataParams, PHHAL_HW_RC523_CMD_TRANSMIT, pTxBuffer, wTxLength);

    return status;
}

phStatus_t phhalHw_Rc523_Receive(
                                 phhalHw_Rc523_DataParams_t * pDataParams,
                                 uint16_t wOption,
                                 uint8_t ** ppRxBuffer,
                                 uint16_t * pRxLength
                                 )
{
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bRegister;
    uint8_t     PH_MEMLOC_REM bRegRxWait =0;

    /* Check options */
    if (wOption != PH_RECEIVE_DEFAULT)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Reduce RxWait to minimum */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_RXSEL, &bRegRxWait));
    bRegister = bRegRxWait & (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_RXWAIT;
    bRegister |= 0x03;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXSEL, bRegister));

    /* set the receive length */
    if (pRxLength != NULL)
    {
        *pRxLength = 0;
    }

    if (pDataParams->wTargetMode == PH_OFF)
    {
        /* Terminate a possibly running command */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_IDLE));
    }

    /* Flush FiFo */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FlushFifo(pDataParams));

    /* clear all IRQ0 flags */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_COMMIRQ,
        (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_SET));

    /* clear all IRQ1 flags */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_DIVIRQ,
        (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_SET));

    if (pDataParams->wTargetMode != PH_OFF)
    {
        /* Retrieve contents of Status2 Register to check for Modem state is in Wait for data State. */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS2, &bRegister));
        /* Check if Modem State not equal to Wait for data State. */
        if((bRegister & PHHAL_HW_RC523_MASK_MODEMBITS) != PHHAL_HW_RC523_BIT_WAIT_FOR_DATA)
        {
            if((pDataParams->wTargetMode & PHHAL_HW_RF_FRAMING_OPTION_MASK) == PHHAL_HW_RF_TYPE_ACTIVE_FRAMING)
            {
                /* For meeting Active Time Delay as per ISO18092, minimum delay is been introduced here to meet
                 * Tadt + Trfw + Tarfg (189.5us + 113us + 75.5us = 378us). Delay of only 150us is provided here
                 * as additional delay will be provided by HAL Wait. */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Wait(pDataParams, PHHAL_HW_TIME_MICROSECONDS, 120));

                /* During Mute conditions and if Target is in Active Mode then switch off the RF Field to receive next packet.
                 * Retrieve the content of the TxControl register. */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, &bRegister));
                /* Switch off the field */
                bRegister &= (uint8_t)~(uint8_t)(PHHAL_HW_RC523_BIT_TX1RFEN | PHHAL_HW_RC523_BIT_TX2RFEN);
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, bRegister));
            }

            /* Start the Transceive command in Target Mode as Transceive will first do Receive action and then Transmit
             * unlike what is said in Pn512 DataSheet. */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
                pDataParams,
                PHHAL_HW_RC523_REG_COMMAND,
                PHHAL_HW_RC523_CMD_TRANSCEIVE));
        }

        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, &bRegister));
        /* As in Pn512 during error scenarios in Active Target Mode, Modem status is not changed and is still in Receive state and also
         * no mechanism is available to detect if own RF Filed is switched ON. So by default after minimum delay Field OFF is been done. */
        if (((pDataParams->wTargetMode & PHHAL_HW_RF_FRAMING_OPTION_MASK) == PHHAL_HW_RF_TYPE_ACTIVE_FRAMING) &&
            ((pDataParams->bSkipSend == PH_ON) || ((bRegister & (PHHAL_HW_RC523_BIT_TX1RFEN | PHHAL_HW_RC523_BIT_TX2RFEN)) == (PHHAL_HW_RC523_BIT_TX1RFEN | PHHAL_HW_RC523_BIT_TX2RFEN))))
        {
            /* For meeting Active Time Delay as per ISO18092, minimum delay is been introduced here to meet
             * Tadt + Trfw + Tarfg (189.5us + 113us + 75.5us = 378us). Delay of only 150us is provided here
             * as additional delay will be provided by HAL Wait. */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Wait(pDataParams, PHHAL_HW_TIME_MICROSECONDS, 120));

            /* During Mute conditions and if Target is in Active Mode then switch off the RF Field to receive next packet.
             * Retrieve the content of the TxControl register. */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, &bRegister));
            /* Switch off the field */
            bRegister &= (uint8_t)~(uint8_t)(PHHAL_HW_RC523_BIT_TX1RFEN | PHHAL_HW_RC523_BIT_TX2RFEN);
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, bRegister));

            /* Start the Transceive command in Target Mode as Transceive will first do Receive action and then Transmit
             * unlike what is said in Pn512 DataSheet. */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
                pDataParams,
                PHHAL_HW_RC523_REG_COMMAND,
                PHHAL_HW_RC523_CMD_TRANSCEIVE));
        }

        /* Disable SKIP Send before any Receive Operation. */
        pDataParams->bSkipSend = PH_OFF;
    }
    else
    {
        /* Timer should be running, otherwise we'd wait forever.
         * This functionality is not required in Target Mode as Receive should wait until RF is ON. */
        /* Retrieve content of Status1 register to check Timer running Bit. */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS1, &bRegister));

        /* Check TRunning flag */
        if (!(bRegister & PHHAL_HW_RC523_BIT_TRUNNUNG))
        {
            return PH_ADD_COMPCODE(PH_ERR_IO_TIMEOUT, PH_COMP_HAL);
        }

        /* start the receive command */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
            pDataParams,
            PHHAL_HW_RC523_REG_COMMAND,
            PHHAL_HW_RC523_CMD_RECEIVE));
    }

    /* perform receive operation */
    status = phhalHw_Rc523_ExchangeReceive(pDataParams, ppRxBuffer, pRxLength);

    if (((status & PH_ERR_MASK) == PH_ERR_INTEGRITY_ERROR) &&
        (pDataParams->bActiveMode == PH_ON) &&
        (pDataParams->wTargetMode != PH_OFF))
    {
        /* Enable SKIP Send, to provide Active RF delay. */
        pDataParams->bSkipSend = PH_ON;
    }
    /* restore RxWait setting */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXSEL, bRegRxWait));

    return status;
}

phStatus_t phhalHw_Rc523_ApplyProtocolSettings(
                                               phhalHw_Rc523_DataParams_t * pDataParams,
                                               uint8_t bMode
                                               )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_COUNT wIndex;
    uint16_t *  PH_MEMLOC_REM pShadowDefault;
    uint16_t    PH_MEMLOC_REM wShadowCount = 0;
    uint8_t     PH_MEMLOC_REM bUseDefaultShadow;
    uint16_t    PH_MEMLOC_REM wConfig;
    uint8_t     PH_MEMLOC_REM bRegister;

    /* Store new card type */
    if (bMode != PHHAL_HW_CARDTYPE_CURRENT)
    {
        pDataParams->bCardType         = bMode;
        pDataParams->bTimeoutUnit      = PHHAL_HW_TIME_MICROSECONDS;
        bUseDefaultShadow              = 1;

        /* Initialize configuration shadow */
        memset(pDataParams->wCfgShadow, 0x00, PHHAL_HW_RC523_SHADOW_COUNT);  /* PRQA S 3200 */
    }
    else
    {
        bUseDefaultShadow = 0;
    }

    /* configure general settings */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODE, 0x00));

    /* Set TypeB register to default value. Found to impact Type A if it is not reset. */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TYPEB, 0));

    pDataParams->bActiveMode       = PH_OFF;
    pDataParams->wTargetMode       = PH_OFF;
    pDataParams->bJewelActivated   = PH_OFF;
    pDataParams->bNfcipMode        = PH_OFF;
    pDataParams->bSkipSend         = PH_OFF;

    /* configure reader IC for current card */
    switch (pDataParams->bCardType)
    {
        /* configure hardware for ISO14443A */
    case PHHAL_HW_CARDTYPE_ISO14443A:

        /* configure Tx path for ISO14443A */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, PHHAL_HW_RC523_BIT_MIFARE));
        /* configure Rx path for ISO14443A */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, PHHAL_HW_RC523_BIT_MIFARE));
        /* configure default TxModWidth */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODWIDTH, 0x26));
        /* configure the RxThreshold for ISO14443A */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_I14443A));
        /* Set initiator mode */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_CONTROL, PHHAL_HW_RC523_BIT_INITIATOR));

        /* Use 14443a default shadow */
        pShadowDefault = (uint16_t*)wRc523_DefaultShadow_I14443a;
        wShadowCount = sizeof(wRc523_DefaultShadow_I14443a) / (sizeof(uint16_t) * 2);
        if (pDataParams->bOpeMode == RD_LIB_MODE_ISO)
        {
            pDataParams->bEmdNoiseMaxDataLen = PHHAL_HW_RC523_ISO_NOISE_MAXLEN;
        }
        else
        {
            pDataParams->bEmdNoiseMaxDataLen = PHHAL_HW_RC523_EMV_NOISE_MAXLEN;
        }
        break;

        /* configure hardware for ISO14443B */
    case PHHAL_HW_CARDTYPE_ISO14443B:

        /* configure Tx path for ISO14443B */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, PHHAL_HW_RC523_BIT_TYPEB));
        /* configure Rx path for ISO14443B */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, PHHAL_HW_RC523_BIT_TYPEB));
        /* configure the RxThreshold for ISO14443B */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_I14443B));
        /* Set TypeB register to default */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TYPEB, PHHAL_HW_RC523_MASK_RXEOFREQ));
        /* Set initiator mode */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_CONTROL, PHHAL_HW_RC523_BIT_INITIATOR));

        /* Use 14443b shadow */
        pShadowDefault = (uint16_t*)wRc523_DefaultShadow_I14443b;
        wShadowCount = sizeof(wRc523_DefaultShadow_I14443b) / (sizeof(uint16_t) * 2);
        if (pDataParams->bOpeMode == RD_LIB_MODE_ISO)
        {
            /* Max EMD noise length is 2 bytes (excluding 2 bytes of CRC).
             * For Type B, RX last bits will not be updated in register. So
             * to manage noise error due to residual bits, max noise length
             * is set to 2 bytes (1 + 1 bytes). */
            pDataParams->bEmdNoiseMaxDataLen = PHHAL_HW_RC523_ISO_NOISE_MAXLEN + 1;
        }
        else
        {
            /* Max EMD noise length is 2 bytes (excluding 2 bytes of CRC).
             * For Type B, RX last bits will not be updated in register. So
             * to manage noise error due to residual bits, max noise length
             * is set to 3 bytes (2 + 1 bytes). */
            pDataParams->bEmdNoiseMaxDataLen = PHHAL_HW_RC523_EMV_NOISE_MAXLEN + 1;
        }
        break;

        /* configure hardware for Felica - 212kbps */
    case PHHAL_HW_CARDTYPE_FELICA_212:

        /* configure Tx path for Felica */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, PHHAL_HW_RC523_BIT_FELICA));
        /* configure Rx path for Felica */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, PHHAL_HW_RC523_BIT_FELICA));
        /* configure the RxThreshold for Felica  */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_FELICA));
        /* Set initiator mode */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_CONTROL, PHHAL_HW_RC523_BIT_INITIATOR));

        /* Use Felica shadow */
        pShadowDefault = (uint16_t*)wRc523_DefaultShadow_Felica;
        wShadowCount = sizeof(wRc523_DefaultShadow_Felica) / (sizeof(uint16_t) * 2);
        break;

        /* configure hardware for Felica - 424kbps */
    case PHHAL_HW_CARDTYPE_FELICA_424:

        /* configure Tx path for Felica */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, PHHAL_HW_RC523_BIT_FELICA));
        /* configure Rx path for Felica */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, PHHAL_HW_RC523_BIT_FELICA));
        /* configure the RxThreshold for Felica  */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_FELICA));
        /* Set initiator mode */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_CONTROL, PHHAL_HW_RC523_BIT_INITIATOR));

        /* Use Felica shadow */
        pShadowDefault = (uint16_t*)wRc523_DefaultShadow_Felica_424;
        wShadowCount = sizeof(wRc523_DefaultShadow_Felica_424) / (sizeof(uint16_t) * 2);
        break;

        /* configure hardware for I18092mA at 106 */
    case PHHAL_HW_CARDTYPE_I18092M_ACTIVE_106:

        /* Enable active mode */
        pDataParams->bActiveMode = PH_ON;
        /* configure Tx path for Active Initiator */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, PHHAL_HW_RC523_BIT_ACTIVE));
        /* configure Rx path for Active Initiator */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, PHHAL_HW_RC523_BIT_ACTIVE));
        /* configure default TxModWidth */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODWIDTH, 0x26));
        /* configure the RxThreshold for I18092mPI  */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_I14443A));
        /* Set initiator mode */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_CONTROL, PHHAL_HW_RC523_BIT_INITIATOR));

        /* Enable Detect Sync Bit */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_MODE, &bRegister));
        bRegister |= PHHAL_HW_RC523_BIT_DETECT_SYNC;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODE, bRegister));

        /* Its 106 */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXBITPHASE, 0x0F));

        /* Use I18092mPI shadow */
        pShadowDefault = (uint16_t*)wRc523_DefShadow_I18092m_Active;
        wShadowCount = sizeof(wRc523_DefShadow_I18092m_Active) / (sizeof(uint16_t) * 2);
        break;

        /* configure hardware for I18092mPI at 212 */
    case PHHAL_HW_CARDTYPE_I18092M_ACTIVE_212:

        /* Enable active mode */
        pDataParams->bActiveMode = PH_ON;
        /* configure Tx path for Active Initiator */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, PHHAL_HW_RC523_BIT_ACTIVE));
        /* configure Rx path for Active Initiator */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, PHHAL_HW_RC523_BIT_ACTIVE));
        /* configure default TxModWidth */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODWIDTH, 0x26));
        /* configure the RxThreshold for I18092mPI  */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_FELICA));
        /* Set initiator mode */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_CONTROL, PHHAL_HW_RC523_BIT_INITIATOR));

        /* Use I18092mPI 212 shadow */
        pShadowDefault = (uint16_t*)wRc523_DefShadow_I18092m_212_Active;
        wShadowCount = sizeof(wRc523_DefShadow_I18092m_212_Active) / (sizeof(uint16_t) * 2);
        break;

        /* configure hardware for I18092mPI at 424 */
    case PHHAL_HW_CARDTYPE_I18092M_ACTIVE_424:

        /* Enable active mode */
        pDataParams->bActiveMode = PH_ON;
        /* configure Tx path for Active Initiator */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, PHHAL_HW_RC523_BIT_ACTIVE));
        /* configure Rx path for Active Initiator */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, PHHAL_HW_RC523_BIT_ACTIVE));
        /* configure default TxModWidth */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODWIDTH, 0x26));
        /* configure the RxThreshold for I18092mPI  */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_FELICA));
        /* Set initiator mode */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_CONTROL, PHHAL_HW_RC523_BIT_INITIATOR));

        /* Use I18092mPI 424 shadow */
        pShadowDefault = (uint16_t*)wRc523_DefShadow_I18092m_Active_424;
        wShadowCount = sizeof(wRc523_DefShadow_I18092m_Active_424) / (sizeof(uint16_t) * 2);
        break;

    default:
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* configure the gain factor to 38dB for Initiator*/
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RFCFG, 0x59));

    /* configure the RxSel Register */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXSEL, 0x80));

    /* configure the conductance if no modulation is active */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_GSN, 0xFF));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_CWGSP, 0x3F));

    /* configure the conductance for LoadModulation */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_GSNOFF, PHHAL_HW_RC523_MODINDEX_TARGET));

    /* reset bit-framing register */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_BITFRAMING, 0x00));

    /* configure the timer */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TMODE, PHHAL_HW_RC523_BIT_TAUTO));

    /* configure the water level */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_WATERLEVEL, PHHAL_HW_RC523_FIFOSIZE - 8));

    /* Apply shadowed registers */
    for (wIndex = 0; wIndex < wShadowCount; ++wIndex)
    {
        /* Get wConfig */
        wConfig = pShadowDefault[wIndex << 1];

        /* Apply only one the correct timeout unit */
        if (!(((wConfig == PHHAL_HW_CONFIG_TIMEOUT_VALUE_US) &&
            (pDataParams->bTimeoutUnit != PHHAL_HW_TIME_MICROSECONDS)) ||
            ((wConfig == PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS) &&
            (pDataParams->bTimeoutUnit != PHHAL_HW_TIME_MILLISECONDS))))
        {
            /* Default shadow: */
            if (bUseDefaultShadow)
            {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, wConfig, pShadowDefault[(wIndex << 1) + 1]));
            }
            /* Current shadow: */
            else
            {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, wConfig, pDataParams->wCfgShadow[wConfig]));
            }
        }
    }

    /* MIFARE Crypto1 state is disabled by default */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams, PHHAL_HW_CONFIG_DISABLE_MF_CRYPTO1, PH_ON));

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_SetConfig(
                                   phhalHw_Rc523_DataParams_t * pDataParams,
                                   uint16_t wConfig,
                                   uint16_t wValue
                                   )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wIndex;
    uint8_t *   PH_MEMLOC_REM pBuffer;
    uint16_t    PH_MEMLOC_REM wBufferSize;
    uint16_t    PH_MEMLOC_REM wConfigShadow;
    uint16_t *  PH_MEMLOC_REM pShadowDefault = NULL;
    uint16_t    PH_MEMLOC_REM wBufferLen = 0;
    uint16_t    PH_MEMLOC_REM wFraming;
    uint8_t     PH_MEMLOC_REM bRegister;
    uint8_t     PH_MEMLOC_REM bValue;
    uint8_t     PH_MEMLOC_REM bModWidthReg;
    uint8_t     PH_MEMLOC_REM bModChange = 0;

    switch (wConfig)
    {
    case PHHAL_HW_CONFIG_OPE_MODE:

        pDataParams->bOpeMode = (uint8_t)wValue;
        if (pDataParams->bOpeMode == RD_LIB_MODE_EMVCO)
        {
            pDataParams->bEmdNoiseMaxDataLen = PHHAL_HW_RC523_EMV_NOISE_MAXLEN;
        }
        break;

    case PHHAL_HW_CONFIG_PARITY:

        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_MFRX, &bRegister));

        if (wValue == PH_OFF)
        {
            bValue = bRegister | PHHAL_HW_RC523_BIT_PARITYDISABLE;
        }
        else
        {
            bValue = bRegister & (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_PARITYDISABLE;
        }

        /* Only perform the operation, if the new value is different */
        if (bValue != bRegister)
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MFRX, bValue));
        }

        /* Write configuration data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        break;

    case PHHAL_HW_CONFIG_TXCRC:

        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, &bRegister));

        if (wValue == PH_OFF)
        {
            bValue = bRegister & (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_CRCEN;
        }
        else
        {
            bValue = bRegister | PHHAL_HW_RC523_BIT_CRCEN;
        }

        /* Only perform the operation, if the new value is different */
        if (bValue != bRegister)
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, bValue));
        }

        /* Write configuration data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        break;

    case PHHAL_HW_CONFIG_RXCRC:

        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, &bRegister));

        if (wValue == PH_OFF)
        {
            bValue = bRegister & (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_CRCEN;
        }
        else
        {
            bValue = bRegister | PHHAL_HW_RC523_BIT_CRCEN;
        }

        /* Only perform the operation, if the new value is different */
        if (bValue != bRegister)
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, bValue));
        }

        /* Write configuration data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        break;

    case PHHAL_HW_CONFIG_TXLASTBITS:

        /* check parameter */
        if (wValue > PHHAL_HW_RC523_MASK_TXBITS)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_BITFRAMING, &bRegister));
        bRegister &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_TXBITS;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_BITFRAMING, bRegister | (uint8_t)wValue));

        /* Write configuration data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        break;

    case PHHAL_HW_CONFIG_RXALIGN:

        /* check parameter */
        if (wValue > 7)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        /* adjust parameter */
        wValue = (uint16_t)(wValue << 4);

        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_BITFRAMING, &bRegister));
        bRegister &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_RXALIGN;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_BITFRAMING, bRegister |  (uint8_t)wValue));

        /* Write configuration data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        break;

    case PHHAL_HW_CONFIG_TXWAIT_US:

        /* Write configuration data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetFdt(pDataParams, PHHAL_HW_CONFIG_TXWAIT_US, wValue));

        break;

    case PHHAL_HW_CONFIG_TXWAIT_MS:

        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetFdt(pDataParams, PHHAL_HW_CONFIG_TXWAIT_MS, wValue));

        break;

    case PHHAL_HW_CONFIG_RXWAIT_US:

        /* Write configuration data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;

        /* Retrieve Rx-Datarate */
        wFraming = pDataParams->wCfgShadow[PHHAL_HW_CONFIG_RXDATARATE_FRAMING] & PHHAL_HW_RF_DATARATE_OPTION_MASK;
        /* update current data rate */
        switch (wFraming)
        {
        case PHHAL_HW_RF_DATARATE_106:
            wValue = (uint16_t)((float32_t)wValue/PHHAL_HW_RC523_ETU_106);
            break;
        case PHHAL_HW_RF_DATARATE_212:
            wValue = (uint16_t)((float32_t)wValue/PHHAL_HW_RC523_ETU_212);
            break;
        case PHHAL_HW_RF_DATARATE_424:
            wValue = (uint16_t)((float32_t)wValue/PHHAL_HW_RC523_ETU_424);
            break;
        case PHHAL_HW_RF_DATARATE_848:
            wValue = (uint16_t)((float32_t)wValue/PHHAL_HW_RC523_ETU_848);
            break;
        default:
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        /* Check if the Value can be set. */
        if (wValue > PHHAL_HW_RC523_MASK_RXWAIT)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_RXSEL, &bRegister));
        bRegister &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_RXWAIT;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXSEL, bRegister |  (uint8_t)wValue));
        break;

    case PHHAL_HW_CONFIG_TXDATARATE_FRAMING:

        /* Retrieve TxMode register */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, &bRegister));
        bValue = bRegister & (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_SPEED;

        wFraming = wValue & PHHAL_HW_RF_FRAMING_OPTION_MASK;
        wValue = wValue & PHHAL_HW_RF_DATARATE_OPTION_MASK;

        /* update current data rate */
        switch (wValue)
        {
        case PHHAL_HW_RF_DATARATE_106:
            bValue |= PHHAL_HW_RC523_BIT_106KBPS;
            bModWidthReg = 0x26;
            break;
        case PHHAL_HW_RF_DATARATE_212:
            bValue |= PHHAL_HW_RC523_BIT_212KBPS;
            bModWidthReg = 0x15;
            break;
        case PHHAL_HW_RF_DATARATE_424:
            bValue |= PHHAL_HW_RC523_BIT_424KBPS;
            bModWidthReg = 0x0A;
            break;
        case PHHAL_HW_RF_DATARATE_848:
            bValue |= PHHAL_HW_RC523_BIT_848KBPS;
            bModWidthReg = 0x05;
            break;
        default:
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        /* Check if Target is activated and perform required change in Register to switch BaudRate. */
        if (pDataParams->wTargetMode != PH_OFF)
        {
            if (wFraming == PHHAL_HW_RF_TYPE_A_FRAMING)
            {
                /* If Target is Activated in Card Mode (ISO14443) then no Shadow needs to be updated. */
                if ((pDataParams->wTargetMode & PHHAL_HW_RF_FRAMING_OPTION_MASK) != PHHAL_HW_RF_TYPE_A_FRAMING)
                {
                    /* TypeA mode:
                     * If the Card mode changes, we need to update the configuration */
                    if (wValue != (bRegister & PHHAL_HW_RC523_MASK_SPEED))
                    {
                        /* Update RxThreshold*/
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_I14443A));

                        /* Update Framing */
                        bValue &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_FRAMING;
                        bValue |= PHHAL_HW_RC523_BIT_MIFARE;

                        /* Set shadow */
                        pShadowDefault = (uint16_t*)wRc523_DepShadow_I14443a;
                        wBufferLen = sizeof(wRc523_DepShadow_I14443a) / (sizeof(uint16_t) * 2);
                    }
                    bModChange = 1;
                }
            }
            /* Felica Mode: 212kbit/s or 424kbit/s */
            else if(wFraming == PHHAL_HW_RF_TYPE_F_FRAMING)
            {
                if((wValue != PHHAL_HW_RF_DATARATE_106) && (wValue < PHHAL_HW_RF_DATARATE_848))
                {
                    /* If the Card mode changes, we need to update the registers */
                    if ((bRegister & PHHAL_HW_RC523_MASK_SPEED) == PHHAL_HW_RF_DATARATE_106)
                    {
                        /* Update RxThreshold*/
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_FELICA));

                        /* Update Framing */
                        bValue &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_FRAMING;
                        bValue |= PHHAL_HW_RC523_BIT_FELICA;

                        /* Set shadow */
                        pShadowDefault = (uint16_t*)wRc523_DefaultShadow_Felica;
                        wBufferLen = sizeof(wRc523_DefaultShadow_Felica) / (sizeof(uint16_t) * 2);

                        bModChange = 1;
                    }
                }
            }
            else if(wFraming == PHHAL_HW_RF_TYPE_ACTIVE_FRAMING)
            {
                if (wValue != PHHAL_HW_RF_DATARATE_106)
                {
                    /* Update Framing */
                    bValue &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_FRAMING;
                    bValue |= PHHAL_HW_RC523_BIT_ACTIVE;

                    /* Set shadow */
                    pShadowDefault = (uint16_t*)wRc523_DefaultShadow_I18092mPT;
                    wBufferLen = sizeof(wRc523_DefaultShadow_I18092mPT) / (sizeof(uint16_t) * 2);

                    bModChange = 1;
                }
                else
                {
                    /* Update Framing */
                    bValue &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_FRAMING;
                    bValue |= PHHAL_HW_RC523_BIT_ACTIVE;

                    /* Set shadow */
                    pShadowDefault = (uint16_t*)wRc523_DepShadow_I14443a;
                    wBufferLen = sizeof(wRc523_DepShadow_I14443a) / (sizeof(uint16_t) * 2);
                }
            }
            else
            {
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
            }
            pDataParams->wTargetMode = wFraming | wValue;
        }
        else
        {
            /* protocol specific handling */
            switch (pDataParams->bCardType)
            {
                /* Felica */
            case PHHAL_HW_CARDTYPE_FELICA_212:
            case PHHAL_HW_CARDTYPE_FELICA_424:
                /* Switch to Type A if received speed change is 106kbps */
                if(wFraming == PHHAL_HW_RF_TYPE_A_FRAMING)
                {
                    if (wValue == PHHAL_HW_RF_DATARATE_106)
                    {
                        /* If the Card mode changes, we need to update the configuration */
                        if ((bRegister & PHHAL_HW_RC523_MASK_SPEED) != wValue)
                        {
                            /* Update RxThreshold*/
                            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_I14443A));

                            /* Update Framing */
                            bValue &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_FRAMING;
                            bValue |= PHHAL_HW_RC523_BIT_MIFARE;

                            /* Set shadow */
                            pShadowDefault = (uint16_t*)wRc523_DepShadow_I14443a;
                            wBufferLen = sizeof(wRc523_DepShadow_I14443a) / (sizeof(uint16_t) * 2);
                        }
                    }
                }
                else if(wFraming == PHHAL_HW_RF_TYPE_F_FRAMING)
                {
                    /* Change speed if received speed change is 212kbps or 424kbps */
                    if ((wValue != PHHAL_HW_RF_DATARATE_212) && (wValue != PHHAL_HW_RF_DATARATE_424))
                    {
                        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
                    }
                }
                else
                {
                    /* Invalid Scenario */
                    return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
                }
                break;
                /* Mifare */
            case PHHAL_HW_CARDTYPE_ISO14443A:
                /* Switch to Type F if received framing is Type-F and speed change is either 212kbps or 414kbps */
                if(wFraming == PHHAL_HW_RF_TYPE_F_FRAMING)
                {
                    /* Switch to Felica Mode: 212kbit/s or 424kbit/s */
                    if((wValue == PHHAL_HW_RF_DATARATE_212) || (wValue == PHHAL_HW_RF_DATARATE_424))
                    {
                        /* If the Card mode changes, we need to update the registers.
                        * Check the current configured BAUDRATE */
                        if ((bRegister & PHHAL_HW_RC523_MASK_SPEED) == PHHAL_HW_RF_DATARATE_106)
                        {
                            /* Update RxThreshold*/
                            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_FELICA));

                            /* Update Framing */
                            bValue &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_FRAMING;
                            bValue |= PHHAL_HW_RC523_BIT_FELICA;

                            /* Set shadow */
                            pShadowDefault = (uint16_t*)wRc523_DefaultShadow_Felica;
                            wBufferLen = sizeof(wRc523_DefaultShadow_Felica) / (sizeof(uint16_t) * 2);
                        }
                    }
                }
                break;
                /* I18000p3m3 */
            case PHHAL_HW_CARDTYPE_I18092M_ACTIVE_106:
            case PHHAL_HW_CARDTYPE_I18092M_ACTIVE_212:
            case PHHAL_HW_CARDTYPE_I18092M_ACTIVE_424:
                /* If the Speed changes, we need to update the registers.
                * Check the current configured BAUDRATE */
                if (((bRegister & PHHAL_HW_RC523_MASK_SPEED) >> PHHAL_HW_RC523_SHIFT_SPEED) != wValue)
                {
                    if (wValue == PHHAL_HW_RF_DATARATE_106)
                    {
                        /* Update RxThreshold*/
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_I14443A));

                        /* Set shadow */
                        pShadowDefault = (uint16_t*)wRc523_DefShadow_I18092m_Active;
                        wBufferLen = sizeof(wRc523_DefShadow_I18092m_Active) / (sizeof(uint16_t) * 2);
                    }
                    else if((wValue == PHHAL_HW_RF_DATARATE_212) || (wValue == PHHAL_HW_RF_DATARATE_424))
                    {
                        /* Update RxThreshold*/
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_FELICA));

                        /* Set shadow */
                        pShadowDefault = (uint16_t*)wRc523_DefShadow_I18092m_212_Active;
                        wBufferLen = sizeof(wRc523_DefShadow_I18092m_212_Active) / (sizeof(uint16_t) * 2);
                    }
                    else
                    {
                        /* Invalid Scenario */
                        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
                    }
                }
                break;
                /* Others */
            default:
                break;
            }
        }

        /* Also Update registers from default shadow */
        if (pShadowDefault != NULL)
        {
            for (wIndex = 0; wIndex < wBufferLen; ++wIndex)
            {
                /* Get wConfigShadow */
                wConfigShadow = pShadowDefault[wIndex << 1];

                /* Apply configuration */
                if ((wConfigShadow != PHHAL_HW_CONFIG_TXDATARATE_FRAMING) &&
                    (wConfigShadow != PHHAL_HW_CONFIG_RXDATARATE_FRAMING) &&
                    (wConfigShadow != PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS))
                {
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams, wConfigShadow, pShadowDefault[(wIndex << 1) + 1]));
                }
            }
        }

        /* Only apply the settings if the speed has changed */
        if (bValue != bRegister)
        {
            /* Apply data rate */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, bValue));

            /* Configure the modulation width based on the Speed */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODWIDTH, bModWidthReg));
        }

        /* Write configuration data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue | wFraming;

        /* When switching between technologies during PSL exchange to move to higher baudrates, in Pn512 Command register should
        * be loaded with IDLE Command and then TRANSCEIVE Command should loaded back to reflect the change in Register settings. */
        if((pDataParams->wTargetMode != PH_OFF) && (bModChange == 1))
        {
            if (wValue != PHHAL_HW_RF_DATARATE_106)
            {
                /* Configure the modulation width register */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODWIDTH, 0x26));

                if (pDataParams->bActiveMode == PH_ON)
                {
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_MODE, &bRegister));
                    bRegister &= (uint8_t)~(uint8_t) PHHAL_HW_RC523_BIT_DETECT_SYNC;
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODE, bRegister));
                }
            }
            else
            {
                if (pDataParams->bActiveMode == PH_ON)
                {
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_MODE, &bRegister));
                    bRegister |= PHHAL_HW_RC523_BIT_DETECT_SYNC;
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODE, bRegister));
                }
            }

            /* Load IDLE command. */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_IDLE));
            /* Load TRANSCEIVE command back. */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_TRANSCEIVE));

            if ((pDataParams->bActiveMode == PH_ON) && (wValue != PHHAL_HW_RF_DATARATE_106))
            {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, &bRegister));
                bRegister |= PHHAL_HW_RC523_BIT_TX2RFEN | PHHAL_HW_RC523_BIT_TX1RFEN;
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, bRegister));
            }
        }
        break; /* PHHAL_HW_CONFIG_TXDATARATE_FRAMING */

    case PHHAL_HW_CONFIG_RXDATARATE_FRAMING:

        /* Retrieve RxMode register */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, &bRegister));
        bValue = bRegister & (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_SPEED;

        wFraming = wValue & PHHAL_HW_RF_FRAMING_OPTION_MASK;
        wValue = wValue & PHHAL_HW_RF_DATARATE_OPTION_MASK;

        /* update current data rate */
        switch (wValue)
        {
        case PHHAL_HW_RF_DATARATE_106:
            bValue |= PHHAL_HW_RC523_BIT_106KBPS;
            break;
        case PHHAL_HW_RF_DATARATE_212:
            bValue |= PHHAL_HW_RC523_BIT_212KBPS;
            break;
        case PHHAL_HW_RF_DATARATE_424:
            bValue |= PHHAL_HW_RC523_BIT_424KBPS;
            break;
        case PHHAL_HW_RF_DATARATE_848:
            bValue |= PHHAL_HW_RC523_BIT_848KBPS;
            break;
        default:
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        /* Check if Target is activated and perform required change in Register to switch BaudRate. */
        if(pDataParams->wTargetMode != PH_OFF)
        {
            /* TypeA mode: 106kbit/s */
            if(wFraming == PHHAL_HW_RF_TYPE_A_FRAMING)
            {
                /* If Target is Activated in Card Mode (ISO14443) then no Shadow needs to be updated. */
                if ((pDataParams->wTargetMode & PHHAL_HW_RF_FRAMING_OPTION_MASK) != PHHAL_HW_RF_TYPE_A_FRAMING)
                {
                    /* TypeA mode:
                     * If the Card mode changes, we need to update the configuration */
                    if (wValue != (bRegister & PHHAL_HW_RC523_MASK_SPEED))
                    {
                        /* Update RxThreshold*/
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_I14443A));

                        /* Update Framing */
                        bValue &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_FRAMING;
                        bValue |= PHHAL_HW_RC523_BIT_MIFARE;

                        /* Set values apart from common bits that are applicable to only Rx Framing. */
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXCRC, PH_ON));
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_RXCRC, PH_ON));
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_RXWAIT_US, 76));
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, PHHAL_HW_RC523_DEFAULT_TIMEOUT));
                    }
                }
            }
            /* Felica Mode: 212kbit/s or 424kbit/s */
            else if(wFraming == PHHAL_HW_RF_TYPE_F_FRAMING)
            {
                if((wValue != PHHAL_HW_RF_DATARATE_106) && (wValue < PHHAL_HW_RF_DATARATE_848))
                {
                    /* If the Card mode changes, we need to update the registers */
                    if ((bRegister & PHHAL_HW_RC523_MASK_SPEED) == PHHAL_HW_RF_DATARATE_106)
                    {
                        /* Update RxThreshold*/
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_FELICA));

                        /* Update Framing */
                        bValue &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_FRAMING;
                        bValue |= PHHAL_HW_RC523_BIT_FELICA;

                        /* Set values apart from common bits that are applicable to only Rx Framing. */
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXCRC, PH_ON));
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_RXCRC, PH_ON));
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXWAIT_US, 730));
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_RXWAIT_US, 15));
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, PHHAL_HW_RC523_DEFAULT_TIMEOUT));
                    }
                }
            }
            else if(wFraming == PHHAL_HW_RF_TYPE_ACTIVE_FRAMING)
            {
                if (wValue != PHHAL_HW_RF_DATARATE_106)
                {
                    /* Update Framing */
                    bValue &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_FRAMING;
                    bValue |= PHHAL_HW_RC523_BIT_ACTIVE;

                    /* Set values apart from common bits that are applicable to only Rx Framing. */
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXCRC, PH_ON));
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_RXCRC, PH_ON));
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, PHHAL_HW_RC523_DEFAULT_TIMEOUT));
                }
                else
                {
                    /* Update Framing */
                    bValue &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_FRAMING;
                    bValue |= PHHAL_HW_RC523_BIT_ACTIVE;

                    /* Set values apart from common bits that are applicable to only Rx Framing. */
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXCRC, PH_ON));
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_RXCRC, PH_ON));
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, PHHAL_HW_RC523_DEFAULT_TIMEOUT));
                }
            }
            else
            {
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
            }
        }
        else
        {
            /* protocol specific handling */
            switch (pDataParams->bCardType)
            {
                /* Felica */
            case PHHAL_HW_CARDTYPE_FELICA_212:
            case PHHAL_HW_CARDTYPE_FELICA_424:
                /* Switch to Type A if received speed change is 106kbps */
                if(wFraming == PHHAL_HW_RF_TYPE_A_FRAMING)
                {
                    if (wValue == PHHAL_HW_RF_DATARATE_106)
                    {
                        /* If the Card mode changes, we need to update the configuration */
                        if ((bRegister & PHHAL_HW_RC523_MASK_SPEED) != wValue)
                        {
                            /* Update RxThreshold*/
                            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_I14443A));

                            /* Update Framing */
                            bValue &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_FRAMING;
                            bValue |= PHHAL_HW_RC523_BIT_MIFARE;

                            /* Set shadow */
                            pShadowDefault = (uint16_t*)wRc523_DepShadow_I14443a;
                            wBufferLen = sizeof(wRc523_DepShadow_I14443a) / (sizeof(uint16_t) * 2);
                        }
                    }
                }
                else if(wFraming == PHHAL_HW_RF_TYPE_F_FRAMING)
                {
                    /* Change speed if received speed change is 212kbps or 424kbps */
                    if ((wValue != PHHAL_HW_RF_DATARATE_212) && (wValue != PHHAL_HW_RF_DATARATE_424))
                    {
                        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
                    }
                }
                else
                {
                    /* Invalid Scenario */
                    return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
                }
                break;
            case PHHAL_HW_CARDTYPE_ISO14443A:
                /* Switch to Type F if received framing is Type-F and speed change is either 212kbps or 414kbps */
                if(wFraming == PHHAL_HW_RF_TYPE_F_FRAMING)
                {
                    /* Switch to Felica Mode: 212kbit/s or 424kbit/s */
                    if((wValue == PHHAL_HW_RF_DATARATE_212) || (wValue == PHHAL_HW_RF_DATARATE_424))
                    {
                        /* If the Card mode changes, we need to update the registers.
                        * Check the current configured BAUDRATE */
                        if ((bRegister & PHHAL_HW_RC523_MASK_SPEED) == PHHAL_HW_RF_DATARATE_106)
                        {
                            /* Update RxThreshold*/
                            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_FELICA));

                            /* Update Framing */
                            bValue &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_FRAMING;
                            bValue |= PHHAL_HW_RC523_BIT_FELICA;

                            /* Set shadow */
                            pShadowDefault = (uint16_t*)wRc523_DefaultShadow_Felica;
                            wBufferLen = sizeof(wRc523_DefaultShadow_Felica) / (sizeof(uint16_t) * 2);
                        }
                    }
                }
                break;
                /* I18000p3m3 */
            case PHHAL_HW_CARDTYPE_I18092M_ACTIVE_106:
            case PHHAL_HW_CARDTYPE_I18092M_ACTIVE_212:
            case PHHAL_HW_CARDTYPE_I18092M_ACTIVE_424:
                /* If the Speed changes, we need to update the registers.
                * Check the current configured BAUDRATE */
                if (((bRegister & PHHAL_HW_RC523_MASK_SPEED) >> PHHAL_HW_RC523_SHIFT_SPEED) != wValue)
                {
                    if (wValue == PHHAL_HW_RF_DATARATE_106)
                    {
                        /* Update RxThreshold*/
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_I14443A));

                        /* Set shadow */
                        pShadowDefault = (uint16_t*)wRc523_DefShadow_I18092m_Active;
                        wBufferLen = sizeof(wRc523_DefShadow_I18092m_Active) / (sizeof(uint16_t) * 2);

                        if (pDataParams->bActiveMode == PH_ON)
                        {
                            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_MODE, &bRegister));
                            bRegister |= PHHAL_HW_RC523_BIT_DETECT_SYNC;
                            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODE, bRegister));
                        }
                    }
                    else if((wValue == PHHAL_HW_RF_DATARATE_212) || (wValue == PHHAL_HW_RF_DATARATE_424))
                    {
                        /* Update RxThreshold*/
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_FELICA));

                        /* Set shadow */
                        pShadowDefault = (uint16_t*)wRc523_DefShadow_I18092m_212_Active;
                        wBufferLen = sizeof(wRc523_DefShadow_I18092m_212_Active) / (sizeof(uint16_t) * 2);

                        if (pDataParams->bActiveMode == PH_ON)
                        {
                            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_MODE, &bRegister));
                            bRegister &= (uint8_t)~(uint8_t) PHHAL_HW_RC523_BIT_DETECT_SYNC;
                            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODE, bRegister));
                        }
                    }
                    else
                    {
                        /* Invalid Scenario */
                        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
                    }
                }
                break;
                /* Others */
            default:
                break;
            }
        }

        /* Also Update registers from default shadow */
        if (pShadowDefault != NULL)
        {
            for (wIndex = 0; wIndex < wBufferLen; ++wIndex)
            {
                /* Get wConfigShadow */
                wConfigShadow = pShadowDefault[wIndex << 1];

                /* Apply configuration */
                if ((wConfigShadow != PHHAL_HW_CONFIG_TXDATARATE_FRAMING) &&
                    (wConfigShadow != PHHAL_HW_CONFIG_RXDATARATE_FRAMING) &&
                    (wConfigShadow != PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS))
                {
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams, wConfigShadow, pShadowDefault[(wIndex << 1) + 1]));
                }
            }
        }

        /* Apply data rate */
        if (bValue != bRegister)
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, bValue));
        }

        /* Write configuration data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue | wFraming;
        break; /* PHHAL_HW_CONFIG_RXDATARATE_FRAMING */

    case PHHAL_HW_CONFIG_MODINDEX:

        if (wValue & (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_MODGSP)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        /* Set ModGsP register */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODGSP, (uint8_t)wValue));

        /* Write configuration data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        break;

    case PHHAL_HW_CONFIG_ASK100:

        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, &bRegister));
        /* switch off 100% ASK */
        if (wValue == PH_OFF)
        {
            bRegister &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_FORCE100ASK;
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, bRegister));
        }
        /* switch on 100% ASK */
        else
        {
            bRegister |= PHHAL_HW_RC523_BIT_FORCE100ASK;
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, bRegister));
        }

        /* Write configuration data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        break;

    case PHHAL_HW_CONFIG_TIMEOUT_VALUE_US:
    case PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS:

        /* Calculate values for Microsecond values */
        if (wConfig == PHHAL_HW_CONFIG_TIMEOUT_VALUE_US)
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetFdt(pDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, wValue));
            pDataParams->bTimeoutUnit = PHHAL_HW_TIME_MICROSECONDS;
        }
        /* Calculate values for Millisecond values */
        else
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetFdt(pDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS, wValue));
            pDataParams->bTimeoutUnit = PHHAL_HW_TIME_MILLISECONDS;
        }

        /* Write configuration data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        break;

    case PHHAL_HW_CONFIG_TIMING_MODE:

        /* Check supported option bits */
        switch (wValue & PHHAL_HW_TIMING_MODE_OPTION_MASK)
        {
        case PHHAL_HW_TIMING_MODE_OPTION_DEFAULT:
        case PHHAL_HW_TIMING_MODE_OPTION_AUTOCLEAR:
            break;
        default:
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        /* Check supported timing modes */
        switch (wValue & (uint16_t)~(uint16_t)PHHAL_HW_TIMING_MODE_OPTION_MASK)
        {
        case PHHAL_HW_TIMING_MODE_OFF:
        case PHHAL_HW_TIMING_MODE_FDT:
            pDataParams->dwTimingUs = 0;
            pDataParams->wTimingMode = wValue;
            break;
        case PHHAL_HW_TIMING_MODE_COMM:
            return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_PARAMETER, PH_COMP_HAL);
        default:
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        break;

    case PHHAL_HW_CONFIG_FIELD_OFF_TIME:

        /* Parameter Check */
        if (wValue == 0)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        /* Store configuration data */
        pDataParams->wFieldOffTime = wValue;
        break;

    case PHHAL_HW_CONFIG_FIELD_RECOVERY_TIME:

        /* Store configuration data */
        pDataParams->wFieldRecoveryTime = wValue;
        break;

    case PHHAL_HW_CONFIG_JEWEL_MODE:
        if((wValue != PH_ON) && (wValue != PH_OFF))
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
        /* Write configuration data into shadow */
        pDataParams->bJewelActivated = (uint8_t)wValue;
        break;

    case PHHAL_HW_CONFIG_DISABLE_MF_CRYPTO1:

        /* Disable crypto, enabling is not supported */
        if (wValue != PH_OFF)
        {
            /* Retrieve Status2 register */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS2, &bRegister));

            /* Clear Crypto1On bit */
            bValue = bRegister & (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_CRYPTO1ON;

            /* Only perform the operation, if the new value is different */
            if (bValue != bRegister)
            {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS2, bValue));
            }
        }
        break;

    case PHHAL_HW_CONFIG_TXBUFFER_OFFSET:

        /* Modify additional info parameter */
        pDataParams->wAdditionalInfo = wValue;

        if ((pDataParams->bNfcipMode == PH_ON) && (pDataParams->bActiveMode == PH_OFF))
        {
            if ((pDataParams->wCfgShadow[PHHAL_HW_CONFIG_TXDATARATE_FRAMING] & PHHAL_HW_RF_DATARATE_OPTION_MASK) == PHHAL_HW_RF_DATARATE_106)
            {
                pDataParams->wAdditionalInfo += 1;
            }
        }
        break;

    case PHHAL_HW_CONFIG_RXBUFFER_STARTPOS:

        /* Boundary check */
        if (wValue >= pDataParams->wRxBufSize)
        {
            return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
        }

        /* Set start position */
        pDataParams->wRxBufStartPos = wValue;
        break;

    case PHHAL_HW_CONFIG_TXBUFFER_LENGTH:

        /* Retrieve TxBuffer parameters */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetTxBuffer(pDataParams, PH_ON, &pBuffer, &wBufferLen, &wBufferSize));

        if ((pDataParams->bNfcipMode == PH_ON) && (wValue > 0) && (pDataParams->bActiveMode == PH_OFF))
        {
            if ((pDataParams->wCfgShadow[PHHAL_HW_CONFIG_TXDATARATE_FRAMING] & PHHAL_HW_RF_DATARATE_OPTION_MASK) == PHHAL_HW_RF_DATARATE_106)
            {
                wValue += 1;
            }
        }

        /* Check parameter, must not exceed TxBufferSize */
        if (wValue > wBufferSize)
        {
            return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
        }

        /* set buffer length */
        pDataParams->wTxBufLen = wValue;
        break;

    case PHHAL_HW_CONFIG_TXBUFFER:

        /* Retrieve TxBuffer parameters */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetTxBuffer(pDataParams, PH_ON, &pBuffer, &wBufferLen, &wBufferSize));

        /* Check parameter, must not exceed TxBufferSize */
        if (pDataParams->wAdditionalInfo >= wBufferSize)
        {
            return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
        }

        /* Modify TxBuffer byte */
        pBuffer[pDataParams->wAdditionalInfo] = (uint8_t)wValue;
        break;

    case PHHAL_HW_CONFIG_MAX_PRECACHED_BYTES:

        if (wValue > PHHAL_HW_RC523_FIFOSIZE)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        /* Store parameter*/
        pDataParams->wMaxPrecachedBytes = wValue;
        break;

    case PHHAL_HW_CONFIG_BAL_CONNECTION:

        /* parameter check */
        if (wValue > PHHAL_HW_BAL_CONNECTION_I2C)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        pDataParams->bBalConnectionType = (uint8_t)wValue;
        break;

    case PHHAL_HW_CONFIG_SERIAL_BITRATE:

        switch (wValue)
        {
        case PHHAL_HW_RS232_BITRATE_9600:
            bValue = PHHAL_HW_RC523_SERIALSPEED_9600;
            break;
        case PHHAL_HW_RS232_BITRATE_19200:
            bValue = PHHAL_HW_RC523_SERIALSPEED_19200;
            break;
        case PHHAL_HW_RS232_BITRATE_38400:
            bValue = PHHAL_HW_RC523_SERIALSPEED_38400;
            break;
        case PHHAL_HW_RS232_BITRATE_57600:
            bValue = PHHAL_HW_RC523_SERIALSPEED_57600;
            break;
        case PHHAL_HW_RS232_BITRATE_115200:
            bValue = PHHAL_HW_RC523_SERIALSPEED_115200;
            break;
        case PHHAL_HW_RS232_BITRATE_230400:
            bValue = PHHAL_HW_RC523_SERIALSPEED_230400;
            break;
        case PHHAL_HW_RS232_BITRATE_460800:
            bValue = PHHAL_HW_RC523_SERIALSPEED_460800;
            break;
        default:
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        /* Set the register value */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_SERIALSPEED, bValue));
        break;

    case PHHAL_HW_CONFIG_RFRESET_ON_TIMEOUT:

        if (wValue == PH_OFF)
        {
            pDataParams->bRfResetAfterTo = PH_OFF;
        }
        else
        {
            pDataParams->bRfResetAfterTo = PH_ON;
        }
        break;

    case PHHAL_HW_CONFIG_SETMINFDT:

        statusTmp = phhalHw_Rc523_SetMinFDT(pDataParams, wValue); /*QAC_Fix: Rule QL3:MISRA.16.10 */
        break;

    case PHHAL_HW_CONFIG_RXMULTIPLE:

        if (wValue == PH_ON)
        {
            PH_CHECK_SUCCESS_FCT(statusTmp,
                phhalHw_Rc523_ReadRegister(
                pDataParams,
                PHHAL_HW_RC523_REG_RXMODE,
                &bRegister));
            bRegister |= PHHAL_HW_RC523_BIT_RXMULTIPLE;

            PH_CHECK_SUCCESS_FCT(statusTmp,
                phhalHw_Rc523_WriteRegister(
                pDataParams,
                PHHAL_HW_RC523_REG_RXMODE,
                bRegister
                ));

            pDataParams->bRxMultiple = PH_ON;
        }
        else
        {
            pDataParams->bRxMultiple = PH_OFF;
            PH_CHECK_SUCCESS_FCT(statusTmp,
                phhalHw_Rc523_ReadRegister(
                pDataParams,
                PHHAL_HW_RC523_REG_RXMODE,
                &bRegister));
            bRegister = bRegister & (uint8_t)(~(uint8_t)PHHAL_HW_RC523_BIT_RXMULTIPLE);
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
                pDataParams,
                PHHAL_HW_RC523_REG_RXMODE,
                bRegister
                ));
        }
        break;

    case PHHAL_HW_CONFIG_MFHALTED:
        if((wValue != PH_ON) && (wValue != PH_OFF))
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        PH_CHECK_SUCCESS_FCT(statusTmp,
            phhalHw_Rc523_ReadRegister(
            pDataParams,
            PHHAL_HW_RC523_REG_MFTX,
            &bRegister));

        if(wValue == PH_ON)
        {
            bRegister |= PHHAL_HW_RC523_BIT_MFHALTED;
        }
        else
        {
            bRegister = bRegister & (uint8_t)(~(uint8_t)PHHAL_HW_RC523_BIT_MFHALTED);
        }

        PH_CHECK_SUCCESS_FCT(statusTmp,
            phhalHw_Rc523_WriteRegister(
            pDataParams,
            PHHAL_HW_RC523_REG_MFTX,
            bRegister));
        break;

    case PHHAL_HW_CONFIG_RFCA:
        if((wValue != PH_ON) && (wValue != PH_OFF))
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
        pDataParams->bRfca = (uint8_t)wValue;
        break;

    case PHHAL_HW_CONFIG_RFON_INTERRUPT:
        if((wValue != PH_ON) && (wValue != PH_OFF))
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        PH_CHECK_SUCCESS_FCT(statusTmp,
            phhalHw_Rc523_ReadRegister(
            pDataParams,
            PHHAL_HW_RC523_REG_DIVIEN,
            &bRegister));

        if(wValue == PH_ON)
        {
            /* Clear all other IRQ1 interrupt sources except RF-ON */
            bRegister &= PHHAL_HW_RC523_BIT_IRQPUSHPULL;
            bRegister |= PHHAL_HW_RC523_BIT_EXT_RF_ON;
            PH_CHECK_SUCCESS_FCT(statusTmp,
                phhalHw_Rc523_WriteRegister(
                pDataParams,
                PHHAL_HW_RC523_REG_DIVIEN,
                bRegister));

            /* Clear IRQ0 interrupt sources */
            PH_CHECK_SUCCESS_FCT(statusTmp,
                phhalHw_Rc523_ReadRegister(
                pDataParams,
                PHHAL_HW_RC523_REG_COMMIEN,
                &bRegister));
            bRegister &= PHHAL_HW_RC523_BIT_IRQINV;
            PH_CHECK_SUCCESS_FCT(statusTmp,
                phhalHw_Rc523_WriteRegister(
                pDataParams,
                PHHAL_HW_RC523_REG_COMMIEN,
                bRegister));
        }
        else
        {
            bRegister = bRegister & (uint8_t)(~(uint8_t)PHHAL_HW_RC523_BIT_EXT_RF_ON);
            PH_CHECK_SUCCESS_FCT(statusTmp,
                phhalHw_Rc523_WriteRegister(
                pDataParams,
                PHHAL_HW_RC523_REG_DIVIEN,
                bRegister));
        }
        break;

    case PHHAL_HW_CONFIG_NFCIP_STARTBYTE:
        if((wValue != PH_ON) && (wValue != PH_OFF))
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
        pDataParams->bNfcipMode = (uint8_t)wValue;
        break;

    case PHHAL_HW_CONFIG_SET_EMD:
        if((wValue != PH_ON) && (wValue != PH_OFF))
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
        pDataParams->bCheckEmdErr = (uint8_t)wValue;
        break;

    default:
        return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_PARAMETER, PH_COMP_HAL);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_GetConfig(
                                   phhalHw_Rc523_DataParams_t * pDataParams,
                                   uint16_t wConfig,
                                   uint16_t * pValue
                                   )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bRegister;
    uint8_t *   PH_MEMLOC_REM pBuffer;
    uint16_t    PH_MEMLOC_REM wBufferLen;
    uint16_t    PH_MEMLOC_REM wBufferSize;

    switch (wConfig)
    {
    case PHHAL_HW_CONFIG_OPE_MODE:
        *pValue = pDataParams->bOpeMode;
        break;

    case PHHAL_HW_CONFIG_PARITY:
    case PHHAL_HW_CONFIG_TXCRC:
    case PHHAL_HW_CONFIG_RXCRC:
    case PHHAL_HW_CONFIG_TXLASTBITS:
        /* Read configuration from shadow */
        *pValue = pDataParams->wCfgShadow[wConfig];
        break;

    case PHHAL_HW_CONFIG_TXBUFFER_OFFSET:
    case PHHAL_HW_CONFIG_RXLASTBITS:

        *pValue = pDataParams->wAdditionalInfo;
        break;

    case PHHAL_HW_CONFIG_RXWAIT_US:
    case PHHAL_HW_CONFIG_TXWAIT_US:
    case PHHAL_HW_CONFIG_TXDATARATE_FRAMING:
    case PHHAL_HW_CONFIG_RXDATARATE_FRAMING:
    case PHHAL_HW_CONFIG_MODINDEX:
    case PHHAL_HW_CONFIG_ASK100:
        /* Read configuration from shadow */
        *pValue = pDataParams->wCfgShadow[wConfig];
        break;

    case PHHAL_HW_CONFIG_TIMEOUT_VALUE_US:

        if (pDataParams->bTimeoutUnit == PHHAL_HW_TIME_MICROSECONDS)
        {
            *pValue = pDataParams->wCfgShadow[wConfig];
        }
        else
        {
            if (pDataParams->wCfgShadow[PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS] > (0xFFFF / 1000))
            {
                return PH_ADD_COMPCODE(PH_ERR_PARAMETER_OVERFLOW, PH_COMP_HAL);
            }
            *pValue = pDataParams->wCfgShadow[PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS] * 1000;
        }
        break;

    case PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS:

        if (pDataParams->bTimeoutUnit == PHHAL_HW_TIME_MILLISECONDS)
        {
            *pValue = pDataParams->wCfgShadow[wConfig];
        }
        else
        {
            *pValue = pDataParams->wCfgShadow[PHHAL_HW_CONFIG_TIMEOUT_VALUE_US] / 1000;
        }
        break;

    case PHHAL_HW_CONFIG_TIMING_MODE:

        *pValue = pDataParams->wTimingMode;
        break;

    case PHHAL_HW_CONFIG_TIMING_US:

        if (pDataParams->dwTimingUs > 0xFFFF)
        {
            return PH_ADD_COMPCODE(PH_ERR_PARAMETER_OVERFLOW, PH_COMP_HAL);
        }

        *pValue = (uint16_t)pDataParams->dwTimingUs;
        pDataParams->dwTimingUs = 0;
        break;

    case PHHAL_HW_CONFIG_TIMING_MS:

        if (pDataParams->dwTimingUs > (0xFFFF * 1000))
        {
            pDataParams->dwTimingUs = 0;
            return PH_ADD_COMPCODE(PH_ERR_PARAMETER_OVERFLOW, PH_COMP_HAL);
        }

        *pValue = (uint16_t)(pDataParams->dwTimingUs / 1000);
        pDataParams->dwTimingUs = 0;
        break;

    case PHHAL_HW_CONFIG_FIELD_OFF_TIME:

        *pValue = pDataParams->wFieldOffTime;
        break;

    case PHHAL_HW_CONFIG_FIELD_RECOVERY_TIME:

        *pValue = pDataParams->wFieldRecoveryTime;
        break;

    case PHHAL_HW_CONFIG_JEWEL_MODE:

        /* Write configuration data into shadow */
        *pValue = pDataParams->bJewelActivated;
        break;

    case PHHAL_HW_CONFIG_DISABLE_MF_CRYPTO1:

        /* Retrieve Status register */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS2, &bRegister));

        /* Check Crypto1On bit */
        if (bRegister & PHHAL_HW_RC523_BIT_CRYPTO1ON)
        {
            *pValue = PH_OFF; /* OFF in this case means "Crypto1 not disabled --> enabled" */
        }
        else
        {
            *pValue = PH_ON; /* ON in this case means "Crypto1 is disabled" */
        }
        break;

    case PHHAL_HW_CONFIG_RXBUFFER_STARTPOS:

        /* Return parameter */
        *pValue = pDataParams->wRxBufStartPos;
        break;

    case PHHAL_HW_CONFIG_RXBUFFER_BUFSIZE:

        /* Retrieve RxBuffer parameters */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetRxBuffer(pDataParams, PH_ON, &pBuffer, &wBufferLen, &wBufferSize));

        /* Return parameter */
        *pValue = wBufferSize;
        break;

    case PHHAL_HW_CONFIG_TXBUFFER_BUFSIZE:

        /* Retrieve TxBuffer parameters */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetTxBuffer(pDataParams, PH_ON, &pBuffer, &wBufferLen, &wBufferSize));

        /* In case of 18092 Communication in Passive Type-A at 106kbps then Start Byte '0xF0' is handled in HAL and buffer length should be decremented by 1. */
        if ((pDataParams->bNfcipMode == PH_ON)  && (pDataParams->bActiveMode == PH_OFF))
        {
            /* Read the TxMode Register to read the BaudRate */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, &bRegister));
            if (((bRegister & PHHAL_HW_RC523_MASK_SPEED) == PHHAL_HW_RC523_BIT_106KBPS) && (wBufferSize > 0))
            {
                wBufferSize--;
            }
        }

        /* Return parameter */
        *pValue = wBufferSize;
        break;

    case PHHAL_HW_CONFIG_TXBUFFER_LENGTH:

        /* Retrieve TxBuffer parameters */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetTxBuffer(pDataParams, PH_ON, &pBuffer, &wBufferLen, &wBufferSize));

        if ((pDataParams->bNfcipMode == PH_ON) && (wBufferLen != 0) && (pDataParams->bActiveMode == PH_OFF))
        {
            /* Read the TxMode Register to read the BaudRate */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, &bRegister));
            if (((bRegister & PHHAL_HW_RC523_MASK_SPEED) == PHHAL_HW_RC523_BIT_106KBPS) && (wBufferLen > 0))
            {
                wBufferLen -= 1;
            }
        }
        /* Return parameter */
        *pValue = wBufferLen;
        break;

    case PHHAL_HW_CONFIG_TXBUFFER:

        /* Retrieve TxBuffer parameters */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetTxBuffer(pDataParams, PH_ON, &pBuffer, &wBufferLen, &wBufferSize));

        /* Check additional info parameter */
        if (pDataParams->wAdditionalInfo >= wBufferSize)
        {
            return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
        }

        /* Retrieve TxBuffer byte */
        *pValue = (uint16_t)pBuffer[pDataParams->wAdditionalInfo];
        break;

    case PHHAL_HW_CONFIG_MAX_PRECACHED_BYTES:

        /* Return parameter */
        *pValue = pDataParams->wMaxPrecachedBytes;
        break;

    case PHHAL_HW_CONFIG_BAL_CONNECTION:

        /* Return parameter */
        *pValue = (uint16_t)pDataParams->bBalConnectionType;
        break;

    case PHHAL_HW_CONFIG_SERIAL_BITRATE:

        /* Read the register value */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_SERIALSPEED, &bRegister));

        switch (bRegister)
        {
        case PHHAL_HW_RC523_SERIALSPEED_9600:
            *pValue = PHHAL_HW_RS232_BITRATE_9600;
            break;
        case PHHAL_HW_RC523_SERIALSPEED_19200:
            *pValue = PHHAL_HW_RS232_BITRATE_19200;
            break;
        case PHHAL_HW_RC523_SERIALSPEED_38400:
            *pValue = PHHAL_HW_RS232_BITRATE_38400;
            break;
        case PHHAL_HW_RC523_SERIALSPEED_57600:
            *pValue = PHHAL_HW_RS232_BITRATE_57600;
            break;
        case PHHAL_HW_RC523_SERIALSPEED_115200:
            *pValue = PHHAL_HW_RS232_BITRATE_115200;
            break;
        case PHHAL_HW_RC523_SERIALSPEED_230400:
            *pValue = PHHAL_HW_RS232_BITRATE_230400;
            break;
        case PHHAL_HW_RC523_SERIALSPEED_460800:
            *pValue = PHHAL_HW_RS232_BITRATE_460800;
            break;
        default:
            return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
        }
        break;

    case PHHAL_HW_CONFIG_RFRESET_ON_TIMEOUT:

        /* Return parameter */
        *pValue = (uint16_t)pDataParams->bRfResetAfterTo;
        break;

    case PHHAL_HW_CONFIG_ACTIVEMODE:

        /* Return parameter */
        *pValue = (uint16_t)pDataParams->bActiveMode;
        break;

    case PHHAL_HW_CONFIG_EXT_RF_ON:
        /* Retrieve Status register. Read the External RF Field Detection Bit from Status Register */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS1, &bRegister));
        if(bRegister & PHHAL_HW_RC523_BIT_RFON)
        {
            *pValue = PH_ON; /* ON if external RF field is detected */
        }
        else
        {
            *pValue = PH_OFF; /* OFF if no external RF field is detected */
        }
        break;

    case PHHAL_HW_CONFIG_RXMULTIPLE:
        *pValue = (uint16_t)pDataParams->bRxMultiple;
        break;

    case PHHAL_HW_CONFIG_TARGET_DATARATE_FRAMING:
        *pValue = pDataParams->wTargetMode;
        break;

    case PHHAL_HW_CONFIG_NFCIP_STARTBYTE:
        *pValue = pDataParams->bNfcipMode;
        break;

    default:
        return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_PARAMETER, PH_COMP_HAL);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_FieldOn(
                                 phhalHw_Rc523_DataParams_t * pDataParams
                                 )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t PH_MEMLOC_REM bTxControlReg;
    uint8_t PH_MEMLOC_REM bTxAutoReg;
    uint8_t PH_MEMLOC_REM bStatus1Reg;

    /* Retrieve the content of the TxControl register */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, &bTxControlReg));
    if ((bTxControlReg & (PHHAL_HW_RC523_BIT_TX1RFEN | PHHAL_HW_RC523_BIT_TX2RFEN)) == (PHHAL_HW_RC523_BIT_TX1RFEN | PHHAL_HW_RC523_BIT_TX2RFEN))
    {
        /* If Field is already ON then return Success. */
        return PH_ERR_SUCCESS;
    }

    bTxControlReg |= PHHAL_HW_RC523_BIT_INVTX2ON | PHHAL_HW_RC523_BIT_CHECKRF;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, bTxControlReg));

    if(pDataParams->bRfca)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_IDLE));

        /* Retrieve the content of the TxAutoReg register */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, &bTxAutoReg));
        /* Enable RF CA bits */
        bTxAutoReg |= PHHAL_HW_RC523_BIT_INITIAL_RF_ON | PHHAL_HW_RC523_BIT_TX1RFAutoEN | PHHAL_HW_RC523_BIT_TX2RFAutoEN | PHHAL_HW_RC523_BIT_CA_ON;
        /* Start RF CA process */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, bTxAutoReg));

        /* Wait till Initial RF CA procedure ends (i.e. external RF detected or our RF enabled ) */
        do
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS1, &bStatus1Reg));
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, &bTxControlReg));
        }
        while(!(((bStatus1Reg & PHHAL_HW_RC523_BIT_RFON) == PHHAL_HW_RC523_BIT_RFON) ||
              ((bTxControlReg & (PHHAL_HW_RC523_BIT_TX1RFEN | PHHAL_HW_RC523_BIT_TX2RFEN)) == (PHHAL_HW_RC523_BIT_TX1RFEN | PHHAL_HW_RC523_BIT_TX2RFEN))));

        /* If external RF is ON, return RF error */
        if(bStatus1Reg & PHHAL_HW_RC523_BIT_RFON)
        {
            return PH_ADD_COMPCODE(PH_ERR_RF_ERROR, PH_COMP_HAL);
        }

        if((pDataParams->bCardType == PHHAL_HW_CARDTYPE_I18092M_ACTIVE_106) ||
            (pDataParams->bCardType == PHHAL_HW_CARDTYPE_I18092M_ACTIVE_212) ||
            (pDataParams->bCardType == PHHAL_HW_CARDTYPE_I18092M_ACTIVE_424))
        {
            /* Retrieve the content of the TxAutoReg register */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, &bTxAutoReg));

            /* If Apply protocol settings for Active Mode is done then enable AutoRFoff, CollisionAvoidance ON, Tx1/2AutoRFON. */
            bTxAutoReg |= PHHAL_HW_RC523_BIT_AUTORF_OFF;
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, bTxAutoReg));
        }
    }
    else
    {
        if((pDataParams->bCardType == PHHAL_HW_CARDTYPE_I18092M_ACTIVE_106) ||
            (pDataParams->bCardType == PHHAL_HW_CARDTYPE_I18092M_ACTIVE_212) ||
            (pDataParams->bCardType == PHHAL_HW_CARDTYPE_I18092M_ACTIVE_424))
        {
            /* If Apply protocol settings for Active Mode is done then enable AutoRFoff, CollisionAvoidance ON, Tx1/2AutoRFON. */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, &bTxAutoReg));
            bTxAutoReg |= (PHHAL_HW_RC523_BIT_CA_ON | PHHAL_HW_RC523_BIT_AUTORF_OFF | PHHAL_HW_RC523_BIT_TX2RFAutoEN | PHHAL_HW_RC523_BIT_TX1RFAutoEN);
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, bTxAutoReg));
        }

        /* Switch on the field again */
        bTxControlReg |= PHHAL_HW_RC523_BIT_TX1RFEN | PHHAL_HW_RC523_BIT_TX2RFEN;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, bTxControlReg));
    }

    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Rc523_FieldOff(
                                  phhalHw_Rc523_DataParams_t * pDataParams
                                  )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bRegister;

    /* Disable AutoRFoff, CollisionAvoidance, Tx1/2AutoRFON. */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, &bRegister));
    bRegister &= (uint8_t)~(uint8_t)(PHHAL_HW_RC523_BIT_CA_ON | PHHAL_HW_RC523_BIT_TX2RFAutoEN | PHHAL_HW_RC523_BIT_TX1RFAutoEN | PHHAL_HW_RC523_BIT_AUTORF_OFF);
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, bRegister));

    /* Retrieve the content of the TxControl register */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, &bRegister));
    /* Switch off the field */
    bRegister &= (uint8_t)~(uint8_t)(PHHAL_HW_RC523_BIT_TX1RFEN | PHHAL_HW_RC523_BIT_TX2RFEN);
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, bRegister));

    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Rc523_FieldReset(
                                    phhalHw_Rc523_DataParams_t * pDataParams
                                    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;

    /* Switch off the field */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FieldOff(pDataParams));

    /* wait for field-off timeout */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_Wait(
        pDataParams,
        PHHAL_HW_TIME_MILLISECONDS,
        pDataParams->wFieldOffTime));

    /* switch on the field again */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FieldOn(pDataParams));

    /* wait for field-recovery timeout */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_Wait(
        pDataParams,
        PHHAL_HW_TIME_MILLISECONDS,
        pDataParams->wFieldRecoveryTime));

    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Rc523_Wait(
                              phhalHw_Rc523_DataParams_t * pDataParams,
                              uint8_t bUnit,
                              uint16_t wTimeout
                              )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bRegister;
    uint8_t        PH_MEMLOC_REM bIrq0Wait;
    uint8_t        PH_MEMLOC_REM bIrq1Wait;
    uint16_t    PH_MEMLOC_REM wTimerShift;
    uint16_t    PH_MEMLOC_REM wTimeoutNew;

    /* Parameter check */
    if ((bUnit != PHHAL_HW_TIME_MICROSECONDS) && (bUnit != PHHAL_HW_TIME_MILLISECONDS))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Terminate a probably running command */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_IDLE));

    /* Retrieve RxDataRate */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetConfig(pDataParams, PHHAL_HW_CONFIG_RXDATARATE_FRAMING, &wTimerShift));
    ++wTimerShift;

    /* Calculate timer shift */
    wTimerShift = (uint16_t)(PHHAL_HW_RC523_TIMER_SHIFT * (PHHAL_HW_RC523_ETU_106 / (float32_t)wTimerShift));

    /* do as long as we have an overflow in the IC timer */
    do
    {
        /* Set temporary timeout */
        if (bUnit == PHHAL_HW_TIME_MICROSECONDS)
        {
            wTimeoutNew = (wTimeout > wTimerShift) ? (wTimeout - wTimerShift) : 0;
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetFdt(pDataParams, PHHAL_HW_TIME_MICROSECONDS, wTimeoutNew));
            wTimeout = 0;
        }
        else
        {
            /* Timer would overflow -> use maximum value and decrement overall value for next iteration */
            if (wTimeout > ((PHHAL_HW_RC523_TIMER_MAX_VALUE_US / 1000) - 1))
            {
                wTimeoutNew = (PHHAL_HW_RC523_TIMER_MAX_VALUE_US / 1000) - 1;
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetFdt(pDataParams, PHHAL_HW_TIME_MILLISECONDS, wTimeoutNew));
                wTimeout = wTimeout - wTimeoutNew;
            }
            /* No overflow -> set the complete time */
            else
            {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetFdt(pDataParams, PHHAL_HW_TIME_MILLISECONDS, wTimeout));
                wTimeout = 0;
            }
        }

        /* retrieve content of Control register */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_CONTROL, &bRegister));

        /* clear all irq flags */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIRQ, (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_SET));

        /* start timer */
        bRegister |= PHHAL_HW_RC523_BIT_TSTARTNOW;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_CONTROL, bRegister));

        /* wait for timer interrupt */
        bIrq0Wait = PHHAL_HW_RC523_BIT_ERRI | PHHAL_HW_RC523_BIT_TIMERI;
        bIrq1Wait = 0x00;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WaitIrq(pDataParams, bIrq0Wait, bIrq1Wait, &bRegister, &bRegister));

        /* clear all irq flags */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIRQ, (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_SET));
    }
    while (wTimeout > 0);

    /* Restore previous timeout */
    if (pDataParams->bTimeoutUnit == PHHAL_HW_TIME_MICROSECONDS)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetFdt(
            pDataParams,
            PHHAL_HW_TIME_MICROSECONDS,
            pDataParams->wCfgShadow[PHHAL_HW_CONFIG_TIMEOUT_VALUE_US]));
    }
    else
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetFdt(
            pDataParams,
            PHHAL_HW_TIME_MILLISECONDS,
            pDataParams->wCfgShadow[PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS]));
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_MfcAuthenticateKeyNo(
    phhalHw_Rc523_DataParams_t * pDataParams,
    uint8_t bBlockNo,
    uint8_t bKeyType,
    uint16_t wKeyNo,
    uint16_t wKeyVersion,
    uint8_t * pUid
    )
{
#ifdef NXPBUILD__PH_KEYSTORE
    phStatus_t  PH_MEMLOC_REM status;
    uint8_t     PH_MEMLOC_REM aKey[PHHAL_HW_MFC_KEY_LENGTH * 2];
    uint8_t *   PH_MEMLOC_REM pKey;
    uint16_t    PH_MEMLOC_REM bKeystoreKeyType;

    /* Bail out if we haven't got a keystore */
    if (pDataParams->pKeyStoreDataParams == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_USE_CONDITION, PH_COMP_HAL);
    }

    /* retrieve KeyA & KeyB from keystore */
    status = phKeyStore_GetKey(
        pDataParams->pKeyStoreDataParams,
        wKeyNo,
        wKeyVersion,
        sizeof(aKey),
        aKey,
        &bKeystoreKeyType);

    /* Change component code for invalid paramter */
    if ((status & PH_ERR_MASK) == PH_ERR_INVALID_PARAMETER)
    {
        return PH_ADD_COMPCODE((status & PH_ERR_MASK), PH_COMP_HAL);
    }
    else
    {
        PH_CHECK_SUCCESS(status);
    }

    /* check key type */
    if (bKeystoreKeyType != PH_KEYSTORE_KEY_TYPE_MIFARE)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Evaluate which key to use */
    if (bKeyType == PHHAL_HW_MFC_KEYA)
    {
        /* Use KeyA */
        pKey = aKey;
    }
    else if (bKeyType == PHHAL_HW_MFC_KEYB)
    {
        /* Use KeyB */
        pKey = &aKey[PHHAL_HW_MFC_KEY_LENGTH];
    }
    else
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Call the authentication function using the key */
    /*Warning Fix : aKey to pKey*/
    return phhalHw_Rc523_MfcAuthenticate(pDataParams, bBlockNo, bKeyType, pKey, pUid);
#else
    /* satisfy compiler */
    if (pDataParams || bBlockNo || bKeyType || wKeyNo || wKeyVersion || pUid);
    return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_HAL);
#endif
}

phStatus_t phhalHw_Rc523_MfcAuthenticate(
    phhalHw_Rc523_DataParams_t * pDataParams,
    uint8_t bBlockNo,
    uint8_t bKeyType,
    uint8_t * pKey,
    uint8_t * pUid
    )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bFifoData[6 + PHHAL_HW_MFC_KEY_LENGTH];
    uint16_t    PH_MEMLOC_REM wFifoDataLen = 0;
    uint8_t     PH_MEMLOC_REM bRegister;
    uint8_t        PH_MEMLOC_REM bIrq1Rq;
    uint8_t     PH_MEMLOC_REM bIrq0Wait = 0x0;
    uint8_t     PH_MEMLOC_REM bIrq1Wait = 0x0;

    /* Evaluate which key to use */
    if (bKeyType == PHHAL_HW_MFC_KEYA)
    {
        /* Set authentication command code */
        bFifoData[wFifoDataLen++] = PHHAL_HW_RC523_MFC_AUTHA_CMD;
    }
    else if (bKeyType == PHHAL_HW_MFC_KEYB)
    {
        /* Set authentication command code */
        bFifoData[wFifoDataLen++] = PHHAL_HW_RC523_MFC_AUTHB_CMD;
    }
    else
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* set block number */
    bFifoData[wFifoDataLen++] = bBlockNo;

    /* copy the sector key */
    memcpy(&bFifoData[wFifoDataLen], pKey, PHHAL_HW_MFC_KEY_LENGTH);  /* PRQA S 3200 */
    wFifoDataLen += PHHAL_HW_MFC_KEY_LENGTH;

    /* set serial number key */
    memcpy(&bFifoData[wFifoDataLen], pUid, 4);  /* PRQA S 3200 */
    wFifoDataLen += 4;

    /* write command data into FiFo buffer */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteFifo(pDataParams, bFifoData, wFifoDataLen, &wFifoDataLen));

    /* clear all irq flags */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIRQ, (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_SET));

    /* write the command into the command register */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_AUTHENT));

    /* wait until command is finished */
    bIrq0Wait = PHHAL_HW_RC523_BIT_RXI | PHHAL_HW_RC523_BIT_ERRI | PHHAL_HW_RC523_BIT_TIMERI | PHHAL_HW_RC523_BIT_IDLEI;
    status = phhalHw_Rc523_WaitIrq(pDataParams, bIrq0Wait, bIrq1Wait,&bRegister,&bIrq1Rq);

    /* stop the command */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_IDLE));

    /* Flush FiFo */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FlushFifo(pDataParams));

    /* Status check */
    PH_CHECK_SUCCESS(status);

    /* Check for timeout */
    if (bRegister & PHHAL_HW_RC523_BIT_TIMERI)
    {
        return PH_ADD_COMPCODE(PH_ERR_IO_TIMEOUT, PH_COMP_HAL);
    }

    /* Check auth success */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS2, &bRegister));
    if ((bRegister & PHHAL_HW_RC523_BIT_CRYPTO1ON) != PHHAL_HW_RC523_BIT_CRYPTO1ON)
    {
        return PH_ADD_COMPCODE(PH_ERR_AUTH_ERROR, PH_COMP_HAL);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_SetMinFDT(
                                   phhalHw_Rc523_DataParams_t * pDataParams,
                                   uint16_t wValue
                                   )
{
    phStatus_t    PH_MEMLOC_REM status = 0;
    uint16_t    PH_MEMLOC_REM wTimer = 0;
    uint16_t    PH_MEMLOC_REM wTxRate = 0;

    if (wValue == PH_ON)
    {
        /*Backup the old Timer values and set min FDT*/
        PH_CHECK_SUCCESS_FCT(status, phhalHw_Rc523_GetConfig(pDataParams,
            PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS, &wTimer));
        pDataParams->dwFdtPc = wTimer;
        /* Get the data rate */
        PH_CHECK_SUCCESS_FCT(status, phhalHw_Rc523_GetConfig(pDataParams,
            PHHAL_HW_CONFIG_TXDATARATE_FRAMING, &wTxRate));

        wTxRate = wTxRate & PHHAL_HW_RF_DATARATE_OPTION_MASK;

        switch(wTxRate)
        {
        case PHHAL_HW_RF_DATARATE_106:
            wTimer = PHHAL_HW_MINFDT_106_US;
            break;
        case PHHAL_HW_RF_DATARATE_212:
            wTimer = PHHAL_HW_MINFDT_212_US;
            break;
        case PHHAL_HW_RF_DATARATE_424:
            wTimer = PHHAL_HW_MINFDT_424_US;
            break;
        case PHHAL_HW_RF_DATARATE_848:
            wTimer = PHHAL_HW_MINFDT_848_US;
            break;
        default:
            break;
        }
        /* Calculate values for Microsecond values */
        PH_CHECK_SUCCESS_FCT(status, phhalHw_Rc523_SetConfig(pDataParams,
            PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, wTimer));
    }
    else if (wValue == PH_OFF)
    {
        PH_CHECK_SUCCESS_FCT(status, phhalHw_Rc523_SetConfig(pDataParams,
            PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS, pDataParams->dwFdtPc));
    }
    else
    {
        /* Do nothing*/
    }
    return status;
}

phStatus_t phhalHw_Rc523_Autocoll(
                                  phhalHw_Rc523_DataParams_t * pDataParams,
                                  uint16_t wMode,
                                  uint8_t ** ppRxBuffer,
                                  uint16_t * pRxLength,
                                  uint16_t * pProtParams
                                  )
{
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wFifoBytes;
    uint16_t    PH_MEMLOC_REM wInteropTolerance;
    uint8_t     PH_MEMLOC_REM bIrq0WaitFor;
    uint8_t     PH_MEMLOC_REM bIrq1WaitFor;
    uint8_t     PH_MEMLOC_REM bIrq0Rq;
    uint8_t     PH_MEMLOC_REM bIrq1Rq;
    uint8_t     PH_MEMLOC_REM bReg = 0;
    uint8_t     PH_MEMLOC_REM bRegister;
    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint16_t    PH_MEMLOC_REM wTmpBufferSize;
    uint8_t     PH_MEMLOC_REM offsetPos = 0;
    uint8_t     PH_MEMLOC_REM offsetLen = 0;
    uint8_t     PH_MEMLOC_REM bStatus2 = 0;
    uint8_t     PH_MEMLOC_REM bSpeed = 0;
    uint8_t     PH_MEMLOC_REM bActiveSpeed = 0;
    uint8_t     PH_MEMLOC_REM bFraming = 0;
    uint8_t     PH_MEMLOC_REM bStartByte = 0;
    uint8_t     PH_MEMLOC_REM bDisActiveTarget;

    /* Check if listen for any active technology enabled [MSB of wMode] */
    if(wMode & 0xFF00)
    {
        bDisActiveTarget = 0x00;
    }
    else
    {
        bDisActiveTarget = 0x01;
    }

    /* Reset the Flag used to indicate if Passive Target is activated by ISO/IEC 18092 Frame */
    pDataParams->wTargetMode = PH_OFF;
    pDataParams->bActiveMode = PH_OFF;
    pDataParams->bNfcipMode  = PH_OFF;

    /* Calculate Interop tolerance. By default tolerance is set to 300us */
    if (PHHAL_HW_RC523_ACTIVE_RF_OFF_INTEROP_TOLERANCE > 250)
    {
        wInteropTolerance = PHHAL_HW_RC523_ACTIVE_RF_OFF_INTEROP_TOLERANCE - 250;
    }
    else
    {
        wInteropTolerance = 0x01;
    }

    /* Terminate a probably running command */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_IDLE));

    /* Flush FiFo */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FlushFifo(pDataParams));

    statusTmp = wMode;

    /* configure Tx path for TypeA */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, PHHAL_HW_RC523_BIT_MIFARE));
    /* configure Rx path for TypeA */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, PHHAL_HW_RC523_BIT_MIFARE));
    /* configurate default TxModWidth */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODWIDTH, 0x26));
    /* configure the RxThreshold for TypeA  */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXTHRESHOLD, PHHAL_HW_RC523_RXTHRESHOLD_I14443A));
    /* Set target mode */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_CONTROL, 0x00));

    /* Change the Timeout Unit to Milliseconds in Target Mode */
    pDataParams->bTimeoutUnit = PHHAL_HW_TIME_MILLISECONDS;

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXDATARATE_FRAMING, PHHAL_HW_RF_DATARATE_106 | PHHAL_HW_RF_TYPE_A_FRAMING));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_RXDATARATE_FRAMING, PHHAL_HW_RF_DATARATE_106 | PHHAL_HW_RF_TYPE_A_FRAMING));

    /* Enable parity calculation */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_PARITY, PH_ON));

    /* CRC should be enabled in-case of Type-F, so that CRC is removed by the Hardware. */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXCRC, PH_ON));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_RXCRC, PH_ON));

    /* Enable Force100ASK bit in TxAutoReg */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_ASK100, PH_ON));

    /* configure the gain factor to 23dB for Target */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RFCFG, 0x59));

    /* configure the RxSel Register */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXSEL, 0x80));

    /* configure general settings */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODE, 0x00));

    /* configure the conductance if no modulation is active */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_GSN, 0xFF));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_CWGSP, 0x3F));

    /* configure the conductance for LoadModulation */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_GSNOFF, PHHAL_HW_RC523_MODINDEX_TARGET));

    /* reset BitFraming register */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_BITFRAMING, 0x00));

    /* configure the timer */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TMODE, 0x00));

    /* configure the water level */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_WATERLEVEL, PHHAL_HW_RC523_FIFOSIZE - 8));

    /* MIFARE Crypto1 state is disabled by default */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams, PHHAL_HW_CONFIG_DISABLE_MF_CRYPTO1, PH_ON));

    /* set the receive length */
    if (pRxLength != NULL)
    {
        *pRxLength = 0;
    }

    /* clear all IRQ0 flags */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_COMMIRQ,
        (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_SET));

    /* clear all IRQ1 flags */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_DIVIRQ,
        (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_SET));

    /* Reset the contents of TxControl Register. */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, 0x00));

    /* Read the External RF Field Detection Bit from Status Register */
    PH_CHECK_SUCCESS_FCT(status, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS1, &bRegister));
    if(!(bRegister & PHHAL_HW_RC523_BIT_RFON))
    {
        /* External RF Field is OFF, monitor for Timer and External RF Field ON IRQ's */
        return PH_ADD_COMPCODE(PH_ERR_EXT_RF_ERROR, PH_COMP_HAL);
    }

    /* Below register settings are required to send the Active Frames once Autocoll exists successfully in Active Target mode. */
    if(bDisActiveTarget == PH_OFF)
    {
        /* Configure TX2 inverted of but do not change the field status */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, &bRegister));
        bRegister &= PHHAL_HW_RC523_BIT_TX2RFEN | PHHAL_HW_RC523_BIT_TX1RFEN;
        bRegister |= PHHAL_HW_RC523_BIT_INVTX2ON | PHHAL_HW_RC523_BIT_CHECKRF;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, bRegister));

        /* Enable Active Communication Mode. */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_FELNFC2, &bRegister));
        bRegister &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_WAIT_FOR_SELECTED;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_FELNFC2, bRegister));
    }
    else
    {
        /* Disable Active Communication Mode. */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_FELNFC2, &bRegister));
        bRegister |= PHHAL_HW_RC523_WAIT_FOR_SELECTED;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_FELNFC2, bRegister));
    }

    /* start the command */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_AUTOCOLL));

    if(bDisActiveTarget == PH_OFF)
    {
        /* Enable Bits to detect Active Communication Frames */
        PH_CHECK_SUCCESS_FCT(statusTmp,
            phhalHw_Rc523_ReadRegister(
            pDataParams,
            PHHAL_HW_RC523_REG_TXASK,
            &bRegister));
        bReg = bRegister;
        bRegister |= (PHHAL_HW_RC523_BIT_TX2RFAutoEN | PHHAL_HW_RC523_BIT_TX1RFAutoEN
            | PHHAL_HW_RC523_BIT_AUTORF_OFF | PHHAL_HW_RC523_BIT_CA_ON);

        PH_CHECK_SUCCESS_FCT(statusTmp,
            phhalHw_Rc523_WriteRegister(
            pDataParams,
            PHHAL_HW_RC523_REG_TXASK,
            bRegister));
    }

    bIrq0WaitFor = PHHAL_HW_RC523_BIT_IDLEI | PHHAL_HW_RC523_BIT_ERRI;
    bIrq1WaitFor = PHHAL_HW_RC523_BIT_EXT_RF_OFF;

    /* Wait until the command is finished */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WaitIrq(
        pDataParams,
        bIrq0WaitFor,
        bIrq1WaitFor,
        &bIrq0Rq,
        &bIrq1Rq));

    /* Autocoll returned because of External RF OFF error.
     * If Idle Interrupt is Set then Skip below sequence introduced for better Interop for HCE. */
    if ((bIrq1Rq & PHHAL_HW_RC523_BIT_EXT_RF_OFF) && (!(bIrq0Rq & PHHAL_HW_RC523_BIT_IDLEI)))
    {
        if(bDisActiveTarget == PH_OFF)
        {
            /* Disable Active Mode settings. */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, 0x00));
        }

        /* Terminate Autocoll command before existing. */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_IDLE));

        return PH_ADD_COMPCODE(PH_ERR_EXT_RF_ERROR, PH_COMP_HAL);
    }

    if (bIrq0Rq & PHHAL_HW_RC523_BIT_ERRI)
    {
        /* Get the Error status from Error Register. */
        status = phhalHw_Rc523_GetError(pDataParams);

        /*
         * If error is buffer overflow in Passive mode start receive command to receive next ATR_REQ. In Active mode return
         * error code to the application. Application should take care of restart of Autocoll to receive next request.
         * In passive mode, it is required to handle the next correct ATR_REQ in order to preserve the target activation state.
         *
         * Note: For TypeF 424 for the same scenario none of the error bits are set in Error register even though
         * error IRQ has occurred and this is interpreted as INTERNAL_ERROR. So checking for INTERNAL_ERROR below.
         * */
        if ((status == PH_ERR_BUFFER_OVERFLOW) || (status == PH_ERR_INTERNAL_ERROR))
        {
            /* Wait until any of Rx/Idle/RF OFF IRQs.
             * In case of buffer overflow, the IDLE state and/or RXI are set slightly later. So waiting for
             * the IDLE or RXI to occur so that a next receive can be called cleanly. */
            do
            {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIRQ, &bIrq0Rq));
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_DIVIRQ, &bIrq1Rq));
            }
            while (((bIrq0Rq & (PHHAL_HW_RC523_BIT_IDLEI | PHHAL_HW_RC523_BIT_RXI)) == 0) &&
                ((bIrq1Rq & PHHAL_HW_RC523_BIT_EXT_RF_OFF) == 0));

            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, &bRegister));

            /* Get the Framing and Rx data rate*/
            bSpeed = (bRegister & PHHAL_HW_RC523_MASK_SPEED) >> PHHAL_HW_RC523_SHIFT_SPEED;
            bFraming = (bRegister & PHHAL_HW_RC523_MASK_FRAMING);

            /* Check for activated Mode. */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS2, &bStatus2));
            if (bStatus2 & PHHAL_HW_RC523_BIT_TARGET_ACTIVATED)
            {
                /* Passive mode */
                pDataParams->bActiveMode = PH_OFF;

                if(bFraming == PHHAL_HW_RC523_BIT_MIFARE)
                {
                    pDataParams->wTargetMode = PHHAL_HW_RF_TYPE_A_FRAMING | bSpeed;
                    pDataParams->bNfcipMode = PH_ON;
                    status = PH_ERR_SUCCESS;
                }
                else if(bFraming == PHHAL_HW_RC523_BIT_FELICA)
                {
                    pDataParams->wTargetMode = PHHAL_HW_RF_TYPE_F_FRAMING | bSpeed;
                    status = PH_ERR_SUCCESS;
                }
                else
                {
                    /* Do nothing */
                }

                if (status == PH_ERR_SUCCESS)
                {
                    /* Wait for next ATR_REQ. */
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_Receive(pDataParams, 0x00, ppRxBuffer, pRxLength));
                }
            }
            else
            {
                /* Active mode.
                 * In active mode, autocoll returns with the error code (BUFFER_OVERFLOW in this case).
                 * The application is expected to restart autocoll.
                 * */
                /*
                 * Wait till the external RF is turned OFF.
                 */
                do
                {
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_DIVIRQ, &bIrq1Rq));
                }
                while ((bIrq1Rq & PHHAL_HW_RC523_BIT_EXT_RF_OFF) == 0);

                /*
                 * Since there is no response from target to this error packet, the target has to turn OFF the internal field
                 * which was automatically switched ON after the external RF was turned OFF.
                 * The minimum delay between internal RF ON to RF OFF should be 56 microseconds.
                 * The Pn512 IC takes some time (174us) to turn ON internal field.
                 * The following delay considers the above as well as a minimum turn ON period > 56us.
                 * The Delay function's register read/writes take ~270us at SPI of 5MHz.
                 */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_Delay(pDataParams, PHHAL_HW_RC523_ACTIVE_INTERNAL_RF_OFF_TOLERANCE));
            }
        }

        if (status != PH_ERR_SUCCESS)
        {
            /* Disable Auto RF Off and turn off the internal RF. */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, 0x00));
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, 0x00));

            /* Terminate Autocoll command before existing. */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_IDLE));
        }

        return PH_ADD_COMPCODE(status, PH_COMP_HAL);
    }

    if (bIrq0Rq & PHHAL_HW_RC523_BIT_IDLEI)
    {
        /* we need to check if target is activated */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS2, &bStatus2));

        if (bStatus2 & PHHAL_HW_RC523_BIT_TARGET_ACTIVATED)
        {
            /* Activated in Passive Communication Mode.
            * In case External RF OFF also occurred then exit with RF Error. */
            if (bIrq1Rq & PHHAL_HW_RC523_BIT_EXT_RF_OFF)
            {
                /* If Active Mode is Enabled then Disable Active Mode settings. */
                if(bDisActiveTarget == PH_OFF)
                {
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, 0x00));
                }

                /* Terminate Autocoll command before existing. */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_IDLE));

                return PH_ADD_COMPCODE(PH_ERR_EXT_RF_ERROR, PH_COMP_HAL);
            }
            if(bDisActiveTarget == PH_OFF)
            {
                /* Restore TXASK register value. */
                PH_CHECK_SUCCESS_FCT(statusTmp,
                    phhalHw_Rc523_WriteRegister(
                    pDataParams,
                    PHHAL_HW_RC523_REG_TXASK,
                    bReg));
            }
        }

        /* Reset receive buffer length */
        pDataParams->wRxBufLen = 0;

        /* Retrieve receive buffer properties */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetRxBuffer(pDataParams, PH_ON, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

        /* Do the following if no timeout occurred */
        /* mask out high-alert */
        bIrq0WaitFor &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_HIALERTI;

        do
        {
            /* check if there is an error or of modem status went to Wait for StartSend */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIRQ, &bIrq0Rq));
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS2, &bStatus2));
        }
        while ((!(bIrq0Rq & bIrq0WaitFor)) && !((bStatus2 & 0x07) == 0x01));

        /* retrieve FIFO bytes */
        do
        {
            /* retrieve bytes from FiFo */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadFifo(
                pDataParams,
                wTmpBufferSize,
                &pTmpBuffer[wTmpBufferLen],
                &wFifoBytes));

            /* advance receive buffer */
            wTmpBufferLen = wTmpBufferLen + wFifoBytes;
            wTmpBufferSize = wTmpBufferSize - wFifoBytes;

            /* read interrupt status */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIRQ, &bIrq0Rq));
        }
        while ((!(bIrq0Rq & bIrq0WaitFor)) || (wFifoBytes != 0));

        /* Check for errors */
        if (bIrq0Rq & PHHAL_HW_RC523_BIT_ERRI)
        {
            /* Get the Error status from Error Register. */
            status = phhalHw_Rc523_GetError(pDataParams);
        }

        /* Receive was successful */
        if (status == PH_ERR_SUCCESS)
        {
            /* Get the Rx DataRate and Framing */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, &bRegister));
            /* Get the Rx data rate*/
            bActiveSpeed = (bRegister & PHHAL_HW_RC523_MASK_SPEED);
            bSpeed = (bRegister & PHHAL_HW_RC523_MASK_SPEED) >> PHHAL_HW_RC523_SHIFT_SPEED;
            bFraming = (bRegister & PHHAL_HW_RC523_MASK_FRAMING);

            if(!(bStatus2 & PHHAL_HW_RC523_BIT_TARGET_ACTIVATED))
            {
                /* Activated in Active Communication Mode. */
                if(bDisActiveTarget == PH_ON)
                {
                    /* Reset the contents of TxControl Register. */
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, 0x00));
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, 0x00));

                    /* Disable Active Mode Flag is Set but Target activated in Active Communication mode. */
                    return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_HAL);
                }

                /* In Active communication mode wait for internal RF field turn ON, default 300 microseconds is set. */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_Delay(pDataParams, wInteropTolerance));

                /* Check if Internal RF is turned ON. */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, &bReg));
                if (bReg & (PHHAL_HW_RC523_BIT_TX2RFEN | PHHAL_HW_RC523_BIT_TX1RFEN))
                {
                    /* Enable active mode */
                    pDataParams->bActiveMode = PH_ON;

                    /* Perform Active Mode specific settings */
                    if (bSpeed == PHHAL_HW_RF_DATARATE_106)
                    {
                        /* Its 106 */
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXBITPHASE, 0x0F));

                        /* Set Force100ASK */
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, &bRegister));
                        bRegister |= PHHAL_HW_RC523_BIT_FORCE100ASK;
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, bRegister));
                    }
                    else
                    {
                        /* Clear Force100ASK */
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, &bRegister));
                        bRegister = bRegister & (uint8_t)(~ (uint8_t)(PHHAL_HW_RC523_BIT_FORCE100ASK));
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, bRegister));
                    }

                    /* Enable Detect Sync Bit */
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_MODE, &bRegister));
                    bRegister |= PHHAL_HW_RC523_BIT_DETECT_SYNC;
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODE, bRegister));

                    /* Enable active Tx mode and speed */
                    bRegister = (PHHAL_HW_RC523_BIT_ACTIVE | bActiveSpeed | PHHAL_HW_RC523_BIT_CRCEN);
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXMODE, bRegister));
                    /* Enable active Rx mode and speed */
                    bRegister = (PHHAL_HW_RC523_BIT_ACTIVE | bActiveSpeed | PHHAL_HW_RC523_BIT_CRCEN);
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, bRegister));

                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_MODE, &bRegister));
                    bRegister |= PHHAL_HW_RC523_BIT_TXWAITRF;
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_MODE, bRegister));

                    /* If DetectSync bit is set for active mode(106 baud rate), Hardware removes the "F0"
                    ** But incase of target, "Detectsync" would be enabled after receiving ATR, so to
                    ** normalize the response, "F0" is offset here
                    */
                    if(bSpeed == PHHAL_HW_RF_DATARATE_106)
                    {
                        /* Offset for StartByte and CRC as it included if Active Communication in ISO/IEC 18092 */
                        offsetPos = 1;
                        offsetLen = 3;
                    }
                    bFraming = PHHAL_HW_RC523_BIT_ACTIVE;
                }
                else
                {
                    /* Reset the contents of TxControl Register. */
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, 0x00));
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, 0x00));

                    /* its a protocol error*/
                    return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_HAL);
                }
            }
            else
            {
                bStartByte = *pTmpBuffer;
                if((bSpeed == PHHAL_HW_RF_DATARATE_106) && (bStartByte == PHPAL_HW_RC523_I18092MT_START_BYTE))
                {
                    /* Offset for StartByte as it included if Passive Communication in ISO/IEC 18092 */
                    offsetPos = 1;
                    offsetLen = 1;
                }

                /* Observing RF ON in case of Passive Target in isolated cases.
                * Forcefully switch off the RF-Field. */
                /* Retrieve the content of the TxControl register */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, &bRegister));

                /* Switch off the field */
                bRegister &= (uint8_t)~(uint8_t)(PHHAL_HW_RC523_BIT_TX1RFEN | PHHAL_HW_RC523_BIT_TX2RFEN);
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, bRegister));
            }

            /* No bytes received -> timeout */
            if (wTmpBufferLen == 0)
            {
                status = PH_ERR_IO_TIMEOUT;
            }
            /* Else retrieve valid bits of last byte */
            else
            {
                /* Retrieve RxBits */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_CONTROL, &bRegister));

                /* Mask out valid bits of last byte */
                bRegister &= PHHAL_HW_RC523_MASK_RXBITS;

                /* Set RxLastBits */
                pDataParams->wAdditionalInfo = bRegister;

                /* Set incomplete byte status if applicable */
                if (bRegister != 0x00)
                {
                    status = PH_ERR_SUCCESS_INCOMPLETE_BYTE;
                }
            }
        }

        /* Flush the FIFO on error */
        if ((status != PH_ERR_SUCCESS) &&
            (status != PH_ERR_SUCCESS_INCOMPLETE_BYTE))
        {
            /* Flush FiFo */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FlushFifo(pDataParams));

            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, 0x00));
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, 0x00));
        }
        else
        {
            /* Switch on CRC after successful activation */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXCRC, PH_ON));
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams, PHHAL_HW_CONFIG_RXCRC, PH_ON));

            if(bFraming == PHHAL_HW_RC523_BIT_MIFARE)
            {
                *pProtParams = PHHAL_HW_RF_TYPE_A_FRAMING | bSpeed;
            }
            else if(bFraming == PHHAL_HW_RC523_BIT_ACTIVE)
            {
                *pProtParams = PHHAL_HW_RF_TYPE_ACTIVE_FRAMING | bSpeed;
            }
            else if(bFraming == PHHAL_HW_RC523_BIT_FELICA)
            {
                *pProtParams = PHHAL_HW_RF_TYPE_F_FRAMING | bSpeed;
            }
            else
            {
                /* Invalid Framing as PN512 will not emulate other than Type-A, Type-F and Active Frame */
                status = PH_ERR_FRAMING_ERROR;
            }

            /* Set the TargetActivated flag after successful Target activation. */
            pDataParams->wTargetMode = *pProtParams;
        }

        /* Store received data length in dataparams */
        pDataParams->wRxBufLen = pDataParams->wRxBufStartPos + wTmpBufferLen;

        /* Return RxBuffer pointer */
        if (ppRxBuffer != NULL)
        {
            /* Offset "F0" incase of Active communication */
            *ppRxBuffer = pDataParams->pRxBuffer + offsetPos;
        }

        /* Return RxBuffer length */
        if (pRxLength != NULL)
        {
            /* Offset "F0" + CRC incase of Active communication */
            *pRxLength = pDataParams->wRxBufLen - offsetLen;
        }
    }

    return PH_ADD_COMPCODE(status, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_AsyncAbort(
                                    phhalHw_Rc523_DataParams_t * pDataParams
                                    )
{
#ifndef _WIN32
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;
    /* Post Abort Event. */
    PH_CHECK_SUCCESS_FCT(status, phOsal_Event_Post(E_PH_OSAL_EVT_ABORT, E_PH_OSAL_EVT_SRC_ISR, E_PH_OSAL_EVT_DEST_LIB));
#else
    /* satisfy compiler */
    if (pDataParams);
#endif
    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Rc523_DeInit(
                                phhalHw_Rc523_DataParams_t * pDataParams
                                )
{
    phStatus_t PH_MEMLOC_REM status;
    uint8_t    PH_MEMLOC_REM bRegister;

    /* Disable IRQ0 interrupt sources */
    PH_CHECK_SUCCESS_FCT(status, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIEN, &bRegister));
    /* Clear IRQ0 interrupt sources */
    bRegister &= PHHAL_HW_RC523_BIT_IRQINV;
    PH_CHECK_SUCCESS_FCT(status, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIEN, bRegister));

    /* Disable IRQ1 interrupt sources */
    PH_CHECK_SUCCESS_FCT(status, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_DIVIEN, &bRegister));
    /* Clear IRQ1 interrupt sources */
    bRegister &= PHHAL_HW_RC523_BIT_IRQPUSHPULL;
    PH_CHECK_SUCCESS_FCT(status, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_DIVIEN, bRegister));

    /* De-Init. private data */
    pDataParams->wId                    = 0;
    pDataParams->pBalDataParams         = NULL;
    pDataParams->pKeyStoreDataParams    = NULL;
    pDataParams->pTxBuffer              = NULL;
    pDataParams->wTxBufSize             = 0;
    pDataParams->wTxBufLen              = 0;
    pDataParams->pRxBuffer              = NULL;
    pDataParams->wRxBufSize             = 0;
    pDataParams->wRxBufLen              = 0;
    pDataParams->wRxBufStartPos         = 0;
    pDataParams->bCardType              = PHHAL_HW_CARDTYPE_UNKNOWN;
    pDataParams->wTimingMode            = PHHAL_HW_TIMING_MODE_OFF;
    pDataParams->bTimeoutUnit           = PHHAL_HW_TIME_MICROSECONDS;
    pDataParams->dwTimingUs             = 0;
    pDataParams->wFieldOffTime          = PHHAL_HW_FIELD_OFF_DEFAULT;
    pDataParams->wFieldRecoveryTime     = PHHAL_HW_FIELD_RECOVERY_DEFAULT;
    pDataParams->wMaxPrecachedBytes     = PHHAL_HW_RC523_PRECACHED_BYTES;
    pDataParams->wAdditionalInfo        = 0;
    pDataParams->bRfResetAfterTo        = PH_OFF;
    pDataParams->bRxMultiple            = PH_OFF;
    pDataParams->bActiveMode            = PH_OFF;
    pDataParams->bRfca                  = PH_ON;
    pDataParams->wTargetMode            = PH_OFF;
    pDataParams->bOpeMode               = RD_LIB_MODE_NFC;
    pDataParams->bNfcipMode             = PH_OFF;
    pDataParams->wTxWaitReloadVal       = 0;
    pDataParams->wTimeoutReloadVal      = 0;
    pDataParams->bTxWaitPrescaler       = 0;
    pDataParams->bTimeoutPrescaler      = 0;
    pDataParams->bSkipSend              = PH_OFF;

    return PH_ERR_SUCCESS;
}

#endif /* NXPBUILD__PHHAL_HW_RC523 */
