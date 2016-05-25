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
* Internal functions for Rc523 specific HAL-Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4243 $ (NFCRDLIB_V4.010.01.001603 : 4087 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  CHu: Generated 07. October 2010
*
*/
#include <phhalHw.h>
#include <ph_RefDefs.h>

#ifdef NXPBUILD__PHHAL_HW_RC523

#include "phhalHw_Rc523.h"
#include "phhalHw_Rc523_Int.h"
#include "phhalHw_Rc523_Config.h"
#include <phhalHw_Rc523_Reg.h>

/*Registers list (IRQ0, error, FIFO level and control registers) */
static uint8_t phhalHw_Rc523_GetReg[] =
{
    (PHHAL_HW_RC523_REG_COMMIRQ<<1) | (0x01<<7),
    (PHHAL_HW_RC523_REG_ERROR<<1)  | (0x01<<7),
    (PHHAL_HW_RC523_REG_FIFOLEVEL<<1) | (0x01<<7),
    (PHHAL_HW_RC523_REG_CONTROL<<1) | (0x01<<7),
    0x80
};

phStatus_t phhalHw_Rc523_WriteFifo(
                                   phhalHw_Rc523_DataParams_t * pDataParams,
                                   uint8_t * pData,
                                   uint16_t wLength,
                                   uint16_t * pBytesWritten
                                   )
{
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bValue;

    /* Read out the FiFo Level register */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_FIFOLEVEL, &bValue));
    bValue &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_FLUSHBUFFER;

    /* Check if FiFO has enough space */
    if (wLength > (PHHAL_HW_RC523_FIFOSIZE - bValue))
    {
        /* Truncate byte count */
        wLength = PHHAL_HW_RC523_FIFOSIZE - bValue;

        /* Return buffer overflow error */
        status = PH_ERR_BUFFER_OVERFLOW;
    }

    /* Perform actual write to FiFo */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteData(pDataParams, PHHAL_HW_RC523_REG_FIFODATA, pData, wLength));

    /* Return number of written bytes */
    *pBytesWritten = wLength;

    return PH_ADD_COMPCODE(status, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_ReadFifo(
                                  phhalHw_Rc523_DataParams_t * pDataParams,
                                  uint16_t wBufSize,
                                  uint8_t * pData,
                                  uint16_t * pLength
                                  )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bValue;

    /* read out the FiFo Level register */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_FIFOLEVEL, &bValue));
    *pLength = (uint16_t)(bValue & (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_FLUSHBUFFER);

    /* check for buffer overflow */
    if (*pLength > wBufSize)
    {
        /* read maximum possible number of bytes */
        *pLength = wBufSize;

        /* return buffer overflow status */
        status = PH_ERR_BUFFER_OVERFLOW;
    }
    else
    {
        /* Buffer is big enough */
        status = PH_ERR_SUCCESS;
    }

    /* Perform actual reading operation */
    if (*pLength > 0)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadData(pDataParams, PHHAL_HW_RC523_REG_FIFODATA, *pLength, pData));
    }

    /* Clear FIFO on buffer overflow */
    if ((status & PH_ERR_MASK) == PH_ERR_BUFFER_OVERFLOW)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FlushFifo(pDataParams));
    }

    return PH_ADD_COMPCODE(status, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_FlushFifo(
                                   phhalHw_Rc523_DataParams_t * pDataParams
                                   )
{
    return phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_FIFOLEVEL, PHHAL_HW_RC523_BIT_FLUSHBUFFER);
}

