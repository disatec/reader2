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
* PN5180 generic HAL interface.
*
* $Author: Jayaprakash Desai (ing08160) $
* $Revision$ (NFCRDLIB_V4.010.03.001609  (NFCRDLIB_V4.010.01.001603 : 3612 ))
* $Date: 2015-08-25 19:01:08 +0530 (Tue, 25 Aug 2015) $
*/

#include <ph_Status.h>
#include <phbalReg.h>
#include <phhalHw.h>
#include <ph_RefDefs.h>

#ifdef NXPBUILD__PH_KEYSTORE
#include <phKeyStore.h>
#endif

#ifdef NXPBUILD__PHHAL_HW_PN5180

#include "phhalHw_Pn5180.h"
#include "phhalHw_Pn5180_Instr.h"
#include "phhalHw_Pn5180_Reg.h"
#include "phhalHw_Pn5180_Int.h"
#include "phhalHw_Pn5180_Wait.h"
#include <phTools.h>


/* Default shadow for ISO14443-3A Mode */
static const uint16_t PH_MEMLOC_CONST_ROM wPn5180_DefaultShadow_I14443a[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_ON},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_OFF},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_OFF},
    {PHHAL_HW_CONFIG_RXWAIT_US,             76},
    {PHHAL_HW_CONFIG_TXWAIT_US,             85},
    {PHHAL_HW_CONFIG_TXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_106 | PHHAL_HW_RF_TYPE_A_FRAMING},
    {PHHAL_HW_CONFIG_RXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_106 | PHHAL_HW_RF_TYPE_A_FRAMING},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_PN5180_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_ASK100,                PH_ON}
};

/* Default shadow for ISO14443-3B Mode */
static const uint16_t PH_MEMLOC_CONST_ROM wPn5180_DefaultShadow_I14443b[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_OFF},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXWAIT_US,             76},
    {PHHAL_HW_CONFIG_TXWAIT_US,             85},
    {PHHAL_HW_CONFIG_TXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_106 | PHHAL_HW_RF_TYPE_B_FRAMING},
    {PHHAL_HW_CONFIG_RXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_106 | PHHAL_HW_RF_TYPE_B_FRAMING},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_PN5180_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_MODINDEX,              PHHAL_HW_PN5180_MODINDEX_I14443B},
    {PHHAL_HW_CONFIG_ASK100,                PH_OFF}
};

/* Default shadow for FeliCa Mode */
static const uint16_t PH_MEMLOC_CONST_ROM wPn5180_DefaultShadow_Felica[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_OFF},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXWAIT_US,             88},
    {PHHAL_HW_CONFIG_TXWAIT_US,             0x0000},
    {PHHAL_HW_CONFIG_TXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_212 | PHHAL_HW_RF_TYPE_F_FRAMING},
    {PHHAL_HW_CONFIG_RXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_212 | PHHAL_HW_RF_TYPE_F_FRAMING},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_PN5180_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_MODINDEX,              PHHAL_HW_PN5180_MODINDEX_FELICA},
    {PHHAL_HW_CONFIG_ASK100,                PH_OFF}
};

/* Default shadow for FeliCa Mode at 424 baud rate */
static const uint16_t PH_MEMLOC_CONST_ROM wPn5180_DefaultShadow_Felica_424[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_OFF},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXWAIT_US,             85},
    {PHHAL_HW_CONFIG_TXWAIT_US,             0x0000},
    {PHHAL_HW_CONFIG_TXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_424 | PHHAL_HW_RF_TYPE_F_FRAMING},
    {PHHAL_HW_CONFIG_RXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_424 | PHHAL_HW_RF_TYPE_F_FRAMING},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_PN5180_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_MODINDEX,              PHHAL_HW_PN5180_MODINDEX_FELICA},
    {PHHAL_HW_CONFIG_ASK100,                PH_OFF}
};



/* Default shadow for ISO15693 Mode */
static const uint16_t PH_MEMLOC_CONST_ROM wPn5180_DefaultShadow_I15693[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_OFF},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXWAIT_US,             114},
    {PHHAL_HW_CONFIG_TXWAIT_US,             0x0000},
    {PHHAL_HW_CONFIG_TXDATARATE_FRAMING,    PHHAL_HW_RF_TX_DATARATE_1_OUT_OF_4},
    {PHHAL_HW_CONFIG_RXDATARATE_FRAMING,    PHHAL_HW_RF_RX_DATARATE_HIGH},
    {PHHAL_HW_CONFIG_SUBCARRIER,            PHHAL_HW_SUBCARRIER_SINGLE},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_PN5180_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_MODINDEX,              PHHAL_HW_PN5180_MODINDEX_I15693},
    {PHHAL_HW_CONFIG_ASK100,                PH_ON}
};

/* Default shadow for ISO18000-3 Mode3 */
static const uint16_t PH_MEMLOC_CONST_ROM wPn5180_DefaultShadow_I18000p3m3[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_OFF},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXWAIT_US,             10},
    {PHHAL_HW_CONFIG_TXWAIT_US,             0x0000},
    {PHHAL_HW_CONFIG_TXDATARATE_FRAMING,    PHHAL_HW_RF_TX_DATARATE_I18000P3M3},
    {PHHAL_HW_CONFIG_RXDATARATE_FRAMING,    PHHAL_HW_RX_I18000P3M3_FL_423_MAN2},
    {PHHAL_HW_CONFIG_SUBCARRIER,            PHHAL_HW_SUBCARRIER_DUAL},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_PN5180_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_MODINDEX,              PHHAL_HW_PN5180_MODINDEX_I18000P3M3},
    {PHHAL_HW_CONFIG_ASK100,                PH_OFF}
};



/* Default shadow for ISO18092 Active Initiator Mode at 106kbps. */
static const uint16_t PH_MEMLOC_CONST_ROM wPn5180_DefShadow_I18092m_Active[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_ON},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_TXWAIT_US,             0},
    {PHHAL_HW_CONFIG_RXWAIT_US,             29},
    {PHHAL_HW_CONFIG_TXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_106 | PHHAL_HW_RF_TYPE_ACTIVE_FRAMING},
    {PHHAL_HW_CONFIG_RXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_106 | PHHAL_HW_RF_TYPE_ACTIVE_FRAMING},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_PN5180_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_MODINDEX,              PHHAL_HW_PN5180_MODINDEX_FELICA},
    {PHHAL_HW_CONFIG_ASK100,                PH_ON}
};

/* Default shadow for ISO18092 Active Initiator Mode at 212kbps. */
static const uint16_t PH_MEMLOC_CONST_ROM wPn5180_DefShadow_I18092m_212_Active[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_OFF},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_TXWAIT_US,             0},
    {PHHAL_HW_CONFIG_RXWAIT_US,             15},
    {PHHAL_HW_CONFIG_TXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_212 | PHHAL_HW_RF_TYPE_ACTIVE_FRAMING},
    {PHHAL_HW_CONFIG_RXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_212 | PHHAL_HW_RF_TYPE_ACTIVE_FRAMING},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_PN5180_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_MODINDEX,              PHHAL_HW_PN5180_MODINDEX_FELICA},
    {PHHAL_HW_CONFIG_ASK100,                PH_OFF}
};

/* Default shadow for ISO18092 Active Initiator Mode at 424kbps. */
static const uint16_t PH_MEMLOC_CONST_ROM wPn5180_DefShadow_I18092m_424_Active[][2] =
{
    {PHHAL_HW_CONFIG_PARITY,                PH_OFF},
    {PHHAL_HW_CONFIG_TXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_RXCRC,                 PH_ON},
    {PHHAL_HW_CONFIG_TXWAIT_US,             0},
    {PHHAL_HW_CONFIG_RXWAIT_US,             8},
    {PHHAL_HW_CONFIG_TXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_424 | PHHAL_HW_RF_TYPE_ACTIVE_FRAMING},
    {PHHAL_HW_CONFIG_RXDATARATE_FRAMING,    PHHAL_HW_RF_DATARATE_424 | PHHAL_HW_RF_TYPE_ACTIVE_FRAMING},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,      PHHAL_HW_PN5180_DEFAULT_TIMEOUT},
    {PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,      0x0000},
    {PHHAL_HW_CONFIG_MODINDEX,              PHHAL_HW_PN5180_MODINDEX_FELICA},
    {PHHAL_HW_CONFIG_ASK100,                PH_OFF}
};

#ifdef _WIN32

phStatus_t phHalHw_Pn5180_GetDataParamsSize(uint16_t * pDataParamsSize)
{
    if (pDataParamsSize == NULL)
    {
        return PH_ERR_INVALID_PARAMETER;
    }

    *pDataParamsSize = (uint16_t)sizeof(phhalHw_Pn5180_DataParams_t);

    return PH_ERR_SUCCESS;
}

#endif

phStatus_t phhalHw_Pn5180_Init(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint16_t wSizeOfDataParams,
    void * pBalDataParams,
    void * pKeyStoreDataParams,
    uint8_t * pTxBuffer,
    uint16_t wTxBufSize,
    uint8_t * pRxBuffer,
    uint16_t wRxBufSize
    )
{
    phStatus_t PH_MEMLOC_REM statusTmp;

    if(sizeof(phhalHw_Pn5180_DataParams_t) != wSizeOfDataParams)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_DATA_PARAMS, PH_COMP_HAL);
    }
    /* The working buffers cannot be zero */
    if (wTxBufSize == 0 || wRxBufSize == 0)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Check whether all the pointers supplied are non NULL */
    PH_ASSERT_NULL(pDataParams);
    PH_ASSERT_NULL(pBalDataParams);
    PH_ASSERT_NULL(pTxBuffer);
    PH_ASSERT_NULL(pRxBuffer);

    /* This is the constructor for the Pn5180 HAL, let's initialize the private data */
    pDataParams->wId                    = PH_COMP_HAL | PHHAL_HW_PN5180_ID;
    pDataParams->pBalDataParams         = pBalDataParams;
    pDataParams->pKeyStoreDataParams    = pKeyStoreDataParams;

    if (pTxBuffer != NULL)
    {
        /* The first two bytes are reserved for the SPI commands */
        pDataParams->pTxBuffer          = &pTxBuffer[1];
        pDataParams->wTxBufSize         = wTxBufSize-1;
    }
    else
    {
        pDataParams->pTxBuffer          = pTxBuffer;
        pDataParams->wTxBufSize         = wTxBufSize;
    }

    pDataParams->wTxBufLen              = 0;
    if (pRxBuffer != NULL)
    {
        /* The first two bytes are reserved for the SPI commands */
        pDataParams->pRxBuffer          = &pRxBuffer[2];
        pDataParams->wRxBufSize         = wRxBufSize-2;
    }
    else
    {
        pDataParams->pRxBuffer          = pRxBuffer;
        pDataParams->wRxBufSize         = wRxBufSize;
    }

    pDataParams->wRxBufLen              = 0;
    pDataParams->wRxBufStartPos         = 0;

    pDataParams->wTxBufLen              = 0;
    pDataParams->wTxBufStartPos         = 0;

    pDataParams->bActiveMode            = PH_OFF;
    pDataParams->wTimingMode            = PHHAL_HW_TIMING_MODE_OFF;
    pDataParams->bCardType              = PHHAL_HW_CARDTYPE_ISO14443A;
    pDataParams->wTimingMode            = PHHAL_HW_TIMING_MODE_OFF;
    pDataParams->bTimeoutUnit           = PHHAL_HW_TIME_MICROSECONDS;
    pDataParams->dwTimingUs             = 0;
    pDataParams->wFieldOffTime          = PHHAL_HW_FIELD_OFF_DEFAULT;
    pDataParams->wFieldRecoveryTime     = PHHAL_HW_FIELD_RECOVERY_DEFAULT;
    pDataParams->bSymbolStart           = PH_OFF;
    pDataParams->bSymbolEnd             = PH_OFF;
    pDataParams->wAdditionalInfo        = 0;

    pDataParams->bRfResetAfterTo        = PH_OFF;
    pDataParams->bOpeMode               = RD_LIB_MODE_NFC;
    pDataParams->bRxMultiple            = PH_OFF;
    pDataParams->bNfcipMode             = PH_OFF;
    pDataParams->bJewelActivated        = PH_OFF;
    pDataParams->bLpcdMode              = PHHAL_HW_PN5180_LPCD_MODE_DEFAULT;
    pDataParams->wWakeupCounterInMs     = 3;
    pDataParams->bRfca                  = PH_ON;
    pDataParams->wTargetMode            = PH_OFF;
    pDataParams->dwTxWaitMs             = 0;
    pDataParams->bSkipSend              = PH_OFF;
    pDataParams->bTransmited            = PH_OFF;
    pDataParams->bMfcCryptoEnabled      = PH_OFF;
    pDataParams->bCardMode              = PH_OFF;

    PH_CHECK_SUCCESS_FCT(statusTmp, phbalReg_SetConfig(pDataParams->pBalDataParams, PHBAL_REG_CONFIG_HAL_HW_TYPE, PHBAL_REG_HAL_HW_PN5180));

    /* Disable Idle IRQ */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, IRQ_ENABLE, (uint32_t)~IRQ_SET_CLEAR_IDLE_IRQ_CLR_MASK));

    /* Clear all IRQs  */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_SET_CLEAR, PHHAL_HW_PN5180_IRQ_SET_CLEAR_ALL_MASK));

    /*clear previously occurred unwanted events*/
    (void)phOsal_Event_Consume(E_PH_OSAL_EVT_RF, E_PH_OSAL_EVT_SRC_HAL);


    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Pn5180_WriteRegister(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t  bAddress,
    uint32_t dwValue
    )
{
    return phhalHw_Pn5180_Instr_WriteRegister(  \
        pDataParams,  \
        bAddress,  \
        dwValue);
}


phStatus_t phhalHw_Pn5180_ReadRegister(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bAddress,
    uint32_t * pValue)
{
    return phhalHw_Pn5180_Instr_ReadRegister(   \
        pDataParams,  \
        bAddress,  \
        pValue);   \

}

