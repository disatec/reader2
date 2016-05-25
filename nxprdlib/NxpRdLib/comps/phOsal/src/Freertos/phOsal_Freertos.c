/*
*         Copyright (c), NXP Semiconductors Bangalore / India
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
* particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

/** \file
* Freertos OSAL Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 3935 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  PC: Generated 23. Aug 2012
*
*/
#include <ph_Status.h>
#include <phOsal.h>

#ifdef NXPBUILD__PH_OSAL_FREERTOS

#include <FreeRTOS.h>
#include <event_groups.h>
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* -------------------------------------------------------------------------------------
 * RTOS SEMAPHORES RELATED API's
 * ------------------------------------------------------------------------------------- */

/**
* Global Status of ISR Event Register Status to Receive Events from ISR
*/
static EventGroupHandle_t gphOsal_EventGroup;

/**
* Source of Events
*/
static volatile phOsal_EventSource_t gphOsal_EvtLastSource = E_PH_OSAL_EVT_SRC_NONE;
/**
* Destination of Events
*/
static volatile phOsal_EventDestination_t gphOsal_EvtLastDest = E_PH_OSAL_EVT_DEST_NONE;
/**
* Event Detail
*/
static volatile phOsal_EventDetail_t gphOsal_EvtDetail = E_PH_OSAL_EVT_DETAIL_SUCCESS;

phStatus_t phOsal_Event_Init(void)
{
    gphOsal_EventGroup = xEventGroupCreate();
    if(gphOsal_EventGroup == NULL)
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);

    gphOsal_EvtLastSource = E_PH_OSAL_EVT_SRC_NONE;
    gphOsal_EvtLastDest = E_PH_OSAL_EVT_DEST_NONE;
    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Event_GetEventHandle(void **pHandle)
{
    /* In case it is not initialized yet */
    if(gphOsal_EventGroup == NULL)
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);

    *pHandle = gphOsal_EventGroup;
    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Event_Post(
                       phOsal_EventType_t eEvtType,
                       phOsal_EventSource_t eSrc,
                       phOsal_EventDestination_t eDest
                       )
{
    BaseType_t  xHigherPriorityTaskWoken, xResult;
    xHigherPriorityTaskWoken = PH_OFF;

    /* In case it is not initialized yet */
    if(gphOsal_EventGroup == NULL)
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);

    if(eSrc == E_PH_OSAL_EVT_SRC_ISR)    /* Specifically For ISR */
    {
        xResult = (EventBits_t)xEventGroupSetBitsFromISR(
                gphOsal_EventGroup, eEvtType, &xHigherPriorityTaskWoken);

        if( xResult == PH_ON )
        {
            /* Inform Scheduler about the Task Wake up*/
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

            /* Set Default Event Detail */
            gphOsal_EvtDetail = E_PH_OSAL_EVT_DETAIL_SUCCESS;
        }
        else
        {
            /* Set Default Event Detail */
            gphOsal_EvtDetail = E_PH_OSAL_EVT_DETAIL_INTERNAL_ERROR;
        }
    }
    else
    {
        xEventGroupSetBits(gphOsal_EventGroup, eEvtType);
    }

    /* Set the Last Source */
    gphOsal_EvtLastSource = eSrc;

    /* Set the Last Destination */
    gphOsal_EvtLastDest = eDest;

    return PH_ERR_SUCCESS;
}

void phOsal_Event_PostwithDetail(
                                 phOsal_EventType_t eEvtType,
                                 phOsal_EventSource_t eSrc,
                                 phOsal_EventDestination_t eDest,
                                 phOsal_EventDetail_t eDetail
                                 )
{
    (void)phOsal_Event_Post(eEvtType, eSrc, eDest);
    phOsal_Event_SetDetail(eDetail);
}