phStatus_t phhalHw_Rc523_WriteData(
                                   phhalHw_Rc523_DataParams_t * pDataParams,
                                   uint8_t bAddress,
                                   uint8_t * pData,
                                   uint16_t wLength
                                   )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_COUNT wIndex;
    uint16_t    PH_MEMLOC_REM wBytesRead;
    uint8_t     PH_MEMLOC_REM bBackup;

    /* SPI protocol */
    if (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_SPI)
    {
        /* shift address and clear RD/NWR bit to indicate write operation */
        bAddress = (uint8_t)(bAddress << 1);

        /* decrement the buffer pointer and backup first byte (for chaining) */
        --pData;
        bBackup = pData[0];

        /* add address as first byte */
        pData[0] = bAddress;
        ++wLength;

        /* perform exchange */
        PH_CHECK_SUCCESS_FCT(statusTmp, phbalReg_Exchange(
            pDataParams->pBalDataParams,
            PH_EXCHANGE_DEFAULT,
            pData,
            wLength,
            0,
            NULL,
            &wBytesRead));

        /* restore backuped byte */
        pData[0] = bBackup;

        /* check number of received bytes */
        if (wBytesRead != wLength)
        {
            return PH_ADD_COMPCODE(PH_ERR_INTERFACE_ERROR, PH_COMP_HAL);
        }
    }
    /* I2C protocol */
    else if (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_I2C)
    {
        /* decrement the buffer pointer and backup first byte (for chaining) */
        --pData;
        bBackup = pData[0];

        /* add address as first byte */
        pData[0] = bAddress;
        ++wLength;

        /* perform exchange */
        PH_CHECK_SUCCESS_FCT(statusTmp, phbalReg_Exchange(
            pDataParams->pBalDataParams,
            PH_EXCHANGE_DEFAULT,
            pData,
            wLength,
            0,
            NULL,
            NULL));

        /* restore backuped byte */
        pData[0] = bBackup;
    }
    /* RS232 protocol */
    else
    {
        /* Write each single data byte */
        for (wIndex = 0; wIndex < wLength; ++wIndex)
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, bAddress, pData[wIndex]));
        }
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_ReadData(
                                  phhalHw_Rc523_DataParams_t * pDataParams,
                                  uint8_t bAddress,
                                  uint16_t wLength,
                                  uint8_t * pData
                                  )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_COUNT wIndex;
    uint16_t    PH_MEMLOC_REM wBytesRead;
    uint8_t     PH_MEMLOC_REM aProtocol[1];
    uint8_t     PH_MEMLOC_REM bBackup;

    /* SPI protocol */
    if (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_SPI)
    {
        /* shift address and set RD/NWR bit to indicate read operation */
        bAddress = (uint8_t)(bAddress << 1) | 0x80U;

        /* decrement the buffer pointer and backup first byte (for chaining) */
        --pData;
        bBackup = pData[0];

        /* Write each single data byte */
        memset(pData, (int)bAddress, wLength); /* PRQA S 3200 */

        /* add the 0x00 byte on the end */
        pData[wLength++] = 0x00;

        /* perform exchange */
        PH_CHECK_SUCCESS_FCT(statusTmp, phbalReg_Exchange(
            pDataParams->pBalDataParams,
            PH_EXCHANGE_DEFAULT,
            pData,
            wLength,
            wLength,
            pData,
            &wBytesRead));

        /* restore backuped byte */
        pData[0] = bBackup;

        /* check number of received bytes */
        if (wBytesRead != wLength)
        {
            return PH_ADD_COMPCODE(PH_ERR_INTERFACE_ERROR, PH_COMP_HAL);
        }
    }
    /* I2C protocol */
    else if (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_I2C)
    {
        /* add address as first byte */
        aProtocol[0] = bAddress;

        /* perform exchange */
        PH_CHECK_SUCCESS_FCT(statusTmp, phbalReg_Exchange(
            pDataParams->pBalDataParams,
            PH_EXCHANGE_DEFAULT,
            aProtocol,
            1,
            wLength,
            pData,
            &wBytesRead));

        /* check number of received bytes */
        if (wBytesRead != wLength)
        {
            return PH_ADD_COMPCODE(PH_ERR_INTERFACE_ERROR, PH_COMP_HAL);
        }
    }
    /* RS232 protocol */
    else
    {
        /* Read each single data byte */
        for (wIndex = 0; wIndex < wLength; ++wIndex)
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, bAddress, &pData[wIndex]));
        }
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_LoadTimeout(phhalHw_Rc523_DataParams_t * pDataParams)
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bRegister;

    /* Wait till TxWait timer to elapsed */
    do
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS1, &bRegister));
    }while(bRegister & PHHAL_HW_RC523_BIT_TRUNNUNG);

    /* clear all irq flags */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIRQ, (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_SET));

    /* write Prescaler value */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TPRESCALER, (uint8_t)(pDataParams->bTimeoutPrescaler & 0xFF)));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TMODE, &bRegister));
    bRegister &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_TPRESCALER_HI;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TMODE, bRegister | (uint8_t)((pDataParams->bTimeoutPrescaler >> 8) & 0x0F)));

    /* write Counter value */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TRELOADLO, (uint8_t)(pDataParams->wTimeoutReloadVal & 0xFF)));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TRELOADHI, (uint8_t)((pDataParams->wTimeoutReloadVal >> 8) & 0xFF)));

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_StartTxWaitTimer(phhalHw_Rc523_DataParams_t * pDataParams)
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bRegister;

    /* write Prescaler value */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TPRESCALER, (uint8_t)(pDataParams->bTxWaitPrescaler & 0xFF)));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TMODE, &bRegister));
    bRegister &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_TPRESCALER_HI;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TMODE, bRegister | (uint8_t)((pDataParams->bTxWaitPrescaler >> 8) & 0x0F)));

    /* write Counter value */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TRELOADLO, (uint8_t)(pDataParams->wTxWaitReloadVal & 0xFF)));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TRELOADHI, (uint8_t)((pDataParams->wTxWaitReloadVal >> 8) & 0xFF)));

    /* Disable Timer Irq */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIEN, &bRegister));
    bRegister = bRegister & (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_TIMERI;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIEN, bRegister));

    /* retrieve content of Timer Control register */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_CONTROL, &bRegister));
    /* manually start Timer */
    bRegister |= PHHAL_HW_RC523_BIT_TSTARTNOW;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_CONTROL, bRegister));

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_ExchangeTransmit(
    phhalHw_Rc523_DataParams_t * pDataParams,
    uint8_t bCmdCode,
    uint8_t * pTxBuffer,
    uint16_t wTxLength
    )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wFifoBytes;
    uint8_t     PH_MEMLOC_REM bIrq0WaitFor;
    uint8_t     PH_MEMLOC_REM bIrq1WaitFor;
    uint8_t     PH_MEMLOC_REM bIrq0Rq;
    uint8_t     PH_MEMLOC_REM bIrq1Rq;
    uint8_t     PH_MEMLOC_REM bRegister;
    uint8_t     PH_MEMLOC_REM bStatus1;
    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint16_t    PH_MEMLOC_REM wTmpBufferSize;
    uint16_t    PH_MEMLOC_REM wNumPrecachedBytes;

    if (pDataParams->bActiveMode == PH_ON)
    {
        /* If External RF Field is ON before transmitting then return External RF Error. */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS1, &bStatus1));
        if (bStatus1 & PHHAL_HW_RC523_BIT_RFON)
        {
            status = PH_ERR_EXT_RF_ERROR;
        }

        /* During Active Communication mode if internal RF Field is not ON before transmit then Switch ON the RF Field.
         * Retrieve the content of the TxControl register */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, &bRegister));
        if (!((bRegister & (PHHAL_HW_RC523_BIT_TX1RFEN | PHHAL_HW_RC523_BIT_TX2RFEN)) == (PHHAL_HW_RC523_BIT_TX1RFEN | PHHAL_HW_RC523_BIT_TX2RFEN)))
        {
            /* Switch ON the RF Field. */
            bRegister |= (PHHAL_HW_RC523_BIT_TX1RFEN | PHHAL_HW_RC523_BIT_TX2RFEN);
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, bRegister));
        }
    }

    /* Wait till TxWait is elapsed and load RWT values into timer register. */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_LoadTimeout(pDataParams));

    /* retrieve transmit buffer */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetTxBuffer(pDataParams, PH_ON, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

    /* Precache buffered contents into FIFO */
    wNumPrecachedBytes = (wTmpBufferLen > pDataParams->wMaxPrecachedBytes) ? pDataParams->wMaxPrecachedBytes : wTmpBufferLen;
    if (wNumPrecachedBytes > 0)
    {
        /* Perform actual write to FIFO */
        status = phhalHw_Rc523_WriteFifo(pDataParams, pTmpBuffer, wNumPrecachedBytes, &wFifoBytes);

        /* Ignore FiFo overflow warning */
        if ((status & PH_ERR_MASK) != PH_ERR_BUFFER_OVERFLOW)
        {
            PH_CHECK_SUCCESS(status);
        }

        /* Update buffer pointer and length */
        pTmpBuffer += wFifoBytes;
        wTmpBufferLen = wTmpBufferLen - wFifoBytes;
    }
    else
    {
        status = PH_ERR_SUCCESS;
    }

    /* Precache given buffer contents into FIFO */
    if ((status & PH_ERR_MASK) != PH_ERR_BUFFER_OVERFLOW)
    {
        wNumPrecachedBytes = (wTxLength > (pDataParams->wMaxPrecachedBytes - wNumPrecachedBytes)) ? (pDataParams->wMaxPrecachedBytes - wNumPrecachedBytes) : wTxLength;

        if (wNumPrecachedBytes > 0)
        {
            /* Perform actual write to FIFO */
            status = phhalHw_Rc523_WriteFifo(pDataParams, pTxBuffer, wNumPrecachedBytes, &wFifoBytes);

            /* Ignore FiFo overflow warning */
            if ((status & PH_ERR_MASK) != PH_ERR_BUFFER_OVERFLOW)
            {
                PH_CHECK_SUCCESS(status);
            }

            /* Update buffer pointer and length */
            pTxBuffer += wFifoBytes;
            wTxLength = wTxLength - wFifoBytes;
        }
    }

    if (pDataParams->wTargetMode == PH_OFF)
    {
        /* Write the command */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, bCmdCode));
    }
    else
    {
        /* Retrieve contents of Status2 Register to check for Modem state is in Wait for data State. */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS2, &bRegister));
        /* Check if Modem State not equal to Wait for data State. */
        if((bRegister & PHHAL_HW_RC523_MASK_MODEMBITS) != PHHAL_HW_RC523_BIT_WAIT_FOR_STARTSEND)
        {
            /* Start the Transmit command in Target Mode if Modem status is not in Wait for StartSend.  */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
                    pDataParams,
                    PHHAL_HW_RC523_REG_COMMAND,
                    PHHAL_HW_RC523_CMD_TRANSMIT));
        }
    }

    /* Start sending */
    if ((bCmdCode == PHHAL_HW_RC523_CMD_TRANSCEIVE) || (pDataParams->wTargetMode != PH_OFF))
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_BITFRAMING, &bRegister));
        bRegister |= PHHAL_HW_RC523_BIT_STARTSEND;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_BITFRAMING, bRegister));
    }

    /* Write the remaining data into the Fifo */
    /* (Only possible if interface to RC is fast enough!) */
    while ((wTmpBufferLen != 0) || (wTxLength != 0))
    {
        /* Switch from prechache buffer to given buffer if it's empty */
        if (wTmpBufferLen == 0)
        {
            pTmpBuffer = pTxBuffer;
            wTmpBufferLen = wTxLength;
            wTxLength = 0;
        }
        /* */
        /* Check for FIFO underflow */
        /* */
#ifdef PHHAL_HW_RC523_FEATURE_FIFO_UNDERFLOW_CHECK
        /* read interrupt status */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIRQ, &bIrq0Rq));

        /* If we got data to transmit but the Tx-command aborted, we were too slow! */
        if ((bIrq0Rq & PHHAL_HW_RC523_BIT_IDLEI) || (bIrq0Rq & PHHAL_HW_RC523_BIT_TXI))
        {
            /* stop the command */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
                pDataParams,
                PHHAL_HW_RC523_REG_COMMAND,
                PHHAL_HW_RC523_CMD_IDLE));

            /* Flush FiFo */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FlushFifo(pDataParams));

            return PH_ADD_COMPCODE(PH_ERR_INTERFACE_ERROR, PH_COMP_HAL);
        }