phStatus_t phhalHw_Pn5180_ApplyProtocolSettings(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bCardType)
{
    phStatus_t PH_MEMLOC_REM statusTmp;
    uint16_t * PH_MEMLOC_REM pShadowDefault;
    uint16_t   PH_MEMLOC_REM wShadowCount;
    uint8_t    PH_MEMLOC_REM bUseDefaultShadow;
    uint16_t   PH_MEMLOC_COUNT wIndex;
    uint16_t   PH_MEMLOC_REM wConfig;
    uint16_t   PH_MEMLOC_REM wTxDatarate;
    uint16_t   PH_MEMLOC_REM wRxDatarate;

    /* Store new card type */
    if (bCardType != PHHAL_HW_CARDTYPE_CURRENT)
    {
        pDataParams->bCardType = bCardType;
        pDataParams->bTimeoutUnit = PHHAL_HW_TIME_MICROSECONDS;
        bUseDefaultShadow = 1;
    }
    else
    {
        bUseDefaultShadow = 0;
    }

    pDataParams->bActiveMode       = PH_OFF;
    pDataParams->wTargetMode       = PH_OFF;
    pDataParams->bJewelActivated   = PH_OFF;
    pDataParams->bNfcipMode        = PH_OFF;
    pDataParams->bSkipSend         = PH_OFF;

    /* configure reader IC for current card */
    switch (pDataParams->bCardType)
    {
        /* configure hardware for ISO 14443A */
    case PHHAL_HW_CARDTYPE_ISO14443A:
        /* Use 14443a default shadow */
        PH_CHECK_SUCCESS_FCT(statusTmp,phhalHw_Pn5180_Instr_LoadRfConfiguration(pDataParams, PHHAL_HW_PN5180_RF_TX_ISO14443A_106_MILLER, PHHAL_HW_PN5180_RF_RX_ISO14443A_106_MANCH_SUBC));  /* Masking RxDataRate with Minimum Configuration Index */
        pShadowDefault = (uint16_t*)wPn5180_DefaultShadow_I14443a;
        wShadowCount = sizeof(wPn5180_DefaultShadow_I14443a) / (sizeof(uint16_t) * 2);
        /* Update Datarate in shadow for parity setting */
        wTxDatarate = PHHAL_HW_RF_DATARATE_106;
        wRxDatarate = PHHAL_HW_RF_DATARATE_106;
        break;

    case PHHAL_HW_CARDTYPE_ISO14443B:
        /* Use 14443b shadow */
        PH_CHECK_SUCCESS_FCT(statusTmp,phhalHw_Pn5180_Instr_LoadRfConfiguration(pDataParams, PHHAL_HW_PN5180_RF_TX_ISO14443B_106_NRZ, PHHAL_HW_PN5180_RF_RX_ISO14443B_106_BPSK));  /* Masking RxDataRate with Minimum Configuration Index */
        pShadowDefault = (uint16_t*)wPn5180_DefaultShadow_I14443b;
        wShadowCount = sizeof(wPn5180_DefaultShadow_I14443b) / (sizeof(uint16_t) * 2);
        /* Update Datarate in shadow for parity setting */
        wTxDatarate = PHHAL_HW_RF_DATARATE_106;
        wRxDatarate = PHHAL_HW_RF_DATARATE_106;
        break;

    case PHHAL_HW_CARDTYPE_FELICA_212:
        /* Use Felica shadow */
        PH_CHECK_SUCCESS_FCT(statusTmp,phhalHw_Pn5180_Instr_LoadRfConfiguration(pDataParams, PHHAL_HW_PN5180_RF_TX_FELICA_212, PHHAL_HW_PN5180_RF_RX_FELICA_212));  /* Masking RxDataRate with Minimum Configuration Index */
        pShadowDefault = (uint16_t*)wPn5180_DefaultShadow_Felica;
        wShadowCount = sizeof(wPn5180_DefaultShadow_Felica) / (sizeof(uint16_t) * 2);
        wTxDatarate = PHHAL_HW_RF_DATARATE_212;
        wRxDatarate = PHHAL_HW_RF_DATARATE_212;
        break;


    case PHHAL_HW_CARDTYPE_FELICA_424:
        /* Use Felica shadow for 424*/
        PH_CHECK_SUCCESS_FCT(statusTmp,phhalHw_Pn5180_Instr_LoadRfConfiguration(pDataParams, PHHAL_HW_PN5180_RF_TX_FELICA_424, PHHAL_HW_PN5180_RF_RX_FELICA_424));  /* Masking RxDataRate with Minimum Configuration Index */

        pShadowDefault = (uint16_t*)wPn5180_DefaultShadow_Felica_424;
        wShadowCount = sizeof(wPn5180_DefaultShadow_Felica_424) / (sizeof(uint16_t) * 2);
        wTxDatarate = PHHAL_HW_RF_DATARATE_424;
        wRxDatarate = PHHAL_HW_RF_DATARATE_424;
        break;

    case PHHAL_HW_CARDTYPE_I18092M_ACTIVE_106:
        /* Use I18092m_Active shadow */
        PH_CHECK_SUCCESS_FCT(statusTmp,phhalHw_Pn5180_Instr_LoadRfConfiguration(pDataParams, PHHAL_HW_PN5180_RF_TX_NFC_AI_106_106, PHHAL_HW_PN5180_RF_RX_NFC_AI_106));  /* Masking RxDataRate with Minimum Configuration Index */
        pShadowDefault = (uint16_t*)wPn5180_DefShadow_I18092m_Active;
        wShadowCount = sizeof(wPn5180_DefShadow_I18092m_Active) / (sizeof(uint16_t) * 2);
        wTxDatarate = PHHAL_HW_RF_DATARATE_106;
        wRxDatarate = PHHAL_HW_RF_DATARATE_106;
        /* Enable active mode */
        pDataParams->bActiveMode = PH_ON;
        pDataParams->bNfcipMode = PH_ON;
        break;

    case PHHAL_HW_CARDTYPE_I18092M_ACTIVE_212:
        /* Use I18092m_Active_212 shadow */
        PH_CHECK_SUCCESS_FCT(statusTmp,phhalHw_Pn5180_Instr_LoadRfConfiguration(pDataParams, PHHAL_HW_PN5180_RF_TX_NFC_AI_212_212, PHHAL_HW_PN5180_RF_RX_NFC_AI_212));  /* Masking RxDataRate with Minimum Configuration Index */
        pShadowDefault = (uint16_t*)wPn5180_DefShadow_I18092m_212_Active;
        wShadowCount = sizeof(wPn5180_DefShadow_I18092m_212_Active) / (sizeof(uint16_t) * 2);
        wTxDatarate = PHHAL_HW_RF_DATARATE_212;
        wRxDatarate = PHHAL_HW_RF_DATARATE_212;
        /* Enable active mode */
        pDataParams->bActiveMode = PH_ON;
        break;

    case PHHAL_HW_CARDTYPE_I18092M_ACTIVE_424:
        /* Use I18092m_Active_424 shadow */
        PH_CHECK_SUCCESS_FCT(statusTmp,phhalHw_Pn5180_Instr_LoadRfConfiguration(pDataParams, PHHAL_HW_PN5180_RF_TX_NFC_AI_424_424, PHHAL_HW_PN5180_RF_RX_NFC_AI_424));  /* Masking RxDataRate with Minimum Configuration Index */

        pShadowDefault = (uint16_t*)wPn5180_DefShadow_I18092m_424_Active;
        wShadowCount = sizeof(wPn5180_DefShadow_I18092m_424_Active) / (sizeof(uint16_t) * 2);
        wTxDatarate = PHHAL_HW_RF_DATARATE_424;
        wRxDatarate = PHHAL_HW_RF_DATARATE_424;
        /* Enable active mode */
        pDataParams->bActiveMode = PH_ON;
        break;


    case PHHAL_HW_CARDTYPE_ISO15693:
        /* Use 15693 shadow */
        PH_CHECK_SUCCESS_FCT(statusTmp,phhalHw_Pn5180_Instr_LoadRfConfiguration(pDataParams, PHHAL_HW_PN5180_RF_TX_ISO15693_26_1OF4_ASK100, PHHAL_HW_PN5180_RF_RX_ISO15693_26_1OF4_SC));  /* Masking RxDataRate with Minimum Configuration Index */
        pShadowDefault = (uint16_t*)wPn5180_DefaultShadow_I15693;
        wShadowCount = sizeof(wPn5180_DefaultShadow_I15693) / (sizeof(uint16_t) * 2);
        wTxDatarate = PHHAL_HW_RF_TX_DATARATE_1_OUT_OF_4;
        wRxDatarate = PHHAL_HW_RF_RX_DATARATE_LOW;

        break;

    case PHHAL_HW_CARDTYPE_I18000P3M3:
        /* Use 18000p3m3 shadow */
        PH_CHECK_SUCCESS_FCT(statusTmp,phhalHw_Pn5180_Instr_LoadRfConfiguration(pDataParams, PHHAL_HW_PN5180_RF_TX_ISO180003M3_TARI_9_44_ASK, PHHAL_HW_PN5180_RF_RX_ISO180003M3_MANCH424_2_PERIOD));  /* Masking RxDataRate with Minimum Configuration Index */

        pShadowDefault = (uint16_t*)wPn5180_DefaultShadow_I18000p3m3;
        wShadowCount = sizeof(wPn5180_DefaultShadow_I18000p3m3) / (sizeof(uint16_t) * 2);
        wTxDatarate = PHHAL_HW_RF_TX_DATARATE_I18000P3M3;
        wRxDatarate = PHHAL_HW_RX_I18000P3M3_FL_423_MAN2;
        /*A Timeout of 5 ms has been added*/
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS, 5));
        break;

    default:
        return PH_ADD_COMPCODE(PH_ERR_INVALID_DATA_PARAMS,PH_COMP_HAL);
        break;
    }

    /* Copy over default shadow contents into current shadow. */
    if (bUseDefaultShadow)
    {
        if(pShadowDefault == NULL)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_DATA_PARAMS,PH_COMP_HAL);
        }
        /* Initialize config shadow */
        memset(pDataParams->wCfgShadow, 0x00, PHHAL_HW_PN5180_SHADOW_COUNT);  /* PRQA S 3200 */

        for (wIndex = 0; wIndex < wShadowCount; ++wIndex)
        {
            wConfig = pShadowDefault[wIndex << 1];
            pDataParams->wCfgShadow[wConfig] = pShadowDefault[(wIndex << 1) + 1];
            if((wConfig == PHHAL_HW_CONFIG_TXWAIT_US) && (pDataParams->bActiveMode) && (pDataParams->wTargetMode == PH_OFF))
            {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_SetConfig(pDataParams, wConfig, pDataParams->wCfgShadow[wConfig]));
            }
        }

        pDataParams->wCfgShadow[PHHAL_HW_CONFIG_TXDATARATE_FRAMING] = wTxDatarate;
        pDataParams->wCfgShadow[PHHAL_HW_CONFIG_RXDATARATE_FRAMING] = wRxDatarate;

    }

    /* MIFARE Crypto1 state is disabled by default */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams, PHHAL_HW_CONFIG_DISABLE_MF_CRYPTO1, PH_ON));


    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}


