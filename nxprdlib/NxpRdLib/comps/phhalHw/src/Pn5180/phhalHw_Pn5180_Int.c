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
* Internal functions for Pn5180 specific HAL-Component of Reader Library Framework.
*
* $Author: nxp62726 $
* $Revision$ (NFCRDLIB_V4.010.03.001609  (NFCRDLIB_V4.010.01.001603 : 172 ))
* $Date: 2015-02-18 13:51:16 +0530 (Wed, 18 Feb 2015) $
*/
#include <phhalHw.h>
#include <ph_RefDefs.h>
#include <ph_Status.h>

#ifdef NXPBUILD__PHHAL_HW_PN5180
#include <phTools.h>
#include "phhalHw_Pn5180.h"
#include "phhalHw_Pn5180_Int.h"
#include "phhalHw_Pn5180_Reg.h"
#include "phhalHw_Pn5180_Instr.h"
#include "phhalHw_Pn5180_Wait.h"

/* Digital delay for all protocols and speeds in microseconds */
static const uint16_t PH_MEMLOC_CONST_ROM gkphhalHw_Pn5180_DigiDelays_Us[][4] =
{
    /* PHHAL_HW_CARDTYPE_ISO14443A */
    {/* 106KBaud */ 48, /* 212KBaud */ 24, /* 424KBaud */ 12, /* 848KBaud */ 6},

    /* PHHAL_HW_CARDTYPE_ISO14443B */
    {/* 106KBaud */ 182, /* 212KBaud */ 91, /* 424KBaud */ 46, /* 848KBaud */ 23},

    /* PHHAL_HW_CARDTYPE_FELICA */
    {/* N/A */ 0, /* 212KBaud */ 95, /* 424KBaud */ 48, /* N/A */ 0},

    /* PHHAL_HW_CARDTYPE_ISO15693 */
    {/* LOW */ 400, /* HIGH */ 321, /* FAST_LOW */ 800, /* FAST_HIGH */ 161},

    /* PHHAL_HW_CARDTYPE_ICODEEPCUID */
    {/* ALL */ 61, /* N/A */ 0, /* N/A */ 0, /* N/A */ 0},

    /* PHHAL_HW_CARDTYPE_I18000P3M3 */
    {/* 2/424 */ 75, /* 4/424 */ 121, /* 2/848 */ 11, /* 4/848 */ 47},

    /* PHHAL_HW_CARDTYPE_I18092MPI */
    {/* 106KBaud */ 17, /* 212KBaud */ 322, /* 424KBaud */ 165, /* N/A */ 0}
};


