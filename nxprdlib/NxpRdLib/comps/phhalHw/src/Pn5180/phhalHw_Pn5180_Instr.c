/*
*         Copyright (c), NXP Semiconductors
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
* PN5180 Instruction HAL interface.
*
* $Author: nxp62726 $
* $Revision$ (NFCRDLIB_V4.010.03.001609  (NFCRDLIB_V4.010.01.001603 : 172 ))
* $Date: 2015-02-18 13:51:16 +0530 (Wed, 18 Feb 2015) $
*/

#include <ph_RefDefs.h>
#include <ph_Status.h>
#include <phhalHw.h>

#ifdef NXPBUILD__PHHAL_HW_PN5180
#include "phhalHw_Pn5180.h"
#include <phhalHw_Pn5180_Instr.h>
#include "phhalHw_Pn5180_Int.h"
#include <phhalHw_Pn5180_Reg.h>


static phStatus_t phhalHw_Pn5180_Check_Reg_Readonly(uint8_t bRegister);
static void phhalHw_Pn5180_Instr_GetInstrBuffer(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t ** pTxBuffer,
    uint16_t * pTxBufferLen
    );

/* PN5180 READ ONLY Register Table. */
static const uint8_t PH_MEMLOC_CONST_ROM phhalHw_Pn5180_Instr_RO_Reg_Table[] =
{
    IRQ_STATUS,
    TIMER0_STATUS,
    TIMER1_STATUS,
    TIMER2_STATUS,
    RX_STATUS,
    RF_STATUS,
    SYSTEM_STATUS
};

static void phhalHw_Pn5180_Instr_GetInstrBuffer(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t ** pTxBuffer,
    uint16_t * pTxBufferLen
    )
{
    /*Return Instr buffer*/
    *pTxBuffer =pDataParams->pInstrBuffer;
    /* Return stored length */
    *pTxBufferLen = INSTR_BUFFER_SIZE;
}

phStatus_t phhalHw_Pn5180_Instr_Execute(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bInstructionCode,
    uint8_t * pInstrPayload,
    uint16_t wInstrPayloadLength,
    uint16_t wExpectedResponseLength,
    uint8_t ** ppInstrResponse
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint16_t    PH_MEMLOC_REM wBufferLength;

    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint16_t    PH_MEMLOC_REM wTmpBufferSize;
    uint8_t*    PH_MEMLOC_REM pReceivedDataBuffer;

    /* Validate
    * pDataParams is not NULL,
    * pInstrPayload is not NULL,
    * ppInstrResponse is not NULL
    * wInstrPayloadLength ( wInstrPayloadLength is not < 1 and wInstrPayloadLength not > 260 bytes
    * wExpectedResponseLength is not > 260 bytes*/
    if ( (NULL == pDataParams) \
        || (NULL == pInstrPayload)\
        || (NULL == ppInstrResponse) \
        || (wExpectedResponseLength > PHHAL_HW_PN5180_MAX_INST_RESPONSE_LENGTH) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }


    /* Get Buffer*/
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetTxBuffer(pDataParams, PH_ON, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

    /* Check if the buffer size is sufficient*/
    if (wTmpBufferSize < (wInstrPayloadLength + 1))
    {
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetRxBuffer(pDataParams, PH_ON, &pReceivedDataBuffer, &wTmpBufferLen, &wTmpBufferSize));

    /* Check if the buffer size is sufficient*/
    if (wTmpBufferSize < (wExpectedResponseLength))
    {
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }

    /* Build the command frame */
    wBufferLength = 0;
    /* Write Tx Data */
    pTmpBuffer[wBufferLength++] = bInstructionCode;

    /* Copy the data to be transmitted*/
    memcpy(&pTmpBuffer[wBufferLength], pInstrPayload, wInstrPayloadLength);  /* PRQA S 3200 */
    wBufferLength+= wInstrPayloadLength;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        pTmpBuffer,
        wBufferLength,
        wExpectedResponseLength,
        pReceivedDataBuffer,
        &wDataLenTmp));

    /* return the pointer to the buffer*/
    *ppInstrResponse = pReceivedDataBuffer;

    return PH_ERR_SUCCESS;
}

static phStatus_t phhalHw_Pn5180_Check_Reg_Readonly(
    uint8_t bRegister
    )
{
    uint8_t     PH_MEMLOC_REM bIndex;

    /* Validate the registers */
    for (bIndex = 0; bIndex < sizeof(phhalHw_Pn5180_Instr_RO_Reg_Table); bIndex++)
    {
        /* Check if the register is read-only */
        if (bRegister == phhalHw_Pn5180_Instr_RO_Reg_Table[bIndex])
        {
            return PH_ADD_COMPCODE(PH_ERR_READ_WRITE_ERROR, PH_COMP_HAL);
        }
    }
    return PH_ERR_SUCCESS;

}

