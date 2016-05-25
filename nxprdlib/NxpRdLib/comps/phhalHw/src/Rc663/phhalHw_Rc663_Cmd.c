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
* Command functions for Rc663 specific HAL-Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 4078 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  CHu: Generated 19. March 2010
*
*/
#include <phhalHw.h>
#include <phbalReg.h>
#include <ph_RefDefs.h>

#ifdef NXPBUILD__PHHAL_HW_RC663

#include "phhalHw_Rc663.h"
#include "phhalHw_Rc663_Int.h"
#include "phhalHw_Rc663_Config.h"
#include <phhalHw_Rc663_Reg.h>
#include <phhalHw_Rc663_Cmd.h>

phStatus_t phhalHw_Rc663_Cmd_Lpcd(
                                  phhalHw_Rc663_DataParams_t * pDataParams
                                  )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM aCmd;
    uint16_t    PH_MEMLOC_REM wOption;
    uint8_t     PH_MEMLOC_REM bIrq0WaitFor;
    uint8_t     PH_MEMLOC_REM bIrq1WaitFor;
    float32_t   PH_MEMLOC_REM fReload;
    uint16_t    PH_MEMLOC_REM wReload;
    uint8_t     PH_MEMLOC_REM bRegister;
    uint8_t     PH_MEMLOC_REM bRxAnaBackup;
    uint8_t     PH_MEMLOC_REM bQMin;
    uint8_t     PH_MEMLOC_REM bQMax;
    uint8_t     PH_MEMLOC_REM bIMin;
    uint8_t     PH_MEMLOC_REM bIMax;

    /* Prepare command and option */
    aCmd = PHHAL_HW_RC663_CMD_LPCD;
    wOption = PH_EXCHANGE_DEFAULT;

    /* Prepare wait IRQs */
    bIrq0WaitFor = 0x00;
    bIrq1WaitFor = PHHAL_HW_RC663_BIT_LPCDIRQ;

    /* Parameter check */
    switch (pDataParams->bMode & (uint8_t)~(uint8_t)PHHAL_HW_RC663_CMD_LPCD_MODE_OPTION_MASK)
    {
    case PHHAL_HW_RC663_CMD_LPCD_MODE_DEFAULT:

        /* start and wait for timer */
        wOption |= PHHAL_HW_RC663_RXTX_TIMER_START;
        bIrq1WaitFor |= PHHAL_HW_RC663_BIT_TIMER1IRQ;

        /* Write default. T3 reload value */
        wReload = PHHAL_HW_RC663_LPCD_T3_RELOAD_MIN;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_T3RELOADHI, (uint8_t)(wReload >> 8)));
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_T3RELOADLO, (uint8_t)(wReload)));

        /* Write min. T4 reload value */
        wReload = PHHAL_HW_RC663_LPCD_T4_RELOAD_MIN;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_T4RELOADHI, (uint8_t)(wReload >> 8)));
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_T4RELOADLO, (uint8_t)(wReload)));

        /* Configure T4 for AutoLPCD & AutoRestart */
        bRegister = PHHAL_HW_RC663_BIT_T4AUTOLPCD | PHHAL_HW_RC663_BIT_T4AUTORESTARTED;
        /* Use 64Khz LFO */
        bRegister |= PHHAL_HW_RC663_VALUE_TCLK_LFO_64_KHZ;
        /* Start T4 */
        bRegister |= PHHAL_HW_RC663_BIT_T4STARTSTOPNOW | PHHAL_HW_RC663_BIT_T4RUNNING;
        /* Set AutoTrimm bit if requested */
        if (pDataParams->bMode & PHHAL_HW_RC663_CMD_LPCD_MODE_OPTION_TRIMM_LPO)
        {
            bRegister |= PHHAL_HW_RC663_BIT_T4AUTOTRIMM;
        }

        /* Write T4Control register */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_T4CONTROL, bRegister));

        break;

    case PHHAL_HW_RC663_CMD_LPCD_MODE_POWERDOWN_GUARDED:

        /* start and wait for timer */
        wOption |= PHHAL_HW_RC663_RXTX_TIMER_START;
        bIrq1WaitFor |= PHHAL_HW_RC663_BIT_TIMER1IRQ;

    case PHHAL_HW_RC663_CMD_LPCD_MODE_POWERDOWN:

        /* Set standby bit */
        aCmd |= PHHAL_HW_RC663_BIT_STANDBY;

        /* Calculate timer 3 reload value */
        fReload = (float32_t)pDataParams->wDetectionTimeUs * PHHAL_HW_RC663_TIMER0_FREQ;
        if (fReload > (float32_t)PHHAL_HW_RC663_TIMER_RELOAD_MAX)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        /* Since timer IRQ is raised at underflow, wReload has to decremented by one.
        However, if the resulting reload value has commas, it must be 'ceiled' */
        wReload = (uint16_t)fReload;
        if (!(fReload > (float32_t)wReload))
        {
            --wReload;
        }

        /* Write T3 reload value */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_T3RELOADHI, (uint8_t)(wReload >> 8)));
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_T3RELOADLO, (uint8_t)(wReload)));

        /* Calculate timer 4 reload value. 16khz clock used. Last two bits of T4Control = 00b */
        fReload = (float32_t)(pDataParams->wPowerDownTimeMs * 16);
        if (fReload > (float32_t)PHHAL_HW_RC663_TIMER_RELOAD_MAX)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }

        /* Since timer IRQ is raised at underflow, wReload has to decremented by one.
        However, if the resulting reload value has commas, it must be 'ceiled' */
        wReload = (uint16_t)fReload;
        if (!(fReload > (float32_t)wReload))
        {
            --wReload;
        }

        /* Write T4 reload value */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_T4RELOADHI, (uint8_t)(wReload >> 8)));
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_T4RELOADLO, (uint8_t)(wReload)));

        /* Configure T4 for AutoLPCD and AutoRestart / Autowakeup */
        bRegister = PHHAL_HW_RC663_BIT_T4AUTOLPCD | PHHAL_HW_RC663_BIT_T4AUTORESTARTED | PHHAL_HW_RC663_BIT_T4AUTOWAKEUP;

        /* Using 16 KHz -  implies last two bits 00. Nothing to be OR'd here. */

        /* Start T4 */
        bRegister |= PHHAL_HW_RC663_BIT_T4STARTSTOPNOW | PHHAL_HW_RC663_BIT_T4RUNNING;
        /* Set AutoTrimm bit if requested */
        if (pDataParams->bMode & PHHAL_HW_RC663_CMD_LPCD_MODE_OPTION_TRIMM_LPO)
        {
            bRegister |= PHHAL_HW_RC663_BIT_T4AUTOTRIMM;
        }

        /* Write T4Control register */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_T4CONTROL, bRegister));

        break;

    default:
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Clear LPCD result */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_LPCD_RESULT_Q, PHHAL_HW_RC663_BIT_LPCDIRQ_CLR));

    /* Set I & Q values */
    if (!(pDataParams->bMode & PHHAL_HW_RC663_CMD_LPCD_MODE_OPTION_IGNORE_IQ))
    {
        /* Calculate I/Q min/max. values */
        bQMin = pDataParams->bQ - 1;
        bQMax = pDataParams->bQ + 1;
        bIMin = pDataParams->bI - 1;
        bIMax = pDataParams->bI + 1;

        /* Set Qmin register */
        bRegister = bQMin | (uint8_t)((bIMax & 0x30) << 2);
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_LPCD_QMIN, bRegister));

        /* Set Qmax register */
        bRegister = bQMax | (uint8_t)((bIMax & 0x0C) << 4);
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_LPCD_QMAX, bRegister));

        /* Set Imin register */
        bRegister = bIMin | (uint8_t)((bIMax & 0x03) << 6);
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_LPCD_IMIN, bRegister));
    }

    /* Set Mix2Adc bit */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_RCV, &bRegister));
    bRegister |= PHHAL_HW_RC663_BIT_RX_SHORT_MIX2ADC;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_RCV, bRegister));

    /* Backup current RxAna setting */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_RXANA, &bRxAnaBackup));
    /* Raise receiver gain to maximum */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_RXANA, 0x03));

    /* Wait until T4 is started*/
    do
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_T4CONTROL, &bRegister));
    }
    while(!(bRegister & PHHAL_HW_RC663_BIT_T4RUNNING));

    /* Perform command */
    status = phhalHw_Rc663_Command_Int(
        pDataParams,
        aCmd,
        wOption,
        bIrq0WaitFor,
        bIrq1WaitFor,
        NULL,
        0,
        0,
        NULL,
        NULL);

    /* Restore RxAna register */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_RXANA, bRxAnaBackup));

    /* Clear Mix2Adc bit */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_RCV, &bRegister));
    bRegister &= (uint8_t)~(uint8_t)PHHAL_HW_RC663_BIT_RX_SHORT_MIX2ADC;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_RCV, bRegister));

    /* Stop Timer4 if started */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_T4CONTROL, &bRegister));
    bRegister |= PHHAL_HW_RC663_BIT_T4STARTSTOPNOW;
    bRegister &= (uint8_t)~(uint8_t)PHHAL_HW_RC663_BIT_T4RUNNING;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_T4CONTROL, bRegister));

    /* Command error check (ignore timeout since interface could have been too slow) */
    if ((status & PH_ERR_MASK) != PH_ERR_IO_TIMEOUT)
    {
        PH_CHECK_SUCCESS(status);
    }

    /* Check if LPCD-IRQ is set */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_IRQ1, &bRegister));
    if (!(bRegister & PHHAL_HW_RC663_BIT_LPCDIRQ))
    {
        return PH_ADD_COMPCODE(PH_ERR_IO_TIMEOUT, PH_COMP_HAL);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc663_Cmd_Lpcd_GetConfig(
                                        phhalHw_Rc663_DataParams_t * pDataParams,
                                        uint8_t * pI,
                                        uint8_t * pQ
                                        )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bMode;
    uint16_t    PH_MEMLOC_REM wTimeoutPrev;
    uint8_t     PH_MEMLOC_REM bTimeoutPrevInMs;

    /* Set Qmin register */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_LPCD_QMIN, 0xC0));

    /* Set Qmax register */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_LPCD_QMAX, 0xFF));

    /* Set Imin register */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_LPCD_IMIN, 0xC0));

    /* Use default mode, trimm lpo and permit the function to set I and Q values */
    bMode = PHHAL_HW_RC663_CMD_LPCD_MODE_DEFAULT | PHHAL_HW_RC663_CMD_LPCD_MODE_OPTION_TRIMM_LPO | PHHAL_HW_RC663_CMD_LPCD_MODE_OPTION_IGNORE_IQ;

    /* Retrieve current timeout */
    status = phhalHw_Rc663_GetConfig(pDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, &wTimeoutPrev);
    bTimeoutPrevInMs = 0;

    /* Timeout is out of range, retrieve it in milliseconds */
    if ((status & PH_ERR_MASK) == PH_ERR_PARAMETER_OVERFLOW)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS, &wTimeoutPrev));
        bTimeoutPrevInMs = 1;
    }
    else
    {
        PH_CHECK_SUCCESS(status);
    }

    /* Set configuration timeout */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_SetConfig(pDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, 300));

    /* copy configuration LPCD values */
    status = phhalHw_Rc663_Cmd_Lpcd_SetConfig(pDataParams, bMode, 0x00, 0x00, 0x00, 0x00);

    /* Execute trimming procedure */
    status = phhalHw_Rc663_Cmd_Lpcd(pDataParams);

    /* Restore previous timeout */
    if (bTimeoutPrevInMs == 0)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_SetConfig(pDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, wTimeoutPrev));
    }
    else
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_SetConfig(pDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS, wTimeoutPrev));
    }

    /* Ignore timeout error */
    if ((status & PH_ERR_MASK) != PH_ERR_IO_TIMEOUT)
    {
        PH_CHECK_SUCCESS(status);
    }

    /* Retrieve I and Q results */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_LPCD_RESULT_I, pI));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_LPCD_RESULT_Q, pQ));

    /* Mask out irrelevant bits */
    *pI &= 0x3F;
    *pQ &= 0x3F;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc663_Cmd_LoadKey(
                                     phhalHw_Rc663_DataParams_t * pDataParams,
                                     uint8_t * pKey
                                     )
{
    uint8_t PH_MEMLOC_REM bIrq0WaitFor;
    uint8_t PH_MEMLOC_REM bIrq1WaitFor;

    /* Set wait IRQs */
    bIrq0WaitFor = 0x00;
    bIrq1WaitFor = 0x00;

    /* Perform command */
    return phhalHw_Rc663_Command_Int(
        pDataParams,
        PHHAL_HW_RC663_CMD_LOADKEY,
        PH_EXCHANGE_DEFAULT,
        bIrq0WaitFor,
        bIrq1WaitFor,
        pKey,
        PHHAL_HW_RC663_MFC_KEY_LENGTH,
        0,
        NULL,
        NULL);
}