phStatus_t phhalHw_Pn5180_GetTxBuffer(
    phhalHw_Pn5180_DataParams_t * pDataParams,
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
        return phhalHw_Pn5180_GetRxBuffer(pDataParams, bIsExchange, pTxBuffer, pTxBufferLen, pTxBufferSize);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Pn5180_GetRxBuffer(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bIsExchange,
    uint8_t ** pRxBuffer,
    uint16_t * pRxBufferLen,
    uint16_t * pRxBufferSize
    )
{
    uint8_t bOffset = 0;

    /* Exchange command -> TxBuffer overwriting is allowed */
    if (bIsExchange != PH_OFF)
    {
#ifndef PN5180_P2P_HW_SYNC_BYTE
        if (pDataParams->bNfcipMode == PH_ON)
        {
            bOffset = 1;
        }
#endif
        *pRxBuffer = &pDataParams->pRxBuffer[pDataParams->wRxBufStartPos - bOffset];
        *pRxBufferSize = pDataParams->wRxBufSize - pDataParams->wRxBufStartPos - bOffset;
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
phStatus_t phhalHw_Pn5180_ReadData(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t ** ppData,
    uint16_t * pBytesRead
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp = PH_ERR_SUCCESS;

    uint16_t    PH_MEMLOC_REM wNumExpBytes;
    uint32_t    PH_MEMLOC_REM dwValue;

    /* Read the RX_STATUS */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_ReadRegister(
        pDataParams,
        RX_STATUS,
        &dwValue));
    if(pDataParams->bRxMultiple == PH_ON)
    {
        wNumExpBytes = (uint16_t)((dwValue & RX_STATUS_RX_NUM_BYTES_RECEIVED_MASK) |
            (((dwValue & RX_STATUS_RX_NUM_FRAMES_RECEIVED_MASK) >>
            RX_STATUS_RX_NUM_FRAMES_RECEIVED_POS) * 32));
    }
    else
    {
        wNumExpBytes = (uint16_t)(dwValue & RX_STATUS_RX_NUM_BYTES_RECEIVED_MASK);
    }

    if(wNumExpBytes != 0)
    {

        /* preapre the command for reading the RX FIFO of the Pn5180 */
        /* Read the RX_STATUS */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_RetrieveRxData(
            pDataParams,
            ppData,
            wNumExpBytes));
    }

    /* Update the actual number of bytes received */
    *pBytesRead = wNumExpBytes;

    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Pn5180_TimerStart(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bTimer,
    uint32_t dwStartCond,
    uint32_t dwStopCond,
    uint32_t wPrescaler,
    uint32_t dwLoadValue
    )
{
    phStatus_t  PH_MEMLOC_REM  statusTmp;
    uint8_t     PH_MEMLOC_REM  bTmrConfigReg;
    uint8_t     PH_MEMLOC_REM  bTmrRelaodReg;
    uint32_t    PH_MEMLOC_REM  dwEnableMask;


    uint32_t    PH_MEMLOC_REM dwTemp;
    uint8_t     PH_MEMLOC_BUF wRegTypeValueSets[18];
    uint16_t    PH_MEMLOC_REM wSizeOfRegTypeValueSets;

    /* Populate the timer configure and reload registers */

    switch(bTimer)
    {
    case TIMER0_CONFIG:
        bTmrConfigReg = TIMER0_CONFIG;
        bTmrRelaodReg = TIMER0_RELOAD;
        dwEnableMask = TIMER0_CONFIG_T0_ENABLE_MASK;
        break;

    case TIMER1_CONFIG:
        bTmrConfigReg = TIMER1_CONFIG;
        bTmrRelaodReg = TIMER1_RELOAD;
        dwEnableMask = TIMER1_CONFIG_T1_ENABLE_MASK;
        break;

    case TIMER2_CONFIG:
        bTmrConfigReg = TIMER2_CONFIG;
        bTmrRelaodReg = TIMER2_RELOAD;
        dwEnableMask = TIMER2_CONFIG_T2_ENABLE_MASK;
        break;

    default:
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /*write 0 to stop timer*/
    wSizeOfRegTypeValueSets = 0;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = bTmrConfigReg;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = 0x00;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = 0x00;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = 0x00;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = 0x00;

    /*load the timer  reload value*/
    dwTemp = (dwLoadValue & TMR_RELOAD_VALUE_MASK);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = bTmrRelaodReg;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

    /* Timer MODE_SEL is defined by ePrescaler, 0x01 enables the timer */
    dwTemp = (dwStartCond | dwStopCond | wPrescaler | dwEnableMask );
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = bTmrConfigReg;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterMultiple( pDataParams, wRegTypeValueSets, wSizeOfRegTypeValueSets));

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Pn5180_SetConfig_Int(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint16_t wConfig,
    uint16_t wValue
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint32_t    PH_MEMLOC_REM dwValue;
    uint8_t     PH_MEMLOC_BUF wRegTypeValueSets[12];
    uint16_t    PH_MEMLOC_REM wSizeOfRegTypeValueSets;
    uint32_t    PH_MEMLOC_REM dwTemp;

    switch(wConfig)
    {
    case PHHAL_HW_CONFIG_PARITY:

        /* Tx-Parity,  Rx-Parity is OFF */
        if (wValue == PH_OFF)
        {
            dwValue = (uint32_t)~(uint32_t)TX_CONFIG_TX_PARITY_ENABLE_MASK;
            /* Perform write */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, TX_CONFIG, dwValue));

            dwValue = (uint32_t)~(uint32_t)CRC_RX_CONFIG_RX_PARITY_ENABLE_MASK;
            /* Perform write */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, CRC_RX_CONFIG, dwValue));
        }
        else
        {
            /* Turn ON Tx-Parity */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterOrMask(pDataParams, TX_CONFIG, TX_CONFIG_TX_PARITY_ENABLE_MASK));

            /* Turn ON Rx-Parity */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterOrMask(pDataParams, CRC_RX_CONFIG, CRC_RX_CONFIG_RX_PARITY_ENABLE_MASK));
        }

        break;

    case PHHAL_HW_CONFIG_TXCRC:

        if (wValue == PH_OFF)
        {
            /* CRC calculator, your services are not required */
            dwValue = (uint32_t)~(uint32_t)CRC_TX_CONFIG_TX_CRC_ENABLE_MASK;

            /* Perform write */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, CRC_TX_CONFIG, dwValue));
        }
        else
        {

            wSizeOfRegTypeValueSets = 0;

            /*Clear the Bits*/
            dwTemp = (uint32_t) ~( CRC_TX_CONFIG_TX_CRC_TYPE_MASK);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = CRC_TX_CONFIG;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_AND_MASK;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

            /*Set the new value  */
            dwTemp = (uint32_t)(CRC_TX_CONFIG_TX_CRC_ENABLE_MASK);
            /* If the card type is other than I18000p3m3, operate the CRC in 16-bit mode */
            if (pDataParams->bCardType == PHHAL_HW_CARDTYPE_I18000P3M3)
            {
                /* Just set the bit for 5-bit mode operation */
                dwTemp |= (uint32_t)CRC_TX_CONFIG_TX_CRC_TYPE_MASK;
            }

            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = CRC_TX_CONFIG;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_OR_MASK;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

            /*Send the array to the IC*/
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterMultiple(pDataParams,  wRegTypeValueSets, wSizeOfRegTypeValueSets));

        }
        break;

    case PHHAL_HW_CONFIG_RXCRC:

        if (wValue == PH_OFF)
        {
            /* CRC calculator, your services are not required */
            dwValue = (uint32_t)~(uint32_t)CRC_RX_CONFIG_RX_CRC_ENABLE_MASK;

            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, CRC_RX_CONFIG, dwValue));
        }
        else
        {

            wSizeOfRegTypeValueSets = 0;

            /*Clear the Bits */
            dwTemp = (uint32_t) ~( CRC_RX_CONFIG_RX_CRC_TYPE_MASK);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = CRC_RX_CONFIG;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_AND_MASK;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

            /*Set the new value  */
            dwTemp = (uint32_t)CRC_RX_CONFIG_RX_CRC_ENABLE_MASK;
            /* If the card type is other than I18000p3m3, operate the CRC in 16-bit mode */
            if (pDataParams->bCardType == PHHAL_HW_CARDTYPE_I18000P3M3)
            {
                /* Just set the bit for 5-bit mode operation */
                dwTemp |= (uint32_t)CRC_RX_CONFIG_RX_CRC_TYPE_MASK;
            }

            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = CRC_RX_CONFIG;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_OR_MASK;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

            /*Send the array to the IC*/
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterMultiple(pDataParams,  wRegTypeValueSets, wSizeOfRegTypeValueSets));


        }
        break;


    case PHHAL_HW_CONFIG_ASK100:

        /* switch off 100% ASK */
        if (wValue == PH_OFF)
        {
            /*Do nothing here*/;
        }
        /* switch on 100% ASK */
        else
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, ANA_RF_CONTROL_TX, ~(uint32_t)ANA_RF_CONTROL_TX_TX_RESIDUAL_CARRIER_MASK));
        }

        break;




    default:
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Pn5180_SetCardMode(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint16_t wTxDataRate,
    uint16_t wRxDataRate,
    uint16_t wSubcarrier )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bTxConfig = 0;
    uint8_t     PH_MEMLOC_REM bRxConfig = 0;

    if(pDataParams->wTargetMode == PH_OFF)
    {
        if(wTxDataRate == pDataParams->wCfgShadow[PHHAL_HW_CONFIG_TXDATARATE_FRAMING])
        {
            wTxDataRate = PHHAL_HW_RF_DATARATE_NO_CHANGE;
        }
        if(wRxDataRate == pDataParams->wCfgShadow[PHHAL_HW_CONFIG_RXDATARATE_FRAMING])
        {
            wRxDataRate = PHHAL_HW_RF_DATARATE_NO_CHANGE;
        }
    }

    switch (pDataParams->bCardType)
    {
    case PHHAL_HW_CARDTYPE_ISO14443A:

        /* Check if Target is activated and perform required change to switch BaudRate. */
        if (pDataParams->wTargetMode != PH_OFF)
        {
            if(pDataParams->bActiveMode == PH_OFF)
            {
                switch (wTxDataRate)
                {
                    case PHHAL_HW_RF_DATARATE_106:
                        bTxConfig = PHHAL_HW_PN5180_RF_TX_ISO14443A_PICC_106_MANCH_SUBC;
                        /*pTxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443A_106;*/
                        break;
                    case PHHAL_HW_RF_DATARATE_212:
                        bTxConfig = PHHAL_HW_PN5180_RF_TX_ISO14443A_PICC_212_BPSK;
                        /*pTxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443A_212;*/
                        break;
                    case PHHAL_HW_RF_DATARATE_424:
                        bTxConfig = PHHAL_HW_PN5180_RF_TX_ISO14443A_PICC_424_BPSK;
                        /* pTxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443A_424;*/
                        break;
                    case PHHAL_HW_RF_DATARATE_848:
                        bTxConfig = PHHAL_HW_PN5180_RF_TX_ISO14443A_PICC_848_BPSK;
                        /* pTxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443A_848;*/
                        break;
                    case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                        bTxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                        break;
                    default:
                        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
                }

                switch (wRxDataRate)
                {
                    case PHHAL_HW_RF_DATARATE_106:
                        bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO14443A_PICC_106_MILLER;
                        /*pRxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443A_106;*/
                        break;
                    case PHHAL_HW_RF_DATARATE_212:
                        bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO14443A_PICC_212_MILLER;
                        /*pRxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443A_212;*/
                        break;
                    case PHHAL_HW_RF_DATARATE_424:
                        bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO14443A_PICC_424_MILLER;
                        /* pRxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443A_424;*/
                        break;
                    case PHHAL_HW_RF_DATARATE_848:
                        bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO14443A_PICC_848_MILLER;
                        /* pRxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443A_848;*/
                        break;
                    case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                        bRxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                        break;
                    default:
                        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
                }
            }
            else
            {
                switch (wTxDataRate)
                {
                    case PHHAL_HW_RF_DATARATE_106:
                        bTxConfig = PHHAL_HW_PN5180_RF_TX_NFC_AT_106;
                        break;
                    case PHHAL_HW_RF_DATARATE_212:
                        bTxConfig = PHHAL_HW_PN5180_RF_TX_NFC_AT_212;
                        break;
                    case PHHAL_HW_RF_DATARATE_424:
                        bTxConfig = PHHAL_HW_PN5180_RF_TX_NFC_AT_424;
                        break;
                    case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                        bTxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                        break;
                    default:
                        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
                }
                switch (wRxDataRate)
                {
                    case PHHAL_HW_RF_DATARATE_106:
                        bRxConfig = PHHAL_HW_PN5180_RF_RX_NFC_AT_106;
                        break;
                    case PHHAL_HW_RF_DATARATE_212:
                        bRxConfig = PHHAL_HW_PN5180_RF_RX_NFC_AT_212;
                        break;
                    case PHHAL_HW_RF_DATARATE_424:
                        bRxConfig = PHHAL_HW_PN5180_RF_RX_NFC_AT_424;
                        break;
                    case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                        bRxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                        break;
                    default:
                        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
                }
            }

        }
        else
        {
            switch (wTxDataRate)
            {
                case PHHAL_HW_RF_DATARATE_106:
                    bTxConfig = PHHAL_HW_PN5180_RF_TX_ISO14443A_106_MILLER;
                    /*pTxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443A_106;*/
                    break;
                case PHHAL_HW_RF_DATARATE_212:
                    bTxConfig = PHHAL_HW_PN5180_RF_TX_ISO14443A_212_MILLER;
                    /*pTxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443A_212;*/
                    break;
                case PHHAL_HW_RF_DATARATE_424:
                    bTxConfig = PHHAL_HW_PN5180_RF_TX_ISO14443A_424_MILLER;
                    /* pTxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443A_424;*/
                    break;
                case PHHAL_HW_RF_DATARATE_848:
                    bTxConfig = PHHAL_HW_PN5180_RF_TX_ISO14443A_848_MILLER;
                    /* pTxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443A_848;*/
                    break;
                case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                    bTxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                    break;
                default:
                    return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
            }
            switch (wRxDataRate)
            {
                case PHHAL_HW_RF_DATARATE_106:
                    bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO14443A_106_MANCH_SUBC;
                    /*pRxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443A_106;*/
                    break;
                case PHHAL_HW_RF_DATARATE_212:
                    bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO14443A_212_BPSK;
                    /*pRxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443A_212;*/
                    break;
                case PHHAL_HW_RF_DATARATE_424:
                    bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO14443A_424_BPSK;
                    /* pRxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443A_424;*/
                    break;
                case PHHAL_HW_RF_DATARATE_848:
                    bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO14443A_848_BPSK;
                    /* pRxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443A_848;*/
                    break;
                case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                    bRxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                    break;
                default:
                    return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
            }
        }
        break;

    case PHHAL_HW_CARDTYPE_ISO14443B:

        switch (wTxDataRate)
        {
            case PHHAL_HW_RF_DATARATE_106:
                bTxConfig = PHHAL_HW_PN5180_RF_TX_ISO14443B_106_NRZ;
                /* pTxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443B_106;*/
                break;
            case PHHAL_HW_RF_DATARATE_212:
                bTxConfig = PHHAL_HW_PN5180_RF_TX_ISO14443B_212_NRZ;
                /* pTxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443B_212;*/
                break;
            case PHHAL_HW_RF_DATARATE_424:
                bTxConfig = PHHAL_HW_PN5180_RF_TX_ISO14443B_424_NRZ;
                /*pTxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443B_424;*/
                break;
            case PHHAL_HW_RF_DATARATE_848:
                bTxConfig = PHHAL_HW_PN5180_RF_TX_ISO14443B_848_NRZ;
                /*pTxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443B_848;*/
                break;
            case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                bTxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                break;
            default:
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        switch (wRxDataRate)
        {
            case PHHAL_HW_RF_DATARATE_106:
                bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO14443B_106_BPSK;
                /* pRxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443B_106;*/
                break;
            case PHHAL_HW_RF_DATARATE_212:
                bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO14443B_212_BPSK;
                /*pRxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443B_212;*/
                break;
            case PHHAL_HW_RF_DATARATE_424:
                bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO14443B_424_BPSK;
                /* pRxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443B_424;*/
                break;
            case PHHAL_HW_RF_DATARATE_848:
                bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO14443B_848_BPSK;
                /*  pRxRegisterSet = (const uint8_t*)gkphhalHw_Pn5180_I14443B_848;*/
                break;
            case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                bRxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                break;
            default:
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
        break;

    case PHHAL_HW_CARDTYPE_FELICA_212:
    case PHHAL_HW_CARDTYPE_FELICA_424:

        if (pDataParams->wTargetMode != PH_OFF)
        {
            if(pDataParams->bActiveMode == PH_OFF)
            {
                switch (wTxDataRate)
                {
                    case PHHAL_HW_RF_DATARATE_212:
                        bTxConfig = PHHAL_HW_PN5180_RF_TX_NFC_PT_212;
                        break;

                    case PHHAL_HW_RF_DATARATE_424:
                        bTxConfig = PHHAL_HW_PN5180_RF_TX_NFC_PT_424;
                        break;
                    case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                        bTxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                        break;
                    default:
                        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
                }

                switch (wRxDataRate)
                {
                    case PHHAL_HW_RF_DATARATE_212:
                        bRxConfig = PHHAL_HW_PN5180_RF_RX_NFC_PT_212;
                        break;

                    case PHHAL_HW_RF_DATARATE_424:
                        bRxConfig = PHHAL_HW_PN5180_RF_RX_NFC_PT_424;
                        break;
                    case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                        bRxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                        break;
                    default:
                        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
                }
            }
            else
            {
                switch (wTxDataRate)
                {
                    case PHHAL_HW_RF_DATARATE_212:
                        bTxConfig = PHHAL_HW_PN5180_RF_TX_NFC_AT_212;
                        break;
                    case PHHAL_HW_RF_DATARATE_424:
                        bTxConfig = PHHAL_HW_PN5180_RF_TX_NFC_AT_424;
                        break;
                    case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                        bTxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                        break;
                    default:
                        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
                }

                switch (wRxDataRate)
                {
                    case PHHAL_HW_RF_DATARATE_212:
                        bRxConfig = PHHAL_HW_PN5180_RF_RX_NFC_AT_212;
                        break;
                    case PHHAL_HW_RF_DATARATE_424:
                        bRxConfig = PHHAL_HW_PN5180_RF_RX_NFC_AT_424;
                        break;
                    case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                        bRxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                        break;
                    default:
                        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
                }
            }
        }
        else
        {
            switch (wTxDataRate)
            {
                case PHHAL_HW_RF_DATARATE_212:
                    bTxConfig = PHHAL_HW_PN5180_RF_TX_FELICA_212;
                    break;

                case PHHAL_HW_RF_DATARATE_424:
                    bTxConfig = PHHAL_HW_PN5180_RF_TX_FELICA_424;
                    break;

                case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                    bTxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                    break;

                default:
                    return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
            }

            switch (wRxDataRate)
            {
                case PHHAL_HW_RF_DATARATE_212:
                    bRxConfig = PHHAL_HW_PN5180_RF_RX_FELICA_212;
                    break;

                case PHHAL_HW_RF_DATARATE_424:
                    bRxConfig = PHHAL_HW_PN5180_RF_RX_FELICA_424;
                    break;

                case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                    bRxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                    break;

                default:
                    return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
            }

        }
        break;

    case PHHAL_HW_CARDTYPE_ISO15693:

        switch (wTxDataRate)
        {
            case PHHAL_HW_RF_TX_DATARATE_1_OUT_OF_4:
                bTxConfig = PHHAL_HW_PN5180_RF_TX_ISO15693_26_1OF4_ASK10;
                break;

            case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                bTxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                break;

            default:
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        switch (wRxDataRate)
        {
            case PHHAL_HW_RF_RX_DATARATE_HIGH:
                bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO15693_26_1OF4_SC;
                break;

            case PHHAL_HW_RF_RX_DATARATE_FAST_HIGH:
                bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO15693_53_1OF4_SC;
                break;

            case PHHAL_HW_RF_RX_DATARATE_LOW:
            case PHHAL_HW_RF_RX_DATARATE_FAST_LOW:
                return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_PARAMETER, PH_COMP_HAL);

            case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                bRxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                break;

            default:
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
        break;

    case PHHAL_HW_CARDTYPE_I18000P3M3:

        switch (wTxDataRate)
        {
            case PHHAL_HW_RF_TX_DATARATE_I18000P3M3:
                /* Subcarrier check */
                if ((wSubcarrier != PHHAL_HW_SUBCARRIER_DUAL) &&
                    (wSubcarrier != PHHAL_HW_SUBCARRIER_QUAD))
                {
                    return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
                }

                bTxConfig = PHHAL_HW_PN5180_RF_TX_ISO180003M3_TARI_9_44_ASK;
                break;

            case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                bTxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                break;

            default:
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        switch (wRxDataRate)
        {
            case PHHAL_HW_RX_I18000P3M3_FL_423_MAN2:
                bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO180003M3_MANCH424_2_PERIOD;
                break;
            case PHHAL_HW_RX_I18000P3M3_FL_423_MAN4:
                bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO180003M3_MANCH424_4_PERIOD;
                break;
            case PHHAL_HW_RX_I18000P3M3_FL_847_MAN2:
                bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO180003M3_MANCH848_2_PERIOD;
                break;
            case PHHAL_HW_RX_I18000P3M3_FL_847_MAN4:
                bRxConfig = PHHAL_HW_PN5180_RF_RX_ISO180003M3_MANCH848_4_PERIOD;
                break;
            case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                bRxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                break;
            default:
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
        break;

    case PHHAL_HW_CARDTYPE_I18092M_ACTIVE_106:
    case PHHAL_HW_CARDTYPE_I18092M_ACTIVE_212:
    case PHHAL_HW_CARDTYPE_I18092M_ACTIVE_424:

        switch (wTxDataRate)
        {
            case PHHAL_HW_RF_DATARATE_106:
                bTxConfig = PHHAL_HW_PN5180_RF_TX_NFC_AI_106_106;
                break;

            case PHHAL_HW_RF_DATARATE_212:
                bTxConfig = PHHAL_HW_PN5180_RF_TX_NFC_AI_212_212;
            break;

            case PHHAL_HW_RF_DATARATE_424:
                bTxConfig = PHHAL_HW_PN5180_RF_TX_NFC_AI_424_424;
                break;

            case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                bTxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                break;

            default:
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        switch (wRxDataRate)
        {
            case PHHAL_HW_RF_DATARATE_106:
                bRxConfig = PHHAL_HW_PN5180_RF_RX_NFC_AI_106;
                break;

            case PHHAL_HW_RF_DATARATE_212:
                bRxConfig = PHHAL_HW_PN5180_RF_RX_NFC_AI_212;
            break;

            case PHHAL_HW_RF_DATARATE_424:
                bRxConfig = PHHAL_HW_PN5180_RF_RX_NFC_AI_424;
                break;

            case PHHAL_HW_RF_DATARATE_NO_CHANGE:
                bRxConfig = PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX;
                break;

            default:
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        break;

    case PHHAL_HW_CARDTYPE_ICODEEPCUID:
    default:
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    if((PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX != bTxConfig) || (PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX != bRxConfig))
    {
        PH_CHECK_SUCCESS_FCT(statusTmp,
            phhalHw_Pn5180_Instr_LoadRfConfiguration(
            pDataParams,
            (uint8_t) bTxConfig,
            (uint8_t) bRxConfig ));
    }

    /* If Datarate is changed, then Update Data-rate in shadow for parity setting */
    if(wTxDataRate != PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX)
    {
        pDataParams->wCfgShadow[PHHAL_HW_CONFIG_TXDATARATE_FRAMING] = wTxDataRate;
    }
    if(wRxDataRate != PHHAL_HW_PN5180_CURRENT_RF_CONFIGURATION_INDEX)
    {
        pDataParams->wCfgShadow[PHHAL_HW_CONFIG_RXDATARATE_FRAMING] = wRxDataRate;
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}


phStatus_t phhalHw_Pn5180_SetTmo(
    phhalHw_Pn5180_DataParams_t *pDataParams,
    uint16_t wTimeout,
    uint8_t  bUnit
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint32_t    PH_MEMLOC_REM wPrescaler;
    uint32_t    PH_MEMLOC_REM dwLoadValue;
    uint16_t    PH_MEMLOC_REM wUDigitalDelay;

    /* Parameter check */
    if ((bUnit != PHHAL_HW_TIME_MICROSECONDS) && (bUnit != PHHAL_HW_TIME_MILLISECONDS))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }


    if(wTimeout == 0)
    {
        return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
    }

    if (bUnit == PHHAL_HW_TIME_MICROSECONDS)
    {

        PH_CHECK_SUCCESS_FCT(statusTmp,
            phhalHw_Pn5180_Int_GetDigiDelay(
            pDataParams,
            PH_ON,
            &wUDigitalDelay));

        /* here wTimeout will be in uS */
        wPrescaler = 0x00;
        /*Reducing the division by 2 digits to retain the 2 digit decimal places which were getting wiped out*/
        dwLoadValue =(uint32_t) ( PHHAL_HW_PN5180_MAX_FREQ / (PHHAL_HW_PN5180_CONVERSION_US_SEC/100));
        /*Restoring the division done in the earlier step*/
        dwLoadValue =(uint32_t) (((wTimeout + wUDigitalDelay) * dwLoadValue)/100);

    }
    else
    {
        wPrescaler = 0x3C;
        if(wTimeout <= PHHAL_HW_PN5180_MAX_TIME_DELAY_MS)
        {
            dwLoadValue =(uint32_t) (  wTimeout * ( PHHAL_HW_PN5180_MIN_FREQ  / PHHAL_HW_PN5180_CONVERSION_MS_SEC) );
        }
        else
        {
            return PH_ADD_COMPCODE(PH_ERR_PARAMETER_OVERFLOW, PH_COMP_HAL);
        }
    }

    PH_CHECK_SUCCESS_FCT(statusTmp,
        phhalHw_Pn5180_TimerStart(
        pDataParams,
        TIMER1_CONFIG,
        TIMER1_CONFIG_T1_START_ON_TX_ENDED_MASK,
        TIMER1_CONFIG_T1_STOP_ON_RX_STARTED_MASK,
        wPrescaler,
        dwLoadValue
        ));

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}


phStatus_t phhalHw_Pn5180_Int_Wait(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint32_t dwLoadValue,
    uint32_t wPrescaler)
{
    phStatus_t  PH_MEMLOC_REM  statusTmp;
    uint8_t     PH_MEMLOC_REM  bTmrConfigReg;
    uint8_t     PH_MEMLOC_REM  bTmrReloadReg;
    uint32_t    PH_MEMLOC_REM  dwEnableMask;
    uint32_t    PH_MEMLOC_REM dwStartCond;
    uint32_t    PH_MEMLOC_REM dwStopCond;
    uint32_t    PH_MEMLOC_REM dwTemp;
    uint8_t     PH_MEMLOC_BUF wRegTypeValueSets[18];
    uint16_t    PH_MEMLOC_REM wSizeOfRegTypeValueSets;
    uint32_t    PH_MEMLOC_REM dwIrqStatusReg;

    /* Setting Timer1 & Reload value Registers */
    bTmrConfigReg = TIMER0_CONFIG;
    bTmrReloadReg = TIMER0_RELOAD;
    dwEnableMask = TIMER0_CONFIG_T0_ENABLE_MASK;

    /*write 0 to stop timer1*/
    wSizeOfRegTypeValueSets = 0;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = bTmrConfigReg;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = 0x00;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = 0x00;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = 0x00;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = 0x00;

    /*load the timer 2 reload value*/
    dwTemp = (dwLoadValue & TMR_RELOAD_VALUE_MASK);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = bTmrReloadReg;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

    dwStartCond = TIMER0_CONFIG_T0_START_NOW_MASK;
    dwStopCond = 0x00;

    /* T2_MODE_SEL is defined by ePrescaler, 0x01 enables the timer */
    dwTemp = (dwStartCond | dwStopCond | wPrescaler | dwEnableMask);

    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = bTmrConfigReg;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

    /* Clear Interrupt  */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_SET_CLEAR, IRQ_SET_CLEAR_TIMER1_IRQ_CLR_MASK));

    (void)phOsal_Event_Consume(E_PH_OSAL_EVT_RF, E_PH_OSAL_EVT_SRC_HAL);

    /* Enable IRQ source */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_ENABLE, IRQ_ENABLE_TIMER0_IRQ_SET_ENABLE_MASK));

    /*Start the timer*/
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterMultiple( pDataParams, wRegTypeValueSets, wSizeOfRegTypeValueSets));

    PH_CHECK_SUCCESS_FCT(statusTmp,phhalHw_Pn5180_WaitIrq(pDataParams,PH_ON,PH_OFF,IRQ_STATUS_TIMER0_IRQ_MASK,&dwIrqStatusReg));

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams,TIMER0_CONFIG, ~TIMER0_CONFIG_T0_ENABLE_MASK));

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}