phStatus_t phhalHw_Pn5180_Instr_WriteRegister(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bRegister,
    uint32_t dwValue
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM bDataBuffer[6];
    uint8_t     PH_MEMLOC_REM pData[1];

    /* Validate pDataParams
    * Check if the Register address is invalid
    * */
    if ( (NULL == pDataParams)\
        ||((bRegister >= PHHAL_HW_PN5180_INVALID_REG_START_ADDRESS) \
        && (bRegister <= PHHAL_HW_PN5180_INVALID_REG_END_ADDRESS)) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Check for read-only registers */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Check_Reg_Readonly(bRegister));


    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_WRITE_REGISTER;
    bDataBuffer[wBufferLength++] = bRegister;
    /* LSB1 */
    bDataBuffer[wBufferLength++] = (uint8_t)(dwValue & 0xFF);
    /* LSB2 */
    bDataBuffer[wBufferLength++] = (uint8_t)((dwValue >> 8) & 0xFF);
    /* MSB1 */
    bDataBuffer[wBufferLength++] = (uint8_t)((dwValue >> 16) & 0xFF);
    /* MSB2 */
    bDataBuffer[wBufferLength++] = (uint8_t)((dwValue >> 24) & 0xFF);

    /* No Response expected*/
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bNumExpBytes,
        pData,
        &wDataLenTmp));

    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_Instr_WriteRegisterOrMask(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bRegister,
    uint32_t dwMask
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM bDataBuffer[6];
    uint8_t     PH_MEMLOC_REM pData[1];

    /* Validate pDataParams
    * Check if the Register address is invalid
    * */
    if ( (NULL == pDataParams)\
        ||((bRegister >= PHHAL_HW_PN5180_INVALID_REG_START_ADDRESS) \
        && (bRegister <= PHHAL_HW_PN5180_INVALID_REG_END_ADDRESS)) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Check for read-only registers */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Check_Reg_Readonly(bRegister));

    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_WRITE_REGISTER_OR_MASK;
    bDataBuffer[wBufferLength++] = bRegister;
    /* LSB1 */
    bDataBuffer[wBufferLength++] = (uint8_t)(dwMask & 0xFF);
    /* LSB2 */
    bDataBuffer[wBufferLength++] = (uint8_t)((dwMask >> 8) & 0xFF);
    /* MSB1 */
    bDataBuffer[wBufferLength++] = (uint8_t)((dwMask >> 16) & 0xFF);
    /* MSB2 */
    bDataBuffer[wBufferLength++] = (uint8_t)((dwMask >> 24) & 0xFF);

    /* No Response expected */
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bNumExpBytes,
        pData,
        &wDataLenTmp));

    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Pn5180_Instr_WriteRegisterAndMask(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bRegister,
    uint32_t dwMask
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM bDataBuffer[6];
    uint8_t     PH_MEMLOC_REM pData[1];

    /* Validate pDataParams
    *  Check if the Register address is invalid */
    if ( (NULL == pDataParams)\
        ||((bRegister >= PHHAL_HW_PN5180_INVALID_REG_START_ADDRESS) \
        && (bRegister <= PHHAL_HW_PN5180_INVALID_REG_END_ADDRESS)) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Check for read-only registers */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Check_Reg_Readonly(bRegister));


    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_WRITE_REGISTER_AND_MASK;
    bDataBuffer[wBufferLength++] = bRegister;
    /* LSB1 */
    bDataBuffer[wBufferLength++] = (uint8_t)(dwMask & 0xFF);
    /* LSB2 */
    bDataBuffer[wBufferLength++] = (uint8_t)((dwMask >> 8) & 0xFF);
    /* MSB1 */
    bDataBuffer[wBufferLength++] = (uint8_t)((dwMask >> 16) & 0xFF);
    /* MSB2 */
    bDataBuffer[wBufferLength++] = (uint8_t)((dwMask >> 24) & 0xFF);

    /* No Response expected */
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bNumExpBytes,
        pData,
        &wDataLenTmp));

    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_Instr_WriteRegisterMultiple(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t * pRegTypeValueSets,
    uint16_t wSizeOfRegTypeValueSets
    )
{
    uint8_t     PH_MEMLOC_REM bReg_offset;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint16_t    PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;

    uint8_t*    PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;

    uint8_t     PH_MEMLOC_REM pReceivedData[1];


    /* Validate
    * pDataParams
    * pRegTypeValueSets
    * wSizeOfRegTypeValueSets
    * wSizeOfRegTypeValueSets for modulus of 6. One set of write register is of 6 byte*/

    if ((NULL == pDataParams) \
        || (NULL == pRegTypeValueSets) \
        || (wSizeOfRegTypeValueSets > PHHAL_HW_PN5180_MAX_REGISTER_TYPE_VALUE_SET) \
        || (wSizeOfRegTypeValueSets < PHHAL_HW_PN5180_MIN_REGISTER_TYPE_VALUE_SET) \
        || ( (wSizeOfRegTypeValueSets % PHHAL_HW_PN5180_MIN_REGISTER_TYPE_VALUE_SET) != 0 ) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }


    /* Validate if the register addresses given in pRegTypeValueSets buffer are not READ only and have valid addresses */
    for (bReg_offset = 0; bReg_offset < wSizeOfRegTypeValueSets; bReg_offset += PHHAL_HW_PN5180_MIN_REGISTER_TYPE_VALUE_SET)
    {
        /* Check for read-only registers */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Check_Reg_Readonly(*(pRegTypeValueSets+bReg_offset)));

        /* Check if the Register address is invalid */
        if((*(pRegTypeValueSets+bReg_offset) >= PHHAL_HW_PN5180_INVALID_REG_START_ADDRESS) \
            && (*(pRegTypeValueSets+bReg_offset) <= PHHAL_HW_PN5180_INVALID_REG_END_ADDRESS)) \
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        /* Check if the operation TYPE are valid -  Type will be present after register address in every set */
        if ( ( *(pRegTypeValueSets+bReg_offset+1) <  PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE ) || \
            ( *(pRegTypeValueSets+bReg_offset+1) >  PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_AND_MASK) )
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
    }
    /* Get Buffer */

    phhalHw_Pn5180_Instr_GetInstrBuffer(pDataParams, &pTmpBuffer, &wTmpBufferLen);

    /* Check if the buffer size is sufficient*/
    if (wTmpBufferLen < (wSizeOfRegTypeValueSets + 1))
    {
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }

    /* Build the command frame */
    wBufferLength = 0;
    pTmpBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_WRITE_REGISTER_MULTIPLE;

    /* Copy the Instruction payload and update the buffer length*/
    memcpy(&pTmpBuffer[wBufferLength], pRegTypeValueSets, wSizeOfRegTypeValueSets);   /* PRQA S 3200 */
    wBufferLength+= wSizeOfRegTypeValueSets;

    /* No Response expected */
    bNumExpBytes = 0;


    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        pTmpBuffer,
        wBufferLength,
        bNumExpBytes,
        pReceivedData,
        &wDataLenTmp));

    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Pn5180_Instr_ReadRegister(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bRegister,
    uint32_t * pValue
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint16_t    PH_MEMLOC_REM wTempRxDataLen;
    uint16_t    PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM bDataBuffer[2];
    uint8_t     PH_MEMLOC_REM bRecBuffer[5];

    /* Validate pDataParams
    * Check if the Register address is invalid*/
    if ( (NULL == pDataParams ) \
        || (NULL == pValue) \
        ||((bRegister >= PHHAL_HW_PN5180_INVALID_REG_START_ADDRESS) \
        && (bRegister <= PHHAL_HW_PN5180_INVALID_REG_END_ADDRESS)) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_GET_INSTR_READ_REGISTER;
    bDataBuffer[wBufferLength++] = bRegister;

    /* Expected number of bytes */
    bNumExpBytes = 4;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bNumExpBytes,
        bRecBuffer,
        &wTempRxDataLen));

    if (wTempRxDataLen != bNumExpBytes)
    {
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
    }

    /* Pack the data that was received */
    *pValue = bRecBuffer[0];
    *pValue |= (((uint32_t) bRecBuffer[1]) << 8);
    *pValue |= (((uint32_t) bRecBuffer[2]) << 16);
    *pValue |= (((uint32_t) bRecBuffer[3]) << 24);

    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_Instr_ReadRegisterMultiple(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t * pRegisters,
    uint8_t bNumOfRegisters,
    uint8_t ** ppValues
    )
{
    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;

    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM bDataBuffer[19];
    uint8_t     PH_MEMLOC_REM regIndex;

    /* Validate pDataParams
    * Validate pRegisters
    * Validate ppValues
    * Validate bNumOfRegisters
    * */
    if ( (NULL == pDataParams) || (NULL == pRegisters) || (NULL == ppValues) \
        || (0 == bNumOfRegisters) \
        || (bNumOfRegisters > PHHAL_HW_PN5180_MAX_REGISTERS_READ_MULTIPLE) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }


    /* Check if all the register addresses are valid */
    for (regIndex = 0; regIndex < bNumOfRegisters; regIndex++)
    {
        /* Check if the Register address is invalid */
        if((pRegisters[regIndex] >= PHHAL_HW_PN5180_INVALID_REG_START_ADDRESS) \
            && (pRegisters[regIndex] <= PHHAL_HW_PN5180_INVALID_REG_END_ADDRESS)) \
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
    }

    /* Get Buffer*/
    phhalHw_Pn5180_Instr_GetInstrBuffer(pDataParams, &pTmpBuffer, &wTmpBufferLen);

    /* total number of expected bytes*/
    bNumExpBytes = (bNumOfRegisters * PHHAL_HW_PN5180_BYTES_PER_REGISTER);

    /* Check if the buffer size is sufficient*/
    if (wTmpBufferLen < bNumExpBytes)
    {
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }
    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_GET_INSTR_READ_REGISTER_MULTIPLE;

    /* Copy the Instruction payload and update the buffer length*/
    memcpy(&bDataBuffer[wBufferLength], pRegisters, bNumOfRegisters);  /* PRQA S 3200 */
    wBufferLength+= bNumOfRegisters;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bNumExpBytes,
        pTmpBuffer,
        &wDataLenTmp));

    /* Validate the response length */
    if (wDataLenTmp  != bNumExpBytes)
    {
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
    }

    /* return the pointer to the buffer*/
    *ppValues = pTmpBuffer;

    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Pn5180_Instr_WriteE2Prom(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bE2PromAddress,
    uint8_t * pDataToWrite,
    uint8_t bDataLength
    )
{
    uint8_t     PH_MEMLOC_REM bE2PLength;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;

    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint8_t     PH_MEMLOC_REM pReceivedData[1];

    /* Validate pDataParams and pDataToWrite
    * Validate E2PROM address
    * Validate the data length */
    if ( (NULL == pDataParams) || (NULL == pDataToWrite) \
        || (bE2PromAddress > PHHAL_HW_PN5180_E2PROM_MAX_ADDESS) \
        || (bE2PromAddress < PHHAL_HW_PN5180_E2PROM_MIN_ADDESS) \
        || (0 == bDataLength) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Calculate maximum writable data from current address*/
    bE2PLength = (PHHAL_HW_PN5180_E2PROM_MAX_ADDESS - bE2PromAddress) + 1;

    /* Validate The E2PROM minimum payload length of 1 byte */
    if (bE2PLength < bDataLength)
    {
        return PH_ADD_COMPCODE(PH_ERR_PARAMETER_OVERFLOW, PH_COMP_HAL);
    }

    /* Get Buffer */
    phhalHw_Pn5180_Instr_GetInstrBuffer(pDataParams, &pTmpBuffer, &wTmpBufferLen);

    /* Check if the buffer size is sufficient*/
    if (wTmpBufferLen < (bDataLength + PHHAL_HW_PN5180_WRITE_E2PROM_FIXED_CMD_BYTES))
    {
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }

    /* Build the command frame */
    wBufferLength = 0;
    pTmpBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_WRITE_E2PROM;
    pTmpBuffer[wBufferLength++] = bE2PromAddress;

    /* Copy the Instruction payload and update the buffer length*/
    memcpy(&pTmpBuffer[wBufferLength], pDataToWrite, bDataLength);  /* PRQA S 3200 */
    wBufferLength += bDataLength;

    /* No Response expected*/
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        pTmpBuffer,
        wBufferLength,
        bNumExpBytes,
        pReceivedData,
        &wDataLenTmp));

    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Pn5180_Instr_ReadE2Prom(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bE2PromAddress,
    uint8_t * pReadData,
    uint8_t bDataLength
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bDataBuffer[3];
    uint8_t     PH_MEMLOC_REM bE2PLength;
    bE2PLength = 0;

    /* Validate pDataParams
    * Validate E2PROM maximum address value
    * Validate the datalength*/
    if ( (NULL == pDataParams) || (NULL == pReadData) \
        || (bE2PromAddress > PHHAL_HW_PN5180_E2PROM_MAX_ADDESS) \
        || (0 == bDataLength) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }


    /* Calculate maximum readable data from current address*/
    bE2PLength = (PHHAL_HW_PN5180_E2PROM_MAX_ADDESS - bE2PromAddress) + 1;

    /* Validate The E2PROM minimum payload length of 1 byte */
    if  (bE2PLength < bDataLength)
    {
        return PH_ADD_COMPCODE(PH_ERR_PARAMETER_OVERFLOW, PH_COMP_HAL);
    }

    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_GET_INSTR_READ_E2PROM;
    bDataBuffer[wBufferLength++] = bE2PromAddress;
    bDataBuffer[wBufferLength++] = bDataLength;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bDataLength,
        pReadData,
        &wDataLenTmp));

    /* Validate the response, if any */
    if (wDataLenTmp  != bDataLength)
    {
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
    }

    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_Instr_WriteTxData(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t * pTxBuffer,
    uint16_t wTxBufferLength
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint16_t    PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;

    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint16_t    PH_MEMLOC_REM wTmpBufferSize;
    uint8_t     PH_MEMLOC_REM pReceivedData[1];

    /* Validate pDataParams
    * Validate transmission buffer length
    * */
    if ( (NULL == pDataParams) || (NULL == pTxBuffer) \
        || (wTxBufferLength > PHHAL_HW_PN5180_TX_DATA_MAX_LENGTH) \
        || (wTxBufferLength < PHHAL_HW_PN5180_TX_DATA_MIN_LENGTH) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Get Buffer*/
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetTxBuffer(pDataParams, PH_ON, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));


    /* Check if the buffer size is sufficient*/
    if (wTmpBufferSize < (wTxBufferLength + PHHAL_HW_PN5180_WRITE_TX_DATA_FIXED_CMD_BYTES))
    {
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }

    /* Build the command frame */
    wBufferLength = 0;
    pTmpBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_WRITE_TX_DATA;

    /* Copy the data to be transmitted*/
    memcpy(&pTmpBuffer[wBufferLength + pDataParams->wTxBufLen], pTxBuffer, wTxBufferLength);  /* PRQA S 3200 */
    wBufferLength+= (wTxBufferLength + pDataParams->wTxBufLen);
    pDataParams->wTxBufStartPos = 0; /* Reset for the next transaction */

    /* No Response expected*/
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        pTmpBuffer,
        wBufferLength,
        bNumExpBytes,
        pReceivedData,
        &wDataLenTmp));

    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_Instr_SendData(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t * pTxBuffer,
    uint16_t wTxBufferLength,
    uint8_t bNumberOfValidBits
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint16_t    PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;

    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint16_t    PH_MEMLOC_REM wTmpBufferSize;
    uint8_t     PH_MEMLOC_REM pReceivedData[1];

    /* Validate pDataParams
    * Validate transmission buffer length
    * To validate bNumberOfValidBits indicate the exact data length to be transmitted.
    * */
    if ( (NULL == pDataParams) || (NULL == pTxBuffer) \
        || (wTxBufferLength > PHHAL_HW_PN5180_TX_DATA_MAX_LENGTH) \
        || (wTxBufferLength < PHHAL_HW_PN5180_TX_DATA_MIN_LENGTH) \
        || (bNumberOfValidBits > PHHAL_HW_PN5180_MAX_NUM_OF_BITS_FOR_LAST_BYTE ) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }


    /* Get Buffer*/
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetTxBuffer(pDataParams, PH_ON, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

    /* Check if the buffer size is sufficient*/
    if (wTmpBufferSize < (wTxBufferLength + PHHAL_HW_PN5180_SEND_DATA_FIXED_CMD_BYTES))
    {
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }

    /* Build the command frame */
    wBufferLength = 0;
    pTmpBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_SEND_DATA;
    pTmpBuffer[wBufferLength++] = bNumberOfValidBits;

    /* Copy the data to be transmitted*/
    memcpy(&pTmpBuffer[wBufferLength + pDataParams->wTxBufLen], pTxBuffer, wTxBufferLength);  /* PRQA S 3200 */
    wBufferLength += (wTxBufferLength + pDataParams->wTxBufLen);
    pDataParams->wTxBufStartPos = 0; /* Reset for the next transaction */

    /* No Response expected*/
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        pTmpBuffer,
        wBufferLength,
        bNumExpBytes,
        pReceivedData,
        &wDataLenTmp));

    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_Instr_RetrieveRxData(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t ** pRxBuffer,
    uint16_t wExpectedRxDataLength
    )
{
    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint16_t    PH_MEMLOC_REM wTmpBufferSize;

    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bDataBuffer[2];

    /* Validate pDataParams
    * Validate reception buffer length*/
    if ( (NULL == pDataParams) || (NULL == pRxBuffer) )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Validate reception buffer length*/
    if ( (wExpectedRxDataLength > PHHAL_HW_PN5180_RX_DATA_MAX_LENGTH) \
        || (wExpectedRxDataLength < PHHAL_HW_PN5180_RX_DATA_MIN_LENGTH) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }

    /* Get Buffer*/
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetRxBuffer(pDataParams, PH_ON, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

    /* Check if the buffer size is sufficient*/
    if (wTmpBufferSize < wExpectedRxDataLength)
    {
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }

    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_GET_INSTR_RETRIEVE_RX_DATA;
    /*bDataBuffer[wBufferLength++] = (uint8_t)wExpectedRxDataLength;*/ /* RFU */
    bDataBuffer[wBufferLength++] = 0; /* RFU */

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        wExpectedRxDataLength,
        pTmpBuffer,
        &wDataLenTmp));

    /* Validate the response, if any */
    if (wDataLenTmp  != wExpectedRxDataLength)
    {
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
    }

    /* return the pointer to the buffer */
    *pRxBuffer = pTmpBuffer;

    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_Instr_SwitchModeStandby(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bWakeupControlMask,
    uint16_t wWakeupCounterInMs
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint16_t    PH_MEMLOC_REM wTempRxDataLen;
    uint16_t    PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM bDataBuffer[5];
    uint8_t     PH_MEMLOC_REM bRecBuffer[1];

    bWakeupControlMask &= PHHAL_HW_PN5180_WAKEUP_CONTROL_MASK;
    /*
    Validate pDataParams
    Validate bWakeupControlMask
    Validate wWakeupCounterInMs
    */
    if ( (NULL == pDataParams) \

        /* bWakeupControlMask is zero */
        || (0x00 == bWakeupControlMask)  \
        /* bWakeupControlMask is not exclusive */
        || ( (bWakeupControlMask & PHHAL_HW_PN5180_WAKEUP_CONTROL_MASK_WUP_ON_COUNTER) \
        &&     (bWakeupControlMask & PHHAL_HW_PN5180_WAKEUP_CONTROL_MASK_WUP_ON_EXT_FIELD) )    \
        /*One of the wakeup method is selected*/
        || ( (bWakeupControlMask & (PHHAL_HW_PN5180_WAKEUP_CONTROL_MASK_WUP_ON_COUNTER|PHHAL_HW_PN5180_WAKEUP_CONTROL_MASK_WUP_ON_EXT_FIELD)) == 0 ) \

        /* bWakeupControlMask is configured for PHHAL_HW_PN5180_WAKEUP_CONTROL_MASK_WUP_ON_COUNTER */
        || ( (bWakeupControlMask & PHHAL_HW_PN5180_WAKEUP_CONTROL_MASK_WUP_ON_COUNTER) \
        /* wWakeupCounterInMs is zero or more than the maximum value */
        &&     ( (0x00 == wWakeupCounterInMs) || (wWakeupCounterInMs > PHHAL_HW_PN5180_MAX_WAKEUP_COUNTER) )    \
        )
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_SWITCH_MODE;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_SWITCH_MODE_STANDBY;
    bDataBuffer[wBufferLength++] = bWakeupControlMask;
    /* wWakeupCounterInMs LSB */
    bDataBuffer[wBufferLength++] = (uint8_t)(wWakeupCounterInMs & 0xFF);
    /* wWakeupCounterInMs MSB */
    bDataBuffer[wBufferLength++] = (uint8_t)((wWakeupCounterInMs >> 8) & 0xFF);

    /* Expected number of bytes */
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bNumExpBytes,
        bRecBuffer,
        &wTempRxDataLen));

    /* not yet implemented */
    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Pn5180_Instr_SwitchModeLpcd(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint16_t wWakeupCounterInMs
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint16_t    PH_MEMLOC_REM wTempRxDataLen;
    uint16_t    PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM bDataBuffer[4];
    uint8_t     PH_MEMLOC_REM bRecBuffer[1];

    /*
    Validate pDataParams
    Validate wWakeupCounterInMs
    */
    if ( (NULL == pDataParams) \
        /* wWakeupCounterInMs is zero */
        || (wWakeupCounterInMs == 0) \
        /* wWakeupCounterInMs is more than the maximum value */
        || (wWakeupCounterInMs > PHHAL_HW_PN5180_MAX_WAKEUP_COUNTER)    \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_SWITCH_MODE;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_SWITCH_MODE_LPCD;

    /* wWakeupCounterInMs LSB */
    bDataBuffer[wBufferLength++] = (uint8_t)(wWakeupCounterInMs & 0xFF);
    /* wWakeupCounterInMs MSB */
    bDataBuffer[wBufferLength++] = (uint8_t)((wWakeupCounterInMs >> 8) & 0xFF);

    /* Expected number of bytes */
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bNumExpBytes,
        bRecBuffer,
        &wTempRxDataLen));

    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Pn5180_Instr_SwitchModeAutocoll(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bRfTechnology,
    uint8_t bAutocollMode
    )
{

    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint16_t    PH_MEMLOC_REM wTempRxDataLen;
    uint16_t    PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM bDataBuffer[4];
    uint8_t     PH_MEMLOC_REM bRecBuffer[1];

    bRfTechnology &= PHHAL_HW_PN5180_AUTOCOLL_RF_TECHNOLOGY_MASK;
    /* Validate pDataParams
    Validate bRfTechnologyMask
    Validate bAutocollMode
    */
    if ( (NULL == pDataParams)  \
        || (0 == bRfTechnology) \
        || ( bAutocollMode > 2) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }


    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_SWITCH_MODE;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_SWITCH_MODE_AUTOCOLL;

    /* RF technologies  */
    bDataBuffer[wBufferLength++] = bRfTechnology;
    /* mode of autocoll*/
    bDataBuffer[wBufferLength++] = bAutocollMode;


    /* Expected number of bytes */
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bNumExpBytes,
        bRecBuffer,
        &wTempRxDataLen));

    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Pn5180_Instr_SwitchModeNormal(phhalHw_Pn5180_DataParams_t * pDataParams)
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint16_t    PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM bDataBuffer[2];
    uint8_t     PH_MEMLOC_REM bRecBuffer[1];
    uint16_t    PH_MEMLOC_REM wTempRxDataLen;
    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_SWITCH_MODE;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_SWITCH_MODE_NORMAL;

    /* Expected number of bytes */
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bNumExpBytes,
        bRecBuffer,
        &wTempRxDataLen));

    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Pn5180_Instr_MifareAuthenticate(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t * pKey,
    uint8_t bKeyType,
    uint8_t bBlockNo,
    uint8_t * pUid
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint16_t    PH_MEMLOC_REM wBufferLength;

    uint8_t    PH_MEMLOC_BUF aCmdBuffer[13];
    uint8_t    PH_MEMLOC_BUF aRspBuffer[1];
    uint16_t   PH_MEMLOC_REM bNumExpBytes;
    uint32_t   dwRegister;

    /* Validate pDataParams
    Validate pKey
    Validate pUid
    Validate bKeyType
    */
    if ( (NULL == pDataParams)  \
        || (NULL == pKey)   \
        || (NULL == pUid)   \
        || ((bKeyType != PHHAL_HW_MFC_KEYA) && (bKeyType != PHHAL_HW_MFC_KEYB)) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    bNumExpBytes = 1;

    /* Build the command frame */
    wBufferLength = 0;
    /* Write Tx Data */
    aCmdBuffer[wBufferLength++] = PHHAL_HW_PN5180_GET_INSTR_MFC_AUTHENTICATE;
    /* Copy the data to be transmitted*/
    memcpy(&aCmdBuffer[wBufferLength], pKey, PHHAL_HW_MFC_KEY_LENGTH);  /* PRQA S 3200 */
    wBufferLength += PHHAL_HW_MFC_KEY_LENGTH;

    if(bKeyType == PHHAL_HW_MFC_KEYA)
    {
        aCmdBuffer[wBufferLength++] = PHHAL_HW_PN5180_INSTR_MFC_AUTHENTICATE_KEY_TYPE_A;
    }
    else
    {
        aCmdBuffer[wBufferLength++] = PHHAL_HW_PN5180_INSTR_MFC_AUTHENTICATE_KEY_TYPE_B;
    }

    aCmdBuffer[wBufferLength++] = bBlockNo;
    memcpy(&aCmdBuffer[wBufferLength], pUid, PHHAL_HW_PN5180_MFC_UID_LEN);  /* PRQA S 3200 */
    wBufferLength += PHHAL_HW_PN5180_MFC_UID_LEN;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        aCmdBuffer,
        wBufferLength,
        bNumExpBytes,
        aRspBuffer,
        &wDataLenTmp));

    /* Validate the response */
    if(wDataLenTmp == 0)
    {
        status = PH_ERR_INTERNAL_ERROR;
    }
    if(aRspBuffer[0] & 0x01)
    {
        status = PH_ERR_AUTH_ERROR;
    }
    else if(aRspBuffer[0] & 0x02)
    {
        status = PH_ERR_IO_TIMEOUT;
    }
    else
    {
        /* Check auth success */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_ReadRegister(pDataParams, SYSTEM_CONFIG, &dwRegister));
        if(!(dwRegister & SYSTEM_CONFIG_MFC_CRYPTO_ON_MASK))
        {
            status = PH_ERR_AUTH_ERROR;
            pDataParams->bMfcCryptoEnabled = PH_OFF;
        }
        else
        {
            pDataParams->bMfcCryptoEnabled = PH_ON;
        }
    }

    return PH_ADD_COMPCODE(status, PH_COMP_HAL);
}


