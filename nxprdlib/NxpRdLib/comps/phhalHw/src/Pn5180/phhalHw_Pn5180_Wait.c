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
* Wait Routines for Pn5180 specific HAL-Component of Reader Library Framework.
*
* $Author: nxp62726 $
* $Revision$ (NFCRDLIB_V4.010.03.001609  (NFCRDLIB_V4.010.01.001603 : 172 ))
* $Date: 2015-02-18 13:51:16 +0530 (Wed, 18 Feb 2015) $
*/

#include <ph_Status.h>
#include <phbalReg.h>
#include <ph_RefDefs.h>

#ifdef NXPBUILD__PHHAL_HW_PN5180

#include "phhalHw_Pn5180.h"
#include "phhalHw_Pn5180_Instr.h"
#include "phhalHw_Pn5180_Int.h"
#include "phhalHw_Pn5180_Wait.h"
#include "phhalHw_Pn5180_Reg.h"


phStatus_t phhalHw_Pn5180_WaitIrq(
    phhalHw_Pn5180_DataParams_t * pDataParams,
    uint8_t bEnableIrq,
    uint8_t bWaitUntilPowerUp,
    uint32_t dwIrqWaitFor,
    uint32_t * dwIrqReg
    )
{
    phStatus_t PH_MEMLOC_REM statusTmp;
    uint32_t   PH_MEMLOC_REM dwRegister;
    phOsal_EventType_t PH_MEMLOC_REM tReceivedEvents;

    /* Parameter check */
    if (!dwIrqWaitFor)
    {
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
    }
    if (bWaitUntilPowerUp == PH_ON)
    {
        /* Need to add power up behaviour */
    }

    /* If requested by the user, enable the corresponding IRQs */
    if ((bEnableIrq & PHHAL_HW_CHECK_IRQ_PIN_MASK) != PH_OFF)
    {
            /*wait for IRQ pin event or Abort event*/
            statusTmp = phOsal_Event_WaitAny(E_PH_OSAL_EVT_RF | E_PH_OSAL_EVT_ABORT, PH_MAX_DELAY, &tReceivedEvents);

            /*Handle abort event*/
            if ((E_PH_OSAL_EVT_ABORT & tReceivedEvents) || (statusTmp != PH_ERR_SUCCESS))
            {

                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_int_IdleCommand(pDataParams));

                /* Disable IRQ sources */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, IRQ_ENABLE, (uint32_t)~dwIrqWaitFor));

                (void)phOsal_Event_Consume(E_PH_OSAL_EVT_ABORT, E_PH_OSAL_EVT_SRC_HAL);
                return PH_ADD_COMPCODE(PH_ERR_ABORTED, PH_COMP_HAL);
            }
            else
            {
                /* Read the IRQ register and check if the interrupt has occured */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_ReadRegister(pDataParams, IRQ_STATUS, &dwRegister));

                /* Store the state of status register */
                *dwIrqReg = dwRegister;

                if ((bEnableIrq & PHHAL_HW_DISABLE_IRQ_CLEAR_MASK) == PH_OFF)
                {
                     /* Clear all Interrupts for e.g Tx interrupt during receive */
                     PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_SET_CLEAR, dwRegister));
                }
                /* Disable IRQ sources */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegisterAndMask(pDataParams, IRQ_ENABLE, (uint32_t)~dwIrqWaitFor));

                (void)phOsal_Event_Consume(E_PH_OSAL_EVT_RF, E_PH_OSAL_EVT_SRC_HAL);

                return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);

            }
    }
    else
    {
        /* Wait until any of the IRQ bits that we are subscribing for occurs */
        do
        {
            /* Read the IRQ register and check if the interrupt has occured */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_ReadRegister(pDataParams, IRQ_STATUS, &dwRegister));
        }while(!(dwRegister & dwIrqWaitFor));

        /* Store the state of status register */
        *dwIrqReg = dwRegister;

        if ((bEnableIrq & PHHAL_HW_DISABLE_IRQ_CLEAR_MASK) == PH_OFF)
        {
             /* Clear all Interrupts for e.g Tx interrupt during receive */
             PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Pn5180_Instr_WriteRegister(pDataParams, IRQ_SET_CLEAR, dwRegister));
        }
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);

}

#endif  /* NXPBUILD__PHHAL_HW_PN5180 */

