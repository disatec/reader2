/*
 * phOsal_Posix.c
 *
 *  Created on: 13-01-2016
 *  Author: Donatien Garnier, AppNearMe Ltd
 */
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

#include <ph_Status.h>
#include <phOsal.h>

#ifdef NXPBUILD__PH_OSAL_POSIX

#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include "phOsal_Posix_Int.h"

/* -------------------------------------------------------------------------------------
 * RTOS SEMAPHORES RELATED API's
 * ------------------------------------------------------------------------------------- */

//Custom semaphore struct
struct nxp_sem
{
	sem_t inc; //"Main" semaphore
	sem_t dec; //Semaphore used to see if we ever go over the maximum number of resources allowed
};

//Custom timer struct
struct nxp_timer
{
	timer_t timer; //Posix timer
	pphOsal_TimerCallback_t cb; //Callback function
	uint64_t interval; //Store interval for use in reset function (usecs)
};


struct nxp_thread
{
    pthread_t thread;
    unsigned int eventMask;
    pthread_mutex_t mutex;
    phOsal_SemaphoreHandle_t sem;
    phOsal_EventDetail_t detail;
};

typedef struct nxp_sem nxp_sem_t;
typedef struct nxp_timer nxp_timer_t;
typedef struct nxp_thread nxp_thread_t;

static nxp_thread_t threadLut[E_PH_OSAL_EVT_DEST_NONE];