phStatus_t phhalHw_Pn5180_Int_GetDigiDelay(
                                      phhalHw_Pn5180_DataParams_t * pDataParams,
                                      uint8_t bIsTimeout,
                                      uint16_t * pDelayUs
                                      )
{
    uint16_t    PH_MEMLOC_REM wDataRate;
    uint8_t     PH_MEMLOC_REM bIndex;

    wDataRate = pDataParams->wCfgShadow[PHHAL_HW_CONFIG_RXDATARATE_FRAMING];
    bIndex = pDataParams->bCardType - 1;

    switch (pDataParams->bCardType)
    {
    case PHHAL_HW_CARDTYPE_ISO14443A:
    case PHHAL_HW_CARDTYPE_ISO14443B:
        *pDelayUs = gkphhalHw_Pn5180_DigiDelays_Us[bIndex][wDataRate];
        /* In a timeout case we have to add an additional delay */
        if (bIsTimeout != PH_OFF)
        {
            *pDelayUs = *pDelayUs + PHHAL_HW_PN5180_I14443_ADD_DELAY_US;
        }

        if(pDataParams->bJewelActivated == PH_ON)
        {
            *pDelayUs += 2000;
        }
        break;
    case PHHAL_HW_CARDTYPE_FELICA_212:
    case PHHAL_HW_CARDTYPE_FELICA_424:
        bIndex = PHHAL_HW_CARDTYPE_FELICA_212 - 1;
        *pDelayUs = gkphhalHw_Pn5180_DigiDelays_Us[bIndex][wDataRate];
        break;
    case PHHAL_HW_CARDTYPE_ISO15693:
        *pDelayUs = gkphhalHw_Pn5180_DigiDelays_Us[bIndex][wDataRate - PHHAL_HW_RF_RX_DATARATE_LOW];
        break;
    case PHHAL_HW_CARDTYPE_ICODEEPCUID:
        *pDelayUs = gkphhalHw_Pn5180_DigiDelays_Us[bIndex][0];
        break;
    case PHHAL_HW_CARDTYPE_I18000P3M3:
        switch (wDataRate)
        {
            case PHHAL_HW_RX_I18000P3M3_FL_423_MAN2:
                *pDelayUs = gkphhalHw_Pn5180_DigiDelays_Us[bIndex][0];
                break;
            case PHHAL_HW_RX_I18000P3M3_FL_423_MAN4:
                *pDelayUs = gkphhalHw_Pn5180_DigiDelays_Us[bIndex][1];
                break;
            case PHHAL_HW_RX_I18000P3M3_FL_847_MAN2:
                *pDelayUs = gkphhalHw_Pn5180_DigiDelays_Us[bIndex][2];
                break;
            case PHHAL_HW_RX_I18000P3M3_FL_847_MAN4:
                *pDelayUs = gkphhalHw_Pn5180_DigiDelays_Us[bIndex][3];
                break;
            default:
                return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
        }
        break;
    default:
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}


phStatus_t phhalHw_Pn5180_Int_GetFdt(
                                phhalHw_Pn5180_DataParams_t * pDataParams,
                                phStatus_t wExchangeStatus,
                                uint32_t * pTime
                                )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint32_t     PH_MEMLOC_REM bRegister;
    uint32_t    PH_MEMLOC_REM wT1Reload;
    uint32_t    PH_MEMLOC_REM wT1Value;
    float32_t   PH_MEMLOC_REM fTime;
    uint16_t    PH_MEMLOC_REM wDelay;
    uint32_t    PH_MEMLOC_REM FREQ=1;

    /* Get T1 reload value */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_ReadRegister(pDataParams, TIMER1_RELOAD, &bRegister));
    wT1Reload = ( bRegister & TIMER1_RELOAD_T1_RELOAD_VALUE_MASK);

    /* Get T1 counter value */
    if ((wExchangeStatus & PH_ERR_MASK) != PH_ERR_IO_TIMEOUT)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_ReadRegister(pDataParams, TIMER1_STATUS, &bRegister));
        wT1Value = (uint32_t) (bRegister & TIMER1_RELOAD_T1_RELOAD_VALUE_MASK);
    }
    else
    {
        wT1Value = 0;
    }

    /* Subtract reload- and counter values */
    wT1Value = wT1Reload - wT1Value;

    if (pDataParams->bTimeoutUnit == PHHAL_HW_TIME_MILLISECONDS)
    {
        FREQ = PHHAL_HW_PN5180_MIN_FREQ/1000;
        wT1Value *= PHHAL_HW_PN5180_CONVERSION_MS_SEC;
    }
    else
    {
        FREQ = PHHAL_HW_PN5180_MAX_FREQ/1000000;
        wT1Value *= PHHAL_HW_PN5180_CONVERSION_US_SEC;
    }

    fTime = (float32_t)(wT1Value / FREQ);

    /* Return the value */
    *pTime = (uint32_t)fTime;
    if ((float32_t)*pTime < fTime)
    {
        ++(*pTime);
    }

    /* Retrieve and subtract digital delay */
    if ((wExchangeStatus & PH_ERR_MASK) != PH_ERR_IO_TIMEOUT)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Int_GetDigiDelay(pDataParams, PH_OFF, &wDelay));
        *pTime = *pTime - wDelay;
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}