#endif
        /* write remaining data to transmit into FiFo buffer */
        status = phhalHw_Rc523_WriteFifo(pDataParams, pTmpBuffer, wTmpBufferLen, &wFifoBytes);

        /* Ignore FiFo overflow warning */
        if ((status & PH_ERR_MASK) != PH_ERR_BUFFER_OVERFLOW)
        {
            PH_CHECK_SUCCESS(status);
        }

        /* Update buffer pointer and length */
        pTmpBuffer += wFifoBytes;
        wTmpBufferLen = wTmpBufferLen - wFifoBytes;
    }

    /* Set wait IRQs */
    bIrq0WaitFor = PHHAL_HW_RC523_BIT_TXI | PHHAL_HW_RC523_BIT_IDLEI | PHHAL_HW_RC523_BIT_ERRI;
    bIrq1WaitFor = 0x00;

    /* wait until the command is finished */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WaitIrq(
        pDataParams,
        bIrq0WaitFor,
        bIrq1WaitFor,
        &bIrq0Rq,
        &bIrq1Rq));

    /* Check if an error occurred */
    if (bIrq0Rq & PHHAL_HW_RC523_BIT_ERRI)
    {
        /* read the error register */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_ERROR, &bRegister));

        /* buffer overflow */
        if (bRegister & PHHAL_HW_RC523_BIT_BUFFEROVFL)
        {
            status = PH_ERR_BUFFER_OVERFLOW;
        }
        /* Fifo write error */
        else if (bRegister & PHHAL_HW_RC523_BIT_WRERR)
        {
            status = PH_ERR_READ_WRITE_ERROR;
        }
        /* No error */
        else
        {
            status = PH_ERR_SUCCESS;
        }
    }
    /* No error */
    else
    {
        status = PH_ERR_SUCCESS;
    }

    /* Reset TxLastBits */
    if (pDataParams->wCfgShadow[PHHAL_HW_CONFIG_TXLASTBITS] != 0)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXLASTBITS, 0x00));
    }

    /* Reset buffered bytes */
    pDataParams->wTxBufLen = 0;

    return PH_ADD_COMPCODE(status, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_ExchangeReceive(
    phhalHw_Rc523_DataParams_t * pDataParams,
    uint8_t ** ppRxBuffer,
    uint16_t * pRxLength
    )
{
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wFifoBytes;
    uint8_t     PH_MEMLOC_REM bIrq0WaitFor;
    uint8_t     PH_MEMLOC_REM bIrq1WaitFor;
    uint8_t     PH_MEMLOC_REM bIrq0Rq;
    uint8_t     PH_MEMLOC_REM bIrq1Rq;
    uint8_t     PH_MEMLOC_REM bRegister;
    uint8_t     PH_MEMLOC_REM bStatus;
    uint32_t    PH_MEMLOC_REM dwTimingSingle;
    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint16_t    PH_MEMLOC_REM wTmpBufferSize;
    uint16_t    PH_MEMLOC_REM wIndex = 0;
    uint8_t     PH_MEMLOC_REM bFraming = 0;
    uint8_t     PH_MEMLOC_REM bSbBackup = 0;

    /* Set wait IRQs */
    if (pDataParams->wTargetMode != PH_OFF)
    {
        bIrq0WaitFor = PHHAL_HW_RC523_BIT_RXI | PHHAL_HW_RC523_BIT_ERRI | PHHAL_HW_RC523_BIT_HIALERTI | PHHAL_HW_RC523_BIT_IDLEI;
        bIrq1WaitFor = PHHAL_HW_RC523_BIT_EXT_RF_OFF;
    }
    else
    {
        if(pDataParams->bRxMultiple)
        {
            bIrq0WaitFor = PHHAL_HW_RC523_BIT_TIMERI | PHHAL_HW_RC523_BIT_IDLEI;
        }
        else
        {
            bIrq0WaitFor = PHHAL_HW_RC523_BIT_RXI | PHHAL_HW_RC523_BIT_ERRI | PHHAL_HW_RC523_BIT_HIALERTI | PHHAL_HW_RC523_BIT_TIMERI | PHHAL_HW_RC523_BIT_IDLEI;
        }
        bIrq1WaitFor = 0x00;
    }

    /* Wait until the command is finished */
    if(pDataParams->bCheckEmdErr == PH_ON)
    {
        bIrq0WaitFor &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_ERRI;

        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_CheckForEmdError(
            pDataParams,
            bIrq0WaitFor,
            bIrq1WaitFor,
            &bIrq0Rq,
            &bIrq1Rq));
    }
    else
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WaitIrq(
            pDataParams,
            bIrq0WaitFor,
            bIrq1WaitFor,
            &bIrq0Rq,
            &bIrq1Rq));
    }

    /* Timeout handling */
    if ((bIrq0Rq & PHHAL_HW_RC523_BIT_TIMERI) && (pDataParams->wTargetMode == PH_OFF))
    {
        /* Reset after timeout behavior */
        if (pDataParams->bRfResetAfterTo != PH_OFF)
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FieldReset(pDataParams));
        }

        status = PH_ERR_IO_TIMEOUT;

        if (pDataParams->bActiveMode == PH_ON)
        {
            /* Retrieve the content of the TxControl register */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, &bRegister));
            if ((bRegister & (PHHAL_HW_RC523_BIT_TX1RFEN | PHHAL_HW_RC523_BIT_TX2RFEN)) == (PHHAL_HW_RC523_BIT_TX1RFEN | PHHAL_HW_RC523_BIT_TX2RFEN))
            {
                /* If External RF ON Event occurred and Internal Field is ON then Internal Error Should
                 * not be returned, otherwise Internal Error is returned */
                if (!(bIrq1Rq & PHHAL_HW_RC523_BIT_EXT_RF_ON))
                {
                    /* After RWT Timeout if Internal RF is ON (Stuck) then return Internal Error. */
                    status = PH_ERR_INTERNAL_ERROR;
                }
            }
        }
    }
    else
    {
        status = PH_ERR_SUCCESS;
    }

    /* Handle External RF error in case of Target. */
    if ((bIrq1Rq & PHHAL_HW_RC523_BIT_EXT_RF_OFF) && (pDataParams->wTargetMode != PH_OFF))
    {
        if (pDataParams->bActiveMode == PH_OFF)
        {
            status = PH_ERR_EXT_RF_ERROR;
        }
        else
        {
            if ((bIrq0Rq & (PHHAL_HW_RC523_BIT_IDLEI | PHHAL_HW_RC523_BIT_RXI | PHHAL_HW_RC523_BIT_HIALERTI)) == 0)
            {
                /* Receive exited because of External RF OFF without proper (Rx/idle) interrupt, exit Receive API with error. */
                status = PH_ERR_EXT_RF_ERROR;

                /* Reset the contents of TxControl Register. */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXASK, 0x00));
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TXCONTROL, 0x00));

                /* stop the command */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_IDLE));
            }
        }
    }

    /* Incase of Rxmultiple ON, the timer never stops after
       receiving the first frame. There is always a chance of TIMEOUT */
    if ((pDataParams->bRxMultiple == PH_ON) && (status == PH_ERR_IO_TIMEOUT))
    {
        status = PH_ERR_SUCCESS;
    }

    /* Reset receive buffer length */
    pDataParams->wRxBufLen = 0;

    /* Retrieve receive buffer properties */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetRxBuffer(pDataParams, PH_ON, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

    /* Needed for 18092 PAL RX chaining, backup the byte which will be over
     * written by received data. */
    if(pDataParams->bNfcipMode == PH_ON)
    {
        /* Read the RxMode Register to read the BaudRate */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, &bRegister));
        bFraming = bRegister & PHHAL_HW_RC523_MASK_FRAMING;
        bRegister = bRegister & PHHAL_HW_RC523_MASK_SPEED;
        if((bRegister == PHHAL_HW_RC523_BIT_106KBPS) && (bFraming == PHHAL_HW_RC523_BIT_MIFARE))
        {
            bSbBackup = pTmpBuffer[0];
        }
    }

    /* Do the following if no timeout occured */
    if (status == PH_ERR_SUCCESS)
    {
        do
        {
            do
            {
                /* check if there is an error or of modem status went to idle */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIRQ, &bIrq0Rq));
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS2, &bStatus));
            }
            while ((!(bIrq0Rq & bIrq0WaitFor)) && (bStatus & 0x07));

            /* retrieve bytes from FiFo */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadFifo(
                pDataParams,
                wTmpBufferSize,
                &pTmpBuffer[wTmpBufferLen],
                &wFifoBytes));

            /* advance receive buffer */
            wTmpBufferLen = wTmpBufferLen + wFifoBytes;
            wTmpBufferSize = wTmpBufferSize - wFifoBytes;

            /* now see if there was an error or modem status went to idle - but mask out FIFO HiAlert */
        }
        while (((!(bIrq0Rq & (bIrq0WaitFor & (~PHHAL_HW_RC523_BIT_HIALERTI)))) && (bStatus & 0x07)) || (wFifoBytes != 0));

        /* Check for errors */
        if (bIrq0Rq & PHHAL_HW_RC523_BIT_ERRI)
        {
            /* read the error register */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_ERROR, &bRegister));

            /* Fifo write error */
            if (bRegister & PHHAL_HW_RC523_BIT_WRERR)
            {
                status = PH_ERR_READ_WRITE_ERROR;
            }
            /* temperature error */
            else if (bRegister & PHHAL_HW_RC523_BIT_TEMPERR)
            {
                status = PH_ERR_TEMPERATURE_ERROR;
            }
            /* RF error will be triggered in-case any RF issue in Active Mode. */
            else if (bRegister & PHHAL_HW_RC523_BIT_RFERR)
            {
                status = PH_ERR_EXT_RF_ERROR;
            }
            /* buffer overflow */
            else if (bRegister & PHHAL_HW_RC523_BIT_BUFFEROVFL)
            {
                status = PH_ERR_BUFFER_OVERFLOW;
            }
            /* collision */
            else if (bRegister & PHHAL_HW_RC523_BIT_COLLERR)
            {
                /* No bytes received -> timeout */
                if (wTmpBufferLen == 0)
                {
                    status = PH_ERR_IO_TIMEOUT;
                }
                /* Else perform collision handling */
                else
                {
                    status = PH_ERR_COLLISION_ERROR;

                    /* In EMVCo mode, when collision error is detected, we have
                     * to continue with PICC reset. */
                    if(pDataParams->bOpeMode != RD_LIB_MODE_EMVCO)
                    {
                        /* Retrieve collision position */
                        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_COLL, &bRegister));

                        /* check if collpos is valid */
                        if (!(bRegister & PHHAL_HW_RC523_BIT_COLLPOSNOTVALID))
                        {
                            /* retrieve collpos */
                            bRegister &= PHHAL_HW_RC523_MASK_COLLPOS;

                            /* 0 means collision in the 32th bit */
                            if (bRegister == 0)
                            {
                                bRegister = 0x1F;
                            }
                            else
                            {
                                --bRegister;
                            }
                        }
                        else
                        {
                            /* collision position out of range --> max. */
                            bRegister = 0x1F;
                        }

                        /* Restore wRxBufSize */
                        wTmpBufferSize = wTmpBufferSize + wTmpBufferLen;

                        /* Check for buffer overflow */
                        if ((uint16_t)(bRegister >> 3) > wTmpBufferSize)
                        {
                            /* Return the maximum possible size of data */
                            wTmpBufferLen = wTmpBufferSize;
                            status = PH_ERR_BUFFER_OVERFLOW;
                        }
                        else
                        {
                            /* Adjust valid byte count */
                            wTmpBufferLen = (uint16_t)(bRegister >> 3);
                        }

                        /* Mask out valid bits of last byte */
                        bRegister &= PHHAL_HW_RC523_MASK_RXBITS;

                        /* Incomplete byte also counts as received byte */
                        if (bRegister)
                        {
                            /* Check for buffer overflow */
                            if (wTmpBufferLen == wTmpBufferSize)
                            {
                                status = PH_ERR_BUFFER_OVERFLOW;
                            }
                            else
                            {
                                ++wTmpBufferLen;
                            }
                        }

                        /* ValuesAfterColl is enabled -> mask out invalid bits */
                        if (bRegister)
                        {
                            pTmpBuffer[wTmpBufferLen - 1] &= (uint8_t)(0xFF >> (8 - bRegister));
                        }

                        /* Store valid bits of last byte */
                        pDataParams->wAdditionalInfo = bRegister;
                    }
                }
            }
            /* CRC / parity error */
            else if ((bRegister & PHHAL_HW_RC523_BIT_CRCERR) || (bRegister & PHHAL_HW_RC523_BIT_PARITYERR))
            {
                status = PH_ERR_INTEGRITY_ERROR;
            }
            /* protocol error */
            else if (bRegister & PHHAL_HW_RC523_BIT_PROTERR)
            {
                status = PH_ERR_PROTOCOL_ERROR;
            }
            /* No error */
            else
            {
                status = PH_ERR_SUCCESS;
            }
        }
    }

    /* Receive was successful */
    if (status == PH_ERR_SUCCESS)
    {
        /* Set timeout if RX multiple is enabled and received length is zero. */
        if ((pDataParams->bRxMultiple == PH_ON) && (wTmpBufferLen == 0))
        {
            status = PH_ERR_IO_TIMEOUT;
        }
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

    /* If integrity error, wait until receive is finished (to meet TXWAIT) */
    if (status == PH_ERR_INTEGRITY_ERROR)
    {
        /* Read modem state */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(
            pDataParams,
            PHHAL_HW_RC523_REG_STATUS2,
            &bRegister));

        /* Check if modem state is receiving */
        if ((bRegister & 0x07) == 0x06)
        {
            /* Wait until receive is finished */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WaitIrq(
                pDataParams,
                (PHHAL_HW_RC523_BIT_RXI | PHHAL_HW_RC523_BIT_TIMERI | PHHAL_HW_RC523_BIT_IDLEI),
                0x00,
                &bIrq0Rq,
                &bIrq1Rq));
        }
    }

    /* Stop the command / flush the FIFO on error */
    if ((status != PH_ERR_SUCCESS) &&
        (status != PH_ERR_SUCCESS_INCOMPLETE_BYTE))
    {
        /* stop the command */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_IDLE));

        /* Flush FiFo */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FlushFifo(pDataParams));

        /* If target, then restart the transcieve */
        if(pDataParams->wTargetMode != PH_OFF)
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_TRANSCEIVE));
        }

    }

    /* Store received data length in dataparams */
    pDataParams->wRxBufLen = pDataParams->wRxBufStartPos + wTmpBufferLen;

    /* Remove Start Byte (0xF0) during communication at Passive 106kbps during NFCIP exchange. */
    if ((pDataParams->bNfcipMode == PH_ON) &&
        (status == PH_ERR_SUCCESS))
    {
        /* Read the RxMode Register to read the BaudRate */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_RXMODE, &bRegister));
        bFraming = bRegister & PHHAL_HW_RC523_MASK_FRAMING;
        bRegister = bRegister & PHHAL_HW_RC523_MASK_SPEED;
        if((bRegister == PHHAL_HW_RC523_BIT_106KBPS) && (bFraming == PHHAL_HW_RC523_BIT_MIFARE))
        {
            if(pTmpBuffer[0] == PHHAL_HW_RF_TYPE_A_START_BYTE)
            {
                pTmpBuffer[0] = bSbBackup;
                wIndex = 1;
                pDataParams->wRxBufLen--;
            }
            else
            {
                status = PH_ERR_IO_TIMEOUT;
            }
        }
    }

    /* if Frame received responses in multi-slot frame format if multiple reception is enable */
    if( (status == PH_ERR_SUCCESS) && (pDataParams->bRxMultiple))
    {
        statusTmp = phhalHw_Rc523_FrameRxMultiplePkt(pDataParams);
    }
    /* Return RxBuffer pointer */
    if (ppRxBuffer != NULL)
    {
        *ppRxBuffer = pDataParams->pRxBuffer + wIndex;
    }

    /* Return RxBuffer length */
    if (pRxLength != NULL)
    {
        *pRxLength = pDataParams->wRxBufLen;
    }

    /* Timing */
    if ((pDataParams->wTimingMode & (uint16_t)~(uint16_t)PHHAL_HW_TIMING_MODE_OPTION_MASK) == PHHAL_HW_TIMING_MODE_FDT)
    {
        /* Calculate FDT Value */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetFdt(pDataParams, status, &dwTimingSingle));

        /* FDT single -> replace the previous value */
        if (pDataParams->wTimingMode & PHHAL_HW_TIMING_MODE_OPTION_AUTOCLEAR)
        {
            pDataParams->dwTimingUs = dwTimingSingle;
        }
        /* FDT -> add current value to current count */
        else
        {
            /* Check for overflow */
            if ((0xFFFFFFFF - pDataParams->dwTimingUs) < dwTimingSingle)
            {
                pDataParams->dwTimingUs = 0xFFFFFFFF;
            }
            /* Update global timing value */
            else
            {
                pDataParams->dwTimingUs = pDataParams->dwTimingUs + dwTimingSingle;
            }
        }
    }

    if(pDataParams->wTargetMode == PH_OFF)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_StartTxWaitTimer(pDataParams));
    }

    return PH_ADD_COMPCODE(status, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_SetFdt(
                                phhalHw_Rc523_DataParams_t * pDataParams,
                                uint16_t wConfig,
                                uint16_t wTimeout
                                )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    float32_t   PH_MEMLOC_REM fTimeoutUs;
    float32_t   PH_MEMLOC_REM fTemp;
    uint8_t     PH_MEMLOC_REM bRegister;
    uint16_t    PH_MEMLOC_REM wDataRate;
    uint16_t    PH_MEMLOC_REM wPrescaler;
    uint16_t    PH_MEMLOC_REM wReload;

    if((wConfig != PHHAL_HW_TIME_MICROSECONDS) && (wConfig != PHHAL_HW_TIME_MILLISECONDS) &&
       (wConfig != PHHAL_HW_CONFIG_TXWAIT_US) &&  (wConfig != PHHAL_HW_CONFIG_TXWAIT_MS) &&
       (wConfig != PHHAL_HW_CONFIG_TIMEOUT_VALUE_US) && (wConfig != PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Retrieve RxDataRate */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetConfig(pDataParams, PHHAL_HW_CONFIG_RXDATARATE_FRAMING, &wDataRate));
    wDataRate = wDataRate & PHHAL_HW_RF_DATARATE_OPTION_MASK;
    wDataRate++;

    /* Handle timeout in microseconds */
    if((wConfig == PHHAL_HW_TIME_MICROSECONDS) || (wConfig == PHHAL_HW_CONFIG_TXWAIT_US) || (wConfig == PHHAL_HW_CONFIG_TIMEOUT_VALUE_US))
    {
        fTimeoutUs = (float32_t)wTimeout;
    }
    else
    {
        fTimeoutUs = (float32_t)wTimeout * 1000.0f;
    }

    /* Add timershift to timeout value */
    fTimeoutUs = fTimeoutUs + (PHHAL_HW_RC523_TIMER_SHIFT * (PHHAL_HW_RC523_ETU_106 / (float32_t)wDataRate));

    /* Add digital delay for Felica */
    if ((pDataParams->bCardType == PHHAL_HW_CARDTYPE_FELICA_212) ||
        (
        ((pDataParams->bCardType == PHHAL_HW_CARDTYPE_I18092M_ACTIVE_106) ||
        (pDataParams->bCardType == PHHAL_HW_CARDTYPE_I18092M_ACTIVE_212) ||
        (pDataParams->bCardType == PHHAL_HW_CARDTYPE_I18092M_ACTIVE_424)) &&
        (pDataParams->wCfgShadow[PHHAL_HW_CONFIG_RXDATARATE_FRAMING] > PHHAL_HW_RF_DATARATE_106)
        ))
    {
        fTimeoutUs = fTimeoutUs + PHHAL_HW_RC523_FELICA_ADD_DELAY_US;
    }

    /* Max. timeout check */
    if (fTimeoutUs > PHHAL_HW_RC523_TIMER_MAX_VALUE_US)
    {
        return PH_ADD_COMPCODE(PH_ERR_PARAMETER_OVERFLOW, PH_COMP_HAL);
    }

    /* Calculate the prescaler value */
    /* [timeout[us]  = (1 + 2n) / 13.56] */
    fTemp = (((fTimeoutUs * PHHAL_HW_RC523_TIMER_FREQ) / 65535.0f) - 1.0f) / 2.0f;

    /* Except for a special case, the float32_t value will have some commas
    and therefore needs to be "ceiled" */
    if (fTemp > (float32_t)(uint16_t)fTemp)
    {
        wPrescaler = (uint16_t)(fTemp + 1);
    }
    else
    {
        wPrescaler = (uint16_t)fTemp;
    }

    /* Calculate the reload value */
    fTemp = (fTimeoutUs * PHHAL_HW_RC523_TIMER_FREQ) / (1.0f + (2.0f * (float32_t)wPrescaler));

    /* Except for a special case, the float32_t value will have some commas
    and therefore needs to be "ceiled" */
    if (fTemp > (float32_t)(uint16_t)fTemp)
    {
        wReload = (uint16_t)(fTemp + 1);
    }
    else
    {
        wReload = (uint16_t)fTemp;
    }

    if((wConfig == PHHAL_HW_CONFIG_TXWAIT_US) || (wConfig == PHHAL_HW_CONFIG_TXWAIT_MS))
    {
        pDataParams->wTxWaitReloadVal = wReload;
        pDataParams->bTxWaitPrescaler = wPrescaler;

    }
    else if( (wConfig == PHHAL_HW_CONFIG_TIMEOUT_VALUE_US) ||(wConfig == PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS))
    {
        pDataParams->wTimeoutReloadVal = wReload;
        pDataParams->bTimeoutPrescaler = wPrescaler;
    }
    else
    {
        /* write Prescaler value */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TPRESCALER, (uint8_t)(wPrescaler & 0xFF)));
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TMODE, &bRegister));
        bRegister &= (uint8_t)~(uint8_t)PHHAL_HW_RC523_MASK_TPRESCALER_HI;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TMODE, bRegister | (uint8_t)((wPrescaler >> 8) & 0x0F)));

        /* write Counter value */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TRELOADLO, (uint8_t)(wReload & 0xFF)));
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_TRELOADHI, (uint8_t)((wReload >> 8) & 0xFF)));
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_GetFdt(
                                phhalHw_Rc523_DataParams_t * pDataParams,
                                phStatus_t wExchangeStatus,
                                uint32_t * pTime
                                )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bRegister;
    uint16_t    PH_MEMLOC_REM wDataRate;
    uint16_t    PH_MEMLOC_REM wPsReload;
    uint16_t    PH_MEMLOC_REM wTReload;
    uint16_t    PH_MEMLOC_REM wTValue;
    float32_t   PH_MEMLOC_REM fTimeout;

    /* Retrieve RxDataRate */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetConfig(pDataParams, PHHAL_HW_CONFIG_RXDATARATE_FRAMING, &wDataRate));
    wDataRate = wDataRate & PHHAL_HW_RF_DATARATE_OPTION_MASK;
    wDataRate++;

    /* Get prescaler value */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TMODE, &bRegister));
    wPsReload = (uint16_t)(bRegister & PHHAL_HW_RC523_MASK_TPRESCALER_HI) << 8;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TPRESCALER, &bRegister));
    wPsReload |= (uint16_t)bRegister;

    /* Get reload value */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TRELOADHI, &bRegister));
    wTReload = (uint16_t)bRegister << 8;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TRELOADLO, &bRegister));
    wTReload |= (uint16_t)bRegister;

    /* Get counter value */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TCOUNTERVALHI, &bRegister));
    wTValue = (uint16_t)bRegister << 8;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_TCOUNTERVALLO, &bRegister));
    wTValue |= (uint16_t)bRegister;

    /* Subtract reload- and counter values */
    wTValue = wTReload - wTValue;

    /* Calculate the timeout value */
    fTimeout = ((float32_t)wTValue * (1.0f + (2.0f * (float32_t)wPsReload))) / PHHAL_HW_RC523_TIMER_FREQ;

    /* Subtract timer shift from timeout value */
    fTimeout = fTimeout - (PHHAL_HW_RC523_TIMER_SHIFT * (PHHAL_HW_RC523_ETU_106 / (float32_t)wDataRate));

    /* Return the value */
    *pTime = (uint32_t)fTimeout;

    /* Round the value if necessary */
    if ((fTimeout - (float32_t)*pTime) >= 0.5)
    {
        ++(*pTime);
    }

    /* Subtract digital delay from timeout */
    if ((wExchangeStatus & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT)
    {
        /* Felica */
        if ((pDataParams->bCardType == PHHAL_HW_CARDTYPE_FELICA_212) ||
            (
            ((pDataParams->bCardType == PHHAL_HW_CARDTYPE_I18092M_ACTIVE_106) ||
            (pDataParams->bCardType == PHHAL_HW_CARDTYPE_I18092M_ACTIVE_212) ||
            (pDataParams->bCardType == PHHAL_HW_CARDTYPE_I18092M_ACTIVE_424)) &&
            (pDataParams->wCfgShadow[PHHAL_HW_CONFIG_RXDATARATE_FRAMING] > PHHAL_HW_RF_DATARATE_106)
            ))
        {
            if (*pTime > PHHAL_HW_RC523_FELICA_ADD_DELAY_US)
            {
                *pTime = *pTime - PHHAL_HW_RC523_FELICA_ADD_DELAY_US;
            }
        }
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_GetTxBuffer(
                                     phhalHw_Rc523_DataParams_t * pDataParams,
                                     uint8_t bIsExchange,
                                     uint8_t ** pTxBuffer,
                                     uint16_t * pTxBufferLen,
                                     uint16_t * pTxBufferSize
                                     )
{
    /* Exchange command -> RxBuffer overwriting is allowed */
    if (bIsExchange != PH_OFF)
    {
        /* We need to watch that we do not overwrite content below the RxStartPos though */
        if (pDataParams->pTxBuffer == pDataParams->pRxBuffer)
        {
            *pTxBuffer = &pDataParams->pTxBuffer[pDataParams->wRxBufStartPos];
            *pTxBufferSize = pDataParams->wTxBufSize - pDataParams->wRxBufStartPos;
        }
        /* Else just return the actual Buffer */
        else
        {
            *pTxBuffer = pDataParams->pTxBuffer;
            *pTxBufferSize = pDataParams->wTxBufSize;
        }

        /* Return stored length */
        *pTxBufferLen = pDataParams->wTxBufLen;
    }
    /* Normal command -> do not overwrite Tx- and Rx-Contents */
    else
    {
        /* Redundant code */
        return phhalHw_Rc523_GetRxBuffer(pDataParams, bIsExchange, pTxBuffer, pTxBufferLen, pTxBufferSize);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_GetRxBuffer(
                                     phhalHw_Rc523_DataParams_t * pDataParams,
                                     uint8_t bIsExchange,
                                     uint8_t ** pRxBuffer,
                                     uint16_t * pRxBufferLen,
                                     uint16_t * pRxBufferSize
                                     )
{
    /* Exchange command -> TxBuffer overwriting is allowed */
    if (bIsExchange != PH_OFF)
    {
        *pRxBuffer = &pDataParams->pRxBuffer[pDataParams->wRxBufStartPos];
        *pRxBufferSize = pDataParams->wRxBufSize - pDataParams->wRxBufStartPos;
        *pRxBufferLen = pDataParams->wRxBufLen;
    }
    /* Normal command -> do not overwrite Tx- and Rx-Contents */
    else
    {
        /* Command buffering is NOT supported by this HAL */
        *pRxBufferLen = 0;

        /* TxBuffer equals RxBuffer */
        /* And the buffer contains something that needs to be preserved */
        if ((pDataParams->pTxBuffer == pDataParams->pRxBuffer) &&
            ((pDataParams->wTxBufLen != 0) || (pDataParams->wRxBufStartPos != 0)))
        {
            /* Start at TxLenght if neccessary */
            if ((pDataParams->wRxBufStartPos + pDataParams->wTxBufLen) >= pDataParams->wRxBufStartPos)
            {
                *pRxBuffer = &pDataParams->pTxBuffer[pDataParams->wRxBufStartPos + pDataParams->wTxBufLen];
                *pRxBufferSize = pDataParams->wTxBufSize - (pDataParams->wRxBufStartPos + pDataParams->wTxBufLen);
            }
            /* Else start at wRxBufStartPos */
            else
            {
                *pRxBuffer = &pDataParams->pRxBuffer[pDataParams->wRxBufStartPos];
                *pRxBufferSize = pDataParams->wRxBufSize - pDataParams->wRxBufStartPos;
            }
        }
        /* Buffers are different */
        else
        {
            /* Use TxBuffer if it has more space than RxBuffer */
            if ((pDataParams->wTxBufSize - pDataParams->wTxBufLen) >= (pDataParams->wRxBufSize - pDataParams->wRxBufStartPos))
            {
                *pRxBuffer = &pDataParams->pTxBuffer[pDataParams->wTxBufLen];
                *pRxBufferSize = pDataParams->wTxBufSize - pDataParams->wTxBufLen;
            }
            /* Else use RxBuffer */
            else
            {
                *pRxBuffer = &pDataParams->pRxBuffer[pDataParams->wRxBufStartPos];
                *pRxBufferSize = pDataParams->wRxBufSize - pDataParams->wRxBufStartPos;
            }
        }
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

void phhalHw_Rc523_CreateRespFrame(
                                   uint8_t* bInBuff,
                                   uint8_t bDataPos,
                                   uint8_t * bOutBuff
                                   )
{
    uint8_t bStartPos; /* Holds Start position of Out buff */
    uint8_t bTemp;

    bStartPos = 0;

    /* store number of received byte */
    bTemp = bInBuff[bDataPos];
    /* copy Length byte */
    bOutBuff[bStartPos] = bInBuff[bDataPos++];

    /* copy full id*/
    memcpy(&bOutBuff[bStartPos + 1], &bInBuff[bDataPos], (bTemp - 1)); /* PRQA S 3200 */
    /* Update number of received byte in Status Frame */
    bOutBuff[bStartPos + PHHAL_HW_STATUS_FRAME_LEN_BYTE_POS] = bTemp & PHHAL_HW_STATUS_FRAME_LEN_MASK;
    bDataPos = bDataPos + (bTemp - 1);

    /* Update Err byte in Status Frame */
    bTemp = bInBuff[bDataPos++] & PHHAL_HW_STATUS_FRAME_ERR_MASK;

    /* Update Protocol Error Status */
    if((bTemp & PHHAL_HW_RC523_BIT_PROTERR))
    {
        bOutBuff[bStartPos + PHHAL_HW_STATUS_FRAME_ERR_BYTE_POS] |=  (PHHAL_HW_STATUS_FRAME_PROT_ERR_POS | PHHAL_HW_STATUS_FRAME_CL_ERR_POS);
    }

    /* Update Collision Error Status */
    if((bTemp & PHHAL_HW_RC523_BIT_COLLERR))
    {
        bOutBuff[bStartPos + PHHAL_HW_STATUS_FRAME_ERR_BYTE_POS] |=  (PHHAL_HW_STATUS_FRAME_COLL_ERR_POS | PHHAL_HW_STATUS_FRAME_CL_ERR_POS);
    }

    /* Update CRC and Parity Error Status */
    if(bTemp & (PHHAL_HW_RC523_BIT_CRCERR | PHHAL_HW_RC523_BIT_PARITYERR))
    {
        bOutBuff[bStartPos + PHHAL_HW_STATUS_FRAME_ERR_BYTE_POS] |=  (PHHAL_HW_STATUS_FRAME_DATA_ERR_POS | PHHAL_HW_STATUS_FRAME_CL_ERR_POS);
    }
}

phStatus_t phhalHw_Rc523_FrameRxMultiplePkt(
                                            phhalHw_Rc523_DataParams_t * pDataParams
                                            )
{
    uint8_t bIndex;
    uint8_t bFrameCount;
    uint8_t bRespStartPos;
    uint8_t bFrameStartPos;
    uint8_t bTotalRespSize;
    uint8_t bStartPos;
    uint8_t aRxMulFrame[PHHAL_HW_RESP_FRAME_SIZE];

    bIndex          = 0;
    bStartPos       = 0;
    bFrameCount     = 0; /* Number of frame */
    bRespStartPos   = 0; /* Holds Start position on card response in Rxbuffer */
    bFrameStartPos  = 0; /* Holds Start position of frame in Rxbuffer */
    bTotalRespSize  = 0; /* Holds size of rx multiple packet*/

    /* check Minimum length of ReqC response */
    if(pDataParams->wRxBufLen < PHHAL_HW_REQC_MIN_LEN)
    {
        return PH_ADD_COMPCODE(PH_ERR_LENGTH_ERROR, PH_COMP_HAL);
    }

    /* Check Rx buffer size. It should be enough to hold one felica response */
    if(pDataParams->wRxBufSize < PHHAL_HW_RESP_FRAME_SIZE)
    {
        pDataParams->wRxBufLen = 0;
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }

    /* Calculate total frame to be framed
     * and fetch start position of last response in Rxbuffer
     */
    do{
        /* response length + error byte(append by hardware) */
        bStartPos = bStartPos + (pDataParams->pRxBuffer[bStartPos] + 0x01);
        if((bStartPos) <= pDataParams->wRxBufLen)
        {
            /* Frame Number */
            bFrameCount++;
            if((bStartPos) < pDataParams->wRxBufLen)
            {
                /* start position of last response in Rxbuffer */
                bRespStartPos = bStartPos;
            }
        }
    }while((bStartPos) < pDataParams->wRxBufLen);

    if(bFrameCount > 0)
    {
        do{

            /* clear buffer */
            memset(&aRxMulFrame[0], 0x00, sizeof(aRxMulFrame)); /* PRQA S 3200 */

            /* Create response frame */
            phhalHw_Rc523_CreateRespFrame(pDataParams->pRxBuffer, bRespStartPos, aRxMulFrame);

            /* calculate start position of frame in rxbuffer.
             * Update RxBuffer with 32 byte frame response from that position
             */
            bFrameStartPos =  (PHHAL_HW_RESP_FRAME_SIZE) * (bFrameCount - 1);

            /* Check Rxbuffer size to hold response frame */
            if((bFrameStartPos + PHHAL_HW_RESP_FRAME_SIZE) < pDataParams->wRxBufSize)
            {
                memcpy(&pDataParams->pRxBuffer[bFrameStartPos], &aRxMulFrame[0], PHHAL_HW_RESP_FRAME_SIZE); /* PRQA S 3200 */
                /* Update RxDataLen */
                bTotalRespSize = bTotalRespSize + PHHAL_HW_RESP_FRAME_SIZE;
            }

            /* As card response processed decrement frame number */
            bFrameCount--;

            /* calculate start position of next frame in rxbuffer. */
            bIndex = 1;
            bStartPos = 0;
            while(bIndex < bFrameCount)
            {
              bStartPos = bStartPos + (pDataParams->pRxBuffer[bStartPos] + 0x01);
                if((bStartPos) <= bFrameStartPos)
                {
                    bIndex++;
                    bRespStartPos = bStartPos;
                }
            }
            /* If frame number is 1 than, its start position always be 0 */
            if(bFrameCount == 1)
            {
               bRespStartPos = 0;
            }

        }while(bFrameCount !=0);
    }

    /* Update Rx Buffer length to total response size
     * This should always be multiple of 32
     */
    pDataParams->wRxBufLen = bTotalRespSize;

    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Rc523_ReStartRx(
                                   phhalHw_Rc523_DataParams_t * pDataParams
                                   )
{
    phStatus_t PH_MEMLOC_REM statusTmp;
    uint8_t    PH_MEMLOC_REM bRegister;

    /* Load receive command */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_COMMAND,
        PHHAL_HW_RC523_CMD_RECEIVE));

    /* Flush FIFO */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_FIFOLEVEL,
        PHHAL_HW_RC523_BIT_FLUSHBUFFER));

    /* Clear IRQ0 interrupts */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_COMMIRQ,
        ((uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_SET)));

    /* Start timer */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_CONTROL,
        &bRegister));
    bRegister |= PHHAL_HW_RC523_BIT_TSTARTNOW;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_CONTROL,
        bRegister));

    return statusTmp;
}