phStatus_t phhalHw_Rc663_Cmd_WriteE2(
                                     phhalHw_Rc663_DataParams_t * pDataParams,
                                     uint16_t wAddress,
                                     uint8_t bData
                                     )
{
    uint8_t PH_MEMLOC_REM aCmd[3];
    uint8_t PH_MEMLOC_REM bIrq0WaitFor;
    uint8_t PH_MEMLOC_REM bIrq1WaitFor;

    /* Check Address range */
    if (wAddress >= PHHAL_HW_RC663_EEPROM_SIZE)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Set wait IRQs */
    bIrq0WaitFor = 0x00;
    bIrq1WaitFor = 0x00;

    /* Prepare command */
    aCmd[0] = (uint8_t)(wAddress >> 8);
    aCmd[1] = (uint8_t)(wAddress);
    aCmd[2] = bData;

    /* Perform command */
    return phhalHw_Rc663_Command_Int(
        pDataParams,
        PHHAL_HW_RC663_CMD_WRITEE2,
        PH_EXCHANGE_DEFAULT,
        bIrq0WaitFor,
        bIrq1WaitFor,
        aCmd,
        3,
        0,
        NULL,
        NULL);
}

phStatus_t phhalHw_Rc663_Cmd_WriteE2Page(
    phhalHw_Rc663_DataParams_t * pDataParams,
    uint16_t wAddress,
    uint8_t * pData,
    uint8_t bDataLen
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM aCmd[2];
    uint8_t     PH_MEMLOC_REM bIrq0WaitFor;
    uint8_t     PH_MEMLOC_REM bIrq1WaitFor;
    uint16_t    PH_MEMLOC_REM wFifoBytes;

    /* Check Address range and data length */
    if ((wAddress >= PHHAL_HW_RC663_EEPROM_NUM_PAGES) || (bDataLen == 0) || (bDataLen > PHHAL_HW_RC663_EEPROM_PAGE_SIZE))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Set wait IRQs */
    bIrq0WaitFor = 0x00;
    bIrq1WaitFor = 0x00;

    /* Flush FiFo */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_FlushFifo(pDataParams));

    /* Write Address into FIFO */
    aCmd[1] = (uint8_t)(wAddress);
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteFifo(pDataParams, &aCmd[1], 1, &wFifoBytes));

    /* Perform command */
    return phhalHw_Rc663_Command_Int(
        pDataParams,
        PHHAL_HW_RC663_CMD_WRITEE2PAGE,
        PHHAL_HW_RC663_OPTION_FIFO_NOCLEAR,
        bIrq0WaitFor,
        bIrq1WaitFor,
        pData,
        (uint16_t)bDataLen,
        0,
        NULL,
        NULL);
}