phStatus_t phhalHw_Pn5180_Exchange(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint16_t wOption,
    uint8_t * pTxBuffer,
    uint16_t wTxLength,
    uint8_t ** ppRxBuffer,
    uint16_t * pRxLength
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;
    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen = 0;
    uint16_t    PH_MEMLOC_REM wTmpBufferSize = 0;
    uint32_t    PH_MEMLOC_REM dwIrqWaitFor;
    uint32_t    PH_MEMLOC_REM dwValue;
    uint16_t    PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM pReceivedData[1];
    uint16_t    PH_MEMLOC_REM wDataLenTmp;
    uint8_t     PH_MEMLOC_REM aCrc[2] = {0,0};

    uint32_t    PH_MEMLOC_REM dwTrasceiveState;

    /*bTransmited is a flag which is used to identify if a transmit is followed by a receive.
    If any other api follows a transmit, then it should be reset */
    pDataParams->bTransmited = PH_OFF;

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

    /* retrieve transmit buffer */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetTxBuffer(pDataParams, PH_ON, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

    /* clear internal buffer if requested */
    if (!(wOption & PH_EXCHANGE_LEAVE_BUFFER_BIT))
    {
        pDataParams->wTxBufLen = 0;
        pDataParams->wTxBufStartPos = 0;
    }

    /* Validate received parameter */
    if (pRxLength != NULL)
    {
        *pRxLength = 0;
    }

    if(pDataParams->bJewelActivated == PH_ON)
    {

        wOption = PH_EXCHANGE_DEFAULT;

        /*Enable Crc_B for the data to be transmitted */
        /*PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterOrMask( pDataParams, CRC_TX_CONFIG, CRC_TX_CONFIG_TX_CRC_BYTE2_ENABLE_MASK));*/

        /*Compute Crc_B for the data to be transmitted */
        PH_CHECK_SUCCESS_FCT(statusTmp, phTools_ComputeCrc_B(pTxBuffer, wTxLength, aCrc));
        /* Turn OFF the parity, by default shadow it is ON */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_SetConfig(pDataParams, PHHAL_HW_CONFIG_PARITY, PH_OFF));

        /* Make sure CRC is OFF */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXCRC, PH_OFF));
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_SetConfig(pDataParams, PHHAL_HW_CONFIG_RXCRC, PH_OFF));
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXLASTBITS, 7));

        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterOrMask( pDataParams, TRANSCEIVER_CONFIG, TRANSCEIVER_CONFIG_TX_FRAMESTEP_ENABLE_MASK));
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXWAIT_US,29));

    }


    if ((pDataParams->wTxBufStartPos == 0))
    {
        pTmpBuffer[pDataParams->wTxBufStartPos++] = PHHAL_HW_PN5180_SET_INSTR_SEND_DATA; /* Reserved 0th Location of buffer for Command code */
        pTmpBuffer[pDataParams->wTxBufStartPos++] = 0; /* Reserved 1st Location of buffer for Command code */

        /* Prepend Start Byte (0xF0) in Passive 106kbps during NFCIP exchange. */
#ifndef PN5180_P2P_HW_SYNC_BYTE
        if (pDataParams->bNfcipMode == PH_ON)
        {
            /* Prepend Start Byte 0xF0 in case of Passive Communication at 106kbps BaudRate */
            pTmpBuffer[pDataParams->wTxBufStartPos++] = 0xF0;
        }
#endif
    }

    if (wTxLength != 0)
    {
        /* check wTmpBufferSize fill all the required data to be transmitted */
        if (wTmpBufferSize < (wTxLength + pDataParams->wTxBufLen + pDataParams->wTxBufStartPos))
        {
            /* Can't fit in the HAL buffer what we are trying to send */
            pDataParams->wTxBufLen = 0;
            pDataParams->wTxBufStartPos = 0;
            return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
        }

        /* copy data */
        memcpy(&pTmpBuffer[pDataParams->wTxBufStartPos + pDataParams->wTxBufLen], pTxBuffer, wTxLength);  /* PRQA S 3200 */
        pDataParams->wTxBufLen += wTxLength;

        if(pDataParams->bJewelActivated == PH_ON)
        {
            memcpy(&pTmpBuffer[pDataParams->wTxBufStartPos + pDataParams->wTxBufLen], aCrc, 2);  /* PRQA S 3200 */
            pDataParams->wTxBufLen += 2;
        }
    }

    wTxLength = 0;

    if (wOption & PH_EXCHANGE_BUFFERED_BIT )
    {
        return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
    }

    pDataParams->wTxBufLen += pDataParams->wTxBufStartPos;

    /*Check for maximum bytes that can be sent to IC*/
    if(pDataParams->wTxBufLen > 0x106)
    {
        /*PN5180 TxBuffer is 262 bytes including 2 byte command header*/
        return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
    }

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_ReadRegister(pDataParams,RF_STATUS, &dwValue));
    dwTrasceiveState = (dwValue & RF_STATUS_TRANSCEIVE_STATE_MASK) >> RF_STATUS_TRANSCEIVE_STATE_POS;
    /*Check the Transceive State before loading the Transceive command  if > WAITFORTRANSMIT (1) */
    if((dwTrasceiveState > PHHAL_HW_PN5180_TRANCEIVE_STATE_WAITTRANSMIT) || (dwTrasceiveState == PHHAL_HW_PN5180_TRANCEIVE_STATE_IDLE))
    {
        /*load idle command*/
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_int_IdleCommand(pDataParams));
    }

    /*Execute the Tranceive Command*/
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_int_LoadCommand(pDataParams, PHHAL_HW_PN5180_SYSTEM_TRANSEIVE_CMD));

    /*Set wait IRQ */
    if(pDataParams->bRxMultiple == PH_ON)
    {
        dwIrqWaitFor =  IRQ_STATUS_TIMER1_IRQ_MASK | IRQ_STATUS_GENERAL_ERROR_IRQ_MASK;
    }
    else
    {
        dwIrqWaitFor =  IRQ_STATUS_RX_IRQ_MASK | IRQ_STATUS_TIMER1_IRQ_MASK | IRQ_STATUS_GENERAL_ERROR_IRQ_MASK;
    }
    if (pDataParams->bActiveMode == PH_ON)
    {
        dwIrqWaitFor |= IRQ_STATUS_RF_ACTIVE_ERROR_IRQ_MASK | IRQ_STATUS_RFOFF_DET_IRQ_MASK;
    }

    /* Clear Interrupts  */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_SET_CLEAR, PHHAL_HW_PN5180_IRQ_SET_CLEAR_ALL_MASK));

    (void)phOsal_Event_Consume(E_PH_OSAL_EVT_RF, E_PH_OSAL_EVT_SRC_HAL);

    /* Enable IRQ sources */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_ENABLE, dwIrqWaitFor));

    /* Configure T1 */
    if(pDataParams->bRxMultiple == PH_OFF)
    {
        dwValue = (TIMER1_CONFIG_T1_START_ON_TX_ENDED_MASK | TIMER1_CONFIG_T1_STOP_ON_RX_STARTED_MASK | TIMER1_CONFIG_T1_ENABLE_MASK);
        if(pDataParams->bTimeoutUnit == PHHAL_HW_TIME_MILLISECONDS)
        {
            dwValue |=PHHAL_HW_PN5180_MS_TIMEOUT_PRESCALAR;
        }
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, TIMER1_CONFIG, dwValue ));
    }
    else
    {
        dwValue = (TIMER1_CONFIG_T1_START_ON_TX_ENDED_MASK | TIMER1_CONFIG_T1_ENABLE_MASK);
        if(pDataParams->bTimeoutUnit == PHHAL_HW_TIME_MILLISECONDS)
        {
            dwValue |=PHHAL_HW_PN5180_MS_TIMEOUT_PRESCALAR;
        }
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, TIMER1_CONFIG, dwValue ));
    }

    *(pTmpBuffer+1) = (uint8_t)pDataParams->wCfgShadow[PHHAL_HW_CONFIG_TXLASTBITS];

    /* No Response expected*/
    bNumExpBytes = 0;

    /* During Active Communication mode if internal RF Field is not ON before transmit then Switch ON the RF Field. */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_ReadRegister(pDataParams, RF_STATUS, &dwValue));
    if ((!(dwValue & RF_STATUS_TX_RF_STATUS_MASK)) && (pDataParams->bActiveMode))
    {
        /* field is turned ON */
        phhalHw_Pn5180_FieldOff(pDataParams);
        phhalHw_Pn5180_FieldOn(pDataParams);
    }


    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        pTmpBuffer,
        pDataParams->wTxBufLen,
        bNumExpBytes,
        pReceivedData,
        &wDataLenTmp));

    /* Reset for the next transaction */
    pDataParams->wTxBufStartPos = 0;
    pDataParams->wTxBufLen = 0;

    /* Reset TxLastBits */
    if (pDataParams->wCfgShadow[PHHAL_HW_CONFIG_TXLASTBITS] != 0)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXLASTBITS, 0x00));
    }

    status  = phhalHw_Pn5180_Receive_Int(pDataParams,dwIrqWaitFor,ppRxBuffer,pRxLength,PH_ON);

    if( (status & PH_ERR_MASK) != PH_ERR_SUCCESS)
    {
        /*load idle command*/
        statusTmp = phhalHw_Pn5180_int_IdleCommand(pDataParams);
    }

    statusTmp = phhalHw_Pn5180_Instr_WriteRegisterAndMask( pDataParams, TIMER1_CONFIG, (uint32_t)(~TIMER1_CONFIG_T1_ENABLE_MASK) );

    return status;
}


phStatus_t phhalHw_Pn5180_FieldOn(
    phhalHw_Pn5180_DataParams_t * pDataParams
    )
{
    uint32_t   PH_MEMLOC_REM dwRegister;
    phStatus_t PH_MEMLOC_REM statusTmp;
    uint8_t    PH_MEMLOC_REM  bRFONconfig = 0;

    if(pDataParams->bRfca == PH_OFF)
    {
        bRFONconfig |= 0x01;
    }
    if(pDataParams->bActiveMode == PH_ON)
    {
        bRFONconfig |= 0x02;
    }
    dwRegister = (IRQ_SET_CLEAR_RF_ACTIVE_ERROR_IRQ_CLR_MASK    \
        | IRQ_SET_CLEAR_TX_RFON_IRQ_CLR_MASK    \
        | IRQ_SET_CLEAR_TX_RFOFF_IRQ_CLR_MASK    \
        | IRQ_SET_CLEAR_RFON_DET_IRQ_CLR_MASK    \
        | IRQ_SET_CLEAR_RFOFF_DET_IRQ_CLR_MASK);

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterOrMask(pDataParams, IRQ_SET_CLEAR, dwRegister));

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_RfOn(pDataParams, bRFONconfig));      /* Field ON using Collision Avoidance option */

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_ReadRegister(pDataParams, RF_STATUS, &dwRegister));
    if((dwRegister & RF_STATUS_TX_RF_STATUS_MASK ) == 0)
    {
        if(dwRegister & IRQ_STATUS_RF_ACTIVE_ERROR_IRQ_MASK )
        {
            return PH_ADD_COMPCODE(PH_ERR_EXT_RF_ERROR, PH_COMP_HAL);
        }
        return PH_ADD_COMPCODE(PH_ERR_RF_ERROR, PH_COMP_HAL);
    }

    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_FieldOff(
    phhalHw_Pn5180_DataParams_t * pDataParams
    )
{
    phStatus_t PH_MEMLOC_REM statusTmp;

    /*bTransmited is a flag which is used to identify if a transmit is followed by a receive.
    If any other api follows a transmit, then it should be reset */
    pDataParams->bTransmited = PH_OFF;

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_RfOff(pDataParams));
    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_Wait(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bUnit,
    uint16_t wTimeout
    )
{
    phStatus_t  PH_MEMLOC_REM  statusTmp;
    uint32_t    PH_MEMLOC_REM dwLoadValue;
    uint32_t    PH_MEMLOC_REM wPrescaler;
    uint32_t    PH_MEMLOC_REM wFreq;

    /* Parameter check */
    if ((bUnit != PHHAL_HW_TIME_MICROSECONDS) && (bUnit != PHHAL_HW_TIME_MILLISECONDS))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* When time-out value is zero, there is no wait needed */
    if (wTimeout == 0)
    {
        return PH_ERR_SUCCESS;
    }

    /* Setting the Prescaler frequency according to wTimeout */
    if (bUnit == PHHAL_HW_TIME_MILLISECONDS)
    {
        wFreq = PHHAL_HW_PN5180_MIN_FREQ;
        wPrescaler = 0x3C;
        while(wTimeout > PHHAL_HW_PN5180_MAX_TIME_DELAY_MS)
        {
            wTimeout -= PHHAL_HW_PN5180_MAX_TIME_DELAY_MS;
            /* Calculating the load value */
            dwLoadValue =(uint32_t) (  PHHAL_HW_PN5180_MAX_TIME_DELAY_MS * ( wFreq  / PHHAL_HW_PN5180_CONVERSION_MS_SEC) );
            PH_CHECK_SUCCESS_FCT(statusTmp,phhalHw_Pn5180_Int_Wait( pDataParams, dwLoadValue,  wPrescaler));
        }

        dwLoadValue =(uint32_t) (  wTimeout * ( wFreq  / PHHAL_HW_PN5180_CONVERSION_MS_SEC) );
        PH_CHECK_SUCCESS_FCT(statusTmp,phhalHw_Pn5180_Int_Wait( pDataParams, dwLoadValue,  wPrescaler));
    }
    else
    {
        /* here wTimeout will be in uS and not be Zero */
        wFreq = PHHAL_HW_PN5180_MAX_FREQ;
        wPrescaler = 0x00;
        /*Reducing the division by 2 digits to retain the 2 digit decimal places which were getting wiped out*/
        dwLoadValue =(uint32_t) ( wFreq / (PHHAL_HW_PN5180_CONVERSION_US_SEC/100));
        /*Restoring the division done in the earlier step*/
        dwLoadValue =(uint32_t) ((wTimeout * dwLoadValue)/100);
        PH_CHECK_SUCCESS_FCT(statusTmp,phhalHw_Pn5180_Int_Wait( pDataParams, dwLoadValue,  wPrescaler));
    }

    return PH_ERR_SUCCESS;
}


phStatus_t phhalHw_Pn5180_FieldReset(
    phhalHw_Pn5180_DataParams_t * pDataParams
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;

    /* Switch off the field */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_FieldOff(pDataParams));

    /* wait for field-off time-out */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Wait(
        pDataParams,
        PHHAL_HW_TIME_MILLISECONDS,
        (pDataParams->wFieldOffTime)));

    /* switch on the field again */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_FieldOn(pDataParams));

    /* wait for field-recovery time-out */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Wait(
        pDataParams,
        PHHAL_HW_TIME_MILLISECONDS,
        (pDataParams->wFieldRecoveryTime)));

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}