phStatus_t phhalHw_Rc523_GetMultiReg(
                                     phhalHw_Rc523_DataParams_t * pDataParams,
                                     uint8_t * pData
                                     )
{
    phStatus_t PH_MEMLOC_REM status;
    phStatus_t PH_MEMLOC_REM statusTmp;
    uint16_t   PH_MEMLOC_REM wRxLen = 0;

    /* ON/Enable Multi Reg Read/Write Functionality in Exchange Function */
    PH_CHECK_SUCCESS_FCT(statusTmp, phbalReg_SetConfig(
        pDataParams->pBalDataParams,
        PHBAL_CONFIG_RW_MULTI_REG,
        PH_ON));

    /* Read registers */
    status = phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        phhalHw_Rc523_GetReg,
        sizeof(phhalHw_Rc523_GetReg),
        sizeof(phhalHw_Rc523_GetReg),
        pData,
        &wRxLen);

    /* OFF/Disable Multi Reg Read/Write Functionality in Exchange Function */
    PH_CHECK_SUCCESS_FCT(statusTmp, phbalReg_SetConfig(
        pDataParams->pBalDataParams,
        PHBAL_CONFIG_RW_MULTI_REG,
        PH_OFF));

    return status;
}

phStatus_t phhalHw_Rc523_CheckForEmdError(
                                          phhalHw_Rc523_DataParams_t * pDataParams,
                                          uint8_t bIrq0WaitFor,
                                          uint8_t bIrq1WaitFor,
                                          uint8_t * pIrq0Reg,
                                          uint8_t * pIrq1Reg
                                          )
{
    phOsal_EventType_t PH_MEMLOC_REM dwEventReceived = 0;
    phStatus_t         PH_MEMLOC_REM statusTmp;
    uint8_t            PH_MEMLOC_REM bIrq0EnReg;
    uint8_t            PH_MEMLOC_REM bIrq1EnReg;
    uint8_t            PH_MEMLOC_REM aTempRxBuf[6] = {0};
    uint8_t            PH_MEMLOC_REM bRxBitCtrl;
    uint8_t            PH_MEMLOC_REM bReStartRx = 0;

    /* Parameter check */
    if (!bIrq0WaitFor && !bIrq1WaitFor)
    {
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
    }

#ifndef _WIN32
    /* Clear RF event that might have occurred. */
    PH_CHECK_SUCCESS_FCT(statusTmp, phOsal_Event_Consume(
        E_PH_OSAL_EVT_RF,
        E_PH_OSAL_EVT_SRC_HAL));
#endif

    /* Enable IRQ0 interrupt sources */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_COMMIEN,
        &bIrq0EnReg));

    bIrq0EnReg &= PHHAL_HW_RC523_BIT_IRQINV;
    bIrq0EnReg |= bIrq0WaitFor;

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_COMMIEN,
        bIrq0EnReg));

    /* Enable IRQ1 interrupt sources */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_DIVIEN,
        &bIrq1EnReg));

    bIrq1EnReg &= PHHAL_HW_RC523_BIT_IRQPUSHPULL;
    bIrq1EnReg |= bIrq1WaitFor;

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_DIVIEN,
        bIrq1EnReg));

    /* Wait until RX or timer IRQ occurs */
    do
    {
#ifndef _WIN32
        /* Wait for RF event */
        PH_CHECK_SUCCESS_FCT(statusTmp, phOsal_Event_WaitAny(
             E_PH_OSAL_EVT_RF | E_PH_OSAL_EVT_ABORT,
             PH_MAX_DELAY,
             &dwEventReceived));
#endif
        /* Exit in case of abort event */
        if (dwEventReceived & E_PH_OSAL_EVT_ABORT)
        {
#ifndef _WIN32
            /* Clear abort event. */
            PH_CHECK_SUCCESS_FCT(statusTmp, phOsal_Event_Consume(
                E_PH_OSAL_EVT_ABORT,
                E_PH_OSAL_EVT_SRC_HAL));
#endif
            /* Load IDLE Command and exit with HAL Aborted status. */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
                pDataParams,
                PHHAL_HW_RC523_REG_COMMAND,
                PHHAL_HW_RC523_CMD_IDLE));

            return PH_ADD_COMPCODE(PH_ERR_ABORTED, PH_COMP_HAL);
        }

