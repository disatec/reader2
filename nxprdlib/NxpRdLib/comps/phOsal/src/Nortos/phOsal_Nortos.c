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
* Freertos OSAL Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 3935 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
*/
#include <ph_Status.h>
#include <phOsal.h>

#ifdef NXPBUILD__PH_OSAL_NORTOS

/* *****************************************************************************************************************
* Global and Static Variables
* Total Size: NNNbytes
* ***************************************************************************************************************** */

/**
* Global Status of ISR Event Register Status to Receive Events from ISR
*/
static volatile phOsal_EventType_t gphOsal_EventISR = E_PH_OSAL_EVT_WAITING;

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
    gphOsal_EventISR = E_PH_OSAL_EVT_WAITING;
    gphOsal_EvtLastSource = E_PH_OSAL_EVT_SRC_NONE;
    gphOsal_EvtLastDest = E_PH_OSAL_EVT_DEST_NONE;
    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_OSAL);
}

phStatus_t phOsal_Event_GetEventHandle(void **pHandle)
{
    if(pHandle == NULL)
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);

    *pHandle = (void *)(&gphOsal_EventISR);
    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_OSAL);
}

phStatus_t phOsal_Event_Post(
                             phOsal_EventType_t eEvtType,
                             phOsal_EventSource_t eSrc,
                             phOsal_EventDestination_t eDest
                             )
{
    /* Initiate the Event */
    gphOsal_EventISR = (phOsal_EventType_t)(((uint32_t)eEvtType) | ((uint32_t)gphOsal_EventISR));

    /* Set the Last Source */
    gphOsal_EvtLastSource = eSrc;

    /* Set the Last Destination */
    gphOsal_EvtLastDest = eDest;

    /* Set Default Event Detail */
    gphOsal_EvtDetail = E_PH_OSAL_EVT_DETAIL_SUCCESS;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_OSAL);
}

void phOsal_Event_PostwithDetail(
                                 phOsal_EventType_t eEvtType,
                                 phOsal_EventSource_t eSrc,
                                 phOsal_EventDestination_t eDest,
                                 phOsal_EventDetail_t eDetail
                                 )
{
    phOsal_Event_Post(eEvtType, eSrc, eDest);
    phOsal_Event_SetDetail(eDetail);
}


phStatus_t phOsal_Event_Consume(
                                phOsal_EventType_t eEvtType,
                                phOsal_EventSource_t eSrc
                                )
{
    /* Consume the Event */
    gphOsal_EventISR &= (phOsal_EventType_t) ((uint32_t)~(uint32_t)eEvtType);

    /* Set the Last Source */
    gphOsal_EvtLastSource = eSrc;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_OSAL);
}

phStatus_t phOsal_Event_WaitAny(
                                phOsal_EventType_t eEvtType,
                                uint32_t dwTimeoutCount,
                                phOsal_EventType_t *pRcvdEvt
                                )
{
    uint32_t dwtmp = 0;
    if(eEvtType > E_PH_OSAL_EVT_TYPE_ALL)
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);

    do{
        --dwTimeoutCount;
        /* Delay of 1ms */
        for (dwtmp = 0x2800; dwtmp > 0; dwtmp --);
    }while(((gphOsal_EventISR & eEvtType) == E_PH_OSAL_EVT_WAITING) && (dwTimeoutCount > 0));

    if(pRcvdEvt != NULL)
    {
        /* Return the Events That were Set */
        *pRcvdEvt = gphOsal_EventISR;
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_OSAL);
}

phStatus_t phOsal_Event_WaitAll(
                                phOsal_EventType_t eEvtType,
                                uint32_t dwTimeoutCount,
                                phOsal_EventType_t *pRcvdEvt
                                )
{
    if(eEvtType > E_PH_OSAL_EVT_TYPE_ALL)
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);

    while((gphOsal_EventISR & eEvtType) != eEvtType);

    if(pRcvdEvt != NULL)
    {
        /* Return the Events That were Set */
        *pRcvdEvt = gphOsal_EventISR;
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_OSAL);
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
    gphOsal_EventISR = E_PH_OSAL_EVT_WAITING;
    gphOsal_EvtLastSource = E_PH_OSAL_EVT_SRC_NONE;
    gphOsal_EvtLastDest = E_PH_OSAL_EVT_DEST_NONE;
    gphOsal_EvtDetail = E_PH_OSAL_EVT_DETAIL_SUCCESS;
    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_OSAL);
}

phOsal_SemaphoreHandle_t phOsal_Semaphore_Create(uint32_t dwMaxCount,
                                                uint32_t dwInitialCount
                                                )
{
    phOsal_SemaphoreHandle_t pSemaHandle = NULL;
    return pSemaHandle;
}

phStatus_t phOsal_Semaphore_Give(phOsal_SemaphoreHandle_t semHandle)
{
    return PH_ERR_SUCCESS;
}


phStatus_t phOsal_Semaphore_Take(phOsal_SemaphoreHandle_t semHandle,
                                uint32_t dwBlockTime)
{
    return PH_ERR_SUCCESS;
}


phStatus_t phOsal_Semaphore_Delete(phOsal_SemaphoreHandle_t semHandle)
{
    return PH_ERR_SUCCESS;
}

phOsal_MutexHandle_t phOsal_Mutex_Create(void)
{
    phOsal_SemaphoreHandle_t pMutexHandle = NULL;
    return pMutexHandle;
}

phStatus_t phOsal_Mutex_Take(phOsal_MutexHandle_t mutexHandle,
                             uint32_t dwBlockTime
                             )
{
    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Mutex_Give(phOsal_MutexHandle_t mutexHandle)
{
    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Mutex_Delete(phOsal_MutexHandle_t mutexHandle)
{
    return PH_ERR_SUCCESS;
}

phOsal_TimerHandle_t phOsal_Timer_Create(uint32_t dwTimerID,
                                         pphOsal_TimerCallback_t fpCallbackFunction
                                         )
{
    phOsal_TimerHandle_t pTimerHandle = NULL;
    return pTimerHandle;
}

phStatus_t phOsal_Timer_Start(phOsal_TimerHandle_t timerHandle,
                              uint32_t dwTimeMs
                              )
{
    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Timer_Stop(phOsal_TimerHandle_t timerHandle)
{
    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Timer_Reset(phOsal_TimerHandle_t timerHandle)
{
    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Timer_Delete(phOsal_TimerHandle_t timerHandle)
{
    return PH_ERR_SUCCESS;
}

#endif /* NXPBUILD__PH_OSAL_NORTOS */