phStatus_t phhalHw_Pn5180_SetConfig(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint16_t wConfig,
    uint16_t wValue
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint32_t    PH_MEMLOC_REM dwRegister;
    uint32_t    PH_MEMLOC_REM dwValue;
    uint8_t *   PH_MEMLOC_REM pBuffer;
    uint16_t    PH_MEMLOC_REM wBufferSize;
    uint16_t    PH_MEMLOC_REM wBufferLen = 0;
    uint16_t    PH_MEMLOC_REM wFraming;

    uint8_t     PH_MEMLOC_BUF wRegTypeValueSets[12];
    uint16_t    PH_MEMLOC_REM wSizeOfRegTypeValueSets;
    uint32_t    PH_MEMLOC_REM dwTemp;
    float32_t   PH_MEMLOC_REM fTime=0;


    switch(wConfig)
    {
    case PHHAL_HW_CONFIG_PARITY:
    case PHHAL_HW_CONFIG_ASK100:

        if( pDataParams->wCfgShadow[wConfig] != wValue)
        {
            /* Use internal set config */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_SetConfig_Int(pDataParams, wConfig, wValue));

            /* Write config data into shadow */
            pDataParams->wCfgShadow[wConfig] = wValue;
        }
        break;

    case PHHAL_HW_CONFIG_TXLASTBITS:

        /* Check parameter */
        if (wValue > MASK_TX_LASTBITS)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        /* Write config data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        break;

    case PHHAL_HW_CONFIG_TXCRC:
    case PHHAL_HW_CONFIG_RXCRC:
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_SetConfig_Int(pDataParams, wConfig, wValue));
        /* Write config data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        break;

    case PHHAL_HW_CONFIG_RXALIGN:
        /* Check the parameter */
        if (wValue >  MASK_RX_LASTBITS)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }


        wSizeOfRegTypeValueSets = 0;

        /*Clear the Bits of CRC_RX_CONFIG_RX_BIT_ALIGN_MASK*/
        dwTemp = (uint32_t) ~( CRC_RX_CONFIG_RX_BIT_ALIGN_MASK );
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = CRC_RX_CONFIG;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_AND_MASK;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

        /*Set the new value  of RX_BIT_ALIGN*/
        dwTemp = (uint32_t)((wValue << CRC_RX_CONFIG_RX_BIT_ALIGN_POS) & CRC_RX_CONFIG_RX_BIT_ALIGN_MASK);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = CRC_RX_CONFIG;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_OR_MASK;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

        /*Send the array to the IC*/
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterMultiple(pDataParams,  wRegTypeValueSets, wSizeOfRegTypeValueSets));

        /* Write config data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        break;

    case PHHAL_HW_CONFIG_CLEARBITSAFTERCOLL:

        if (pDataParams->wCfgShadow[wConfig] != wValue)
        {
            /* Disbale */
            if (wValue == PH_OFF)
            {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterOrMask(pDataParams, CRC_RX_CONFIG, CRC_RX_CONFIG_VALUES_AFTER_COLLISION_MASK));
            }
            else
            {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, CRC_RX_CONFIG, (0xFFFFFFFF^CRC_RX_CONFIG_VALUES_AFTER_COLLISION_MASK)));
            }
            pDataParams->wCfgShadow[wConfig] = wValue;
        }
        break;


    case PHHAL_HW_CONFIG_TXWAIT_MS:
        /* Parameter checking */
        if (wValue > 650) /* To set Max value for wValue in uS i.e, 0xFFFF )*/
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
        /* Store the value in terms of uS in shadow table*/
        pDataParams->dwTxWaitMs =  wValue;

        /* Set TxWait */
        fTime = 0;
        dwValue = PHHAL_HW_PN5180_TR_RX_PRESCALAR;
        dwTemp = wValue*1000;
        if(dwTemp)
        {
            fTime = (float32_t)(((float32_t)dwTemp * 13.56) / (float32_t)(dwValue));
            dwTemp = (uint32_t)fTime;
            dwValue |= (uint32_t)((dwTemp ) << TX_WAIT_CONFIG_TX_WAIT_VALUE_POS);
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_WriteRegister(pDataParams, TX_WAIT_CONFIG, dwValue));
        }
        else
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_WriteRegister(pDataParams, TX_WAIT_CONFIG, dwValue));
        }
        break;

    case PHHAL_HW_CONFIG_RXWAIT_US:
        /* Write config data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;

        /* Set Rx Wait(deaf) bits */
        dwValue = PHHAL_HW_PN5180_TR_RX_PRESCALAR;
        dwTemp = wValue;
        if(dwTemp)
        {
            fTime = (float32_t)(((float32_t)dwTemp * 13.56) / (float32_t)(dwValue));
            dwTemp = (uint32_t)fTime;
            dwValue |= (uint32_t)dwTemp << RX_WAIT_CONFIG_RX_WAIT_VALUE_POS;
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, RX_WAIT_CONFIG, dwValue ));
        }
        else
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, RX_WAIT_CONFIG, dwValue ));
        }
        break;

    case PHHAL_HW_CONFIG_TXWAIT_US:

        /* Write configuration data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;

        /* Set TxWait */
        fTime = 0;
        dwValue = PHHAL_HW_PN5180_TR_RX_PRESCALAR;
        dwTemp = wValue;

        if(dwTemp)
        {
            fTime = (float32_t)(((float32_t)dwTemp * 13.56) / (float32_t)(dwValue));
            dwTemp = (uint32_t)fTime;
            dwValue |= (uint32_t)((dwTemp ) << TX_WAIT_CONFIG_TX_WAIT_VALUE_POS);
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_WriteRegister(pDataParams, TX_WAIT_CONFIG, dwValue));
        }
        else
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_WriteRegister(pDataParams, TX_WAIT_CONFIG, dwValue));
        }
        break;

    case PHHAL_HW_CONFIG_MODINDEX:
        /* Parameter checking */
        if (wValue > 0x1F)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        wSizeOfRegTypeValueSets = 0;

        /*Clear the Bits of ANA_RF_CONTROL_TX_TX_RESIDUAL_CARRIER_MASK*/
        dwTemp = (uint32_t) ~( ANA_RF_CONTROL_TX_TX_RESIDUAL_CARRIER_MASK );
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = ANA_RF_CONTROL_TX;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_AND_MASK;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

        /*Set the new value  */
        dwTemp = (uint32_t)((wValue << ANA_RF_CONTROL_TX_TX_RESIDUAL_CARRIER_POS) & ANA_RF_CONTROL_TX_TX_RESIDUAL_CARRIER_MASK);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = ANA_RF_CONTROL_TX;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_OR_MASK;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

        /*Send the array to the IC*/
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterMultiple(pDataParams,  wRegTypeValueSets, wSizeOfRegTypeValueSets));

        /* Write config data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        break;

    case PHHAL_HW_CONFIG_DISABLE_MF_CRYPTO1:
        /* Disable crypto, enabling is not supported */
        if ((wValue != PH_OFF) && ( pDataParams->bMfcCryptoEnabled == PH_ON))
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, SYSTEM_CONFIG, ~(SYSTEM_CONFIG_MFC_CRYPTO_ON_MASK)));
            pDataParams->bMfcCryptoEnabled = PH_OFF;
        }
        break;

    case PHHAL_HW_CONFIG_TXBUFFER_OFFSET:

        /* Modify additional info parameter */
        pDataParams->wAdditionalInfo = wValue + PHHAL_HW_PN5180_EXCHANGE_HEADER_SIZE;
#ifndef PN5180_P2P_HW_SYNC_BYTE
        if(pDataParams->bNfcipMode == PH_ON)
        {
             pDataParams->wAdditionalInfo += 1;
        }
#endif
        break;

    case PHHAL_HW_CONFIG_RXBUFFER_STARTPOS:

        /* Retrieve RxBuffer parameters */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetRxBuffer(pDataParams, PH_ON, &pBuffer, &wBufferLen, &wBufferSize));

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
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetTxBuffer(pDataParams, PH_ON, &pBuffer, &wBufferLen, &wBufferSize));

        /* Check parameter, must not exceed TxBufferSize */
        if (wValue > pDataParams->wTxBufSize)
        {
            return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
        }

        /* set buffer length */
        pDataParams->wTxBufLen = wValue;
        break;

    case PHHAL_HW_CONFIG_TXBUFFER:

        /* Retrieve TxBuffer parameters */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetTxBuffer(pDataParams, PH_ON, &pBuffer, &wBufferLen, &wBufferSize));

        /* Check parameter, must not exceed TxBufferSize */
        if (pDataParams->wAdditionalInfo >= pDataParams->wTxBufSize)
        {
            return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
        }

        /* Modify TxBuffer byte */
        pBuffer[pDataParams->wAdditionalInfo] = (uint8_t)wValue;
        break;

    case PHHAL_HW_CONFIG_TXDATARATE_FRAMING:
        /* Update teh framing, based on the Higher byte */
        wFraming = wValue & PHHAL_HW_RF_FRAMING_OPTION_MASK;
        wFraming = wFraming >> 0x08;

        if((wFraming != PHHAL_HW_CARDTYPE_CURRENT) && (wFraming != pDataParams->bCardType))
        {
            if((wValue & PHHAL_HW_RF_FRAMING_OPTION_MASK) != PHHAL_HW_RF_TYPE_ACTIVE_FRAMING)
            {
                pDataParams->bCardType = (uint8_t)wFraming;
            }
        }

        /* Update the Baudrate based on the lower byte */
        wValue = wValue & PHHAL_HW_RF_DATARATE_OPTION_MASK;

        /* Evaluate hardware settings */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_SetCardMode(
            pDataParams,
            wValue,
            PHHAL_HW_RF_DATARATE_NO_CHANGE,
            pDataParams->wCfgShadow[PHHAL_HW_CONFIG_SUBCARRIER]));


        /* Write config data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        break;

    case PHHAL_HW_CONFIG_RXDATARATE_FRAMING:

        /* Update teh framing, based on the Higher byte */
        wFraming = wValue & PHHAL_HW_RF_FRAMING_OPTION_MASK;
        wFraming = wFraming >> 0x08;

        if((wFraming != PHHAL_HW_CARDTYPE_CURRENT) && (wFraming != pDataParams->bCardType))
        {
            if((wValue & PHHAL_HW_RF_FRAMING_OPTION_MASK) != PHHAL_HW_RF_TYPE_ACTIVE_FRAMING)
            {
                pDataParams->bCardType = (uint8_t)wFraming;
            }
        }

        /* Update the Baudrate based on the lower byte */
        wValue = wValue & PHHAL_HW_RF_DATARATE_OPTION_MASK;


        /* Evaluate hardware settings */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_SetCardMode(
            pDataParams,
            PHHAL_HW_RF_DATARATE_NO_CHANGE,
            wValue,
            pDataParams->wCfgShadow[PHHAL_HW_CONFIG_SUBCARRIER]));


        /* Write config data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        break;

    case PHHAL_HW_CONFIG_TIMEOUT_VALUE_US:

        PH_CHECK_SUCCESS_FCT(statusTmp,
            phhalHw_Pn5180_SetTmo(
            pDataParams,
            wValue,
            PHHAL_HW_TIME_MICROSECONDS));
        /* Write config data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        pDataParams->bTimeoutUnit = PHHAL_HW_TIME_MICROSECONDS;
        break;

    case PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS:

        PH_CHECK_SUCCESS_FCT(statusTmp,
            phhalHw_Pn5180_SetTmo(
            pDataParams,
            wValue,
            PHHAL_HW_TIME_MILLISECONDS));
        /* Write config data into shadow */
        pDataParams->wCfgShadow[wConfig] = wValue;
        pDataParams->bTimeoutUnit = PHHAL_HW_TIME_MILLISECONDS;
        break;

    case PHHAL_HW_CONFIG_SUBCARRIER:

        /* Parameter check */
        if ((pDataParams->bCardType != PHHAL_HW_CARDTYPE_ISO15693) &&
            (pDataParams->bCardType != PHHAL_HW_CARDTYPE_I18000P3M3))
        {
            return PH_ADD_COMPCODE(PH_ERR_USE_CONDITION, PH_COMP_HAL);
        }

        /* Evaluate hardware settings */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_SetCardMode(
            pDataParams,
            pDataParams->wCfgShadow[PHHAL_HW_CONFIG_TXDATARATE_FRAMING],
            pDataParams->wCfgShadow[PHHAL_HW_CONFIG_RXDATARATE_FRAMING],
            wValue));

        /* Update Subcarrier setting */
        pDataParams->wCfgShadow[PHHAL_HW_CONFIG_SUBCARRIER] = wValue;

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

        /* Store config data */
        pDataParams->wFieldOffTime = wValue;
        break;

    case PHHAL_HW_CONFIG_FIELD_RECOVERY_TIME:

        /* Store config data */
        pDataParams->wFieldRecoveryTime = wValue;
        break;

    case PHHAL_HW_CONFIG_JEWEL_MODE:

        if (wValue == PH_ON)
        {
            pDataParams->bJewelActivated = (uint8_t)wValue;
        }
        else if (wValue == PH_OFF)
        {
            pDataParams->bJewelActivated = (uint8_t)wValue;
        }
        else
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
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


    case PHHAL_HW_CONFIG_OPE_MODE:

        pDataParams->bOpeMode = (uint8_t)wValue;
        break;

    case PHHAL_HW_CONFIG_SET_EMD:
        pDataParams->bEmdFlag = (uint8_t)wValue;
        if(wValue == 0x1)
        {
        	PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_WriteRegister(pDataParams, EMD_CONTROL,PHHAL_HW_PN5180_EMD_2P5));
        }
        else
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_WriteRegister(pDataParams, EMD_CONTROL,PHHAL_HW_PN5180_NO_EMD));
        }
        break;

    case PHHAL_HW_CONFIG_SETMINFDT:

        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_SetMinFDT(pDataParams, wValue));
        break;

    case PHHAL_HW_CONFIG_RXMULTIPLE:

        if (wValue == PH_ON)
        {
            pDataParams->bRxMultiple = PH_ON;
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterOrMask( pDataParams, TRANSCEIVER_CONFIG, TRANSCEIVER_CONFIG_RX_MULTIPLE_ENABLE_MASK ));
        }
        else
        {
            pDataParams->bRxMultiple = PH_OFF;
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask( pDataParams, TRANSCEIVER_CONFIG, (uint32_t )~(TRANSCEIVER_CONFIG_RX_MULTIPLE_ENABLE_MASK) ));
        }
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

        if(wValue == PH_ON)
        {
            /* Enable RF-ON Interrupt*/
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_ENABLE, IRQ_ENABLE_RFON_DET_IRQ_SET_ENABLE_MASK));
        }
        else
        {
            /* Disable RF-ON Interrupt*/
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, IRQ_ENABLE,  ~(uint32_t)IRQ_ENABLE_RFON_DET_IRQ_SET_ENABLE_MASK));
        }
        break;


    case PHHAL_HW_CONFIG_NFCIP_STARTBYTE:
        if((wValue != PH_ON) && (wValue != PH_OFF))
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
        pDataParams->bNfcipMode = (uint8_t)wValue;
#ifdef  PN5180_P2P_HW_SYNC_BYTE
        if (pDataParams->bNfcipMode == PH_ON)
        {
            phhalHw_Pn5180_AutoSyncByte(pDataParams);
        }