#ifndef _WIN32
        /* Clear RF event. */
        PH_CHECK_SUCCESS_FCT(statusTmp, phOsal_Event_Consume(
            E_PH_OSAL_EVT_RF,
            E_PH_OSAL_EVT_SRC_HAL));
#endif
        /* Read Registers (IRQ0, error, FIFO level and control registers) */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetMultiReg(
            pDataParams,
            (uint8_t *)&aTempRxBuf));

        /* Bit mask RxLast bits. */
        bRxBitCtrl = aTempRxBuf[4] & PHHAL_HW_RC523_MASK_RXBITS;

        /* Check for EMD */
        if ((PHHAL_HWRC523_EMD_NOISE_CHECK(aTempRxBuf[2]) && (aTempRxBuf[1] & PHHAL_HW_RC523_BIT_RXI)) &&
            (((aTempRxBuf[3] & 0xFF) <  (pDataParams->bEmdNoiseMaxDataLen)) ||
             ((bRxBitCtrl != 0x00) && (aTempRxBuf[3] & 0xFF) <= (pDataParams->bEmdNoiseMaxDataLen))))
        {
            /* Restart reception */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReStartRx(pDataParams));

            /* Continue the loop */
            bReStartRx = true;
        }
        else
        {
            /* Exit loop */
            bReStartRx = false;
        }
    } while (bReStartRx);

    /* Clear IRQ0 interrupt sources */
    bIrq0EnReg &= PHHAL_HW_RC523_BIT_IRQINV;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_COMMIEN,
        bIrq0EnReg));

    /* Clear IRQ1 interrupt sources */
    bIrq1EnReg &= PHHAL_HW_RC523_BIT_IRQPUSHPULL;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_DIVIEN,
        bIrq1EnReg));

    /* Return contents of IRQ0 register */
    *pIrq0Reg = aTempRxBuf[1];

    /* Return contents of IRQ1 register(RFU; not used now; set to 0) */
    *pIrq1Reg = 0x00;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

