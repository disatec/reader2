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
* Timer of MicroController Platform.
* $Author: Kiran Kumar M N (nxp69453) $
* $Revision: 2879 $
* $Date: 2015-07-30 09:07:43 +0200 (Do, 30 Jul 2015) $
*
*/

#ifndef PHPLATFORM_H
#define PHPLATFORM_H

#include <ph_Status.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern "C" {
#endif    /* __cplusplus */

#ifdef NXPBUILD__PH_PLATFORM

#include <phOsal.h>

/** \defgroup phPlatform Component : Stub
* \brief These Components implement the Osal commands.
* @{
*/
#define PLATFORM_MAX_TIMERS             10U         /**< Maximum number of timers available */

/**
*
* \brief Timer callback interface which will be called once registered timer
* timeout expires.
* \param[in] TimerId  Timer Id for which callback is called.
* \retval  None
*/
typedef void (*ppCallBck_t)(void *pContext);

typedef struct Timer_Struct
{
    uint32_t       dwTimerId;                             /**< ID of the timer */
    bool           bTimerFree;                            /**< Indicates whether the current timer is free or used */
    ppCallBck_t    pApplicationCallback;                  /**< The call back function for this timer */
    void          *pContext;                              /**< The argument to the call back function */
    phOsal_TimerHandle_t osalHdle; 							/**< The handle returned from OSAL */
} Timer_Struct_t;

/**
* \brief OSAL parameter structure
*/
typedef struct
{
    uint16_t           wId;                               /**< ID of this component, do not modify */
    Timer_Struct_t     gTimers[PLATFORM_MAX_TIMERS];      /**< Timer structure */
} phPlatform_DataParams_t;

/**
* Initializes the Stub timer component
*
* \return status code
* \retval #PH_ERR_SUCCESS Operation successful.
*
*/
phStatus_t phPlatform_Init(phPlatform_DataParams_t  *pDataParams);    /**<[In] Data parameters representing this component */
/** @} */

/** \defgroup phOsal Operating System Abstraction Layer
* \brief These Components implement the Osal commands.
* @{
*/

#define PH_PLATFORM_TIMER_UNIT_US         0x00U                       /**< Indicates that the specified delay is in microseconds.*/
#define PH_PLATFORM_TIMER_UNIT_MS         0x01U                       /**< Indicates that the specified delay is in milliseconds. */
#define PH_PLATFORM_INVALID_TIMER_ID      0xFFFFFFFF                  /**< Invalid Timer Id. */

/**
* \name Platform error codes
*/
/*@{*/

#define PH_PLATFORM_ERR_NO_FREE_TIMER     (PH_ERR_CUSTOM_BEGIN + 0)   /**< Error condition indicating that no timer is available for allocation. */
#define PH_PLATFORM_ERR_INVALID_TIMER     (PH_ERR_CUSTOM_BEGIN + 1)   /**< Indicates that the timer ID that was supplied was invalid. */

/*@}*/

/* *****************************************************************************************************************
* Function Prototypes
* ***************************************************************************************************************** */
/**
* \brief Initialize platform timers.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
*/
phStatus_t phPlatform_Timer_Init(phPlatform_DataParams_t  *pDataParams);                       /**< [In] Pointer to this layers parameter structure. */


/**
* \brief Allocates a free timer.
* Note:
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval #PH_PLATFORM_ERR_NO_FREE_TIMER No Free Timer available to create.
*/
phStatus_t phPlatform_Timer_Create(phPlatform_DataParams_t   *pDataParams,                     /**< [In] Pointer to this layers parameter structure. */
                                   uint32_t                  *timerId                          /**< [Out] Timer Id */
                                   );

/**
* \brief Start the timer.
* Note: Valid Timer ID should be provided. Timer ID is provided by calling \ref phPlatform_Timer_Create function.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval #PH_ERR_INVALID_PARAMETER Invalid input parameter.
* \retval #PH_PLATFORM_ERR_INVALID_TIMER Provided Timer ID is not created.
*/
phStatus_t phPlatform_Timer_Start(phPlatform_DataParams_t   *pDataParams,                      /**< [In] Pointer to this layers parameter structure. */
                                  uint32_t                   dwTimerId,                        /**< [In] Timer Id */
                                  uint32_t                   dwRegTimeCnt,                     /**< [In] Time delay count */
                                  uint16_t                   wOption,                          /**< [In] Option parameter to mention delay in Units
                                                                                                         (either #PH_PLATFORM_TIMER_UNIT_US or #PH_PLATFORM_TIMER_UNIT_MS). */
                                  ppCallBck_t                pApplication_callback,            /**< [In] Callback to be called on time out */
                                  void                      *pContext                          /**< [In] Callback function context */
                                  );

/**
* \brief Get Elapsed Delay from timer which is created and started before calling this API.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval #PH_ERR_INVALID_PARAMETER Invalid input parameter.
* \retval #PH_PLATFORM_ERR_INVALID_TIMER Provided Timer ID is not created.
*/
phStatus_t phPlatform_Timer_GetElapsedDelay(phPlatform_DataParams_t  *pDataParams,              /**< [In] Pointer to this layers parameter structure. */
                                            uint32_t                  dwTimerId,                /**< [In] Timer Id */
                                            uint16_t                  wOption,                  /**< [In] Option parameter to mention delay in Units
                                                                                                          (either #PH_PLATFORM_TIMER_UNIT_US or #PH_PLATFORM_TIMER_UNIT_MS). */
                                            uint32_t                  *dwGetElapsedDelay        /**< [Out] Measured Delay after Timer Start in milli/micro seconds based on input option. */
                                            );
/**
* \brief Stop the timer.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phPlatform_Timer_Stop(phPlatform_DataParams_t   *pDataParams,                       /**< [In] Pointer to this layers parameter structure. */
                                 uint32_t                   dwTimerId                          /**< [In] Timer Id */
                                 );

/**
* \brief Timer wait function.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval #PH_PLATFORM_ERR_NO_FREE_TIMER No Free Timer available to create.
*/
phStatus_t phPlatform_Timer_Wait(phPlatform_DataParams_t   *pDataParams,                       /**< [In] Pointer to this layers parameter structure. */
                                 uint8_t                    bTimerDelayUnit,                   /**< [In] Delay value unit could be in microseconds or milliseconds */
                                 uint16_t                   wDelay                             /**< [In] Time Delay */
                                 );

/**
* \brief Resets the timer allocated to the application which is created by \ref phPlatform_Timer_Create.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval #PH_PLATFORM_ERR_INVALID_TIMER Provided Timer ID is not created.
*/
phStatus_t phPlatform_Timer_Reset(phPlatform_DataParams_t   *pDataParams,                      /**< [In] Pointer to this layers parameter structure. */
                                  uint32_t                   dwtimerId                         /**< [In] Timer Id */
                                  );


/**
* \brief Delete or Deallocate the timer.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval #PH_PLATFORM_ERR_INVALID_TIMER Provided Timer ID is not created.
*/
phStatus_t phPlatform_Timer_Delete(phPlatform_DataParams_t   *pDataParams,                     /**< [In] Pointer to this layers parameter structure. */
                                   uint32_t                   dwTimerId                        /**< [In] Timer Id */
                                   );

/**
* \brief Execute the Callback registered to the timer.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phPlatform_Timer_ExecCallback(phPlatform_DataParams_t   *pDataParams,               /**< [In] Pointer to this layers parameter structure. */
                                         uint32_t                   dwTimerId                  /**< [In] Timer Id */
                                         );

/** @} */
#endif /* NXPBUILD__PH_PLATFORM */

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* PHPLATFORM_H */