#endif
        break;

    case PHHAL_HW_CONFIG_SET_SYMBOL_SEND:

        /* check parameter */
        if ((wValue != PH_ON) && (wValue != PH_OFF))
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        /* PH_ON implies Clearing Bit-3 (DataEn Bit in TxDataNum). */
        if (wValue == PH_ON)
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, TX_CONFIG,  ~(uint32_t)TX_CONFIG_TX_DATA_ENABLE_MASK));
        }
        /* PH_OFF implies Setting Bit-3 (DataEn Bit in TxDataNum). */
        else
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterOrMask(pDataParams, TX_CONFIG, TX_CONFIG_TX_DATA_ENABLE_MASK));
        }
        break;

    case PHHAL_HW_CONFIG_SYMBOL_START:

        /* Parameter check */
        if ((pDataParams->bCardType != PHHAL_HW_CARDTYPE_ICODEEPCUID) &&
            (pDataParams->bCardType != PHHAL_HW_CARDTYPE_ISO15693) &&
            (pDataParams->bCardType != PHHAL_HW_CARDTYPE_I18000P3M3))
        {
            return PH_ADD_COMPCODE(PH_ERR_USE_CONDITION, PH_COMP_HAL);
        }

        /* Parameter check #2 */
        switch (wValue)
        {
        case PHHAL_HW_SYMBOL_ICODEEPCUID_SSOF:
        case PHHAL_HW_SYMBOL_ICODEEPCUID_LSOF:
        case PHHAL_HW_SYMBOL_ICODEEPCUID_CS:
            if (pDataParams->bCardType != PHHAL_HW_CARDTYPE_ICODEEPCUID)
            {
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
            }
            break;
        case PHHAL_HW_SYMBOL_I15693_SOF:
            if (pDataParams->bCardType != PHHAL_HW_CARDTYPE_ISO15693)
            {
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
            }
            break;
        case PHHAL_HW_SYMBOL_I18000P3M3_PREAMBLE:
        case PHHAL_HW_SYMBOL_I18000P3M3_FSYNC:
            if (pDataParams->bCardType != PHHAL_HW_CARDTYPE_I18000P3M3)
            {
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
            }
            break;
        case PH_OFF:
            break;
        default:
            return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_PARAMETER, PH_COMP_HAL);
        }

        wSizeOfRegTypeValueSets = 0;

        /*Clear the Bits of TX_CONFIG_TX_START_SYMBOL_MASK & DATA_ENABLE*/
        if(wValue == PH_OFF)
        {
            dwTemp = (uint32_t) ~( TX_CONFIG_TX_START_SYMBOL_MASK | TX_CONFIG_TX_DATA_ENABLE_MASK);
        }
        else
        {
            dwTemp = (uint32_t) ~( TX_CONFIG_TX_START_SYMBOL_MASK);
        }


        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = TX_CONFIG;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_AND_MASK;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

        dwValue = 0;

        /* Set new symbol */
        switch (wValue)
        {
        case PHHAL_HW_SYMBOL_ICODEEPCUID_SSOF:
            dwValue = 0x3;
            break;
        case PHHAL_HW_SYMBOL_ICODEEPCUID_LSOF:
            dwValue = 0x1;
            break;
        case PHHAL_HW_SYMBOL_ICODEEPCUID_CS:
            dwValue = 0x2;
            break;
        case PHHAL_HW_SYMBOL_I15693_SOF:
            dwValue = 0x3;
            break;
        case PHHAL_HW_SYMBOL_I18000P3M3_PREAMBLE:
            dwValue = 0x1;
            break;
        case PHHAL_HW_SYMBOL_I18000P3M3_FSYNC:
            dwValue = 0x2;
            break;
        default:
            break;
        }

        /*Set the new value  */
        dwTemp = (uint32_t)((dwValue << TX_CONFIG_TX_START_SYMBOL_POS) & TX_CONFIG_TX_START_SYMBOL_MASK);
        dwTemp |= (uint32_t)((dwValue << TX_CONFIG_TX_DATA_ENABLE_POS) & TX_CONFIG_TX_DATA_ENABLE_MASK);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = TX_CONFIG;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_OR_MASK;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

        /*Send the array to the IC*/
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterMultiple(pDataParams,  wRegTypeValueSets, wSizeOfRegTypeValueSets));

        break;

    case PHHAL_HW_CONFIG_SYMBOL_END:

        /* Parameter check */
        if (pDataParams->bCardType != PHHAL_HW_CARDTYPE_ICODEEPCUID)
        {
            return PH_ADD_COMPCODE(PH_ERR_USE_CONDITION, PH_COMP_HAL);
        }

        /* Parameter check #2 */
        switch (wValue)
        {
        case PHHAL_HW_SYMBOL_ICODEEPCUID_CEOF:
        case PH_OFF:
            break;
        default:
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }


        /* Read out TxFrameCon register */

        /* Set new symbol */
        switch (wValue)
        {
        case PHHAL_HW_SYMBOL_ICODEEPCUID_CEOF:
            wSizeOfRegTypeValueSets = 0;

            /*Clear the Bits of TX_CONFIG_TX_STOP_SYMBOL_MASK*/
            dwTemp = (uint32_t) ~( TX_CONFIG_TX_STOP_SYMBOL_MASK );
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = TX_CONFIG;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_AND_MASK;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

            /*Set the new value  */
            dwTemp = (uint32_t)((0x0C << TX_CONFIG_TX_STOP_SYMBOL_POS) & TX_CONFIG_TX_STOP_SYMBOL_MASK);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = TX_CONFIG;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_OR_MASK;
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
            wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

            /*Send the array to the IC*/
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterMultiple(pDataParams,  wRegTypeValueSets, wSizeOfRegTypeValueSets));
            break;
        default:
            break;
        }


        break;

    case PHHAL_HW_CONFIG_CRCTYPE:


        if(!(wValue & (PHHAL_HW_CRCTYPE_CRC5 | PHHAL_HW_CRCTYPE_CRC16 | PHHAL_HW_CRCOPTION_INVERTED)))
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }


        /* in case of 18000P3M3 for CRC5 calculation we need preset value of 9 which is not supported by
        * So flag is enable to perform software calculation of CRC5 in Pn5180 HAL
        */

        dwValue = 0;
        /*dwRegister will contain all the bits that need to be zeroed out*/
        dwRegister = 0;

        if((wValue & PHHAL_HW_CRCTYPE_CRC5) && (pDataParams->bCardType == PHHAL_HW_CARDTYPE_I18000P3M3))
        {
            dwValue |= CRC_TX_CONFIG_TX_CRC_TYPE_MASK;
            dwRegister |= CRC_TX_CONFIG_TX_CRC_INV_MASK;
            dwRegister |= CRC_TX_CONFIG_TX_CRC_PRESET_SEL_MASK;
            dwValue |= (0x5 << CRC_TX_CONFIG_TX_CRC_PRESET_SEL_POS);
        }
        else if(wValue & PHHAL_HW_CRCTYPE_CRC16)
        {
            dwRegister |= CRC_TX_CONFIG_TX_CRC_TYPE_MASK;
        }
        else
        {
            /* QAC */
        }

        if(wValue & PHHAL_HW_CRCOPTION_INVERTED)
        {
            dwValue |= CRC_TX_CONFIG_TX_CRC_INV_MASK;
        }
        else
        {
            dwRegister |= CRC_TX_CONFIG_TX_CRC_INV_MASK;
        }

        if ((pDataParams->bCardType == PHHAL_HW_CARDTYPE_I18000P3M3) && (wValue & PHHAL_HW_CRCTYPE_CRC16))
        {
            dwRegister |= CRC_TX_CONFIG_TX_CRC_PRESET_SEL_MASK;
            dwValue |= (0x3 << CRC_TX_CONFIG_TX_CRC_PRESET_SEL_POS);
        }


        wSizeOfRegTypeValueSets = 0;

        /*Clear the Bits */
        dwTemp = (uint32_t) ~( dwRegister );
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = CRC_TX_CONFIG;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_AND_MASK;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

        /*Set the new value  */
        dwTemp = dwValue;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = CRC_TX_CONFIG;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = PHHAL_HW_PN5180_WRITE_MULTIPLE_TYPE_WRITE_OR_MASK;
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>8);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>16);
        wRegTypeValueSets[wSizeOfRegTypeValueSets++] = (uint8_t)(dwTemp>>24);

        /*Send the array to the IC*/
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterMultiple(pDataParams,  wRegTypeValueSets, wSizeOfRegTypeValueSets));

        break;

    case PHHAL_HW_CONFIG_MFHALTED:
        if((wValue != PH_ON) && (wValue != PH_OFF))
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
        if(wValue == PH_ON)
        {
            PH_CHECK_SUCCESS_FCT(statusTmp,
                phhalHw_Pn5180_Instr_WriteRegisterOrMask(pDataParams, SYSTEM_CONFIG, SYSTEM_CONFIG_AUTOCOLL_STATE_A_MASK));
           pDataParams->bCardMode  = PH_ON;
        }
        else
        {
            PH_CHECK_SUCCESS_FCT(statusTmp,
                phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, SYSTEM_CONFIG, (uint32_t)~SYSTEM_CONFIG_AUTOCOLL_STATE_A_MASK));
            pDataParams->bCardMode  = PH_OFF;

        }
        break;

    default:
        return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_PARAMETER, PH_COMP_HAL);
        break;
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}


phStatus_t phhalHw_Pn5180_GetConfig(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint16_t wConfig,
    uint16_t * pValue
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t *   PH_MEMLOC_REM pBuffer;
    uint16_t    PH_MEMLOC_REM wBufferLen;
    uint16_t    PH_MEMLOC_REM wBufferSize;
    uint32_t    PH_MEMLOC_REM dwRegister;
    switch(wConfig)
    {

    case PHHAL_HW_CONFIG_OPE_MODE:
        *pValue = pDataParams->bOpeMode;
        break;

    case PHHAL_HW_CONFIG_SET_EMD:
        *pValue = pDataParams->bEmdFlag;
        break;

    case PHHAL_HW_CONFIG_PARITY:
    case PHHAL_HW_CONFIG_TXCRC:
    case PHHAL_HW_CONFIG_RXCRC:
    case PHHAL_HW_CONFIG_TXLASTBITS:
    case PHHAL_HW_CONFIG_CLEARBITSAFTERCOLL:
    case PHHAL_HW_CONFIG_TXDATARATE_FRAMING:
    case PHHAL_HW_CONFIG_RXDATARATE_FRAMING:
    case PHHAL_HW_CONFIG_MODINDEX:
    case PHHAL_HW_CONFIG_ASK100:
    case PHHAL_HW_CONFIG_SUBCARRIER:
    case PHHAL_HW_CONFIG_TXWAIT_US:
    case PHHAL_HW_CONFIG_RXWAIT_US:

        /* Read config from shadow */
        *pValue = pDataParams->wCfgShadow[wConfig];
        break;

    case PHHAL_HW_CONFIG_TXBUFFER_OFFSET:
    case PHHAL_HW_CONFIG_RXLASTBITS:

        *pValue = pDataParams->wAdditionalInfo;
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

    case PHHAL_HW_CONFIG_RFRESET_ON_TIMEOUT:

        *pValue = pDataParams->bRfResetAfterTo;
        break;

    case PHHAL_HW_CONFIG_DISABLE_MF_CRYPTO1:

        /* Check Crypto1On Status */
        if ( pDataParams->bMfcCryptoEnabled == PH_ON)
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
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetRxBuffer(pDataParams, PH_ON, &pBuffer, &wBufferLen, &wBufferSize));

        /* Return parameter */
        *pValue = wBufferSize;
        break;

    case PHHAL_HW_CONFIG_TXBUFFER_BUFSIZE:

        /* Retrieve TxBuffer parameters */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetTxBuffer(pDataParams, PH_ON, &pBuffer, &wBufferLen, &wBufferSize));

        /* Return parameter */
        *pValue = wBufferSize;
        break;

    case PHHAL_HW_CONFIG_TXBUFFER_LENGTH:

        /* Retrieve TxBuffer parameters */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetTxBuffer(pDataParams, PH_ON, &pBuffer, &wBufferLen, &wBufferSize));

        /* Return parameter */
        *pValue = wBufferLen;
        break;

    case PHHAL_HW_CONFIG_TXBUFFER:

        /* Retrieve TxBuffer parameters */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetTxBuffer(pDataParams, PH_ON, &pBuffer, &wBufferLen, &wBufferSize));

        /* Check additional info parameter */
        if (pDataParams->wAdditionalInfo >= pDataParams->wTxBufSize)
        {
            return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
        }

        /* Retrieve TxBuffer byte */
        *pValue = (uint16_t)pBuffer[pDataParams->wAdditionalInfo];
        break;

    case PHHAL_HW_CONFIG_RXMULTIPLE:
        *pValue =  pDataParams->bRxMultiple;
        break;

    case PHHAL_HW_CONFIG_NFCIP_STARTBYTE:
        *pValue = pDataParams->bNfcipMode;
        break;

    case PHHAL_HW_CONFIG_TARGET_DATARATE_FRAMING:
        *pValue = pDataParams->wTargetMode;
        break;

    case PHHAL_HW_CONFIG_ACTIVEMODE:
        /* Return parameter */
        *pValue = (uint16_t)pDataParams->bActiveMode;
        break;

    case PHHAL_HW_CONFIG_EXT_RF_ON:
        /* Retrieve Status register */
        PH_CHECK_SUCCESS_FCT(statusTmp,
            phhalHw_Pn5180_Instr_ReadRegister(pDataParams, RF_STATUS, &dwRegister));

        if (dwRegister & RF_STATUS_RF_DET_STATUS_MASK)
        {
            /* Clear the EXT_RF_ON and EXT_RF_OFF bits*/
            dwRegister = IRQ_SET_CLEAR_RFON_DET_IRQ_CLR_MASK|IRQ_SET_CLEAR_RFOFF_DET_IRQ_CLR_MASK;

            PH_CHECK_SUCCESS_FCT(statusTmp,
                phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_SET_CLEAR, dwRegister));
        }

        /* Retrieve Status register */
        PH_CHECK_SUCCESS_FCT(statusTmp,
            phhalHw_Pn5180_Instr_ReadRegister(pDataParams, RF_STATUS, &dwRegister));

        /* Check EXT_RF_ON bit */
        if (dwRegister & RF_STATUS_RF_DET_STATUS_MASK)
        {
            *pValue = PH_ON; /* ON if external RF field is detected */
        }
        else
        {
            *pValue = PH_OFF; /* OFF if no external RF field is detected */
        }
        break;
    case PHHAL_HW_CONFIG_MFHALTED:
    {
        PH_CHECK_SUCCESS_FCT(statusTmp,
            phhalHw_Pn5180_Instr_ReadRegister(pDataParams, SYSTEM_CONFIG, &dwRegister));
        if(dwRegister & SYSTEM_CONFIG_AUTOCOLL_STATE_A_MASK)
        {
            *pValue = PH_ON;
        }
        else
        {
            *pValue = PH_OFF;
        }
        break;
    }

    default:
        return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_PARAMETER, PH_COMP_HAL);
        break;
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}


phStatus_t phhalHw_Pn5180_MfcAuthenticateKeyNo(
    phhalHw_Pn5180_DataParams_t * pDataParams,
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

    /* Change component code for invalid parameter */
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
    return phhalHw_Pn5180_MfcAuthenticate(pDataParams, bBlockNo, bKeyType, pKey, pUid);
#else
    /* satisfy compiler */
    if (pDataParams || bBlockNo || bKeyType || wKeyNo || wKeyVersion || pUid);
    return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_HAL);
#endif
}

phStatus_t phhalHw_Pn5180_MfcAuthenticate(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bBlockNo,
    uint8_t bKeyType,
    uint8_t * pKey,
    uint8_t * pUid
    )
{
    phStatus_t PH_MEMLOC_REM statusTmp;
    phStatus_t PH_MEMLOC_REM status;

    status = phhalHw_Pn5180_Instr_MifareAuthenticate(
        pDataParams,
        pKey,
        bKeyType,
        bBlockNo,
        pUid);

    /* Validate the Response */
    if( status )
    {   /* If status is other than success load idle command */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_int_IdleCommand(pDataParams));
    }

    return PH_ADD_COMPCODE(status, PH_COMP_HAL);
}


phStatus_t phhalHw_Pn5180_SetMinFDT(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint16_t wValue
    )
{
    phStatus_t PH_MEMLOC_REM statusTmp = 0;
    uint16_t   PH_MEMLOC_REM wTimer = 0;
    uint16_t   PH_MEMLOC_REM wTxRate = 0;

    if (wValue == PH_ON)
    {
        /*Backup the old Timer values and set min FDT*/
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetConfig(pDataParams,
            PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS, &wTimer));
        pDataParams->dwFdtPc = wTimer;
        /* Calculate values for Microsecond values */
        /* Get the data rate */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetConfig(pDataParams,
            PHHAL_HW_CONFIG_TXDATARATE_FRAMING, &wTxRate));
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
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_SetConfig(pDataParams,
            PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, wTimer));
    }
    else if (wValue == PH_OFF)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_SetConfig(pDataParams,
            PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,pDataParams->dwFdtPc));
    }
    else
    {
        /* Do nothing*/
    }
    return PH_ERR_SUCCESS;
}



