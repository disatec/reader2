/*
*                    Copyright (c), NXP Semiconductors
*
*                       (C) NXP Semiconductors 2014
*
*         All rights are reserved. Reproduction in whole or in part is
*        prohibited without the written consent of the copyright owner.
*    NXP reserves the right to make changes without notice at any time.
*   NXP makes no warranty, expressed, implied or statutory, including but
*   not limited to any implied warranty of merchantability or fitness for any
*  particular purpose, or that the use will not infringe any third party patent,
*   copyright or trademark. NXP must not be liable for any loss or damage
*                            arising from its use.
*/

/** @file
*
* phTools_Q.c:  <The purpose and scope of this file>
*
* Project:  NxpNfcRdLib
*
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 3725 ))
*/


/* *****************************************************************************************************************
* Includes
* ***************************************************************************************************************** */

#include "ph_NxpBuild.h"
#include <ph_Status.h>
#include <phTools.h>

#include <phOsal.h>

/* *****************************************************************************************************************
* Global and Static Variables
* Total Size: NNNbytes
* ***************************************************************************************************************** */
static phOsal_MutexHandle_t gphTools_Q_Mutex;
static phOsal_MutexHandle_t gphTools_Q_MsgMutex;
static phOsal_SemaphoreHandle_t gphTools_Q_MsgSema;

static phTools_Q_t gsphTools_Q_Pool[PH_TOOLS_Q_MAX_ELEMENTS];
static phTools_Q_t *gpphTools_Q_PoolHead;
static phTools_Q_t *gpphTools_Q_Msgs;
static uint8_t     bQCount;

/* *****************************************************************************************************************
* Private Functions
* ***************************************************************************************************************** */
static phStatus_t phTools_Q_SendInt(phTools_Q_t *psMsgQueue, uint8_t bFront, uint32_t dwBlockTime, uint16_t wFrameOpt);
static phTools_Q_t* phTools_Q_PopInt(phTools_Q_t **ppQHead, phOsal_MutexHandle_t pMutex, uint32_t dwBlockTime, uint8_t bPriority);
static void phTools_Q_Push(phTools_Q_t **ppsQHead, phTools_Q_t *psMsgQueue, uint8_t bFront);

/* *****************************************************************************************************************
* Public Functions
* ***************************************************************************************************************** */

phStatus_t phTools_Q_Init(void)
{
    uint8_t count;
    phTools_Q_t *psQueue;

    bQCount = 0;
    gpphTools_Q_Msgs = NULL;
    gpphTools_Q_PoolHead = gsphTools_Q_Pool;
    psQueue = gsphTools_Q_Pool;

    gphTools_Q_MsgSema = phOsal_Semaphore_Create(PH_TOOLS_Q_MAX_ELEMENTS, 0);
    if(gphTools_Q_MsgSema == NULL)
    {
        return (PH_ERR_RESOURCE_ERROR | PH_COMP_TOOLS);
    }
    gphTools_Q_Mutex = phOsal_Mutex_Create();
    if(gphTools_Q_Mutex == NULL)
    {
        return (PH_ERR_RESOURCE_ERROR | PH_COMP_TOOLS);
    }
    gphTools_Q_MsgMutex = phOsal_Mutex_Create();
    if(gphTools_Q_MsgMutex == NULL)
    {
        return (PH_ERR_RESOURCE_ERROR | PH_COMP_TOOLS);
    }

    for(count = 1; count < PH_TOOLS_Q_MAX_ELEMENTS; count++)
    {
        psQueue->pNext = &gsphTools_Q_Pool[count];
        psQueue = psQueue->pNext;
    }
    psQueue->pNext = NULL;

    return PH_ERR_SUCCESS;
}

phTools_Q_t *phTools_Q_Receive(uint32_t dwBlockTime)
{
    /* wait for incoming messages */
    if(phOsal_Semaphore_Take(gphTools_Q_MsgSema, dwBlockTime))
    {
        return NULL;
    }

    return phTools_Q_PopInt(&gpphTools_Q_Msgs, gphTools_Q_MsgMutex, dwBlockTime, PH_ON);
}

phTools_Q_t *phTools_Q_Get(uint32_t dwBlockTime, uint8_t bPriority)
{
    phTools_Q_t *pEmptyMsg = phTools_Q_PopInt(&gpphTools_Q_PoolHead, gphTools_Q_Mutex, dwBlockTime, bPriority);

    if(pEmptyMsg == NULL)
    {
        return pEmptyMsg;
    }

    pEmptyMsg->pNext = NULL;

    return pEmptyMsg;
}

phTools_Q_t *phTools_Q_PopInt(phTools_Q_t **ppQHead, phOsal_MutexHandle_t pMutex, uint32_t dwBlockTime, uint8_t bPriority)
{
    void *pQueue;

    /* Shared memory pool, Mutex protected. */
    if(phOsal_Mutex_Take(pMutex, dwBlockTime))
    {
        return NULL;
    }

    if((bPriority == PH_OFF) && (bQCount >= (PH_TOOLS_Q_MAX_ELEMENTS - PH_TOOLS_Q_RESERVED_ELEMENTS)) && (pMutex == gphTools_Q_Mutex))
    {
        return NULL;
    }

    /* Get a msg Queue from the pool */
    if(*ppQHead == NULL)
    {
        return NULL;
    }
    pQueue = *ppQHead;
    *ppQHead = (*ppQHead)->pNext;

    if(pMutex == gphTools_Q_Mutex)
    {
        bQCount++;
    }
    (void)phOsal_Mutex_Give(pMutex);
    return pQueue;
}

