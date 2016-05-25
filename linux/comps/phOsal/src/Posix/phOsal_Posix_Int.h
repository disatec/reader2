/*
 * phOsal_Posix_Int.h
 *
 *  Created on: 27 Jan 2016
 *      Author: donatien
 */

#ifndef PHOSAL_POSIX_INT_H_
#define PHOSAL_POSIX_INT_H_

#include <ph_Status.h>
#include <phOsal.h>

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

phStatus_t phOsal_Timer_Posix_Int_StartUs(phOsal_TimerHandle_t timerHandle,
                              uint64_t dwTimeUs
                              );
phStatus_t phOsal_Timer_Posix_Int_Get_ElapsedUs(phOsal_TimerHandle_t timerHandle,
                              uint64_t* dwTimeUs
                              );
phStatus_t phOsal_Posix_Thread_SetMainThread();

extern bool phOsal_Event_Can_Clear(phOsal_EventType_t eEvtType);

#endif /* PHOSAL_POSIX_INT_H_ */