phStatus_t phhalHw_Rc663_Cmd_ReadE2(
                                    phhalHw_Rc663_DataParams_t * pDataParams,
                                    uint16_t wAddress,
                                    uint16_t wNumBytes,
                                    uint8_t * pData
                                    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM aCmd[3];
    uint8_t     PH_MEMLOC_REM bIrq0WaitFor;
    uint8_t     PH_MEMLOC_REM bIrq1WaitFor;
    uint16_t    PH_MEMLOC_REM wDataLenTmp;

    /* Parameter check */
    if (wNumBytes > 0x100)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* 0 means 256, do this for the check below */
    if (wNumBytes == 0)
    {
        wNumBytes = 0x100;
    }

    /* Limit number of bytes to read to FiFo-Size */
    if (pDataParams->bFifoSize == PHHAL_HW_RC663_VALUE_FIFOSIZE_255)
    {
        if (wNumBytes > 0xFF)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
    }

    /* Check Address range */
    if ((wAddress + wNumBytes) > PHHAL_HW_RC663_EEPROM_SIZE)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Set wait IRQs */
    bIrq0WaitFor = PHHAL_HW_RC663_BIT_HIALERTIRQ;
    bIrq1WaitFor = 0x00;

    /* Prepare command */
    aCmd[0] = (uint8_t)(wAddress >> 8);
    aCmd[1] = (uint8_t)(wAddress);
    aCmd[2] = (uint8_t)wNumBytes;

    /* Perform command */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_Command_Int(
        pDataParams,
        PHHAL_HW_RC663_CMD_READE2,
        PH_EXCHANGE_DEFAULT,
        bIrq0WaitFor,
        bIrq1WaitFor,
        aCmd,
        3,
        wNumBytes,
        pData,
        &wDataLenTmp));

    /* Check received data */
    if (wDataLenTmp != wNumBytes)
    {
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc663_Cmd_LoadReg(
                                     phhalHw_Rc663_DataParams_t * pDataParams,
                                     uint16_t wEEAddress,
                                     uint8_t bRegAddress,
                                     uint8_t bNumBytes
                                     )
{
    uint8_t PH_MEMLOC_REM aCmd[4];
    uint8_t PH_MEMLOC_REM bIrq0WaitFor;
    uint8_t PH_MEMLOC_REM bIrq1WaitFor;

    /* Parameter check */
    if (bNumBytes == 0)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* EEPROM address check */
    if ((wEEAddress < PHHAL_HW_RC663_EEPROM_SECTION2_BEGIN) || ((wEEAddress + bNumBytes) > (PHHAL_HW_RC663_EEPROM_SECTION2_END + 1)))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Register address check */
    if ((bRegAddress + bNumBytes) > PHHAL_HW_RC663_MAX_NUM_REGS)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Set wait IRQs */
    bIrq0WaitFor = 0x00;
    bIrq1WaitFor = 0x00;

    /* Prepare command */
    aCmd[0] = (uint8_t)(wEEAddress >> 8);
    aCmd[1] = (uint8_t)(wEEAddress);
    aCmd[2] = bRegAddress;
    aCmd[3] = bNumBytes;

    /* Perform command */
    return phhalHw_Rc663_Command_Int(
        pDataParams,
        PHHAL_HW_RC663_CMD_LOADREG,
        PH_EXCHANGE_DEFAULT,
        bIrq0WaitFor,
        bIrq1WaitFor,
        aCmd,
        4,
        0,
        NULL,
        NULL);
}

phStatus_t phhalHw_Rc663_Cmd_LoadProtocol(
    phhalHw_Rc663_DataParams_t * pDataParams,
    uint8_t bTxProtocol,
    uint8_t bRxProtocol
    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t PH_MEMLOC_REM aCmd[2];
    uint8_t PH_MEMLOC_REM bIrq0WaitFor;
    uint8_t PH_MEMLOC_REM bIrq1WaitFor;
    uint16_t PH_MEMLOC_REM wDerivate;
    uint8_t PH_MEMLOC_REM pDerivate[2];

    /* Parameter check */
    if ((bTxProtocol > PHHAL_HW_RC663_MAX_TX_SETTINGS) || (bRxProtocol > PHHAL_HW_RC663_MAX_RX_SETTINGS))
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Check the RC663 derivative type by reading the manufacturer info page in the EEPROM*/
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_Cmd_ReadE2(pDataParams, 0, 2, pDerivate));
    wDerivate = pDerivate[1];
    wDerivate = (wDerivate << 8) | pDerivate[0];

    /* Set wait IRQs this bit needs to be set for interrupt mode  */
    bIrq0WaitFor = 0x00;

    bIrq1WaitFor = 0x00;

    if((wDerivate == PHHAL_HW_SLRC610_PRODUCT_ID) && (bTxProtocol >= 0x0A))
    {
        bTxProtocol -= 0xA;
    }
    if((wDerivate == PHHAL_HW_SLRC610_PRODUCT_ID) && (bRxProtocol >= 0x0A))
    {
        bRxProtocol -= 0xA;
    }
    /* Prepare command */
    aCmd[0] = bRxProtocol;
    aCmd[1] = bTxProtocol;

    /* Perform command */
    return phhalHw_Rc663_Command_Int(
        pDataParams,
        PHHAL_HW_RC663_CMD_LOADPROTOCOL,
        PH_EXCHANGE_DEFAULT,
        bIrq0WaitFor,
        bIrq1WaitFor,
        aCmd,
        2,
        0,
        NULL,
        NULL);
}