phStatus_t phhalHw_Pn5180_Transmit(
    phhalHw_Pn5180_DataParams_t * pDataParams,   /**< [In] Pointer to this layer's parameter structure. */
    uint16_t wOption,                            /**< [In] Option parameter. */
    uint8_t * pTxBuffer,                         /**< [In] Data to transmit. */
    uint16_t wTxLength                           /**< [In] Number of bytes to transmit. */
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_INTERNAL_ERROR;
    uint32_t    PH_MEMLOC_REM dwIrqWaitFor = 0;
    uint8_t *   PH_MEMLOC_REM pTmpBuffer;
    uint32_t    PH_MEMLOC_REM dwRegValue;
    uint32_t    PH_MEMLOC_REM dwValue = 0;
    uint32_t    PH_MEMLOC_REM dwTrasceiveState;
    uint16_t    PH_MEMLOC_REM wTmpBufferLen;
    uint16_t    PH_MEMLOC_REM wTmpBufferSize;
    uint16_t    PH_MEMLOC_REM bNumExpBytes;
    uint8_t     PH_MEMLOC_REM pReceivedData[1];
    uint16_t    PH_MEMLOC_REM bOffset=0;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;

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

    /* In Target Mode it's not allowed to have same buffers for transmission and reception
    *  since the data to transmit would get overwritten by received data. */
    if ((pDataParams->wTargetMode != PH_OFF) &&
        (pDataParams->pTxBuffer == pDataParams->pRxBuffer))
    {
        return PH_ADD_COMPCODE(PH_ERR_USE_CONDITION, PH_COMP_HAL);
    }

    if((pDataParams->wTargetMode != PH_OFF))
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_ReadRegister(pDataParams,RF_STATUS, &dwRegValue));
        dwTrasceiveState = (dwRegValue& RF_STATUS_TRANSCEIVE_STATE_MASK) >> RF_STATUS_TRANSCEIVE_STATE_POS;
        if (dwTrasceiveState  !=PHHAL_HW_PN5180_TRANCEIVE_STATE_WAITTRANSMIT )
        {
            return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR , PH_COMP_HAL);
        }
    }

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetTxBuffer(pDataParams, PH_ON, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

    if ( PH_TRANSMIT_PREV_FRAME & wOption )
    {
        /* Length to be transmitted should not be zero and should not be in Reader Mode */
        if ( (pDataParams->wTargetMode == PH_OFF) || (wTxLength == 0x00) )
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
        else
        {
            /* Check parameter, must not exceed TxBufferSize */
            if (wTxLength > wTmpBufferSize)
            {
                return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
            }
            else
            {
                /* Include the PN5180 Instruction Header Bytes len  */
                pDataParams->wTxBufStartPos = PHHAL_HW_PN5180_EXCHANGE_HEADER_SIZE;
#ifndef PN5180_P2P_HW_SYNC_BYTE
                if(pDataParams->bNfcipMode == PH_ON)
                {
                   /* Include the Startbyte Len */
                   pDataParams->wTxBufStartPos++;
                }
#endif
                pDataParams->wTxBufLen = wTxLength;
                wTxLength = 0;
            }
        }
    }

    if (!(wOption & (PH_TRANSMIT_LEAVE_BUFFER_BIT | PH_TRANSMIT_PREV_FRAME)))
    {
        pDataParams->wTxBufLen = 0;
        pDataParams->wTxBufStartPos = 0;
    }

    if ((pDataParams->wTxBufStartPos == 0))
    {
        pTmpBuffer[pDataParams->wTxBufStartPos++] = PHHAL_HW_PN5180_SET_INSTR_SEND_DATA; /* Reserved 0th Location of buffer for Command code */
        pTmpBuffer[pDataParams->wTxBufStartPos++] = 0; /* Reserved 1st Location of buffer for Command code */

#ifndef PN5180_P2P_HW_SYNC_BYTE
        if (pDataParams->bNfcipMode == PH_ON)
        {
            pTmpBuffer[pDataParams->wTxBufStartPos++] = 0xF0;
        }
#endif
        bOffset = pDataParams->wTxBufStartPos;
    }

    if (wTxLength != 0)
    {
        if (wTxLength > (wTmpBufferSize - wTmpBufferLen - bOffset /* Offset Reserved for Transmit Data Command and Detect sync if TYPE A if present */))
        {
            pDataParams->wTxBufLen = 0;
            pDataParams->wTxBufStartPos = 0;
            return PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_HAL);
        }
        memcpy(&pTmpBuffer[pDataParams->wTxBufStartPos + pDataParams->wTxBufLen], pTxBuffer, wTxLength);  /* PRQA S 3200 */
        pDataParams->wTxBufLen += wTxLength;
    }

    wTxLength = 0;

    if (wOption & PH_EXCHANGE_BUFFERED_BIT)
    {
        return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
    }

    pDataParams->wTxBufLen += pDataParams->wTxBufStartPos;

    if(pDataParams->wTargetMode != PH_OFF)
    {
        /* In Passive Target Mode if External RF is OFF then Transmit should return External RF Error,
        * instead of going further and performing Transmit. */
        if (pDataParams->bActiveMode == PH_OFF)
        {
            /* Read the External RF Field Detection Bit from RF Status Register */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_ReadRegister(pDataParams,RF_STATUS, &dwRegValue));
            if(!(dwRegValue & RF_STATUS_RF_DET_STATUS_MASK))
            {
                /* External RF Field is OFF. Transmit should not be performed. */
                return PH_ADD_COMPCODE(PH_ERR_EXT_RF_ERROR, PH_COMP_HAL);
            }
        }
    }

    /*Check the Transceive State before loading the Transceive command  if > WAITFORTRANSMIT (1) */
    if((pDataParams->wTargetMode == PH_OFF))
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_ReadRegister(pDataParams,RF_STATUS, &dwRegValue));
        dwTrasceiveState = (dwRegValue& RF_STATUS_TRANSCEIVE_STATE_MASK) >> RF_STATUS_TRANSCEIVE_STATE_POS;
        if((dwTrasceiveState > PHHAL_HW_PN5180_TRANCEIVE_STATE_WAITTRANSMIT) || (dwTrasceiveState == PHHAL_HW_PN5180_TRANCEIVE_STATE_IDLE))
        {
            /*load idle command*/
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_int_IdleCommand(pDataParams));
            /*Execute the Tranceive Command*/
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_int_LoadCommand(pDataParams, PHHAL_HW_PN5180_SYSTEM_TRANSEIVE_CMD));

        }
    }

    dwIrqWaitFor =  IRQ_STATUS_TX_IRQ_MASK | IRQ_STATUS_GENERAL_ERROR_IRQ_MASK | IRQ_STATUS_TIMER1_IRQ_MASK ;

    if(pDataParams->wTargetMode != PH_OFF)
    {
        dwIrqWaitFor |= IRQ_STATUS_RFOFF_DET_IRQ_MASK;
    }
    else
    {
        /*QAC*/
    }

    /* Wait for the IRQs */
    if(pDataParams->bRxMultiple == PH_ON)
    {
        dwIrqWaitFor &= ~IRQ_STATUS_RX_IRQ_MASK;
    }

    /* Clear all the interrupts */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_WriteRegister(pDataParams, IRQ_SET_CLEAR, PHHAL_HW_PN5180_IRQ_SET_CLEAR_ALL_MASK));

    (void)phOsal_Event_Consume(E_PH_OSAL_EVT_RF, E_PH_OSAL_EVT_SRC_HAL);

    /* Enable IRQ sources */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_ENABLE, dwIrqWaitFor));

    /* Perform the GetTxBuffer for Getting the Updated Buffer Len*/
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_GetTxBuffer(pDataParams, PH_ON, &pTmpBuffer, &wTmpBufferLen, &wTmpBufferSize));

    /* During Active Communication mode if internal RF Field is not ON before transmit then Switch ON the RF Field. */

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_ReadRegister(pDataParams, RF_STATUS, &dwValue));
    if ((!(dwValue & RF_STATUS_TX_RF_STATUS_MASK)) && (pDataParams->bActiveMode))
    {
        /* field is turned ON */
        phhalHw_Pn5180_FieldOff(pDataParams);
        phhalHw_Pn5180_FieldOn(pDataParams);
    }

    /* No Response expected*/
    bNumExpBytes = 0;

    if ( !(PH_TRANSMIT_PREV_FRAME & wOption) )
    {
        *(pTmpBuffer+1) = (uint8_t)pDataParams->wCfgShadow[PHHAL_HW_CONFIG_TXLASTBITS];
    }

    /* Send it to the chip */
    PH_CHECK_SUCCESS_FCT(statusTmp,phbalReg_Exchange(
        pDataParams->pBalDataParams,
        PH_EXCHANGE_DEFAULT,
        pTmpBuffer,
        wTmpBufferLen,
        bNumExpBytes,
        pReceivedData,
        &wDataLenTmp));
    /* Reset TxLastBits */
    if (pDataParams->wCfgShadow[PHHAL_HW_CONFIG_TXLASTBITS] != 0)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_SetConfig(pDataParams, PHHAL_HW_CONFIG_TXLASTBITS, 0x00));
    }

    pDataParams->wTxBufStartPos = 0;
    /* Reset buffered bytes */
    pDataParams->wTxBufLen = 0;

    /* Wait for TX_IRQ only.fail safe. */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_WaitIrq( pDataParams,
                       (PHHAL_HW_DISABLE_IRQ_CLEAR_MASK | PH_ON), PH_OFF, dwIrqWaitFor, &dwValue));

    if (dwValue & IRQ_STATUS_GENERAL_ERROR_IRQ_MASK)
    {
        status = PH_ERR_INTERNAL_ERROR;
    }
    /* Handle External RF error */
    /* Passive mode */
    else if(dwValue & IRQ_STATUS_RFOFF_DET_IRQ_MASK)
    {
        /* In case External RF OFF also occurred then exit with RF Error. */
        if ((pDataParams->bActiveMode == PH_OFF) && (pDataParams->wTargetMode))
        {
            /* Terminate with RF_ERROR*/
            status = PH_ERR_EXT_RF_ERROR;
        }
    }
    else if(dwValue & IRQ_STATUS_TX_IRQ_MASK)
    {
        status = PH_ERR_SUCCESS;
        pDataParams->bTransmited = PH_ON;
    }
    else
    {
        /*QAC*/
    }

    if( status != PH_ERR_SUCCESS)
    {
        /*load idle command*/
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_int_IdleCommand(pDataParams));
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask( pDataParams, TIMER1_CONFIG, (uint32_t)(~TIMER1_CONFIG_T1_ENABLE_MASK)));
    }

    return PH_ADD_COMPCODE(status, PH_COMP_HAL);
}


phStatus_t phhalHw_Pn5180_Receive(
    phhalHw_Pn5180_DataParams_t * pDataParams,     /**< [In]  Pointer to this layer's parameter structure. */
    uint16_t wOption,                              /**< [In]  Option parameter. */
    uint8_t ** ppRxBuffer,                         /**< [Out] Pointer to received data. */
    uint16_t * pRxLength                           /**< [Out] Number of received data bytes. */
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;
    uint32_t    PH_MEMLOC_REM dwIrqWaitFor = 0;
    uint32_t    PH_MEMLOC_REM dwIrqClear = 0;
    uint32_t    PH_MEMLOC_REM dwRegister;
    uint32_t    PH_MEMLOC_REM dwTranceiveState;

    if(wOption)
    {
        /*Coverity*/;
    }

    dwIrqWaitFor =  IRQ_STATUS_GENERAL_ERROR_IRQ_MASK | IRQ_STATUS_RX_IRQ_MASK | IRQ_STATUS_TIMER1_IRQ_MASK;

    if (pDataParams->bActiveMode == PH_ON)
    {
        dwIrqWaitFor |= IRQ_STATUS_RF_ACTIVE_ERROR_IRQ_MASK | IRQ_STATUS_RFOFF_DET_IRQ_MASK;
    }
    else if(pDataParams->wTargetMode != PH_OFF)
    {
        dwIrqWaitFor |= IRQ_STATUS_RFOFF_DET_IRQ_MASK;
    }
    else
    {
        /*QAC*/
    }

    /* Wait for the IRQs */
    if(pDataParams->bRxMultiple == PH_ON)
    {
        dwIrqWaitFor &= ~IRQ_STATUS_RX_IRQ_MASK;
    }
    else
    {
        /*QAC*/
    }

    do
    {
		status = PH_ERR_SUCCESS;

        if(pDataParams->bTransmited == PH_OFF)
        {
            /* set the receive length */
            if (pRxLength != NULL)
            {
                *pRxLength = 0;
            }

            /* Load Tranceive Command with Initiator = 0*/
            /* Check for Transceive state before loading the Transceiver command not equal to PHHAL_HW_PN5180_TRANCEIVE_STATE_WAITRECEIVE or PHHAL_HW_PN5180_TRANCEIVE_STATE_WAITFORDATA */

            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_ReadRegister(pDataParams,RF_STATUS, &dwRegister));
            dwTranceiveState = ((dwRegister & RF_STATUS_TRANSCEIVE_STATE_MASK)>>RF_STATUS_TRANSCEIVE_STATE_POS);

            if((dwTranceiveState == PHHAL_HW_PN5180_TRANCEIVE_STATE_IDLE) \
                || (dwTranceiveState != PHHAL_HW_PN5180_TRANCEIVE_STATE_WAITFORDATA))
            {
                /*Load Idle Command*/
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_int_IdleCommand(pDataParams));

                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, TRANSCEIVER_CONFIG, ~TRANSCEIVER_CONFIG_INITIATOR_MASK));

                /* Load the tranceive command */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_int_LoadCommand(pDataParams, PHHAL_HW_PN5180_SYSTEM_TRANSEIVE_CMD));

            }

            /* clear all IRQs */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_SET_CLEAR, PHHAL_HW_PN5180_IRQ_SET_CLEAR_ALL_MASK));

            (void)phOsal_Event_Consume(E_PH_OSAL_EVT_RF, E_PH_OSAL_EVT_SRC_HAL);
            if((pDataParams->bActiveMode == PH_ON) && (pDataParams->wTargetMode != PH_OFF))
            {
                /* Enable RF-ON Interrupt*/
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_ENABLE, IRQ_ENABLE_RFON_DET_IRQ_SET_ENABLE_MASK));

                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, SYSTEM_CONFIG,(uint32_t)~SYSTEM_CONFIG_ACTIVE_MODE_TX_RF_ENABLE_MASK));

                status = phOsal_Event_WaitAny(E_PH_OSAL_EVT_RF, 400 , NULL);

                if( ( status & PH_ERR_MASK ) == PH_ERR_IO_TIMEOUT)
                {
                    status = PH_ADD_COMPCODE(PH_ERR_EXT_RF_ERROR, PH_COMP_HAL);
                }
                else
                {
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterOrMask(pDataParams, SYSTEM_CONFIG,SYSTEM_CONFIG_ACTIVE_MODE_TX_RF_ENABLE_MASK));
                }

                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, IRQ_ENABLE, ~IRQ_ENABLE_RFON_DET_IRQ_SET_ENABLE_MASK));
                (void)phOsal_Event_Consume(E_PH_OSAL_EVT_RF, E_PH_OSAL_EVT_SRC_HAL);
            }
        }
        else
        {
            pDataParams->bTransmited = PH_OFF;

            dwIrqClear = IRQ_STATUS_TX_IRQ_MASK;

            /* Read the IRQ register and check if the interrupt has occurred */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_ReadRegister(pDataParams, IRQ_STATUS, &dwRegister));

            /* If the Timeout has already occurred no need to wait for receive */
            if(dwRegister & IRQ_STATUS_TIMER1_IRQ_MASK)
            {
                /* Clear the already received Timer IRQ */
                dwIrqClear |= IRQ_STATUS_TIMER1_IRQ_MASK;
                status = PH_ADD_COMPCODE(PH_ERR_IO_TIMEOUT, PH_COMP_HAL);
            }

            /* clear TX IRQ/Timer IRQ if Any */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_SET_CLEAR, dwIrqClear ));
        }

        /* Check to ensure external RF present in case of Passive target mode as the IRQ's are cleared above. */
        if(( PH_ERR_SUCCESS == status) && (pDataParams->bActiveMode == PH_OFF) && (pDataParams->wTargetMode != PH_OFF))
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_ReadRegister(pDataParams, RF_STATUS, &dwRegister));

            if((dwRegister & RF_STATUS_RF_DET_STATUS_MASK) == 0)
            {
                /* Terminate with RF_ERROR*/
                status = PH_ADD_COMPCODE(PH_ERR_EXT_RF_ERROR, PH_COMP_HAL);
            }
        }

        if( PH_ERR_SUCCESS == status)
        {
            /* Enable IRQ sources */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_ENABLE, dwIrqWaitFor));

            /* Wait for the Data to be received */
            status  = phhalHw_Pn5180_Receive_Int(pDataParams,dwIrqWaitFor,ppRxBuffer,pRxLength,PH_ON);
        }
    }while((status & PH_ERR_MASK)  == PHHAL_HW_PN5180_ACTIVE_ERR_CAUSE4);

    if( PH_ERR_SUCCESS  != status )
    {
        /*load idle command*/
        statusTmp = phhalHw_Pn5180_int_IdleCommand(pDataParams);
    }

    statusTmp = phhalHw_Pn5180_Instr_WriteRegisterAndMask( pDataParams, TIMER1_CONFIG, (uint32_t)(~TIMER1_CONFIG_T1_ENABLE_MASK) );

    return status;
}