phStatus_t phhalHw_Pn5180_Instr_EpcGen2Inventory(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t * pSelectCommand,
    uint8_t bSelectCommandLength,
    uint8_t bSelectCommandBitsInLastByte,
    uint8_t * pBeginRoundCommand,
    uint8_t bTimeslotProcessingBehavior
    )
{

    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint16_t    PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;

    uint8_t*    PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint8_t     PH_MEMLOC_REM pReceivedData[1];


    /* Validate pDataParams
    Validate pBeginRoundCommand
    Validate bTimeslotProcessingBehavior */

    if ( (NULL == pDataParams) \
        || (NULL == pBeginRoundCommand)
        || (bTimeslotProcessingBehavior > PHHAL_HW_PN5180_MAX_EPC_GEN2_TIMESLOT)
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Validate the parameters of Select command */
    if (bSelectCommandLength)
    {
        /* Validate pSelectCommand
        Validate bSelectCommandLength
        Validate bSelectCommandBitsInLastByte */
        if (                            \
            (NULL == pSelectCommand)    \
            || (bSelectCommandLength > PHHAL_HW_PN5180_MAX_SELECT_COMMAND_LENGTH)    \
            ||  (bSelectCommandBitsInLastByte > PHHAL_HW_PN5180_MAX_NUM_OF_BITS_FOR_LAST_BYTE) \
            )
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
    }

    /* Get Buffer */
    phhalHw_Pn5180_Instr_GetInstrBuffer(pDataParams, &pTmpBuffer, &wTmpBufferLen);

    /* Check if the buffer size is sufficient*/
    if (wTmpBufferLen < 46)
    {
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }

    /* Build the command frame */
    wBufferLength = 0;
    pTmpBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_EPC_GEN2_INVENTORY;
    pTmpBuffer[wBufferLength++] = bSelectCommandLength;

    /*
    No Select command is set prior to BeginRound command.
    ‘Valid Bits in last Byte’ field and ‘Select command’ field shall not be present.
    */
    if(bSelectCommandLength)
    {
        pTmpBuffer[wBufferLength++] = bSelectCommandBitsInLastByte;

        /* Copy the Instruction payload and update the buffer length*/
        memcpy(&pTmpBuffer[wBufferLength], pSelectCommand, bSelectCommandLength);   /* PRQA S 3200 */
        wBufferLength+= bSelectCommandLength;
    }
    /* Copy the Instruction payload and update the buffer length*/
    memcpy(&pTmpBuffer[wBufferLength], pBeginRoundCommand, PHHAL_HW_PN5180_BEGIN_COMMAND_LENGTH);   /* PRQA S 3200 */
    wBufferLength+= PHHAL_HW_PN5180_BEGIN_COMMAND_LENGTH;

    pTmpBuffer[wBufferLength++] = bTimeslotProcessingBehavior;

    /* No Response expected */
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        pTmpBuffer,
        wBufferLength,
        bNumExpBytes,
        pReceivedData,
        &wDataLenTmp));

    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_Instr_EpcGen2RetrieveResultSize(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint16_t * pResultSize
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint16_t    PH_MEMLOC_REM wTempRxDataLen;
    uint16_t    PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM bDataBuffer[2];
    uint8_t     PH_MEMLOC_REM bRecBuffer[2];

    /*
    Validate pDataParams
    Validate pResultSize
    */
    if ( (NULL == pDataParams)    \
        || (NULL == pResultSize)    \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }


    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_GET_INSTR_EPC_GEN2_RETRIEVE_INVENTORY_RESULT_SIZE;
    bDataBuffer[wBufferLength++] = 0;   /*RFU*/

    /* Expected number of bytes */
    bNumExpBytes = 2;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bNumExpBytes,
        bRecBuffer,
        &wTempRxDataLen));

    /* Validate the response, if any */
    if (wTempRxDataLen != bNumExpBytes)
    {
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
    }

    /* Pack the data that was received */
    *pResultSize = bRecBuffer[0];
    *pResultSize |= (((uint16_t) bRecBuffer[1]) << 8);


    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_Instr_EpcGen2RetrieveResult(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t ** ppInventoryResult,
    uint16_t wResultSize
    )
{
    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;

    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bDataBuffer[2];

    /* Validate pDataParams */
    /* Validate ppInventoryResult */
    if (                        \
        (NULL == pDataParams)   \
        || (NULL == ppInventoryResult)  \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }


    /* Validate wResultSize */
    if (0x00 == wResultSize)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_GET_INSTR_EPC_GEN2_RETRIEVE_INVENTORY_RESULT;
    bDataBuffer[wBufferLength++] = 0;   /*RFU*/

    /* Get Buffer*/
    phhalHw_Pn5180_Instr_GetInstrBuffer(pDataParams, &pTmpBuffer, &wTmpBufferLen);

    /* Check if the buffer size is sufficient*/
    if (wTmpBufferLen < wResultSize)
    {
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        wResultSize,
        pTmpBuffer,
        &wDataLenTmp));

    /* Validate the response, if any */
    if (wDataLenTmp  != wResultSize)
    {
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
    }
    /* return the pointer to the buffer*/
    *ppInventoryResult = pTmpBuffer;

    return PH_ERR_SUCCESS;

}