phStatus_t phhalHw_Rc663_Cmd_LoadKeyE2(
                                       phhalHw_Rc663_DataParams_t * pDataParams,
                                       uint8_t  bKeyNo
                                       )
{
    uint8_t PH_MEMLOC_REM aCmd[1];
    uint8_t PH_MEMLOC_REM bIrq0WaitFor;
    uint8_t PH_MEMLOC_REM bIrq1WaitFor;

    /* Set wait IRQs */
    bIrq0WaitFor = 0x00;
    bIrq1WaitFor = 0x00;

    /* Prepare command */
    aCmd[0] = bKeyNo;

    /* Perform command */
    return phhalHw_Rc663_Command_Int(
        pDataParams,
        PHHAL_HW_RC663_CMD_LOADKEYE2,
        PH_EXCHANGE_DEFAULT,
        bIrq0WaitFor,
        bIrq1WaitFor,
        aCmd,
        1,
        0,
        NULL,
        NULL);
}

phStatus_t phhalHw_Rc663_Cmd_StoreKeyE2(
                                        phhalHw_Rc663_DataParams_t * pDataParams,
                                        uint8_t  bKeyNo,
                                        uint8_t * pKeys,
                                        uint8_t bNumberOfKeys
                                        )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM aCmd[2];
    uint8_t     PH_MEMLOC_REM bIrq0WaitFor;
    uint8_t     PH_MEMLOC_REM bIrq1WaitFor;
    uint16_t    PH_MEMLOC_REM wDataLen;
    uint16_t    PH_MEMLOC_REM wFifoBytes;

    /* Parameter check */
    if (bNumberOfKeys == 0)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Parameter check */
    if ((uint16_t)(bKeyNo + bNumberOfKeys) > PHHAL_HW_RC663_MAX_NUM_KEYS)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
    }

    /* Calculate overal data length */
    wDataLen = (uint16_t)(bNumberOfKeys * PHHAL_HW_RC663_MFC_KEY_LENGTH);

    /* Limit number of bytes to write to FiFo-Size */
    if (pDataParams->bFifoSize == PHHAL_HW_RC663_VALUE_FIFOSIZE_255)
    {
        if (wDataLen > 0xFF)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
    }
    else
    {
        if (wDataLen > 0x200)
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
    }

    /* Set wait IRQs */
    bIrq0WaitFor = 0x00;
    bIrq1WaitFor = 0x00;

    /* Flush FiFo */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_FlushFifo(pDataParams));

    /* Write Keynumber into FIFO */
    aCmd[1] = bKeyNo;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteFifo(pDataParams, &aCmd[1], 1, &wFifoBytes));

    /* Perform command */
    return phhalHw_Rc663_Command_Int(
        pDataParams,
        PHHAL_HW_RC663_CMD_STOREKEYE2,
        PHHAL_HW_RC663_OPTION_FIFO_NOCLEAR,
        bIrq0WaitFor,
        bIrq1WaitFor,
        pKeys,
        wDataLen,
        0,
        NULL,
        NULL);
}

