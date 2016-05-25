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
* Wait Routines for RC523 specific HAL-Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 2673 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  CHu: Generated 19. May 2009
*
*/

#include <ph_Status.h>
#include <phbalReg.h>
#include <ph_RefDefs.h>

#ifdef NXPBUILD__PHHAL_HW_RC523

#include "phhalHw_Rc523.h"
#include "phhalHw_Rc523_Int.h"
#include <phhalHw_Rc523_Reg.h>

phStatus_t phhalHw_Rc523_WaitIrq(
                                 phhalHw_Rc523_DataParams_t * pDataParams,
                                 uint8_t bIrq0WaitFor,
                                 uint8_t bIrq1WaitFor,
                                 uint8_t * pIrq0Reg,
                                 uint8_t * pIrq1Reg
                                 )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bIrq0EnReg;
    uint8_t     PH_MEMLOC_REM bIrq1EnReg;
#ifdef _WIN32
    uint8_t     PH_MEMLOC_REM bRegister;
#else
    phOsal_EventType_t   PH_MEMLOC_REM dwEventReceived = 0;
#endif

    /* Parameter check */
    if (!bIrq0WaitFor && !bIrq1WaitFor)
    {
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
    }

#ifndef _WIN32
    /* Clear all events that might have occurred. */
    PH_CHECK_SUCCESS_FCT(statusTmp, phOsal_Event_Consume(E_PH_OSAL_EVT_RF, E_PH_OSAL_EVT_SRC_HAL));
#endif

    /* Enable IRQ0 interrupt sources */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIEN, &bIrq0EnReg));
    bIrq0EnReg &= PHHAL_HW_RC523_BIT_IRQINV;
    bIrq0EnReg |= bIrq0WaitFor;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIEN, bIrq0EnReg));
    /* Enable IRQ1 interrupt sources */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_DIVIEN, &bIrq1EnReg));
    bIrq1EnReg &= PHHAL_HW_RC523_BIT_IRQPUSHPULL;
    bIrq1EnReg |= bIrq1WaitFor;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_DIVIEN, bIrq1EnReg));

    /* wait until an IRQ occurs */
#ifdef _WIN32
    do
    {
        /* INSERT CODE HERE */

        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_STATUS1, &bRegister));
    }
    while (!(bRegister & PHHAL_HW_RC523_BIT_IRQ));
#else
    PH_CHECK_SUCCESS_FCT(statusTmp, phOsal_Event_WaitAny(E_PH_OSAL_EVT_RF | E_PH_OSAL_EVT_ABORT, PH_MAX_DELAY, &dwEventReceived));

    if (dwEventReceived & E_PH_OSAL_EVT_ABORT)
    {
        /* Clear IRQ0 interrupt sources */
        bIrq0EnReg &= PHHAL_HW_RC523_BIT_IRQINV;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIEN, bIrq0EnReg));
        /* Clear IRQ1 interrupt sources */
        bIrq1EnReg &= PHHAL_HW_RC523_BIT_IRQPUSHPULL;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_DIVIEN, bIrq1EnReg));

        /* Consume abort event */
        PH_CHECK_SUCCESS_FCT(statusTmp, phOsal_Event_Consume(E_PH_OSAL_EVT_ABORT, E_PH_OSAL_EVT_SRC_HAL));
        /* Load IDLE Command and exit with HAL Aborted status. */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMAND, PHHAL_HW_RC523_CMD_IDLE));
        return PH_ADD_COMPCODE(PH_ERR_ABORTED, PH_COMP_HAL);
    }
#endif

    /* Clear IRQ0 interrupt sources */
    bIrq0EnReg &= PHHAL_HW_RC523_BIT_IRQINV;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIEN, bIrq0EnReg));
    /* Clear IRQ1 interrupt sources */
    bIrq1EnReg &= PHHAL_HW_RC523_BIT_IRQPUSHPULL;
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_WriteRegister(pDataParams, PHHAL_HW_RC523_REG_DIVIEN, bIrq1EnReg));

    /* Return contents of IRQ0 register */
    if (pIrq0Reg)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_COMMIRQ, pIrq0Reg));
    }

    /* Return contents of IRQ1 register */
    if (pIrq1Reg)
    {
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc523_ReadRegister(pDataParams, PHHAL_HW_RC523_REG_DIVIRQ, pIrq1Reg));
    }
#ifndef _WIN32
    /* Consume Unwanted Events */
    PH_CHECK_SUCCESS_FCT(statusTmp, phOsal_Event_Consume(E_PH_OSAL_EVT_RF, E_PH_OSAL_EVT_SRC_HAL));
#endif
    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

#endif /* NXPBUILD__PHHAL_HW_RC523 */