phStatus_t phhalHw_Pn5180_Autocoll(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint16_t wMode,
    uint8_t ** ppRxBuffer,
    uint16_t * pRxLength,
    uint16_t * pProtParams
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_INTERNAL_ERROR;
    uint32_t    PH_MEMLOC_REM dwIrqWaitFor;
    uint32_t    PH_MEMLOC_REM dwRegValue = 0;
    uint32_t    PH_MEMLOC_REM dwRfStatus;
    uint8_t     PH_MEMLOC_REM bRfTechnology = 0;
    uint8_t     PH_MEMLOC_REM offsetPos = 0;
    uint8_t     PH_MEMLOC_REM offsetLen = 0;
    uint8_t     PH_MEMLOC_REM bFraming = 0;
    uint8_t     PH_MEMLOC_REM bSpeed = 0;
    uint8_t     PH_MEMLOC_REM pRegisters[4];
    uint8_t*    PH_MEMLOC_REM pTmpBuffer;

    /*Fixed for Autonomous mode not used, i.e. autocoll terminates when external RF field is not present*/
    uint8_t PH_MEMLOC_REM   bAutocollMode = 0;
    uint32_t PH_MEMLOC_REM  dwIrqReg = 0;

    /*bTransmited is a flag which is used to identify if a transmit is followed by a receive.
    If any other api follows a transmit, then it should be reset */
    pDataParams->bTransmited = PH_OFF;

    if ( (NULL == pDataParams) \
        || (NULL == ppRxBuffer)\
        || (NULL == pRxLength) \
        ||  (NULL == pProtParams) \
        )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    if(pDataParams->bCardMode == PH_ON)
    {
        bRfTechnology = PHHAL_HW_PN5180_AUTOCOLL_RF_TECHNOLOGY_MASK_NFC_A;
    }
    else
    {
        /*RFTech A*/
        if((wMode & A_MODE) == A_MODE)
        {
            bRfTechnology |= PHHAL_HW_PN5180_AUTOCOLL_RF_TECHNOLOGY_MASK_NFC_A;
        }
        /*RFTech F*/
        if((wMode & F_MODE) == F_MODE)
        {
            bRfTechnology |= PHHAL_HW_PN5180_AUTOCOLL_RF_TECHNOLOGY_MASK_NFC_F;
        }
        /*RFTech Active A*/
        if((wMode & ACTIVE_A_MODE) == ACTIVE_A_MODE)
        {
            bRfTechnology |= PHHAL_HW_PN5180_AUTOCOLL_RF_TECHNOLOGY_MASK_ISO18092_ACTIVE;
        }
        /*RFTech Active F*/
        if((wMode & ACTIVE_F_MODE) == ACTIVE_F_MODE)
        {
            bRfTechnology |= PHHAL_HW_PN5180_AUTOCOLL_RF_TECHNOLOGY_MASK_NFC_F_ACTIVE;
        }
        /*RFTech B and Active B*/
        if(((wMode & B_MODE) == B_MODE)||((wMode & ACTIVE_B_MODE) == ACTIVE_B_MODE))
        {
            /* Do nothing, ignore -- NFCP576 -- as the NFC forum DTA application expects success for Listen B */
            return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
        }

    }

    /* set the receive length */
    *pRxLength = 0;

    pDataParams->wRxBufLen = 0;

    /* Reset the Flag used to indicate if Passive Target is activated by ISO/IEC 18092 Frame */
    pDataParams->wTargetMode = PH_OFF;
    pDataParams->bActiveMode = PH_OFF;
    pDataParams->bNfcipMode  = PH_OFF;

    /* Terminate a probably running command */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_int_IdleCommand(pDataParams));

    phhalHw_Pn5180_Instr_RfOff(pDataParams);

    /*Set wait IRQ */
    dwIrqWaitFor =  IRQ_STATUS_RX_IRQ_MASK | IRQ_STATUS_GENERAL_ERROR_IRQ_MASK | IRQ_STATUS_RFOFF_DET_IRQ_MASK;

    /* Clear all Interrupts for e.g Tx interrupt during receive */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_SET_CLEAR, PHHAL_HW_PN5180_IRQ_SET_CLEAR_ALL_MASK));

    (void)phOsal_Event_Consume(E_PH_OSAL_EVT_RF, E_PH_OSAL_EVT_SRC_HAL);

    /* Enable IRQ sources */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_ENABLE, dwIrqWaitFor));

    /*Go to autocoll mode*/
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_SwitchModeAutocoll(pDataParams, bRfTechnology, bAutocollMode ) );

    /* Wait for Interrupt to occur */
    statusTmp =  phhalHw_Pn5180_WaitIrq( pDataParams, PH_ON, PH_OFF, dwIrqWaitFor, &dwIrqReg);

    if((statusTmp & PH_ERR_MASK) == PH_ERR_ABORTED)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_SwitchModeNormal(pDataParams) );
        return PH_ADD_COMPCODE(PH_ERR_ABORTED, PH_COMP_HAL);
    }

    if(dwIrqReg & IRQ_STATUS_GENERAL_ERROR_IRQ_MASK)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_SwitchModeNormal(pDataParams) );
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
    }
    else if (dwIrqReg & IRQ_STATUS_RX_IRQ_MASK)
    {

        pRegisters[0] = SIGPRO_CM_CONFIG;
        pRegisters[1] = SIGPRO_CONFIG;
        pRegisters[2] = RX_STATUS;
        pRegisters[3] = RF_STATUS;

        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_ReadRegisterMultiple(pDataParams, pRegisters, 4, &pTmpBuffer));

        dwRegValue = (uint32_t)pTmpBuffer[0];
        dwRegValue |= (uint32_t)pTmpBuffer[1]<<8;
        dwRegValue |= (uint32_t)pTmpBuffer[2]<<16;
        dwRegValue |= (uint32_t)pTmpBuffer[3]<<24;

        bFraming = (uint8_t)((dwRegValue & SIGPRO_CM_CONFIG_RX_FRAMING_MASK) >> SIGPRO_CM_CONFIG_RX_FRAMING_POS);

        dwRegValue = (uint32_t)pTmpBuffer[4];
        dwRegValue |= (uint32_t)pTmpBuffer[5]<<8;
        dwRegValue |= (uint32_t)pTmpBuffer[6]<<16;
        dwRegValue |= (uint32_t)pTmpBuffer[7]<<24;

        bSpeed = (uint8_t)(dwRegValue & SIGPRO_CONFIG_BAUDRATE_MASK);
        bSpeed -= PHHAL_HW_PN5180_SIGPRO_GENERAL_MAPPING;

        /* Read RX_STATUS_REG to know the status of reception */
        dwRegValue = (uint32_t)pTmpBuffer[8];
        dwRegValue |= (uint32_t)pTmpBuffer[9]<<8;
        dwRegValue |= (uint32_t)pTmpBuffer[10]<<16;
        dwRegValue |= (uint32_t)pTmpBuffer[11]<<24;

        /* Read RF_STATUS_REG to know the status of reception */
        dwRfStatus = (uint32_t)pTmpBuffer[12];
        dwRfStatus |= (uint32_t)pTmpBuffer[13]<<8;
        dwRfStatus |= (uint32_t)pTmpBuffer[14]<<16;
        dwRfStatus |= (uint32_t)pTmpBuffer[15]<<24;


        if (dwRegValue & RX_STATUS_RX_DATA_INTEGRITY_ERROR_MASK)
        {
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_SwitchModeNormal(pDataParams));
            return PH_ADD_COMPCODE(PH_ERR_INTEGRITY_ERROR, PH_COMP_HAL);
        }

        PH_CHECK_SUCCESS_FCT(statusTmp,  phhalHw_Pn5180_ReadData(pDataParams, ppRxBuffer, pRxLength));

        if(dwIrqReg & IRQ_STATUS_CARD_ACTIVATED_IRQ_MASK)
        {
            /* In case External RF OFF also occurred then exit with RF Error. */
            if (dwIrqReg & IRQ_STATUS_RFOFF_DET_IRQ_MASK)
            {
                /* Terminate Autocoll command before existing. */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_SwitchModeNormal(pDataParams) );
                return PH_ADD_COMPCODE(PH_ERR_EXT_RF_ERROR, PH_COMP_HAL);
            }
            /* Disable active mode */
            pDataParams->bActiveMode = PH_OFF;
            if((bSpeed == PHHAL_HW_RF_DATARATE_106) && (**ppRxBuffer == PHHAL_HW_PN5180_I18092MT_START_BYTE))
            {
                /* Offset for StartByte as it included if Passive Communication in ISO/IEC 18092 */
                offsetPos = 1;
                offsetLen = 1;
            }
        }
        else if ((dwRfStatus & RF_STATUS_RF_DET_STATUS_MASK) == 0)
        {
            /* Enable active mode */
            pDataParams->bActiveMode = PH_ON;
            if(bSpeed == PHHAL_HW_RF_DATARATE_106)
            {
                /* Offset for StartByte and CRC as it is included if Active Communication in ISO/IEC 18092 */
                offsetPos = 1;
                offsetLen = 3;
            }
            bFraming = PHHAL_HW_PN5180_BIT_ISO18092;
        }
        else
        {
            /* Terminate Autocoll command before existing. */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_SwitchModeNormal(pDataParams) );
            return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_HAL);
        }
        if((pDataParams->bActiveMode == PH_ON) && ((wMode & ACTIVE_A_MODE) != ACTIVE_A_MODE) && (bSpeed == PHHAL_HW_RF_DATARATE_106))
        {
            /* Terminate Autocoll command before existing. */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_SwitchModeNormal(pDataParams) );
            return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_HAL);
        }

        if(bFraming == PHHAL_HW_PN5180_BIT_MIFARE)
        {
            *pProtParams = PHHAL_HW_RF_TYPE_A_FRAMING | bSpeed;
        }
        else if(bFraming == PHHAL_HW_PN5180_BIT_ISO18092)
        {
            *pProtParams = PHHAL_HW_RF_TYPE_ACTIVE_FRAMING | bSpeed;
        }
        else if(bFraming == PHHAL_HW_PN5180_BIT_FELICA)
        {
            *pProtParams = PHHAL_HW_RF_TYPE_F_FRAMING | bSpeed;
        }
        else
        {
            /* Invalid Framing as PN5180 will not emulate other than Type-A, Type-F and Active Frame */
            status = PH_ERR_FRAMING_ERROR;
        }

        /* Store received data length in dataparams */
        pDataParams->wRxBufLen = *pRxLength;

        /* Return RxBuffer pointer */
        /* Offset "F0" incase of Active communication */
        *ppRxBuffer = pDataParams->pRxBuffer + offsetPos;

        /* Return RxBuffer length */
        /* Offset "F0" + CRC incase of Active communication */
        *pRxLength = pDataParams->wRxBufLen - offsetLen;
        pDataParams->wTargetMode = *pProtParams;

        status = PH_ERR_SUCCESS;
    }
    else if(dwIrqReg & IRQ_STATUS_RFOFF_DET_IRQ_MASK)
    {
        status = PH_ERR_EXT_RF_ERROR;
    }
    else
    {
        /* QAC */
    }

    if(status != PH_ERR_SUCCESS)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_SwitchModeNormal(pDataParams) );
    }

    return PH_ADD_COMPCODE(status, PH_COMP_HAL);
}