phStatus_t phhalHw_Pn5180_Instr_EpcGen2ResumeInventory(
    phhalHw_Pn5180_DataParams_t * pDataParams)
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint16_t    PH_MEMLOC_REM wTempRxDataLen;
    uint16_t    PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM bDataBuffer[2];
    uint8_t     PH_MEMLOC_REM bRecBuffer[1];

    /* Validate pDataParams */
    if (NULL == pDataParams)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_EPC_GEN2_RESUME_INVENTORY;
    bDataBuffer[wBufferLength++] = 0;   /*RFU*/

    /* Expected number of bytes */
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bNumExpBytes,
        bRecBuffer,
        &wTempRxDataLen));

    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_Instr_LoadRfConfiguration(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bRfTxConfiguration,
    uint8_t bRfRxConfiguration
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM bDataBuffer[3];
    uint8_t     PH_MEMLOC_REM pData[1];

    /* Validate pDataParams
    *  Validate bRfTxConfiguration
    * bRfTxConfiguration is greater than the maximum RF TX configuration
    * bRfTxConfiguration is not the PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX
    * Validate bRfRxConfiguration
    * bRfRxConfiguration is lesser than the minimum RF RX configuration
    * bRfRxConfiguration is greater than the maximum RF RX configuration
    * and bRfRxConfiguration is not the PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX*/
    if ((NULL == pDataParams) \
        || ((bRfTxConfiguration > PHHAL_HW_PN5180_MAX_RF_TX_CONFIGURATION_INDEX) \
        && (bRfTxConfiguration != PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX)) \
        || (bRfRxConfiguration < PHHAL_HW_PN5180_MIN_RF_RX_CONFIGURATION_INDEX) \
        || ((bRfRxConfiguration > PHHAL_HW_PN5180_MAX_RF_RX_CONFIGURATION_INDEX) \
        && (bRfRxConfiguration != PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX)) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }
    /*bRfTxConfiguration == bRfRxConfiguration == PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX then no change required*/
    if((bRfTxConfiguration & bRfRxConfiguration) == PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX)
    {
        return PH_ERR_SUCCESS;
    }


    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_LOAD_RF_CONFIGURATION;
    bDataBuffer[wBufferLength++] = bRfTxConfiguration;
    bDataBuffer[wBufferLength++] = bRfRxConfiguration;

    /* No Response expected*/
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,
        phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bNumExpBytes,
        pData,
        &wDataLenTmp));

    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_Instr_UpdateRfConfiguration(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t * pRfConfiguration,
    uint8_t bRfConfigurationSize
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;

    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint8_t     PH_MEMLOC_REM pReceivedData[1];

    /* Validate pDataParams
    * Validate pRfConfiguration
    *  Validate bRfConfigurationSize
    * bRfConfigurationSize is not a multiple of PHHAL_HW_PN5180_RF_CONFIGURATION_SIZE
    * bRfConfigurationSize is greater than PHHAL_HW_PN5180_MAX_RF_CONFIGURATION_SIZE
    * */
    if ( (NULL == pDataParams) \
        || (NULL == pRfConfiguration) \
        || (bRfConfigurationSize > PHHAL_HW_PN5180_MAX_RF_CONFIGURATION_SIZE) \
        || (0 != (bRfConfigurationSize % PHHAL_HW_PN5180_RF_CONFIGURATION_SIZE)) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }


    /* Get Buffer */
    phhalHw_Pn5180_Instr_GetInstrBuffer(pDataParams, &pTmpBuffer, &wTmpBufferLen);

    /* Check if the buffer size is sufficient*/
    if (wTmpBufferLen < (bRfConfigurationSize + PHHAL_HW_PN5180_UPDATE_RF_CONFIG_FIXED_CMD_BYTES))
    {
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }
    /* Build the command frame */
    wBufferLength = 0;
    pTmpBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_UPDATE_RF_CONFIGURATION;

    /* Copy the Instruction payload and update the buffer length */
    memcpy(&pTmpBuffer[wBufferLength], pRfConfiguration, bRfConfigurationSize);  /* PRQA S 3200 */
    wBufferLength+= bRfConfigurationSize;

    /* No Response expected*/
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        pTmpBuffer,
        wBufferLength,
        bNumExpBytes,
        pReceivedData,
        &wDataLenTmp));

    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_Instr_RetrieveRfConfigurationSize(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bRfConfiguration,
    uint8_t * pNumOfRegisters
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM bDataBuffer[2];

    /* Validate pDataParams
    * Validate pNumOfRegisters
    * Validate bRfRxConfiguration
    * configurations from 0x1D to 0x7F not valid
    * bRfRxConfiguration is greater than the maximum RF RX configuration
    * bRfRxConfiguration is not the PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX
    * */
    if ( (NULL == pDataParams) \
        || (NULL == pNumOfRegisters) \
        || ((bRfConfiguration < PHHAL_HW_PN5180_MIN_RF_RX_CONFIGURATION_INDEX) \
        && (bRfConfiguration > PHHAL_HW_PN5180_MAX_RF_TX_CONFIGURATION_INDEX)) \
        || (bRfConfiguration > PHHAL_HW_PN5180_MAX_RF_RX_CONFIGURATION_INDEX)  \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_GET_INSTR_RETRIEVE_RF_CONFIGURATION_SIZE;
    bDataBuffer[wBufferLength++] = bRfConfiguration;

    /* total number of expected bytes*/
    bNumExpBytes = 1;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bNumExpBytes,
        pNumOfRegisters,
        &wDataLenTmp));

    /* Validate the response, if any */
    if (wDataLenTmp  != bNumExpBytes)
    {
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
    }

    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_Instr_RetrieveRfConfiguration(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bRfConfiguration,
    uint8_t ** ppRfConfiguration,
    uint8_t bNumOfRegisters
    )
{
    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;

    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM bDataBuffer[2];

    /* Validate pDataParams
    * Validate ppRfConfiguration
    * Validate bRfConfiguration
    * configurations from 0x1D to 0x7F not valid
    * bRfRxConfiguration is greater than the maximum RF RX configuration
    * bRfRxConfiguration is not the PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX
    * Validate bNumOfRegisters*/
    if (
        (NULL == pDataParams)\
        || (NULL == ppRfConfiguration) \
        || ((bRfConfiguration < PHHAL_HW_PN5180_MIN_RF_RX_CONFIGURATION_INDEX) \
        && (bRfConfiguration > PHHAL_HW_PN5180_MAX_RF_TX_CONFIGURATION_INDEX)) \
        || (bRfConfiguration > PHHAL_HW_PN5180_MAX_RF_RX_CONFIGURATION_INDEX)  \
        || (0 == bNumOfRegisters) \
        || (bNumOfRegisters > PHHAL_HW_PN5180_MAX_RF_REGS) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }


    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_GET_INSTR_RETRIEVE_RF_CONFIGURATION;
    bDataBuffer[wBufferLength++] = bRfConfiguration;

    /* total number of expected bytes*/
    bNumExpBytes = bNumOfRegisters * PHHAL_HW_PN5180_RETRIEVE_RF_EACH_CONFIGURATION;

    /* Get Buffer*/
    phhalHw_Pn5180_Instr_GetInstrBuffer(pDataParams, &pTmpBuffer, &wTmpBufferLen);

    /* Check if the buffer size is sufficient*/
    if (wTmpBufferLen < bNumExpBytes)
    {
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }
    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bNumExpBytes,
        pTmpBuffer,
        &wDataLenTmp));

    /* Validate the response, if any */
    if (wDataLenTmp  != bNumExpBytes)
    {
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
    }
    /* return the pointer to the buffer*/
    *ppRfConfiguration = pTmpBuffer;

    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_Instr_RfOn(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bRfOnConfig
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM bDataBuffer[2];
    uint8_t     PH_MEMLOC_REM pData[1];

    /* Validate pDataParams
    * Validate bRfOnConfig
    *
    * */
    if ( (NULL == pDataParams) \
        || (bRfOnConfig > 3) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Build the command frame */

    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_GET_INSTR_FIELD_ON;
    bDataBuffer[wBufferLength++] = bRfOnConfig;

    /* No Response expected*/
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,
        phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bNumExpBytes,
        pData,
        &wDataLenTmp));

    return PH_ERR_SUCCESS;

}