phStatus_t phhalHw_Pn5180_Receive_Int(phhalHw_Pn5180_DataParams_t * pDataParams,
                                      uint32_t dwIrqWaitFor,
                                      uint8_t ** ppRxBuffer,
                                      uint16_t * pRxLength,
                                      uint8_t    bIrqEnable
                                      )
{
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint32_t    PH_MEMLOC_REM dwIrqReg;
    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint16_t    PH_MEMLOC_REM wTmpBufferSize;
    uint32_t    PH_MEMLOC_REM dwValue;
    uint32_t    PH_MEMLOC_REM dwTemp;
    uint32_t    PH_MEMLOC_REM dwColPos = 0;
    uint32_t    PH_MEMLOC_REM dwRegister;
    uint16_t    PH_MEMLOC_REM wIndex = 0;
    uint8_t     PH_MEMLOC_REM bBitlen;
    uint8_t     PH_MEMLOC_REM aCrc[2];
    uint8_t     PH_MEMLOC_REM bBackup = 0;
    uint8_t     PH_MEMLOC_REM bRfActiveErrCause = 0;

    /* Now wait for the IRQ */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_WaitIrq(
        pDataParams,
        bIrqEnable,
        PH_OFF,
        dwIrqWaitFor,
        &dwIrqReg));

    /* Retrieve the content of the RF_STATUS register */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_ReadRegister(pDataParams, RF_STATUS, &dwRegister));

    /* Check if an error has occured */
    if (dwIrqReg & IRQ_STATUS_GENERAL_ERROR_IRQ_MASK)
    {
        status = PH_ERR_INTERNAL_ERROR;
    }
    else if((pDataParams->bRxMultiple == PH_ON) && (dwIrqReg & IRQ_STATUS_TIMER1_IRQ_MASK))
    {
        if(dwIrqReg & IRQ_STATUS_RX_IRQ_MASK)
        {
            status = PH_ERR_SUCCESS;
        }
        else
        {
            status = PH_ERR_IO_TIMEOUT;
        }
    }
    /* Timeout handling */
   else if ((dwIrqReg & IRQ_STATUS_TIMER1_IRQ_MASK))
   {
        /* Reset after timeout behavior */
        if (pDataParams->bRfResetAfterTo != PH_OFF)
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_FieldReset(pDataParams));
        }
        status = PH_ERR_IO_TIMEOUT;

        if ((pDataParams->bActiveMode == PH_ON) && (pDataParams->wTargetMode == PH_OFF))
        {
            if (dwRegister & RF_STATUS_TX_RF_STATUS_MASK )
            {
                /* If External RF ON Event occurred and Internal Field is ON then Internal Error Should
                 * not be returned, otherwise Internal Error is returned */
                if (!(dwIrqReg & IRQ_STATUS_RFON_DET_IRQ_MASK))
                {
                    /* After RWT Timeout if Internal RF is ON (Stuck) then return Internal Error. */
                    status = PH_ERR_INTERNAL_ERROR;
                }
            }
            else if (dwRegister & RF_STATUS_RF_DET_STATUS_MASK)
            {
                /* If External RF Field is ON after FWT Timeout, then return External RF Error. */
                status = PH_ERR_EXT_RF_ERROR;
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    /* Handle External RF error */
    else if ((pDataParams->bActiveMode == PH_ON) && (dwIrqReg & IRQ_STATUS_RF_ACTIVE_ERROR_IRQ_MASK))
    {
        bRfActiveErrCause = (dwRegister & RF_STATUS_RF_ACTIVE_ERROR_CAUSE_MASK) >> RF_STATUS_RF_ACTIVE_ERROR_CAUSE_POS;

        if (pDataParams->wTargetMode != PH_OFF)
        {
            if(bRfActiveErrCause != 4)
            {
                status = PH_ERR_EXT_RF_ERROR;
            }
            else
            {
                status = PHHAL_HW_PN5180_ACTIVE_ERR_CAUSE4;
            }
        }
        else
        {
            if((bRfActiveErrCause == 4))
            {
                status = PH_ERR_IO_TIMEOUT;

            }
            else
            {
                status = PH_ERR_EXT_RF_ERROR;
            }
        }
    }
    /* Passive mode */
    else if(dwIrqReg & IRQ_STATUS_RFOFF_DET_IRQ_MASK)
    {
        /* In case External RF OFF also occurred then exit with RF Error. */
        if ((pDataParams->bActiveMode == PH_OFF) && (pDataParams->wTargetMode))
        {
            /* Terminate with RF_ERROR*/
            status = PH_ERR_EXT_RF_ERROR;
        }
        else if(!(dwIrqReg & IRQ_STATUS_RX_IRQ_MASK))
        {
            status = PH_ERR_INTEGRITY_ERROR;
        }
    }
    else
    {
        /*QAC*/
    }

    /* Receive data */
    if ((dwIrqReg & IRQ_STATUS_RX_IRQ_MASK) && (status == PH_ERR_SUCCESS))
    {
        pDataParams->wRxBufLen = 0;

        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetRxBuffer(pDataParams, PH_ON, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

#ifndef PN5180_P2P_HW_SYNC_BYTE
        if (pDataParams->bNfcipMode == PH_ON)
        {
            bBackup = pTmpBuffer[0];
        }
#endif

        PH_CHECK_SUCCESS_FCT(statusTmp,  phhalHw_Pn5180_ReadData(pDataParams, ppRxBuffer, &wTmpBufferLen));




        /* Read RX_STATUS_REG to know the status of reception */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_ReadRegister( pDataParams, RX_STATUS, &dwValue));

        /* Extract RX last bits */
        dwTemp = dwValue & RX_STATUS_RX_NUM_LAST_BITS_MASK;
        dwTemp = dwTemp >> RX_STATUS_RX_NUM_LAST_BITS_POS;
        /* Set RX last bits */
        pDataParams->wAdditionalInfo = (uint16_t)dwTemp;

        /* Set incomplete byte status if applicable */
        if (dwValue & RX_STATUS_RX_DATA_INTEGRITY_ERROR_MASK)
		{
			status = PH_ERR_INTEGRITY_ERROR;
		}
        else if (dwValue & RX_STATUS_RX_PROTOCOL_ERROR_MASK)
		{
			status = PH_ERR_PROTOCOL_ERROR;
		}
        else if (dwTemp != 0x00 && dwTemp != 0x08)
        {
            status =  PH_ERR_SUCCESS_INCOMPLETE_BYTE;
        }
        else if (dwValue & RX_STATUS_RX_COLLISION_DETECTED_MASK)
        {
            if (wTmpBufferLen == 0)
            {
                /* No data received */
                status = PH_ERR_IO_TIMEOUT;
            }
            else
            {
                status = PH_ERR_COLLISION_ERROR;

                /* Retrieve collision position */
                dwColPos = dwColPos & RX_STATUS_RX_COLL_POS_MASK;
                dwColPos = dwValue >> RX_STATUS_RX_COLL_POS_POS;

                /* Restore RX buffer size */
                wTmpBufferSize = wTmpBufferSize + wTmpBufferLen;
                if (dwColPos > wTmpBufferSize)
                {
                    wTmpBufferLen = wTmpBufferSize;
                    status = PH_ERR_BUFFER_OVERFLOW;
                }
                else
                {
                    wTmpBufferLen = ((uint16_t)dwColPos >> 3);
                }

                /* Extracting valid bits from Collision position */
                if(dwColPos > 8)
                {
                    dwValue = dwColPos % 8 ;
                }
                else
                {
                    dwValue = dwColPos;
                }

                if (dwValue)
                {
                    /* If it is non zero, some valid bits are received */
                    if (wTmpBufferLen == wTmpBufferSize)
                    {
                        status = PH_ERR_BUFFER_OVERFLOW;
                    }
                    else
                    {
                        ++wTmpBufferLen;
                    }
                }

                /* Store valid bits of last byte */
                pDataParams->wAdditionalInfo = (uint16_t)dwValue;
            }
        }
        else
        {
            /* For QAC */
        }

#ifndef PN5180_P2P_HW_SYNC_BYTE
        if ((pDataParams->bNfcipMode) == PH_ON && (wTmpBufferLen != 0))
        {
            if(*ppRxBuffer[0] != 0xF0)
            {
                return PH_ADD_COMPCODE(PH_ERR_IO_TIMEOUT, PH_COMP_HAL);
            }
            pTmpBuffer[0] = bBackup;
            (*ppRxBuffer)++;
            wTmpBufferLen--;
        }
#endif
        /* Store received data length in dataparams */
        pDataParams->wRxBufLen = pDataParams->wRxBufStartPos + wTmpBufferLen;

        if (ppRxBuffer != NULL)
        {
            *ppRxBuffer = pDataParams->pRxBuffer  + wIndex;;
        }

        if (pRxLength != NULL)
        {
            *pRxLength = pDataParams->wRxBufLen;
        }

        if(pDataParams->bJewelActivated == PH_ON && ((status == PH_ERR_SUCCESS) || ( status & PH_ERR_MASK) == PH_ERR_SUCCESS_INCOMPLETE_BYTE))
        {
            if ((ppRxBuffer != NULL) && (pRxLength != NULL))
            {
                /*remove parity of the received data-as harware's parity is off*/
                PH_CHECK_SUCCESS_FCT(
                    statusTmp,
                    phTools_DecodeParity(
                    PH_TOOLS_PARITY_OPTION_ODD ,
                    *ppRxBuffer,
                    *pRxLength,
                    (uint8_t)pDataParams->wAdditionalInfo,
                    *pRxLength,
                    *ppRxBuffer,
                    &wTmpBufferLen,
                    &bBitlen));
            }
            /* Check for response length
            * For Type 1 tag, valid minimum response length is 4 */
            if((wTmpBufferLen >= 4) && (ppRxBuffer != NULL) && (pRxLength != NULL))
            {
                /*verify Crc_B for the received data*/
                PH_CHECK_SUCCESS_FCT(statusTmp,phTools_ComputeCrc_B( *ppRxBuffer, wTmpBufferLen-2, aCrc));
                if(!(aCrc[0] == ((*ppRxBuffer)[wTmpBufferLen - 2]) && aCrc[1] ==  ((*ppRxBuffer)[wTmpBufferLen - 1])))
                {
                    status = PH_ERR_INTEGRITY_ERROR;
                }
                /* subtract CRC from Rx buffer */

                *pRxLength = wTmpBufferLen - 2;
                status = PH_ERR_SUCCESS;
            }
            else
            {
                status = PH_ERR_PROTOCOL_ERROR;
            }
        }

        if (wTmpBufferLen > PHHAL_HW_PN5180_MAX_FSD)  // Frame Size > FSD
        {
            status = PH_ERR_BUFFER_OVERFLOW;
        }
    }
    else
    {
        if(PH_ERR_SUCCESS == status)
        {
            status = PH_ERR_INTERNAL_ERROR;
        }
    }

     return PH_ADD_COMPCODE(status, PH_COMP_HAL);

}


phStatus_t phhalHw_Pn5180_int_IdleCommand(phhalHw_Pn5180_DataParams_t * pDataParams)
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(
        pDataParams,
        SYSTEM_CONFIG,
        (uint32_t)~(SYSTEM_CONFIG_COMMAND_MASK)));