phStatus_t phOsal_Event_Init(void)
{
    /* phOsal_Event_Init is most properly called by the main thread - the LIB thread */
    phOsal_Posix_Thread_SetMainThread();
    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Event_GetEventHandle(void **pHandle)
{
    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Event_Post(
                       phOsal_EventType_t eEvtType,
                       phOsal_EventSource_t eSrc,
                       phOsal_EventDestination_t eDest
                       )
{
	int ret = 0;

    /* map DEST_LIB to DEST_HAL, as both are sharing the same thread */
    if (eDest == E_PH_OSAL_EVT_DEST_LIB)
    {
        eDest = E_PH_OSAL_EVT_DEST_HAL;
    }
    nxp_thread_t* pt = &threadLut[eDest];
	//Lock mutex
    ret = pthread_mutex_lock(&(pt->mutex));
    if( ret != 0 )
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    //Set event types
    pt->eventMask |= eEvtType;


    ret = pthread_mutex_unlock(&pt->mutex);
    if( ret != 0 )
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    phOsal_Semaphore_Give(pt->sem);
    return PH_ERR_SUCCESS;
}

static nxp_thread_t* GetCurrentThread()
{
    int i;
    pthread_t currentThread = pthread_self();

    for (i = 0; i < E_PH_OSAL_EVT_DEST_NONE; i++)
    {
        if (threadLut[i].thread == currentThread)
        {
            return &threadLut[i];
        }
    }

    return NULL;
}

void phOsal_Event_PostwithDetail(
                                 phOsal_EventType_t eEvtType,
                                 phOsal_EventSource_t eSrc,
                                 phOsal_EventDestination_t eDest,
                                 phOsal_EventDetail_t eDetail
                                 )
{

    /* map DEST_LIB to DEST_HAL, as both are sharing the same thread */
    if (eDest == E_PH_OSAL_EVT_DEST_LIB)
    {
        eDest = E_PH_OSAL_EVT_DEST_HAL;
    }
    (void)phOsal_Event_Post(eEvtType, eSrc, eDest);
    threadLut[eDest].detail = eDetail;
}

phStatus_t phOsal_Event_Consume(
                          phOsal_EventType_t eEvtType,
                          phOsal_EventSource_t eSrc
                          )
{
	int ret = 0;


    nxp_thread_t *currentThread = GetCurrentThread();

	//Lock mutex
    ret = pthread_mutex_lock(&(currentThread->mutex));
    if( ret != 0 )
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }
    currentThread->eventMask &= ~eEvtType;

	//Unlock mutex
    ret = pthread_mutex_unlock(&(currentThread->mutex));
    if( ret != 0 )
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    return PH_ERR_SUCCESS;
}

static phStatus_t phOsal_Event_Wait(
                                phOsal_EventType_t eEvtType,
                                uint32_t dwTimeoutCount,
                                phOsal_EventType_t *pRcvdEvt,
                                bool waitAll
                                )
{
    phStatus_t status;
    unsigned int mask;
    int ret;
    nxp_thread_t * currentThread = GetCurrentThread();
    
    if (NULL == currentThread)
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    do
    {
    
        ret = pthread_mutex_lock(&(currentThread->mutex));
        if( ret != 0 )
        {
            return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
        }
        mask = currentThread->eventMask;

        ret = pthread_mutex_unlock(&(currentThread->mutex));
        if( ret != 0 )
        {
            return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
        }

        if (waitAll)
        {
            if ((eEvtType & mask) == eEvtType)
            {
                break;
            }
        }
        else
        {
            if (eEvtType & mask)
            {
                break;
            }
        }
        status = phOsal_Semaphore_Take(currentThread->sem, dwTimeoutCount);
        if (PH_ERR_SUCCESS != status)
        {
            return PH_ADD_COMPCODE(PH_ERR_IO_TIMEOUT, PH_COMP_OSAL);
        }
    }
    while(true);
    if (pRcvdEvt)
        *pRcvdEvt = mask & eEvtType;
    return PH_ERR_SUCCESS;

}

phStatus_t phOsal_Event_WaitAny(
                                phOsal_EventType_t eEvtType,
                                uint32_t dwTimeoutCount,
                                phOsal_EventType_t *pRcvdEvt
                                )
{
    return phOsal_Event_Wait(eEvtType, dwTimeoutCount, pRcvdEvt, false);
}
phStatus_t phOsal_Event_WaitAll(
                                phOsal_EventType_t eEvtType,
                                uint32_t dwTimeoutCount,
                                phOsal_EventType_t *pRcvdEvt
                                )
{
    return phOsal_Event_Wait(eEvtType, dwTimeoutCount, pRcvdEvt, true);
}

void phOsal_Event_SetDetail(phOsal_EventDetail_t eEvtDetail)
{
}

void phOsal_Event_GetDetail(phOsal_EventDetail_t * eEvent_Detail)
{
    nxp_thread_t *currentThread = GetCurrentThread();

    if (currentThread != NULL)
    {
        *eEvent_Detail = currentThread->detail;
    }

}

phStatus_t phOsal_Event_Deinit(void)
{
}

phOsal_SemaphoreHandle_t phOsal_Semaphore_Create(uint32_t dwMaxCount,
                                                uint32_t dwInitialCount
                                                )
{
	nxp_sem_t* pSem = NULL;
	int ret = 0;

	//Allocate Semaphore
	pSem = malloc( sizeof(nxp_sem_t) );
	if( pSem == NULL )
	{
		//No memory left
		return NULL;
	}

	//Initialize semaphore with default parameters
	ret = sem_init( &pSem->inc, 0 /* only shared within process */, dwInitialCount /* initial value */ );
	if( ret != 0 ) //Check error code
	{
		free(pSem);

		return NULL;
	}

	ret = sem_init( &pSem->dec, 0 /* only shared within process */, dwMaxCount - dwInitialCount /* initial value */ );
	if( ret != 0 ) //Check error code
	{
		sem_destroy(&pSem->inc);

		free(pSem);

		return NULL;
	}

	return (phOsal_SemaphoreHandle_t) pSem;
}


phStatus_t phOsal_Semaphore_Give(phOsal_SemaphoreHandle_t semHandle)
{
	nxp_sem_t* pSem = (nxp_sem_t*)semHandle;
	int ret = 0;

    if (semHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    //Check if we have enough resources available (not to hit dwMaxCount)
    ret = sem_trywait(&pSem->dec);
    if( ret != 0 )
    {
    	//No more resources available
    	return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    //Give one resource
    ret = sem_post(&pSem->inc);
    if( ret != 0 ) //This will only happen if pSem is invalid or maximum semaphore value allowed by OS is hit (unlikely in normal conditions)
    {
    	//Free-up used resource
    	sem_post(&pSem->dec);

    	return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    return PH_ERR_SUCCESS;
}


phStatus_t phOsal_Semaphore_Take(phOsal_SemaphoreHandle_t semHandle,
                                uint32_t dwBlockTime)
{
	nxp_sem_t* pSem = (nxp_sem_t*)semHandle;
	struct timespec tspec = {};
	int ret = 0;

    if (semHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    //Get current time
	ret = clock_gettime(CLOCK_REALTIME, &tspec);
	if( ret != 0)
	{
		return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
	}

	//Add dwBlockTime (millisecs)
	tspec.tv_sec += (dwBlockTime / 1000);
	tspec.tv_nsec += (dwBlockTime % 1000) * 1000 * 1000;

	//Make sure tspec is valid
	tspec.tv_sec += tspec.tv_nsec / (1000 * 1000 * 1000);
	tspec.tv_nsec %= (1000 * 1000 * 1000);

    //Try to take semaphore
    ret = sem_timedwait(&pSem->inc, &tspec);
    if( ret != 0 )
    {
    	//Timeout
    	return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    //Give one resource
    ret = sem_post(&pSem->dec);
    if( ret != 0 ) //This will only happen if pSem is invalid or maximum semaphore value allowed by OS is hit (unlikely in normal conditions)
    {
    	//Free-up used resource
    	sem_post(&pSem->inc);

    	return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    return PH_ERR_SUCCESS;
}


phStatus_t phOsal_Semaphore_Delete(phOsal_SemaphoreHandle_t semHandle)
{
	nxp_sem_t* pSem = (nxp_sem_t*)semHandle;
	int ret = 0;

    if (semHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    ret = sem_destroy(&pSem->inc);
    if( ret != 0 )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    ret = sem_destroy(&pSem->dec);
    if( ret != 0 )
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    //Free memory
    free(pSem);

    return PH_ERR_SUCCESS;
}

phOsal_MutexHandle_t phOsal_Mutex_Create(void)
{
	pthread_mutex_t* pMutex = NULL;
	pthread_mutexattr_t attr;
	int ret = 0;

	//Allocate Mutex
	pMutex = malloc( sizeof(pthread_mutex_t) );
	if( pMutex == NULL )
	{
		//No memory left
		return NULL;
	}

	pthread_mutexattr_init(&attr);

	//We need to set this type to have proper error checking
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

	//Initialize mutex with default parameters
	ret = pthread_mutex_init( pMutex, &attr );
	if( ret != 0 ) //Check error code
	{
		free(pMutex);
		return NULL;
	}

    return (phOsal_MutexHandle_t) pMutex;
}

phStatus_t phOsal_Mutex_Take(phOsal_MutexHandle_t mutexHandle,
                             uint32_t dwBlockTime
                             )
{
	pthread_mutex_t* pMutex = (pthread_mutex_t*)mutexHandle;
	struct timespec tspec = {};
	int ret = 0;

    if (pMutex == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    //Get current time
    ret = clock_gettime(CLOCK_REALTIME, &tspec);
    if( ret != 0)
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    //Add dwBlockTime (millisecs)
    tspec.tv_sec += (dwBlockTime / 1000);
    tspec.tv_nsec += (dwBlockTime % 1000) * 1000 * 1000;

	//Make sure tspec is valid
	tspec.tv_sec += tspec.tv_nsec / (1000 * 1000 * 1000);
	tspec.tv_nsec %= (1000 * 1000 * 1000);

    ret = pthread_mutex_timedlock(pMutex, &tspec);
    if( ret != 0)
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }
    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Mutex_Give(phOsal_MutexHandle_t mutexHandle)
{
	pthread_mutex_t* pMutex = (pthread_mutex_t*)mutexHandle;
	int ret = 0;

    if (mutexHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    ret = pthread_mutex_unlock(pMutex);
    if( ret != 0 )
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }
    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Mutex_Delete(phOsal_MutexHandle_t mutexHandle)
{
	pthread_mutex_t* pMutex = (pthread_mutex_t*)mutexHandle;
	int ret = 0;

    if (mutexHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    //Delete mutex
    ret = pthread_mutex_destroy(pMutex);
    if( ret != 0 )
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    //Free memory
    free(pMutex);

    return PH_ERR_SUCCESS;
}

static void phOsal_Posix_Timer_Notify (union sigval sigval)
{
	nxp_timer_t* pTimer = (nxp_timer_t* )sigval.sival_ptr;
	pTimer->cb( (phOsal_TimerHandle_t)pTimer );
}

phOsal_TimerHandle_t phOsal_Timer_Create(uint32_t dwTimerID,
                                         pphOsal_TimerCallback_t fpCallbackFunction
                                         )
{
	nxp_timer_t* pTimer = NULL;
	int ret = 0;
	struct sigevent sev = {0};

	//Allocate Timer
	pTimer = malloc( sizeof(nxp_timer_t) );
	if( pTimer == NULL )
	{
		//No memory left
		return NULL;
	}

	//Store callback function
	pTimer->cb = fpCallbackFunction;

	//Build sigevent structure
	sev.sigev_notify = SIGEV_THREAD; //Start a new thread when expired
	sev.sigev_notify_attributes = NULL; //Default thread creation attributes
	sev.sigev_notify_function = phOsal_Posix_Timer_Notify;
	sev.sigev_value.sival_ptr = pTimer; //Use pTimer as handle

	//Initialize timer
	ret = timer_create( CLOCK_REALTIME, &sev, &pTimer->timer );
	if( ret != 0 ) //Check error code
	{
		free(pTimer);
		return NULL;
	}

    return (phOsal_TimerHandle_t) pTimer;
}

phStatus_t phOsal_Timer_Start(phOsal_TimerHandle_t timerHandle,
                              uint32_t dwTimeMs
                              )
{
	return phOsal_Timer_Posix_Int_StartUs(timerHandle, dwTimeMs * 1000);
}

phStatus_t phOsal_Timer_Stop(phOsal_TimerHandle_t timerHandle)
{
	nxp_timer_t* pTimer = (nxp_timer_t*)timerHandle;
	int ret = 0;
	struct itimerspec spec = {0};

    if (timerHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    //Disarm timer (all fields in spec are 0'ed)
    ret = timer_settime(pTimer->timer, 0 /* relative time */, &spec, NULL);
    if( ret != 0 )
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Timer_Reset(phOsal_TimerHandle_t timerHandle)
{
	nxp_timer_t* pTimer = (nxp_timer_t*)timerHandle;

    if (timerHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    //Re-use stored interval
    return phOsal_Timer_Posix_Int_StartUs(timerHandle, pTimer->interval);
}

phStatus_t phOsal_Timer_Delete(phOsal_TimerHandle_t timerHandle)
{
	nxp_timer_t* pTimer = (nxp_timer_t*)timerHandle;
	int ret = 0;

    if (timerHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    //Delete timer
    ret = timer_delete(pTimer->timer);
    if( ret != 0 )
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    //Free memory
    free(pTimer);

    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Timer_Posix_Int_StartUs(phOsal_TimerHandle_t timerHandle,
                              uint64_t dwTimeUs
                              )
{
	nxp_timer_t* pTimer = (nxp_timer_t*)timerHandle;
	int ret = 0;
	struct itimerspec spec = {0};

    if (timerHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    //Save value for use by reset
    pTimer->interval = dwTimeUs;

    //Set initial timeout and interval
    spec.it_value.tv_sec = (dwTimeUs / 1000000);
    spec.it_value.tv_nsec += (dwTimeUs % 1000000) * 1000;

    spec.it_interval.tv_sec = spec.it_value.tv_sec;
    spec.it_interval.tv_nsec += spec.it_value.tv_nsec;

    ret = timer_settime(pTimer->timer, 0 /* relative time */, &spec, NULL);
    if( ret != 0 )
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Timer_Posix_Int_Get_ElapsedUs(phOsal_TimerHandle_t timerHandle,
        uint64_t* dwTimeUs
                              )
{
	nxp_timer_t* pTimer = (nxp_timer_t*)timerHandle;
	struct itimerspec spec = {0};
	int ret = 0;

    if (timerHandle == NULL)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_OSAL);
    }

    //Get value
    ret = timer_gettime(pTimer->timer, &spec);
    if( ret != 0 )
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    //it_value contains remaining time
    *dwTimeUs = (spec.it_interval.tv_sec * 1000 * 1000 + spec.it_interval.tv_nsec / 1000) - (spec.it_value.tv_sec * 1000 * 1000 + spec.it_value.tv_nsec / 1000);

    return PH_ERR_SUCCESS;
}


phStatus_t phOsal_Posix_Thread_SetMainThread()
{
    int rc;
    threadLut[E_PH_OSAL_EVT_DEST_HAL].sem = phOsal_Semaphore_Create(255, 0);

    if (NULL == threadLut[E_PH_OSAL_EVT_DEST_HAL].sem)
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }
    
    rc = pthread_mutex_init(&(threadLut[E_PH_OSAL_EVT_DEST_HAL].mutex), NULL);
    if(rc != 0)
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    threadLut[E_PH_OSAL_EVT_DEST_HAL].thread = pthread_self();
    threadLut[E_PH_OSAL_EVT_DEST_HAL].eventMask = 0;

    return PH_ERR_SUCCESS;
}

phStatus_t phOsal_Posix_Thread_Create(phOsal_EventDestination_t threadName, void *(*start_routine) (void *), void *arg)
{
    int rc;

    if (threadName >= E_PH_OSAL_EVT_DEST_NONE)
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    /* map DEST_LIB to DEST_HAL, as both are sharing the same thread */
    if (threadName == E_PH_OSAL_EVT_DEST_LIB)
    {
        threadName = E_PH_OSAL_EVT_DEST_HAL;
    }
    threadLut[threadName].sem = phOsal_Semaphore_Create(255, 0);

    if (NULL == threadLut[threadName].sem)
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }
    
    rc = pthread_mutex_init(&(threadLut[threadName].mutex), NULL);
    if(rc != 0)
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    rc = pthread_create(&(threadLut[threadName].thread), NULL, start_routine, arg);

    if (0 != rc)
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    threadLut[threadName].eventMask = 0;
    return PH_ERR_SUCCESS;

}
void phOsal_Posix_Thread_Join(phOsal_EventDestination_t t, void** returnCode)
{
    pthread_join(threadLut[t].thread, returnCode);
}
phStatus_t phOsal_Posix_Thread_Create_Extra(pthread_t *thread, void *(*start_routine) (void *),
        void *arg)
{
    int rc;

    if (!thread)
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    rc = pthread_create(thread, NULL, start_routine, arg);

    if (0 != rc)
    {
        return PH_ADD_COMPCODE(PH_ERR_OSAL_ERROR, PH_COMP_OSAL);
    }

    return PH_ERR_SUCCESS;
}
void phOsal_Posix_Thread_Join_Extra(pthread_t *thread, void** returnCode)
{
    pthread_join(*thread, returnCode);
}
#endif  /* NXPBUILD__PH_OSAL_POSIX */