phStatus_t phhalHw_Pn5180_Instr_RfOff(
    phhalHw_Pn5180_DataParams_t * pDataParams
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM bDataBuffer[2];
    uint8_t     PH_MEMLOC_REM pData[1];

    /* Validate pDataParams */
    if (NULL == pDataParams)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Build the command frame */
    wBufferLength = 0;
    bDataBuffer[wBufferLength++] = PHHAL_HW_PN5180_GET_INSTR_FIELD_OFF;
    bDataBuffer[wBufferLength++] = 0;

    /* No Response expected*/
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,
        phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        bDataBuffer,
        wBufferLength,
        bNumExpBytes,
        pData,
        &wDataLenTmp));

    return PH_ERR_SUCCESS;

}


phStatus_t phhalHw_Pn5180_Instr_ConfigureTestBusDigital(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bTBSelect,
    uint8_t * pTBpos,
    uint8_t bTBposSize
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;

    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint8_t     PH_MEMLOC_REM pReceivedData[1];

    /* Validate pDataParams */
    if ((NULL == pDataParams)
        || ((NULL == pTBpos) && (bTBposSize > 0))\
        || (bTBposSize > 6)
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }
    /* Get Buffer */
    phhalHw_Pn5180_Instr_GetInstrBuffer(pDataParams, &pTmpBuffer, &wTmpBufferLen);

    /* Check if the buffer size is sufficient*/
    if (wTmpBufferLen < (bTBposSize + PHHAL_HW_PN5180_CONFIGURE_TESTBUS_DIGITAL_FIXED_CMD_BYTES))
    {
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }
    /* Build the command frame */
    wBufferLength = 0;
    pTmpBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_CONFIGURE_TESTBUS_DIGITAL;
    pTmpBuffer[wBufferLength++] = bTBSelect;

    /* Copy the Instruction payload and update the buffer length */
    memcpy(&pTmpBuffer[wBufferLength], pTBpos, bTBposSize);  /* PRQA S 3200 */
    wBufferLength+= bTBposSize;

    /* No Response expected*/
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        pTmpBuffer,
        wBufferLength,
        bNumExpBytes,
        pReceivedData,
        &wDataLenTmp));

    return PH_ERR_SUCCESS;

}