phStatus_t phOsal_Event_Consume(
                          phOsal_EventType_t eEvtType,
                          phOsal_EventSource_t eSrc
                          )
{
    /* In case it is not initialized yet */
    if(gphOsal_EventGroup == NULL)
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);

    if(eSrc == E_PH_OSAL_EVT_SRC_ISR)   /* Specifically For ISR */
    {
        xEventGroupClearBitsFromISR(gphOsal_EventGroup, eEvtType);
    }
    else                                /* In Normal Cases */
    {
        xEventGroupClearBits(gphOsal_EventGroup, eEvtType);
    }

    /* Set the Last Source */
    gphOsal_EvtLastSource = eSrc;

    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Event_WaitAny(
                                phOsal_EventType_t eEvtType,
                                uint32_t dwTimeoutCount,
                                phOsal_EventType_t *pRcvdEvt
                                )
{
    phStatus_t eStatus = PH_ERR_OSAL_ERROR;
    EventBits_t xbits = 0;

    /* In case it is not initialized yet */
    if( gphOsal_EventGroup == NULL)
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);

    /* Only Possible from HAL */
    /* Don't Clear flags after ISR and Don't Wait for All bits to be Set */
    xbits = xEventGroupWaitBits(gphOsal_EventGroup, eEvtType, pdFALSE, pdFALSE, dwTimeoutCount);
    if(xbits & ((uint32_t)eEvtType))
    {
        eStatus = PH_ERR_SUCCESS;
    }
    else
    {
        eStatus = PH_ERR_IO_TIMEOUT;
    }

    if(pRcvdEvt != NULL)
    {
        /* Return the Events That were Set */
        *pRcvdEvt = (phOsal_EventType_t)xbits;
    }

    return PH_ADD_COMPCODE(eStatus, PH_COMP_OSAL);
}

phStatus_t phOsal_Event_WaitAll(
                                phOsal_EventType_t eEvtType,
                                uint32_t dwTimeoutCount,
                                phOsal_EventType_t *pRcvdEvt
                                )
{
    phStatus_t eStatus = PH_ERR_OSAL_ERROR;
    EventBits_t xbits = 0;

    /* In case it is not initialized yet */
    if( gphOsal_EventGroup == NULL )
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);

    /* Only Possible from HAL */
    /* Don't Clear flags after ISR and Wait for All bits to be Set */
    xbits = xEventGroupWaitBits(gphOsal_EventGroup, eEvtType, pdFALSE, pdTRUE, dwTimeoutCount);
    if( (xbits & ((uint32_t)eEvtType)) == ((uint32_t)eEvtType))
    {
        eStatus = PH_ERR_SUCCESS;
    }
    else
    {
        eStatus = PH_ERR_IO_TIMEOUT;
    }

    if(pRcvdEvt != NULL)
    {
        /* Return the Events That were Set */
        *pRcvdEvt = (phOsal_EventType_t)xbits;
    }

    return PH_ADD_COMPCODE(eStatus, PH_COMP_OSAL);
}

void phOsal_Event_SetDetail(phOsal_EventDetail_t eEvtDetail)
{
    gphOsal_EvtDetail = eEvtDetail;
}


void phOsal_Event_GetDetail(phOsal_EventDetail_t * eEvent_Detail)
{
    *eEvent_Detail = gphOsal_EvtDetail;
}

phStatus_t phOsal_Event_Deinit(void)
{
    if( gphOsal_EventGroup == NULL )
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);

    /* Delete the Event Group */
    vEventGroupDelete(gphOsal_EventGroup);

    gphOsal_EvtLastSource = E_PH_OSAL_EVT_SRC_NONE;
    gphOsal_EvtLastDest = E_PH_OSAL_EVT_DEST_NONE;
    gphOsal_EvtDetail = E_PH_OSAL_EVT_DETAIL_SUCCESS;
    return PH_ERR_SUCCESS;
}

phOsal_SemaphoreHandle_t phOsal_Semaphore_Create(uint32_t dwMaxCount,
                                                uint32_t dwInitialCount
                                                )
{
    QueueHandle_t pSemaHandle = NULL;
    pSemaHandle = xSemaphoreCreateCounting(dwMaxCount, dwInitialCount);
    return (phOsal_SemaphoreHandle_t)pSemaHandle;
}


