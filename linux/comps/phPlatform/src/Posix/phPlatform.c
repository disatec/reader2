#include <ph_Status.h>
#include <ph_NxpBuild.h>

#ifdef NXPBUILD__PH_PLATFORM

#include <phPlatform.h>
#include <phOsal.h>
#include <phOsal_Posix_Int.h>

#include <stdint.h>
#include <unistd.h>

static void timer_callback(phOsal_TimerHandle_t xTimerHandle);
static phPlatform_DataParams_t* _pParams;

phStatus_t phPlatform_Init(phPlatform_DataParams_t  *pDataParams )
{
	uint32_t i = 0;

	_pParams = pDataParams;

    pDataParams->wId = PH_COMP_PLATFORM;
    for(i = 0; i < PLATFORM_MAX_TIMERS; i++)
    {
    	pDataParams->gTimers[i].bTimerFree = true;
    	pDataParams->gTimers[i].dwTimerId = i;
    	pDataParams->gTimers[i].pApplicationCallback = NULL;
    	pDataParams->gTimers[i].pContext = NULL;
    }
    return PH_ERR_SUCCESS;
}

phStatus_t phPlatform_Timer_Init(phPlatform_DataParams_t  *pDataParams )
{
	//Nothing to do
    return PH_ERR_SUCCESS;
}

phStatus_t phPlatform_Timer_Create(phPlatform_DataParams_t  *pDataParams,
                                   uint32_t                 *pTimerId
                                   )
{
	//Find free id
    for(*pTimerId = 0; *pTimerId  < PLATFORM_MAX_TIMERS; *pTimerId ++)
    {
    	if(pDataParams->gTimers[*pTimerId].bTimerFree)
    	{
    		break;
    	}
    }

    if (*pTimerId >= PLATFORM_MAX_TIMERS )
    {
        /* No timer found, need to return error */
        return PH_ADD_COMPCODE(PH_PLATFORM_ERR_NO_FREE_TIMER,  PH_COMP_PLATFORM);
    }
    else
    {
    	//Create and init timer
    	pDataParams->gTimers[*pTimerId].osalHdle = phOsal_Timer_Create(*pTimerId, timer_callback);

    	if( pDataParams->gTimers[*pTimerId].osalHdle == NULL ) //Error
    	{
            return PH_ADD_COMPCODE(PH_PLATFORM_ERR_NO_FREE_TIMER,  PH_COMP_PLATFORM);
    	}

    	pDataParams->gTimers[*pTimerId].bTimerFree = false;

        return PH_ERR_SUCCESS;
    }
}

phStatus_t phPlatform_Timer_Reset(phPlatform_DataParams_t  *pDataParams,
                                  uint32_t                  dwTimerId
                                  )
{
    return phOsal_Timer_Reset(pDataParams->gTimers[dwTimerId].osalHdle);
}

phStatus_t phPlatform_Timer_Wait(phPlatform_DataParams_t  *pDataParams,
                                 uint8_t                   bTimerDelayUnit,
                                 uint16_t                  wDelay
                                 )
{
	uint32_t delay = wDelay;
	if (bTimerDelayUnit == PH_PLATFORM_TIMER_UNIT_MS)
	{
		usleep( delay * 1000 );
	}
	else
	{
		usleep( delay );
	}

	return PH_ERR_SUCCESS;
}

phStatus_t phPlatform_Timer_Start(phPlatform_DataParams_t  *pDataParams,
                                  uint32_t                  dwTimerId,
                                  uint32_t                  dwRegTimeCnt,
                                  uint16_t                  wOption,
                                  ppCallBck_t               pApplicationCallback,
                                  void                     *pContext
                                  )
{
    phStatus_t status;

    //Do this first to avoid concurrent access
    pDataParams->gTimers[dwTimerId].pApplicationCallback = pApplicationCallback;
    pDataParams->gTimers[dwTimerId].pContext = pContext;

    if (wOption == PH_PLATFORM_TIMER_UNIT_MS)
    {
    	status = phOsal_Timer_Start(pDataParams->gTimers[dwTimerId].osalHdle, dwRegTimeCnt);
    }
    else
    {
    	status = phOsal_Timer_Posix_Int_StartUs(pDataParams->gTimers[dwTimerId].osalHdle, dwRegTimeCnt);
    }

    if( status != PH_ERR_SUCCESS )
    {
        pDataParams->gTimers[dwTimerId].pApplicationCallback = NULL;
        pDataParams->gTimers[dwTimerId].pContext = NULL;

        return PH_ADD_COMPCODE(PH_PLATFORM_ERR_INVALID_TIMER, PH_COMP_PLATFORM);
    }

    return PH_ERR_SUCCESS;
}

phStatus_t phPlatform_Timer_GetElapsedDelay(phPlatform_DataParams_t  *pDataParams,
                                            uint32_t                  dwTimerId,
                                            uint16_t                  wOption,
                                            uint32_t                  *dwGetElapsedDelay
                                            )
{
    phStatus_t status;
    uint64_t elapsed;
    status = phOsal_Timer_Posix_Int_Get_ElapsedUs(pDataParams->gTimers[dwTimerId].osalHdle, &elapsed);

    if (status == PH_ERR_SUCCESS)
    {
        if (wOption == PH_PLATFORM_TIMER_UNIT_MS)
        {
        	*dwGetElapsedDelay = (uint32_t)(elapsed / 1000);
        }
        else
        {
        	*dwGetElapsedDelay = (uint32_t)elapsed;
        }
        return PH_ERR_SUCCESS;
    }
    else
    {
        return PH_ADD_COMPCODE(PH_PLATFORM_ERR_INVALID_TIMER, PH_COMP_PLATFORM);
    }
}

phStatus_t phPlatform_Timer_Delete(phPlatform_DataParams_t  *pDataParams,
                                   uint32_t                  dwTimerId
                                   )
{
    phStatus_t status;
    status = phOsal_Timer_Delete(pDataParams->gTimers[dwTimerId].osalHdle);
    if (status == PH_ERR_SUCCESS)
    {
    	pDataParams->gTimers[dwTimerId].bTimerFree = true;
    	pDataParams->gTimers[dwTimerId].pApplicationCallback = NULL;
    	pDataParams->gTimers[dwTimerId].pContext = NULL;
        return PH_ERR_SUCCESS;
    }
    else
    {
        return PH_ADD_COMPCODE(PH_PLATFORM_ERR_INVALID_TIMER, PH_COMP_PLATFORM);
    }
}

phStatus_t phPlatform_Timer_Stop(phPlatform_DataParams_t  *pDataParams,
                                 uint32_t                  dwTimerId
                                 )
{
    return phOsal_Timer_Stop(pDataParams->gTimers[dwTimerId].osalHdle);
}

void timer_callback(phOsal_TimerHandle_t xTimerHandle)
{
	int i = 0;
    for(i = 0; i < PLATFORM_MAX_TIMERS; i++)
    {
    	if( !_pParams->gTimers[i].bTimerFree
    			&& (_pParams->gTimers[i].pApplicationCallback != NULL)
    			&& (_pParams->gTimers[i].osalHdle == xTimerHandle)
    	)
    	{
    		_pParams->gTimers[i].pApplicationCallback(_pParams->gTimers[i].pContext);
    		return;
    	}
    }
}

#endif /* NXPBUILD__PH_PLATFORM */
/******************************************************************************
**                            End Of File
******************************************************************************/