phStatus_t phhalHw_Pn5180_Instr_ConfigureTestBusAnalog(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bDAC1Config,
    uint8_t bDAC0Config

    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM wBufferLength;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;

    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint8_t     PH_MEMLOC_REM pReceivedData[1];

    /* Validate pDataParams */
    if (NULL == pDataParams)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Get Buffer */
    phhalHw_Pn5180_Instr_GetInstrBuffer(pDataParams, &pTmpBuffer, &wTmpBufferLen);

    /* Check if the buffer size is sufficient*/
    if (wTmpBufferLen < (PHHAL_HW_PN5180_CONFIGURE_TESTBUS_ANALOG_CMD_BYTES))
    {
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }

    /* Build the command frame */
    wBufferLength = 0;
    pTmpBuffer[wBufferLength++] = PHHAL_HW_PN5180_SET_INSTR_CONFIGURE_TESTBUS_ANALOG;
    pTmpBuffer[wBufferLength++] = bDAC1Config;
    pTmpBuffer[wBufferLength++] = bDAC0Config;


    /* No Response expected*/
    bNumExpBytes = 0;

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        pTmpBuffer,
        wBufferLength,
        bNumExpBytes,
        pReceivedData,
        &wDataLenTmp));

    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Pn5180_Instr_GetHALVersion(
    uint16_t * pValue
    )
{
    *pValue = (HAL_MAJOR_VER)<<8;
    *pValue |= HAL_MINOR_VER;
    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_Int_LPCD_SetConfig(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint16_t wConfig,
    uint16_t wValue
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp = PH_ERR_SUCCESS;
    uint8_t PH_MEMLOC_REM bEPromData[2];

    switch(wConfig)
    {

    case PHHAL_HW_CONFIG_SET_LPCD_WAKEUPTIME_MS:
    {
        /* Value should be greater than 3ms */
        if(wValue >= 0x03)
        {
            pDataParams->wWakeupCounterInMs = wValue;
        }
        else
        {
            pDataParams->wWakeupCounterInMs = 0x03;
        }
     }
     break;

    case PHHAL_HW_CONFIG_LPCD_MODE:
        if((wValue == PHHAL_HW_PN5180_LPCD_MODE_DEFAULT) || (wValue ==  PHHAL_HW_PN5180_LPCD_MODE_POWERDOWN))
        {
            pDataParams->bLpcdMode = (uint8_t)wValue;
        }
        else
        {
            statusTmp = PH_ERR_INVALID_PARAMETER;
        }
        break;

    case PHHAL_HW_CONFIG_LPCD_REF:
        wValue= (wValue & (uint16_t)(AGC_REF_CONFIG_AGC_GEAR_MASK | AGC_REF_CONFIG_AGC_VALUE_MASK));
        PH_CHECK_SUCCESS_FCT(statusTmp,  phhalHw_Pn5180_Instr_ReadE2Prom(pDataParams,  PHHAL_HW_PN5180_LPCD_GPO_REFVAL_CONTROL_ADDR, bEPromData, 1));

        if((bEPromData[0] &  LPCD_REFVAL_CONTROL_MASK) == 0)
        {
            bEPromData[0] = (uint8_t)wValue;
            bEPromData[1] = (uint8_t)(wValue>>8);

            PH_CHECK_SUCCESS_FCT(statusTmp,  phhalHw_Pn5180_Instr_WriteE2Prom(pDataParams, PHHAL_HW_PN5180_LPCD_REFERENCE_VALUE_ADDR, bEPromData, 2));
        }
        else
        {
            PH_CHECK_SUCCESS_FCT(statusTmp,  phhalHw_Pn5180_Instr_WriteRegister(pDataParams, AGC_REF_CONFIG, (uint32_t)wValue));
        }
        break;

    default:
        statusTmp = PH_ERR_UNSUPPORTED_PARAMETER;
        break;
    }

    return PH_ADD_COMPCODE(statusTmp, PH_COMP_HAL);
}


phStatus_t phhalHw_Pn5180_Int_LPCD_GetConfig(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint16_t wConfig,
    uint16_t * pValue
    )
{
    switch(wConfig)
    {
    case PHHAL_HW_CONFIG_SET_LPCD_WAKEUPTIME_MS:
        *pValue = pDataParams->wWakeupCounterInMs;
        break;

    case PHHAL_HW_CONFIG_LPCD_MODE:
        *pValue = pDataParams->bLpcdMode;
        break;

    case PHHAL_HW_CONFIG_LPCD_REF:
        *pValue = pDataParams->wLpcdReference;
        break;

    default:
        return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_PARAMETER, PH_COMP_HAL);
        break;
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}



#endif /* NXPBUILD__PHHAL_HW_PN5180 */

