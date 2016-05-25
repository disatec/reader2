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
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 3840 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  KK: Generated 20. Feb 2015
*
*/

#ifndef PHLNLLCP_TIMERS_H
#define PHLNLLCP_TIMERS_H

/* *****************************************************************************************************************
*   Includes
* ***************************************************************************************************************** */
#include "phlnLlcp.h"

/* *****************************************************************************************************************
* MACROS/Defines
* ***************************************************************************************************************** */
#define PHLN_LLCP_TIMERS_SYM_ID     0x00001001UL
#define PHLN_LLCP_TIMERS_LTO_ID     0x00001002UL

#define PHLN_LLCP_LTO_DELTA_PERCENTAGE        30               /**< Delta milli secs as guard time for sw overheads. */
/**
* Delta in percentage to be reduced from LTO for calculating SYMM timeout.
* PHLN_LLCP_SYMM_VALUE is a percentage of LTO value, LTO value is configured by the user.
*/
#define PHLN_LLCP_SYMM_DELTA_PERCENTAGE       (100 - PHLN_LLCP_SYMM_VALUE)

/* *****************************************************************************************************************
*   Extern Variables
* ***************************************************************************************************************** */
extern phOsal_TimerHandle_t xSymHandle;
extern phOsal_TimerHandle_t xLtoHandle;

/* *****************************************************************************************************************
*   Function Prototypes
* ***************************************************************************************************************** */
phStatus_t phlnLlcp_Timers_InitSym(pphOsal_TimerCallback_t phlnLlcp_SymTimerCallback);
phStatus_t phlnLlcp_Timers_InitLto(pphOsal_TimerCallback_t phlnLlcp_LtoTimerCallback);
phStatus_t phlnLlcp_Timers_DeInitSym();
phStatus_t phlnLlcp_Timers_DeInitLto();

phStatus_t phlnLlcp_Timers_StopLto();

phStatus_t phlnLlcp_TimersChange_SymPeriod(uint16_t wSymTimeoutms);
phStatus_t phlnLlcp_TimersChange_LtoPeriod(uint16_t wLtoTimeoutms);
void phlnLlcp_SymTimerCallback();
void phlnLlcp_LtoTimerCallback();

/**
*phStatus_t phlnLlcp_Timers_SymStart(void);
*/
#define phlnLlcp_Timers_SymStart(wSymmTime)  phOsal_Timer_Start(xSymHandle, wSymmTime)

/**
*phStatus_t phlnLlcp_Timers_LtoStart(void);
*/
#define phlnLlcp_Timers_LtoStart(wLtoTime)  phOsal_Timer_Start(xLtoHandle, wLtoTime)

/**
*phStatus_t phlnLlcp_Timers_SymStop(void);
*/
#define phlnLlcp_Timers_SymStop()  phOsal_Timer_Stop(xSymHandle)

/**
*phStatus_t phlnLlcp_Timers_LtoStop(void);
*/
#define phlnLlcp_Timers_LtoStop()  phOsal_Timer_Stop(xLtoHandle)

#endif /* PHLNLLCP_TIMERS_H */