phStatus_t phOsal_Semaphore_Give(phOsal_SemaphoreHandle_t semHandle)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (semHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    if(0/*prvIsIrqContext()*/)
    {
        if(!(xSemaphoreGiveFromISR(semHandle, &xHigherPriorityTaskWoken)))
        {
            return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    else
    {
        if(!(xSemaphoreGive(semHandle)))
        {
            return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
        }
    }
    return PH_ERR_SUCCESS;
}


phStatus_t phOsal_Semaphore_Take(phOsal_SemaphoreHandle_t semHandle,
                                uint32_t dwBlockTime)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (semHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    if(0/*prvIsIrqContext()*/)
    {
        if(!(xSemaphoreTakeFromISR(semHandle, &xHigherPriorityTaskWoken)))
        {
            return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    else
    {
        if(!(xSemaphoreTake(semHandle, dwBlockTime)))
        {
            return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
        }
    }
    return PH_ERR_SUCCESS;
}


phStatus_t phOsal_Semaphore_Delete(phOsal_SemaphoreHandle_t semHandle)
{
    if (semHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }
    vSemaphoreDelete(semHandle);
    return PH_ERR_SUCCESS;
}

phOsal_MutexHandle_t phOsal_Mutex_Create(void)
{
    QueueHandle_t pMutexHandle = NULL;
    pMutexHandle = xSemaphoreCreateMutex();
    return (phOsal_SemaphoreHandle_t)pMutexHandle;
}

phStatus_t phOsal_Mutex_Take(phOsal_MutexHandle_t mutexHandle,
                             uint32_t dwBlockTime
                             )
{
    if (mutexHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }
    if(!(xSemaphoreTake(mutexHandle, dwBlockTime)))
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }
    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Mutex_Give(phOsal_MutexHandle_t mutexHandle)
{
    if (mutexHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }
    if(!(xSemaphoreGive(mutexHandle)))
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }
    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Mutex_Delete(phOsal_MutexHandle_t mutexHandle)
{
    if (mutexHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }
    vSemaphoreDelete(mutexHandle);
    return PH_ERR_SUCCESS;
}

phOsal_TimerHandle_t phOsal_Timer_Create(uint32_t dwTimerID,
                                         pphOsal_TimerCallback_t fpCallbackFunction
                                         )
{
    TimerHandle_t pTimerHandle = NULL;
    pTimerHandle = xTimerCreate("", 600, PH_OFF, (void *)dwTimerID, fpCallbackFunction);
    return (phOsal_TimerHandle_t)pTimerHandle;

}

phStatus_t phOsal_Timer_Start(phOsal_TimerHandle_t timerHandle,
                              uint32_t dwTimeMs
                              )
{
    if (timerHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    /* Block time is configured to 5 Ticks, if with in this time Timer Task is not scheduled then OSAL error is returned. */
    if(!xTimerChangePeriod(timerHandle, dwTimeMs, 0x05))
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    /* Block time is configured to 5 Ticks, if with in this time Timer Task is not scheduled then OSAL error is returned. */
    if(!xTimerStart(timerHandle, 0x05))
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }
    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Timer_Stop(phOsal_TimerHandle_t timerHandle)
{
    if (timerHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    /* Block time is configured to 5 Ticks, if with in this time Timer Task is not scheduled then OSAL error is returned. */
    if(!xTimerStop(timerHandle, 0x05))
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }
    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Timer_Reset(phOsal_TimerHandle_t timerHandle)
{
    if (timerHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    /* Block time is configured to 5 Ticks, if with in this time Timer Task is not scheduled then OSAL error is returned. */
    if(!xTimerReset(timerHandle, 0x05))
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }
    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Timer_Delete(phOsal_TimerHandle_t timerHandle)
{
    if (timerHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    /* Block time is configured to 5 Ticks, if with in this time Timer Task is not scheduled then OSAL error is returned. */
    if(!xTimerDelete(timerHandle, 0x05))
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }
    return PH_ERR_SUCCESS;
}

#endif  /* NXPBUILD__PH_OSAL_FREERTOS */
