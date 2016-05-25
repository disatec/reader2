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
* Command functions for Rc523 specific HAL-Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 2673 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  CHu: Generated 19. April 2010
*
*/
#include <phhalHw.h>
#include <phbalReg.h>
#include <ph_RefDefs.h>
#include <ph_Status.h>

#ifdef NXPBUILD__PHHAL_HW_RC523

#include "phhalHw_Rc523_Int.h"
#include <phhalHw_Rc523_Cmd.h>
#include "phhalHw_Rc523_Reg.h"

phStatus_t phhalHw_Rc523_Cmd_Mem(
                                 phhalHw_Rc523_DataParams_t * pDataParams,
                                 uint8_t * pDataIn,
                                 uint8_t bDataLength,
                                 uint8_t * pDataOut
                                 )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bIrq0WaitFor;
    uint8_t     PH_MEMLOC_REM bIrq1WaitFor;
    uint8_t     PH_MEMLOC_REM bIrq0Rq;
    uint8_t     PH_MEMLOC_REM bIrq1Rq;
    uint8_t     PH_MEMLOC_REM bError;
    uint16_t    PH_MEMLOC_REM wFifoBytes;
    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint16_t    PH_MEMLOC_REM wTmpBufferSize;

    /* Check input length */
    if ((bDataLength != 0) && (bDataLength != 25))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Read Path */
    if (bDataLength == 0)
    {
        /* Flush FiFo */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FlushFifo(pDataParams));

        /* start the command */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_MEM));

        /* wait until the command is finished */
        bIrq0WaitFor = PHHAL_HW_RC523_BIT_IDLEI | PHHAL_HW_RC523_BIT_ERRI;
        bIrq1WaitFor = 0x00;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WaitIrq(pDataParams, bIrq0WaitFor, bIrq1WaitFor, &bIrq0Rq, &bIrq1Rq));

        /* Retrieve receive buffer properties */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetRxBuffer(pDataParams, PH_OFF, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

        /* retrieve bytes from FiFo */
        status = phhalHw_Rc523_ReadFifo(pDataParams, wTmpBufferSize, pTmpBuffer, &wFifoBytes);

        /* bail out on error */
        if ((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
        {
            /* Flush FiFo */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FlushFifo(pDataParams));

            return PH_ADD_COMPCODE(status, PH_COMP_HAL);
        }

        if (!(bIrq0Rq & PHHAL_HW_RC523_BIT_ERRI))
        {
            /* Check received length */
            if (wFifoBytes != 25)
            {
                return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
            }

            /* Return data */
            memcpy(pDataOut, pTmpBuffer, wFifoBytes); /* PRQA S 3200 */
        }
    }
    else /* Write Path */
    {
        /* Flush FiFo */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FlushFifo(pDataParams));

        /* Retrieve transmit buffer properties */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetTxBuffer(pDataParams, PH_OFF, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

        /* Check for buffer overflow */
        if (wTmpBufferSize < 25)
        {
            return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
        }

        /* Copy data */
        memcpy(pTmpBuffer, pDataIn, 25);  /* PRQA S 3200 */

        /* Write FIFO */
        status = phhalHw_Rc523_WriteFifo(pDataParams, pTmpBuffer, 25, &wFifoBytes);

        /* bail out on error */
        if ((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
        {
            /* Flush FiFo */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FlushFifo(pDataParams));

            return PH_ADD_COMPCODE(status, PH_COMP_HAL);
        }

        /* start the command */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_MEM));

        /* wait until the command is finished */
        bIrq0WaitFor = PHHAL_HW_RC523_BIT_IDLEI | PHHAL_HW_RC523_BIT_ERRI;
        bIrq1WaitFor = 0x00;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WaitIrq(pDataParams, bIrq0WaitFor, bIrq1WaitFor, &bIrq0Rq, &bIrq1Rq));
    }

    /* Check for errors */
    if (bIrq0Rq & PHHAL_HW_RC523_BIT_ERRI)
    {
        /* read the error register */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_ERROR, &bError));

        /* Fifo write error */
        if (bError & PHHAL_HW_RC523_BIT_WRERR)
        {
            status = PH_ERR_READ_WRITE_ERROR;
        }
        /* buffer overflow */
        if (bError & PHHAL_HW_RC523_BIT_BUFFEROVFL)
        {
            status = PH_ERR_BUFFER_OVERFLOW;
        }
        /* CRC / parity error */
        if ((bError & PHHAL_HW_RC523_BIT_CRCERR) || (bError & PHHAL_HW_RC523_BIT_PARITYERR))
        {
            status = PH_ERR_INTEGRITY_ERROR;
        }
    }

    return PH_ADD_COMPCODE(status, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_Cmd_GenerateRandomId(
    phhalHw_Rc523_DataParams_t * pDataParams
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bIrq0WaitFor = 0x0;
    uint8_t     PH_MEMLOC_REM bIrq1WaitFor = 0x0;
    uint8_t        PH_MEMLOC_REM bIrq0Rq;
    uint8_t        PH_MEMLOC_REM bIrq1Rq;

    /* start the command */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_RANDOMIDS));

    /* wait until the command is finished */
    bIrq0WaitFor = PHHAL_HW_RC523_BIT_IDLEI;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WaitIrq(pDataParams, bIrq0WaitFor,bIrq1WaitFor,&bIrq0Rq,&bIrq1Rq));

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_Cmd_CalcCrc(
                                     phhalHw_Rc523_DataParams_t * pDataParams,
                                     uint8_t * pData,
                                     uint16_t wDataLength,
                                     uint16_t * pCrcResult
                                     )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wFifoBytes;
    uint8_t     PH_MEMLOC_REM bIrq0WaitFor;
    uint8_t     PH_MEMLOC_REM bIrq1WaitFor;
    uint8_t     PH_MEMLOC_REM bIrq0Rq = 0x00;
    uint8_t     PH_MEMLOC_REM bIrq1Rq = 0x00;
    uint8_t        PH_MEMLOC_REM bRegister;
    uint8_t     PH_MEMLOC_REM bError;
    uint8_t        PH_MEMLOC_REM bCRCRes;
    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint16_t    PH_MEMLOC_REM wTmpBufferSize;

    /* Check length */
    if ((wDataLength == 0) || (wDataLength > 64))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Flush FiFo */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FlushFifo(pDataParams));

    /* Retrieve transmit buffer properties */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_GetTxBuffer(pDataParams, PH_OFF, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

    /* Check for buffer overflow */
    if (wTmpBufferSize < wDataLength)
    {
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }

    /* Copy data */
    memcpy(pTmpBuffer, pData, wDataLength);  /* PRQA S 3200 */

    /* Write FIFO */
    status = phhalHw_Rc523_WriteFifo(pDataParams, pTmpBuffer, wDataLength, &wFifoBytes);

    /* bail out on error */
    if ((status & PH_ERR_MASK) != PH_ERR_SUCCESS)
    {
        /* Flush FiFo */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_FlushFifo(pDataParams));

        return PH_ADD_COMPCODE(status, PH_COMP_HAL);
    }

    /* start the command */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_CALCCRC));

    /* wait until the command is finished */
    bIrq0WaitFor = PHHAL_HW_RC523_BIT_IDLEI | PHHAL_HW_RC523_BIT_ERRI;
    bIrq1WaitFor = PHHAL_HW_RC523_BIT_CRCI;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WaitIrq(pDataParams, bIrq0WaitFor, bIrq1WaitFor, &bIrq0Rq, &bIrq1Rq));

    /* Check for errors */
    if (bIrq0Rq & PHHAL_HW_RC523_BIT_ERRI)
    {
        /* read the error register */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_ERROR, &bError));

        /* Fifo write error */
        if (bError & PHHAL_HW_RC523_BIT_WRERR)
        {
            status = PH_ERR_READ_WRITE_ERROR;
        }
        /* buffer overflow */
        if (bError & PHHAL_HW_RC523_BIT_BUFFEROVFL)
        {
            status = PH_ERR_BUFFER_OVERFLOW;
        }
        /* CRC / parity error */
        if ((bError & PHHAL_HW_RC523_BIT_CRCERR) || (bError & PHHAL_HW_RC523_BIT_PARITYERR))
        {
            status = PH_ERR_INTEGRITY_ERROR;
        }
    }
    else
    {
        /* Read status1 register to check if the CRC READY bit is set */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS1, &bRegister));

        /* Read the CRC registers and return the value */
        if (bRegister & PHHAL_HW_RC523_BIT_CRCREADY)
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_CRCRESULT1, &bCRCRes));
            *pCrcResult = (uint16_t)bCRCRes << 8;
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_CRCRESULT2, &bCRCRes));
            *pCrcResult |= bCRCRes;

            status = PH_ERR_SUCCESS;
        }
        else
        {
            status = PH_ERR_INTERNAL_ERROR;
        }
    }

    return PH_ADD_COMPCODE(status, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_Cmd_SoftReset(
                                       phhalHw_Rc523_DataParams_t * pDataParams
                                       )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;

    /* Terminate a probably running command */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_IDLE));

    /* clear all IRQ flags */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(
        pDataParams,
        PHHAL_HW_RC523_REG_COMMIRQ,
        (uint8_t)~(uint8_t)PHHAL_HW_RC523_BIT_IRQINV));

    /* start the command */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_SOFTRESET));

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_WriteRegister(
                                       phhalHw_Rc523_DataParams_t * pDataParams,
                                       uint8_t bAddress,
                                       uint8_t bValue
                                       )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bDataBuffer[2];
    uint16_t    PH_MEMLOC_REM wBytesRead;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;

    /* RS232 protocol */
    if (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_RS232)
    {
        /* clear RD/NWR bit to indicate write operation */
        bAddress &= 0x7FU;
        bNumExpBytes = 1;
    }
    /* SPI protocol */
    else if (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_SPI)
    {
        /* shift address and clear RD/NWR bit to indicate write operation */
        bAddress = (uint8_t)(bAddress << 1);
        bNumExpBytes = 2;
    }
    /* I2C protocol */
    else if (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_I2C)
    {
        /* nothing to be done */
        bNumExpBytes = 0;
    }
    else
    {
        /* Insert Code for other protocols here */
        bNumExpBytes = 0;
    }

    /* Write the address and data */
    bDataBuffer[0] = bAddress;
    bDataBuffer[1] = bValue;
    PH_CHECK_SUCCESS_FCT(statusTmp, phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        2,
        bNumExpBytes,
        bDataBuffer,
        &wBytesRead));

    /* Check number of received bytes */
    if (wBytesRead != bNumExpBytes)
    {
        return PH_ADD_COMPCODE(PH_ERR_INTERFACE_ERROR, PH_COMP_HAL);
    }

    /* RS232 protocol */
    if (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_RS232)
    {
        /* Returned address should match */
        if (bDataBuffer[0] != bAddress)
        {
            return PH_ADD_COMPCODE(PH_ERR_INTERFACE_ERROR, PH_COMP_HAL);
        }
    }
    /* SPI protocol */
    else if (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_SPI)
    {
        /* nothing to be done */
    }
    /* I2C protocol */
    else if (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_I2C)
    {
        /* nothing to be done */
    }
    else
    {
        /* Insert Code for other protocols here */
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_ReadRegister(
                                      phhalHw_Rc523_DataParams_t * pDataParams,
                                      uint8_t bAddress,
                                      uint8_t * pValue
                                      )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bTxBuffer[2];
    uint16_t    PH_MEMLOC_REM wTxLength;
    uint16_t    PH_MEMLOC_REM wBytesRead;
    uint8_t        PH_MEMLOC_REM bNumExpBytes;

    /* RS232 protocol */
    if (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_RS232)
    {
        /* set RD/NWR bit to indicate read operation */
        bTxBuffer[0] = bAddress | 0x80U;
        wTxLength = 1;
        bNumExpBytes = 1;
    }
    /* SPI protocol */
    else if (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_SPI)
    {
        /* set RD/NWR bit to indicate read operation */
        bTxBuffer[0] = (uint8_t)(bAddress << 1) | 0x80U;
        bTxBuffer[1] = 0x00;
        wTxLength = 2;
        bNumExpBytes = 2;
    }
    /* I2C protocol */
    else if (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_I2C)
    {
        /* nothing to be modified */
        bTxBuffer[0] = bAddress;
        wTxLength = 1;
        bNumExpBytes = 1;
    }
    else
    {
        /* Insert Code for other protocols here */
        wTxLength = 0;
        bNumExpBytes = 0;
    }

    /* Write the address and retrieve the register content */
    PH_CHECK_SUCCESS_FCT(statusTmp, phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bTxBuffer,
        wTxLength,
        bNumExpBytes,
        bTxBuffer,
        &wBytesRead));

    /* Check number of received bytes */
    if (wBytesRead != bNumExpBytes)
    {
        return PH_ADD_COMPCODE(PH_ERR_INTERFACE_ERROR, PH_COMP_HAL);
    }

    /* in case of SPI 2 bytes are received from a read */
    if (pDataParams->bBalConnectionType == PHHAL_HW_BAL_CONNECTION_SPI)
    {
        *pValue = bTxBuffer[1];
    }
    else
    {
        *pValue = bTxBuffer[0];
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc523_SetListenParameters(
                                    phhalHw_Rc523_DataParams_t * pDataParams,
                                    uint8_t * pSensRes,
                                    uint8_t * pNfcId1,
                                    uint8_t SelRes,
                                    uint8_t * pPollingResp,
                                    uint8_t bNfcId3
                                    )
{
    uint8_t PH_MEMLOC_BUF aCmd[25];

    memcpy(&aCmd[0], pSensRes, 2);      /* PRQA S 3200 */
    memcpy(&aCmd[2], pNfcId1, 3);       /* PRQA S 3200 */
    memcpy(&aCmd[5], &SelRes, 1);       /* PRQA S 3200 */
    memcpy(&aCmd[6], pPollingResp, 18); /* PRQA S 3200 */
    memcpy(&aCmd[24], &bNfcId3, 1);     /* PRQA S 3200 */

    return phhalHw_Rc523_Cmd_Mem(pDataParams, aCmd, 25, NULL);
}

#endif /* NXPBUILD__PHHAL_HW_RC523 */