phStatus_t phTools_Q_Release(phTools_Q_t *psMsgQueue, uint32_t dwBlockTime)
{
    phStatus_t wStatus;
    psMsgQueue->pNext = NULL;
    psMsgQueue->bType = 0;

    /* Shared memory pool, Mutex protected. */
    if(phOsal_Mutex_Take(gphTools_Q_Mutex, dwBlockTime))
    {
        return (PH_ERR_RESOURCE_ERROR | PH_COMP_TOOLS);
    }

    if(gpphTools_Q_PoolHead == NULL)
    {
        gpphTools_Q_PoolHead = psMsgQueue;
        gpphTools_Q_PoolHead->pNext = NULL;
        return PH_ERR_SUCCESS;
    }

    psMsgQueue->pNext = gpphTools_Q_PoolHead;
    gpphTools_Q_PoolHead = psMsgQueue;
    bQCount--;
    PH_CHECK_SUCCESS_FCT(wStatus, phOsal_Mutex_Give(gphTools_Q_Mutex));

    return PH_ERR_SUCCESS;
}

phStatus_t phTools_Q_SendInt(phTools_Q_t *psMsgQueue, uint8_t bFront, uint32_t dwBlockTime, uint16_t wFrameOpt)
{
    phStatus_t wStatus;
    static uint8_t bNoOfMsgs = 0;
    uint8_t i;

    /* Shared memory pool, Mutex  protected until all the fragments are queued. */
    if((wFrameOpt == PH_TRANSMIT_BUFFER_FIRST) || (wFrameOpt == PH_TRANSMIT_DEFAULT))
    {
        bNoOfMsgs = 1;
        if(phOsal_Mutex_Take(gphTools_Q_MsgMutex, dwBlockTime))
        {
            return (PH_ERR_RESOURCE_ERROR | PH_COMP_TOOLS);
        }
    }
    else
    {
        bNoOfMsgs++;
    }

    phTools_Q_Push(&gpphTools_Q_Msgs, psMsgQueue, bFront);

    /* Check for the last fragment to be queued, post the semaphore. */
    if((wFrameOpt == PH_TRANSMIT_BUFFER_LAST) || (wFrameOpt == PH_TRANSMIT_DEFAULT))
    {
        /* TODO: chk for posting single semaphore instead multi No of Semaphore. */
        for(i=0; i<bNoOfMsgs;i++)
        {
            PH_CHECK_SUCCESS_FCT(wStatus, phOsal_Semaphore_Give(gphTools_Q_MsgSema));
        }
        PH_CHECK_SUCCESS_FCT(wStatus, phOsal_Mutex_Give(gphTools_Q_MsgMutex));
    }

    return PH_ERR_SUCCESS;
}

phStatus_t phTools_Q_Send(
                          phTools_Q_t* psMsgQueue,
                          uint32_t dwBlockTime,
                          uint16_t wFrameOpt
                          )
{
    return phTools_Q_SendInt(psMsgQueue, false, dwBlockTime, wFrameOpt);
}

phStatus_t phTools_Q_SendFront(
                               phTools_Q_t* psMsgQueue,
                               uint32_t dwBlockTime,
                               uint16_t wFrameOpt
                               )
{
    return phTools_Q_SendInt(psMsgQueue, true, dwBlockTime, wFrameOpt);
}

void phTools_Q_Push(phTools_Q_t **ppsQHead, phTools_Q_t *psMsgQueue, uint8_t bFront)
{
    phTools_Q_t *psQList;

    psMsgQueue->pNext = NULL;

    if(*ppsQHead == NULL)
    {
        *ppsQHead = psMsgQueue;
        return;
    }

    /* Queue to the head */
    if(bFront == true)
    {
        psMsgQueue->pNext = *ppsQHead;
        *ppsQHead = psMsgQueue;
    }
    else
    {
        /* Queue to the tail */
        psQList = *ppsQHead;
        while(psQList->pNext != NULL)
        {
            psQList = psQList->pNext;
        }
        psQList->pNext = psMsgQueue;
    }

    return;
}

void phTools_Q_DeInit(void)
{
    gpphTools_Q_PoolHead = NULL;
    bQCount = 0;

    /* Release the acquired memory. */
    if (gphTools_Q_Mutex != NULL)
    {
        (void)phOsal_Mutex_Delete(gphTools_Q_Mutex);
        gphTools_Q_Mutex = NULL;
    }
    if (gphTools_Q_MsgMutex != NULL)
    {
        (void)phOsal_Mutex_Delete(gphTools_Q_MsgMutex);
        gphTools_Q_MsgMutex = NULL;
    }
    if (gphTools_Q_MsgSema != NULL)
    {
        (void)phOsal_Semaphore_Delete(gphTools_Q_MsgSema);
        gphTools_Q_MsgSema = NULL;
    }
}