phStatus_t phhalHw_Pn5180_Lpcd(
    phhalHw_Pn5180_DataParams_t * pDataParams)
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint32_t    PH_MEMLOC_REM dwRegister;
    uint32_t    PH_MEMLOC_REM dwIrqWaitFor= 0;
    uint32_t    PH_MEMLOC_REM dwIrqReg;
    phStatus_t  PH_MEMLOC_REM status;
    uint8_t     PH_MEMLOC_REM bEPromData[5];
    uint32_t    PH_MEMLOC_REM dwAgcValVariation = 0;
    uint32_t    PH_MEMLOC_REM dwAgcRefVal = 0;
    uint32_t    PH_MEMLOC_REM dwLpcdThreshold = 0;
    uint8_t     PH_MEMLOC_REM bOcProtControl[1];


    switch (pDataParams->bLpcdMode)
    {
    case PHHAL_HW_PN5180_LPCD_MODE_DEFAULT:

        /*Get the current AGC measurement*/
        PH_CHECK_SUCCESS_FCT(statusTmp,  phhalHw_Pn5180_Instr_ReadRegister( pDataParams, AGC_REF_CONFIG, &dwRegister));

        PH_CHECK_SUCCESS_FCT(statusTmp,  phhalHw_Pn5180_Instr_ReadE2Prom( pDataParams, PHHAL_HW_PN5180_LPCD_REFERENCE_VALUE_ADDR, bEPromData,  5));

        PH_CHECK_SUCCESS_FCT(statusTmp,  phhalHw_Pn5180_Instr_ReadE2Prom( pDataParams, PHHAL_HW_PN5180_OCPROT_CONTROL_ADDR, bOcProtControl,  1));

        /*Use EEPROM Value for reference value*/
        if((bEPromData[4] & LPCD_REFVAL_CONTROL_MASK) == 0 )
        {
            /*Get the AGC Ref Value from E2PROM*/
            dwAgcRefVal = (uint8_t)bEPromData[0];
            dwAgcRefVal |= (uint16_t)((uint16_t)bEPromData[1]<<8);
        }

        else
        {
            /*used the stored reference value */
            dwAgcRefVal = pDataParams->wLpcdReference;
        }


        /*Get the AGV Variation threshold*/
        dwLpcdThreshold =(uint8_t)bEPromData[3];

        //Store the read AGC_REF_CONFIG reg value as reference for next comparison
        pDataParams->wLpcdReference = (uint16_t)(dwRegister &(AGC_REF_CONFIG_AGC_GEAR_MASK | AGC_REF_CONFIG_AGC_VALUE_MASK));

        /*If TXOCP is enabled, Then we need to compare gear*/
        if((bOcProtControl[0] & 0x01) && (dwRegister & AGC_REF_CONFIG_AGC_GEAR_MASK)!= (dwAgcRefVal & AGC_REF_CONFIG_AGC_GEAR_MASK))
        {
            status = PH_ERR_SUCCESS;
        }
        else
        {
            /*Calculate the modulus AGC*/
            dwRegister &= AGC_REF_CONFIG_AGC_VALUE_MASK;
            dwAgcRefVal &= AGC_REF_CONFIG_AGC_VALUE_MASK;

            /*calucalate the modulus*/
            dwAgcValVariation = ((dwRegister > dwAgcRefVal) ?
                (dwRegister - dwAgcRefVal) : (dwAgcRefVal - dwRegister));

            /*Compare it with threshold*/
            if (dwAgcValVariation > dwLpcdThreshold)
            {
                status = PH_ERR_SUCCESS;
            }
            else
            {
                status = PH_ERR_IO_TIMEOUT;
            }
        }

        break;

    case PHHAL_HW_PN5180_LPCD_MODE_POWERDOWN:
        dwIrqWaitFor = (IRQ_STATUS_LPCD_IRQ_MASK | IRQ_STATUS_GENERAL_ERROR_IRQ_MASK);

        /* Clear above Interrupts  */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_SET_CLEAR, dwIrqWaitFor));
        (void)phOsal_Event_Consume(E_PH_OSAL_EVT_RF, E_PH_OSAL_EVT_SRC_HAL);

        /* Enable IRQ sources */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_ENABLE, dwIrqWaitFor));

        PH_CHECK_SUCCESS_FCT(statusTmp,  phhalHw_Pn5180_Instr_SwitchModeLpcd(
            pDataParams,
            (uint16_t)(pDataParams->wWakeupCounterInMs))
            );

        /* Now wait for the IRQ */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_WaitIrq(
            pDataParams,
            PH_ON,
            PH_OFF,
            dwIrqWaitFor,
            &dwIrqReg));

        if(dwIrqReg & IRQ_STATUS_LPCD_IRQ_MASK)
        {
            status = PH_ERR_SUCCESS;
        }
        else if (dwIrqReg & IRQ_STATUS_GENERAL_ERROR_IRQ_MASK)
        {
            status = PH_ERR_INVALID_PARAMETER;
        }
        else
        {
            status = PH_ERR_IO_TIMEOUT;
        }
        break;
    default:
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        break;
    }

    return PH_ADD_COMPCODE(status, PH_COMP_HAL);
}


phStatus_t phhalHw_Pn5180_SetListenParameters(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t * pSensRes,
    uint8_t * pNfcId1,
    uint8_t SelRes,
    uint8_t * pPollingResp,
    uint8_t bNfcId3
    )
{
    uint8_t PH_MEMLOC_BUF aCmd[PHHAL_HW_PN5180_SET_LISTEN_LEN];
    if(bNfcId3)
    {
        /*Coverity*/;
    }

    memcpy(&aCmd[0], pSensRes, PHHAL_HW_PN5180_SET_LISTEN_SENRES_LEN);      /* PRQA S 3200 */
    memcpy(&aCmd[2], pNfcId1, PHHAL_HW_PN5180_SET_LISTEN_NFCID1_LEN);       /* PRQA S 3200 */
    memcpy(&aCmd[5], &SelRes, PHHAL_HW_PN5180_SET_LISTEN_SELRES_LEN);       /* PRQA S 3200 */
    memcpy(&aCmd[6], pPollingResp, PHHAL_HW_PN5180_SET_LISTEN_POLLRES_LEN); /* PRQA S 3200 */

    return phhalHw_Pn5180_Instr_WriteE2Prom(pDataParams, PHHAL_HW_PN5180_SET_LISTEN_EEPROM_ADDR, aCmd, PHHAL_HW_PN5180_SET_LISTEN_LEN);
}


phStatus_t phhalHw_Pn5180_AsyncAbort(
    phhalHw_Pn5180_DataParams_t * pDataParams
    )
{

    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;

    /* satisfy compiler */
    PH_UNUSED_VARIABLE(pDataParams);

    /* Post Abort Event. */
    PH_CHECK_SUCCESS_FCT(status, phOsal_Event_Post(E_PH_OSAL_EVT_ABORT, E_PH_OSAL_EVT_SRC_APP, E_PH_OSAL_EVT_DEST_HAL));

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}


phStatus_t phhalHw_Pn5180_I18000p3m3Inventory(phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t * pSelCmd,                  /**< [In] ISO 18000p3m3 Select command frame. */
    uint8_t bSelCmdLen,                 /**< [In] Select command length in bytes- 1 to 39 bytes. */
    uint8_t bNumValidBitsinLastByte,    /**< [In] Number of valid bits in last byte of pSelCmd. */
    uint8_t * pBeginRndCmd,             /**< [In] ISO 18000p3m3 BeginRound command frame. This is 17bits i.e., 3 bytes are expected. CRC5 should not be provided. */
    uint8_t bTSprocessing,              /**< [In] TimeSlot processing behavior. */
    uint8_t ** ppRxBuffer,              /**< [In] Pointer to HAL Rx Buffer containing response from single/multiple tags. */
    uint16_t * wRxBufferLen             /**< [In] Length of response in hal Rx Buffer. */
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;
    uint16_t  PH_MEMLOC_REM ResultSize;
    uint32_t  PH_MEMLOC_REM dwIrqWaitFor;
    uint8_t  PH_MEMLOC_REM bTarget = 0;
    uint8_t  PH_MEMLOC_REM bSession = 0;
    uint32_t PH_MEMLOC_REM dwIrqReg;

    if(bSelCmdLen > 0)
    {
        bTarget = pSelCmd[0];
        bTarget = bTarget>>1;
        bTarget = bTarget&0x07;

        bSession = pBeginRndCmd[1]>>4;
        bSession &= 0x03;

        switch(bSession)
        {
        case 0:
        case 2:
            {
                if(bSession != bTarget)
                {
                    return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
                }
                break;
            }
        default:
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

    }

    dwIrqWaitFor =  (IRQ_STATUS_RX_IRQ_MASK| IRQ_STATUS_GENERAL_ERROR_IRQ_MASK);

    /* Clear Interrupts  */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_SET_CLEAR, dwIrqWaitFor));

    (void)phOsal_Event_Consume(E_PH_OSAL_EVT_RF, E_PH_OSAL_EVT_SRC_HAL);

    /* Enable IRQ sources */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterOrMask(pDataParams, IRQ_ENABLE, dwIrqWaitFor));

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_EpcGen2Inventory(
        pDataParams,
        pSelCmd,
        bSelCmdLen,
        bNumValidBitsinLastByte,
        pBeginRndCmd,
        bTSprocessing
        ));

    /* Now wait for the IRQ */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_WaitIrq(
        pDataParams,
        PH_ON,
        PH_OFF,
        dwIrqWaitFor,
        &dwIrqReg));

    /* Check if an error has occured */
    if (dwIrqReg & IRQ_STATUS_RX_IRQ_MASK)
    {
        /*Retrieve the result size and result*/
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_EpcGen2RetrieveResultSize( pDataParams, &ResultSize));
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_EpcGen2RetrieveResult( pDataParams, ppRxBuffer, ResultSize));

        *wRxBufferLen = ResultSize;
        if(ResultSize > 0)
        {
        	/* Response length is 3bytes by default. With 1st byte indicating Response status.
        	 * If 1st Byte has value '2' it indicates there is no response in any Timeslot,
        	 * all other values indicate response availability.(As specified in PN5180 Instruction Layer document) */
        	if( (**ppRxBuffer) == 2 )
            {
        		status = PH_ERR_IO_TIMEOUT;
            }
        	else
        	{
        		status = PH_ERR_SUCCESS;
        	}
        }
        else
        {
			status = PH_ERR_INTERNAL_ERROR;

        }
    }
    else
    {
        status = PH_ERR_IO_TIMEOUT;
    }

    return PH_ADD_COMPCODE(status, PH_COMP_HAL);
}

phStatus_t phhalHw_Pn5180_18000p3m3ResumeInventory(
    phhalHw_Pn5180_DataParams_t * pDataParams,  /**< [In] Pointer to this layer's parameter structure. */
    uint8_t ** ppRxBuffer,                      /**< [In] Pointer to HAL Rx Buffer containing response from single/multiple tags. */
    uint16_t * wRxBufferLen                     /**< [In] Length of response in hal Rx Buffer. */
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;
    uint16_t  PH_MEMLOC_REM ResultSize;
    uint32_t  PH_MEMLOC_REM dwIrqWaitFor;
    uint32_t PH_MEMLOC_REM dwIrqReg;

    dwIrqWaitFor =  (IRQ_STATUS_RX_IRQ_MASK| IRQ_STATUS_GENERAL_ERROR_IRQ_MASK);

    /* Clear Interrupts  */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_SET_CLEAR, dwIrqWaitFor));

    (void)phOsal_Event_Consume(E_PH_OSAL_EVT_RF, E_PH_OSAL_EVT_SRC_HAL);

    /* Enable IRQ sources */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterOrMask(pDataParams, IRQ_ENABLE, dwIrqWaitFor));


    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_EpcGen2ResumeInventory(pDataParams));

    /* Now wait for the IRQ */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_WaitIrq(
        pDataParams,
        PH_ON,
        PH_OFF,
        dwIrqWaitFor,
        &dwIrqReg));

    /* Check if an error has occurred */
    if (dwIrqReg & IRQ_STATUS_RX_IRQ_MASK)
    {
        /*Retrieve the result size and result*/
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_EpcGen2RetrieveResultSize( pDataParams, &ResultSize));

        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_EpcGen2RetrieveResult( pDataParams, ppRxBuffer, ResultSize));

        *wRxBufferLen = ResultSize;

        if(ResultSize>0)
        {
            status = PH_ERR_SUCCESS;
        }
        else
        {
            status = PH_ERR_INTERNAL_ERROR;
        }
    }
    else
    {
        status = PH_ERR_IO_TIMEOUT;
    }

    return PH_ADD_COMPCODE(status, PH_COMP_HAL);
}


phStatus_t phhalHw_Pn5180_DeInit(
    phhalHw_Pn5180_DataParams_t * pDataParams
    )
{
    phStatus_t PH_MEMLOC_REM statusTmp;

    /* Disable IRQ interrupt sources */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, IRQ_ENABLE, (uint32_t)~0x0FFFF));

    /* Clear IRQ interrupt sources */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_SET_CLEAR, PHHAL_HW_PN5180_IRQ_SET_CLEAR_ALL_MASK));

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
    pDataParams->wTxBufStartPos         = 0;
    pDataParams->bCardType              = PHHAL_HW_CARDTYPE_UNKNOWN;
    pDataParams->wTimingMode            = PHHAL_HW_TIMING_MODE_OFF;
    pDataParams->bTimeoutUnit           = PHHAL_HW_TIME_MICROSECONDS;
    pDataParams->dwTimingUs             = 0;
    pDataParams->wFieldOffTime          = PHHAL_HW_FIELD_OFF_DEFAULT;
    pDataParams->wFieldRecoveryTime     = PHHAL_HW_FIELD_RECOVERY_DEFAULT;
    pDataParams->bSymbolStart           = PH_OFF;
    pDataParams->bSymbolEnd             = PH_OFF;
    pDataParams->wAdditionalInfo        = 0;
    pDataParams->bRfResetAfterTo        = PH_OFF;
    pDataParams->bRxMultiple            = PH_OFF;
    pDataParams->bActiveMode            = PH_OFF;
    pDataParams->bRfca                  = PH_ON;
    pDataParams->wTargetMode            = PH_OFF;
    pDataParams->bOpeMode               = RD_LIB_MODE_NFC;
    pDataParams->bNfcipMode             = PH_OFF;
    pDataParams->bSkipSend              = PH_OFF;
    pDataParams->bTransmited            = PH_OFF;
    pDataParams->bJewelActivated        = PH_OFF;
    pDataParams->bLpcdMode              = PHHAL_HW_PN5180_LPCD_MODE_DEFAULT;
    pDataParams->wWakeupCounterInMs     = 0;
    pDataParams->dwTxWaitMs             = 0;
    pDataParams->bMfcCryptoEnabled      = PH_OFF;
    pDataParams->bCardMode              = PH_OFF;

    return PH_ERR_SUCCESS;
}


#endif  /* NXPBUILD__PHHAL_HW_PN5180 */