#ifndef _WIN32
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_SET_CLEAR, IRQ_SET_CLEAR_IDLE_IRQ_CLR_MASK));
#endif

    return PH_ERR_SUCCESS;
}

phStatus_t phhalHw_Pn5180_int_LoadCommand(phhalHw_Pn5180_DataParams_t * pDataParams, uint8_t bCmd)
{
    phStatus_t  PH_MEMLOC_REM statusTmp;


    uint8_t     PH_MEMLOC_BUF wRegTypeValueSets[12];
    uint16_t    PH_MEMLOC_REM wSizeOfRegTypeValueSets;
    uint32_t    PH_MEMLOC_REM dwTemp;


    wSizeOfRegTypeValueSets = 0;

    /*Clear the Bits of TX_CONFIG_TX_STOP_SYMBOL_MASK*/
    dwTemp = (uint32_t) ~( SYSTEM_CONFIG_COMMAND_MASK );
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = SYSTEM_CONFIG;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_AND_MASK;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

    /*Set the new value  */
    dwTemp = ((uint32_t)bCmd & SYSTEM_CONFIG_COMMAND_MASK);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = SYSTEM_CONFIG;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_OR_MASK;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

    /*Send the array to the IC*/
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterMultiple(pDataParams,  wRegTypeValueSets, wSizeOfRegTypeValueSets));


    return PH_ERR_SUCCESS;

}