phStatus_t phhalHw_Rc663_Cmd_SoftReset(
                                       phhalHw_Rc663_DataParams_t * pDataParams
                                       )
{

    return phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_COMMAND, PHHAL_HW_RC663_CMD_SOFTRESET);
}

phStatus_t phhalHw_Rc663_Cmd_AckReq(
                                    phhalHw_Rc663_DataParams_t * pDataParams,
                                    uint8_t * pCmdBuffer,
                                    uint16_t wLen,
                                    uint8_t * pRxBuffer,
                                    uint16_t * wRxBufferLen
                                    )
{
    phStatus_t PH_MEMLOC_REM statusTmp;
    uint16_t   PH_MEMLOC_REM wBytesWritten;
    uint8_t    PH_MEMLOC_REM bError;
    uint8_t    PH_MEMLOC_REM bRegister;
    uint8_t    PH_MEMLOC_REM bIrq0WaitFor;
    uint8_t    PH_MEMLOC_REM bIrq1WaitFor;
    uint8_t    PH_MEMLOC_REM bIrq0Reg = 0x00;
    uint8_t    PH_MEMLOC_REM bIrq1Reg = 0x00;

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_RXCORR, 0x88));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_RXCTRL, 0x07));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_RXWAIT, 0x3D));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_TXWAITLO, 0x80));

    /* Set Preamble */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_FRAMECON, 0x01));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_TXDATAMOD, 0x00));

    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_TXSYM0H, 0x68));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_TXSYM0L, 0x41));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_TXSYM10LEN, 0x8E));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_TXSYM10BURSTLEN, 0x00));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_TXSYM10BURSTCTRL, 0x00));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_TXSYM10MOD, 0x00));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_TXSYMFREQ, 0x05));
    /* Set preamble done */

    /* Load IDLE command (to terminate a possibly running command) */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(
        pDataParams,
        PHHAL_HW_RC663_REG_COMMAND,
        PHHAL_HW_RC663_CMD_IDLE));

    /* Flush FiFo */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_FlushFifo(pDataParams));

    /* Clear all IRQ0 flags */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(
        pDataParams,
        PHHAL_HW_RC663_REG_IRQ0,
        (uint8_t)~(uint8_t)PHHAL_HW_RC663_BIT_SET));

    /* Clear all IRQ1 flags */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(
        pDataParams,
        PHHAL_HW_RC663_REG_IRQ1,
        (uint8_t)~(uint8_t)PHHAL_HW_RC663_BIT_SET));

    /* Write exchange command to FiFo */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteFifo(
        pDataParams,
        &pCmdBuffer[0],
        wLen,
        &wBytesWritten));

    /* Start the command */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(
        pDataParams,
        PHHAL_HW_RC663_REG_COMMAND,
        PHHAL_HW_RC663_CMD_ACKREQ));

    /* Set wait IRQs */
    bIrq0WaitFor = PHHAL_HW_RC663_BIT_IDLEIRQ;
    bIrq1WaitFor = PHHAL_HW_RC663_BIT_TIMER1IRQ;

    /* Wait for enabled IRQs */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WaitIrq(
        pDataParams,
        PH_ON,
        PH_OFF,
        bIrq0WaitFor,
        bIrq1WaitFor,
        &bIrq0Reg,
        &bIrq1Reg));

    /* Timeout handling */
    if (bIrq1Reg & PHHAL_HW_RC663_BIT_TIMER1IRQ)
    {
        /* Stop the command */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(
            pDataParams,
            PHHAL_HW_RC663_REG_COMMAND,
            PHHAL_HW_RC663_CMD_IDLE));

        /* Flush FiFo */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_FlushFifo(pDataParams));

        /* Retrieve content of Timer Control Register */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_ReadRegister(
            pDataParams,
            PHHAL_HW_RC663_REG_TCONTROL,
            &bRegister));

        /* Manually stop timers */
        bRegister |= PHHAL_HW_RC663_BIT_T0STARTSTOPNOW | PHHAL_HW_RC663_BIT_T1STARTSTOPNOW;
        bRegister &= (uint8_t)~(uint8_t)(PHHAL_HW_RC663_BIT_T0RUNNING | PHHAL_HW_RC663_BIT_T1RUNNING);
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_WriteRegister(
            pDataParams,
            PHHAL_HW_RC663_REG_TCONTROL,
            bRegister));

        /* Return timeout */
        return PH_ADD_COMPCODE(PH_ERR_IO_TIMEOUT, PH_COMP_HAL);
    }

    /* Read received data from FiFo (Max possible response length is 70) */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_ReadFifo(
        pDataParams,
        70,
        pRxBuffer,
        wRxBufferLen));

    /* No bytes received -> timeout */
    if(wRxBufferLen == 0)
    {
        return PH_ADD_COMPCODE(PH_ERR_IO_TIMEOUT, PH_COMP_HAL);
    }

    /* Check if any error occurred */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_ReadRegister(
        pDataParams,
        PHHAL_HW_RC663_REG_ERROR,
        &bError));

    /* Buffer overflow */
    if (bError & PHHAL_HW_RC663_BIT_FIFOOVL)
    {
        statusTmp = PH_ERR_BUFFER_OVERFLOW;
    }
    /* FiFo write error */
    else if (bError & PHHAL_HW_RC663_BIT_FIFOWRERR)
    {
        statusTmp = PH_ERR_READ_WRITE_ERROR;
    }
    /* EEPROM error */
    else if (bError & PHHAL_HW_RC663_BIT_EE_ERR)
    {
        statusTmp = PH_ERR_READ_WRITE_ERROR;
    }
    /* Framing error */
    else if (bError & PHHAL_HW_RC663_BIT_MINFRAMEERR)
    {
        statusTmp = PH_ERR_FRAMING_ERROR;
    }
    /* Collision */
    else if (bError & PHHAL_HW_RC663_BIT_COLLDET)
    {
        statusTmp = PH_ERR_COLLISION_ERROR;
    }
    /* Protocol error */
    else if (bError & PHHAL_HW_RC663_BIT_PROTERR)
    {
        statusTmp = PH_ERR_PROTOCOL_ERROR;
    }
    /* CRC / parity error */
    else if (bError & PHHAL_HW_RC663_BIT_INTEGERR)
    {
        statusTmp = PH_ERR_INTEGRITY_ERROR;
    }
    /* No error */
    else
    {
        statusTmp = PH_ERR_SUCCESS;
    }

    return PH_ADD_COMPCODE(statusTmp, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc663_Cmd_Lpcd_SetConfig(
                                         phhalHw_Rc663_DataParams_t * pDataParams,
                                         uint8_t bMode,
                                         uint8_t bI,
                                         uint8_t bQ,
                                         uint16_t wPowerDownTimeMs,
                                         uint16_t wDetectionTimeUs
                                         )
{
    /* Check I/Q values */
    if (!(bMode & PHHAL_HW_RC663_CMD_LPCD_MODE_OPTION_IGNORE_IQ))
    {
        if ((bI == 0x00) || (bI >= 0x3F) || (bQ == 0x00) || (bQ >= 0x3F))
        {
            return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_HAL);
        }
    }

    pDataParams->bMode = bMode;
    pDataParams->bI = bI;
    pDataParams->bQ = bQ;
    pDataParams->wPowerDownTimeMs = wPowerDownTimeMs;
    pDataParams->wDetectionTimeUs = wDetectionTimeUs;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

phStatus_t phhalHw_Rc663_WriteRegister(
                                       phhalHw_Rc663_DataParams_t * pDataParams,
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

phStatus_t phhalHw_Rc663_ReadRegister(
                                      phhalHw_Rc663_DataParams_t * pDataParams,
                                      uint8_t bAddress,
                                      uint8_t * pValue
                                      )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bTxBuffer[2];
    uint16_t    PH_MEMLOC_REM wTxLength;
    uint16_t    PH_MEMLOC_REM wBytesRead;
    uint8_t     PH_MEMLOC_REM bNumExpBytes;

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
        bTxBuffer[0] = (uint8_t)(bAddress << 1) | 0x01U;
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

#endif /* NXPBUILD__PHHAL_HW_RC663 */