/**
* \brief Get error from ErrorReg of PN512.
* \retval .
*/
phStatus_t phhalHw_Rc523_GetError(
                                  phhalHw_Rc523_DataParams_t * pDataParams
                                  )
{
    phStatus_t PH_MEMLOC_REM status = PH_ERR_INTERNAL_ERROR;
    uint8_t    PH_MEMLOC_REM bError;

    /* read the error register */
    PH_CHECK_SUCCESS_FCT(status, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_ERROR, &bError));

    /* Fifo write error */
    if (bError & PHHAL_HW_RC523_BIT_WRERR)
    {
        status = PH_ERR_READ_WRITE_ERROR;
    }
    /* temperature error */
    else if (bError & PHHAL_HW_RC523_BIT_TEMPERR)
    {
        status = PH_ERR_TEMPERATURE_ERROR;
    }
    /* RF error will be triggered in-case any RF issue in Active Mode. */
    else if (bError & PHHAL_HW_RC523_BIT_RFERR)
    {
        status = PH_ERR_EXT_RF_ERROR;
    }
    /* buffer overflow */
    else if (bError & PHHAL_HW_RC523_BIT_BUFFEROVFL)
    {
        status = PH_ERR_BUFFER_OVERFLOW;
    }
    /* collision */
    else if (bError & PHHAL_HW_RC523_BIT_COLLERR)
    {
        status = PH_ERR_COLLISION_ERROR;
    }
    /* CRC / parity error */
    else if ((bError & PHHAL_HW_RC523_BIT_CRCERR) || (bError & PHHAL_HW_RC523_BIT_PARITYERR))
    {
        status = PH_ERR_INTEGRITY_ERROR;
    }
    /* protocol error */
    else if (bError & PHHAL_HW_RC523_BIT_PROTERR)
    {
        status = PH_ERR_PROTOCOL_ERROR;
    }
    /* No error */
    else
    {
        status = PH_ERR_INTERNAL_ERROR;
    }

    return status;
}

#endif /* NXPBUILD__PHHAL_HW_RC523 */
