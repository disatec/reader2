/*
*         Copyright (c), NXP Semiconductors Bangalore / India
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
* Software LLCP Link layer Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 3725 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  KK: Generated 20. Feb 2015
*
*/

/* *****************************************************************************************************************
* Includes
* ***************************************************************************************************************** */
#include "ph_NxpBuild.h"
#include <ph_Status.h>
#include <phlnLlcp.h>

#ifdef NXPBUILD__PHLN_LLCP
#include "phlnLlcp_Timers.h"
#include "phlnLlcp_Sw_Mac.h"

/* *****************************************************************************************************************
* Global and Static Variables
* Total Size: NNNbytes
* ***************************************************************************************************************** */
phOsal_TimerHandle_t xSymHandle = NULL;
phOsal_TimerHandle_t xLtoHandle = NULL;
static uint8_t gkphlnLlcp_baSymPdu[2] = {0x0, 0x0};
extern phlnLlcp_Transport_Socket_t gsphlnLlcp_Socket;

/* *****************************************************************************************************************
* Private Functions
* ***************************************************************************************************************** */

phStatus_t phlnLlcp_Timers_InitSym(pphOsal_TimerCallback_t phlnLlcp_SymTimerCallback)
{
    xSymHandle = phOsal_Timer_Create(PHLN_LLCP_TIMERS_SYM_ID, phlnLlcp_SymTimerCallback);

    if(xSymHandle == NULL){
        return (PH_ERR_RESOURCE_ERROR | PH_COMP_LN_LLCP);
    }

    return PH_ERR_SUCCESS;
}

phStatus_t phlnLlcp_Timers_InitLto(pphOsal_TimerCallback_t phlnLlcp_LtoTimerCallback)
{
    xLtoHandle = phOsal_Timer_Create(PHLN_LLCP_TIMERS_LTO_ID, phlnLlcp_LtoTimerCallback);

    if(xLtoHandle == NULL){
        return (PH_ERR_RESOURCE_ERROR | PH_COMP_LN_LLCP);
    }

    return PH_ERR_SUCCESS;
}

phStatus_t phlnLlcp_Timers_DeInitSym()
{
    phStatus_t PH_MEMLOC_REM wStatus = PH_ERR_SUCCESS;
    if(xSymHandle != NULL)
    {
        wStatus = phOsal_Timer_Delete(xSymHandle);
    }
    xSymHandle = NULL;
    return wStatus;
}

phStatus_t phlnLlcp_Timers_DeInitLto()
{
    phStatus_t PH_MEMLOC_REM wStatus = PH_ERR_SUCCESS;
    if(xLtoHandle != NULL)
    {
        wStatus = phOsal_Timer_Delete(xLtoHandle);
    }
    xLtoHandle = NULL;
    return wStatus;
}

phStatus_t phlnLlcp_Timers_StopLto()
{
    phStatus_t PH_MEMLOC_REM wStatus = PH_ERR_SUCCESS;
    if(xLtoHandle != NULL)
    {
        wStatus = phOsal_Timer_Stop(xLtoHandle);
    }
    return wStatus;
}

void phlnLlcp_SymTimerCallback()
{
    /* Check whether the DTS message q mutex is available */
    /* If available post a SymmPdu to the DataToBeSent */
    /* else do nothing since already some other service is queuing the DTS msg */

    phTools_Q_t * PH_MEMLOC_REM psMsgQueue = phTools_Q_Get(1, PH_ON);
    /* If Lack of  Free Message Q pool, Symm will fail and LTO expires on the Peer Side.
    * Increase the pool size to avoid this issue. */
    if (psMsgQueue != NULL)
    {
        psMsgQueue->pbData = (uint8_t *)gkphlnLlcp_baSymPdu;
        psMsgQueue->dwLength = 2;
        psMsgQueue->bType = PH_TOOLS_Q_DATA_TO_BE_SENT;
        psMsgQueue->wFrameOpt = PH_TRANSMIT_DEFAULT;
        psMsgQueue->pSender = &gsphlnLlcp_Socket;

        (void)phTools_Q_SendFront(psMsgQueue, 0, psMsgQueue->wFrameOpt);
    }
}

void phlnLlcp_LtoTimerCallback()
{
    /* Abort 18092 operation by calling HAL Shutdown. */
    phlnLlcp_Sw_MacHAL_ShutDown();
}

#endif /* NXPBUILD__PHLN_LLCP */