phStatus_t phhalHw_Pn5180_AutoSyncByte(phhalHw_Pn5180_DataParams_t * pDataParams)
{
    phStatus_t PH_MEMLOC_REM statusTmp;
    uint32_t PH_MEMLOC_REM dwValue;
    uint32_t PH_MEMLOC_REM dwTemp;
    uint8_t     PH_MEMLOC_BUF wRegTypeValueSets[48];
    uint16_t    PH_MEMLOC_REM wSizeOfRegTypeValueSets;

    wSizeOfRegTypeValueSets = 0;

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_ReadRegister(pDataParams, SIGPRO_CONFIG, &dwValue));

    if ((pDataParams->wTargetMode == PH_OFF) && (dwValue == 4))
    {
        dwTemp = 0x1F;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = TX_SYMBOL0_DEF_RFU;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp >> 8);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp >> 16);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp >> 24);

        dwTemp = 0x260104;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = TX_SYMBOL01_MOD_RFU;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);
        if (pDataParams->bActiveMode == PH_ON)
        {
            dwTemp = (uint32_t) ~( SIGPRO_CM_CONFIG_RX_FRAMING_MASK );
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = SIGPRO_CM_CONFIG;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_AND_MASK;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

            dwTemp = (0x01 << SIGPRO_CM_CONFIG_RX_FRAMING_POS);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = SIGPRO_CM_CONFIG;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_OR_MASK;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterMultiple(pDataParams,  wRegTypeValueSets, wSizeOfRegTypeValueSets));
        }
        else
        {
            dwTemp = (uint32_t) ~( SIGPRO_RM_CONFIG_RFU_SYNC_VAL_POS | SIGPRO_RM_CONFIG_RFU_SYNC_LEN_POS);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = SIGPRO_RM_CONFIG_RFU;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_AND_MASK;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

            dwTemp = (0x21F << SIGPRO_RM_CONFIG_RFU_SYNC_VAL_POS) | (0x09 << SIGPRO_RM_CONFIG_RFU_SYNC_LEN_POS);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = SIGPRO_RM_CONFIG_RFU;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_OR_MASK;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterMultiple(pDataParams,  wRegTypeValueSets, wSizeOfRegTypeValueSets));
        }
    }
    else if ((pDataParams->wTargetMode == PH_ON) && (dwValue == 4))
    {
        if (pDataParams->bActiveMode == PH_ON)
        {
            dwTemp = 0x1F;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = TX_SYMBOL0_DEF_RFU;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp >> 8);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp >> 16);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp >> 24);

            dwTemp = 0x260104;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = TX_SYMBOL01_MOD_RFU;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);
        }
        else
        {
            dwTemp = 0x0021F;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = TX_SYMBOL0_DEF_RFU;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp >> 8);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp >> 16);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp >> 24);

            dwTemp = 0x2C;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = TX_SYMBOL01_MOD_RFU;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);
        }
    }
    else
    {
        /*QAC*/
    }
    dwTemp = (uint32_t) ~( SIGPRO_CM_CONFIG_RX_FRAMING_MASK );
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = SIGPRO_CM_CONFIG;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_AND_MASK;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

    dwTemp = (0x01 << SIGPRO_CM_CONFIG_RX_FRAMING_POS);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = SIGPRO_CM_CONFIG;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_OR_MASK;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterMultiple(pDataParams,  wRegTypeValueSets, wSizeOfRegTypeValueSets));

    wSizeOfRegTypeValueSets = 0;

    dwTemp = (uint32_t) ~(TX_SYMBOL_CONFIG_RFU_TX_SYMBOL0_LEN_MASK);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = TX_SYMBOL_CONFIG_RFU;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_AND_MASK;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

    dwTemp = 0x09;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = TX_SYMBOL_CONFIG_RFU;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_OR_MASK;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

    dwTemp = (uint32_t) ~(TX_CONFIG_TX_START_SYMBOL_MASK);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = TX_CONFIG;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_AND_MASK;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

    dwTemp = (0x01 << TX_CONFIG_TX_START_SYMBOL_POS);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = TX_CONFIG;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_OR_MASK;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);


    dwTemp = (uint32_t) ~(CRC_TX_CONFIG_TX_CRC_PRESET_SEL_MASK);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = CRC_TX_CONFIG;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_AND_MASK;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

    dwTemp = (0x02 << CRC_TX_CONFIG_TX_CRC_PRESET_SEL_POS);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = CRC_TX_CONFIG;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_OR_MASK;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

    dwTemp = (uint32_t) ~(CRC_RX_CONFIG_RX_CRC_PRESET_SEL_MASK);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = CRC_RX_CONFIG;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_AND_MASK;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

    dwTemp = (0x02 << CRC_RX_CONFIG_RX_CRC_PRESET_SEL_POS);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = CRC_RX_CONFIG;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_OR_MASK;
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
    wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterMultiple(pDataParams,  wRegTypeValueSets, wSizeOfRegTypeValueSets));

    return PH_ERR_SUCCESS;
}

#endif  /* NXPBUILD__PHHAL_HW_PN5180 */

