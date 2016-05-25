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
* Generic OSAL Component of Reader Library Framework.
* Functions defined here are abstracting the OS Features.
* MicroController Timer functions are in phPlatform.h within the MicroController specific directories.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 3935 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  PC: Generated 23. Aug 2012
*
*/

#ifndef PHOSAL_H
#define PHOSAL_H

#include <ph_Status.h>

#ifdef _WIN32
#pragma warning(push)           /* PRQA S 3116 */
#pragma warning(disable:4001)   /* PRQA S 3116 */
#include <stdlib.h>
#pragma warning(pop)                /* PRQA S 3116 */
#else
#include <stdlib.h>
#endif

#ifdef NXPBUILD__PH_OSAL_FREERTOS
#define portMAXIMUM_DELAY 0xFFFFFFFF   /* Maximum time delay configured in case FreeRTOS (i.e. portMAX_DELAY) support 32bit Tick Type. */
#define PH_MAX_DELAY      portMAXIMUM_DELAY
#else
#define PH_MAX_DELAY      0xFFFF          /* Without RTOS we should wait on interrupt pin going high and not on the event. */
#endif  /* NXPBUILD__PH_OSAL_FREERTOS */

#ifdef  __cplusplus
extern "C" {
#endif    /* __cplusplus */

#ifdef NXPBUILD__PH_OSAL

/** \defgroup phOsal Operating System Abstraction Layer
 *
* \brief These components implement the OSAL APIs.
* The users of NXP reader library are expected to implement these APIs
* using the real time OS of their choice.
* All these functions are used by the NXP reader library internally.
*
* In order to enable the implementation provided by the user for a chosen RTOS,
* the user has to do the following :\n
* The specific OS implementation should be placed under NxpNfcRdLib/phOsal/UserRTOS.\n
* UserRTOS is just an example directory name. User is free to choose any suitable name.
*
* The source file implementing these APIs (say phOsal_UserRTOS.c) should include a #ifdef guard like
* #ifdef NXPBUILD__PH_OSAL_USERRTOS that is used to enable compilation of this implementation. \n
* The build define NXPBUILD__PH_OSAL_USERRTOS should be defined in the appropriate header file as
* explained below \n
*
* If compiling only the library NxpNfcRdLib, the file NxpNfcRdLib/types/ph_NxpBuild_Lpc.h
* should define this.
*
* If compiling an application, say NfcrdlibEx1_BasicDiscoveryLoop, the file NfcrdlibEx1_BasicDiscoveryLoop/intfs/ph_NxpBuild_App.h
* should define this.
*
* @{
*/
#define PH_OSAL_TIMER_UNIT_US            0x00U                    /**< Indicates that the specified delay is in microseconds.*/
#define PH_OSAL_TIMER_UNIT_MS            0x01U                    /**< Indicates that the specified delay is in milliseconds. */
#define PH_OSAL_INVALID_TIMER_ID         0xFFFFFFFF               /**< Invalid Timer Id. */

/* This Macro to be used to resolve unused and unreferenced compiler warnings. */
#define PHOSAL_UNUSED_VARIABLE(x) for((x)=(x);(x)!=(x);)

/* *****************************************************************************************************************
 * Types/Structure Declarations
 * ***************************************************************************************************************** */
/**
 * OSAL task/thread handle.
 */
typedef void * phOsal_TaskHandle_t;
/**
 * OSAL Semaphore handle.
 */
typedef void * phOsal_SemaphoreHandle_t;
/**
 * OSAL Mutex handle.
 */
typedef void * phOsal_MutexHandle_t;
/**
 * OSAL Event group handle.
 */
typedef void * phOsal_EventHandle_t;
/**
 * OSAL Timer handle.
 */
typedef void * phOsal_TimerHandle_t;
/**
 * OSAL Task Creation API signature.
 */
typedef void (*pphOsal_pTask_t) (void *pvArguments);
/**
 * OSAL Timer Callback handler signature.
 */
typedef void (*pphOsal_TimerCallback_t)(phOsal_TimerHandle_t xTimerHandle);

/**
* \name OSAL Event Type.
*
*/
/*@{*/
typedef enum phOsal_EventType{
    E_PH_OSAL_EVT_WAITING          = 0,
    E_PH_OSAL_EVT_RF               = (1 << 0),       /**< Event posted by CLIF ISR once the interrupt line goes high to the CLIF HAL. */
    E_PH_OSAL_EVT_RXDATA_AVAILABLE = (1 << 1),       /**< Event posted by Reader library thread to the application thread once the ISO 18092 packet is received and processed. */
    E_PH_OSAL_EVT_TXDATA_AVAILABLE = (1 << 2),       /**< Event posted by application thread so that the Reader library thread takes the data and sends over ISO 18092. */
    E_PH_OSAL_EVT_RTO_TIMEOUT      = (1 << 3),       /**< Event posted by response time extension timer ISR. Reader library thread listens to this event and sends the RTOx packet. */
    E_PH_OSAL_EVT_ERROR            = (1 << 4),       /**< Event posted by the Reader library thread, in case application should use /ref phOsal_Event_PostwithDetail to knows the error occurred. */
    E_PH_OSAL_EVT_ABORT            = (1 << 5),       /**< Event posted by the application to shut down the reader library thread. */
    E_PH_OSAL_EVT_LLCP_ACTIVATED   = (1 << 6),       /**< Event posted by Reader library thread to the application thread once LLCP is activated. */
    E_PH_OSAL_EVT_TYPE_ALL= (
            E_PH_OSAL_EVT_RF |
            E_PH_OSAL_EVT_RXDATA_AVAILABLE |
            E_PH_OSAL_EVT_TXDATA_AVAILABLE |
            E_PH_OSAL_EVT_RTO_TIMEOUT |
            E_PH_OSAL_EVT_ERROR |
            E_PH_OSAL_EVT_ABORT),                    /**< All Events Enabled. */
    E_PH_OSAL_SIZEADJUST_EVT_TYPE = 0x7FFFFFFFU      /**< For 32bit enum adjustment. */
} phOsal_EventType_t;
/*@}*/

/**
* \name Event detail description.
* This is assigned a value indicating the details of the exact error data
* associated with the event.
*/
/*@{*/
typedef enum phOsal_EventDetail{
    E_PH_OSAL_EVT_DETAIL_SUCCESS = 0,                /**< Success Status. */
    E_PH_OSAL_EVT_DETAIL_IO_TIMEOUT_ERROR,           /**< No reply received, example PICC removal. */
    E_PH_OSAL_EVT_DETAIL_INTEGRITY_ERROR,            /**< Wrong CRC or parity detected. */
    E_PH_OSAL_EVT_DETAIL_COLLISION_ERROR,            /**< Collision in the Receiver. */
    E_PH_OSAL_EVT_DETAIL_BUFFER_OVERFLOW,            /**< Attempt to write beyond buffer size. */
    E_PH_OSAL_EVT_DETAIL_FRAMING_ERROR,              /**< Invalid frame format. */
    E_PH_OSAL_EVT_DETAIL_PROTOCOL_ERROR,             /**< Received response violates protocol. */
    E_PH_OSAL_EVT_DETAIL_AUTH_ERROR,                 /**< Authentication error. */
    E_PH_OSAL_EVT_DETAIL_READ_WRITE_ERROR,           /**< A Read or Write error occurred in RAM/ROM or Flash. */
    E_PH_OSAL_EVT_DETAIL_TEMPERATURE_ERROR,          /**< The RC sensors signal over heating. */
    E_PH_OSAL_EVT_DETAIL_RF_ERROR,                   /**< Error on RF-Interface. */
    E_PH_OSAL_EVT_DETAIL_INTERFACE_ERROR,            /**< An error occurred in RC communication. */
    E_PH_OSAL_EVT_DETAIL_LENGTH_ERROR,               /**< A length error occurred. */
    E_PH_OSAL_EVT_DETAIL_RESOURCE_ERROR,             /**< A resource error. */
    E_PH_OSAL_EVT_DETAIL_TX_NAK_ERROR,               /**< TX rejected by the counterpart. */
    E_PH_OSAL_EVT_DETAIL_RX_NAK_ERROR,               /**< RX request rejected by the counterpart. */
    E_PH_OSAL_EVT_DETAIL_EXT_RF_ERROR,               /**< Error due to External RF. */
    E_PH_OSAL_EVT_DETAIL_ABORTED,                    /**< Used when HAL abort is called. */
    E_PH_OSAL_EVT_DETAIL_INTERNAL_ERROR = 0x007FU,   /**< An internal error occurred. */
    E_PH_OSAL_SIZEADJUST_EVT_DETAIL = 0x7FFFFFFFU    /**< For 32bit enum adjustment. */
} phOsal_EventDetail_t;
/*@}*/

/**
* \name RF Event Source Type
*/
/*@{*/
typedef enum phhalRf_EventSource{
    E_PH_OSAL_EVT_SRC_ISR = 0,                       /**< Event was generated from the ISR. */
    E_PH_OSAL_EVT_SRC_HAL = 1,                       /**< Event Source is HAL. */
    E_PH_OSAL_EVT_SRC_LIB = 2,                       /**< Event Source is Reader library. */
    E_PH_OSAL_EVT_SRC_APP = 3,                       /**< Event Source is Application. */
    E_PH_OSAL_EVT_SRC_NONE,                          /**< Used for initialization/de-initialization only. */
    E_PH_OSAL_SIZEADJUST_EVT_SRC = 0x7FFFFFFFU       /**< For 32bit enum adjustment. */
} phOsal_EventSource_t;
/*@}*/

/**
* \name RF Event Destination Type
*/
/*@{*/
typedef enum phhalRf_EventDestination{
    E_PH_OSAL_EVT_DEST_HAL = 0,                      /**< Event's Destination is HAL. */
    E_PH_OSAL_EVT_DEST_LIB =  1,                     /**< Event's Destination is RdLib. */
    E_PH_OSAL_EVT_DEST_APP = 2,                      /**< Event's Destination is Application. */
    E_PH_OSAL_EVT_DEST_APP2 = 3,                      /**< Event's Destination is Application. */
    E_PH_OSAL_EVT_DEST_NONE,                         /**< None.Used during initialization/de-initialization or for broadcast events. */
    E_PH_OSAL_SIZEADJUST_EVT_DEST = 0x7FFFFFFFU      /**< For 32bit enum adjustment. */
} phOsal_EventDestination_t;
/*@}*/

/* *****************************************************************************************************************
* Function Prototypes
* ***************************************************************************************************************** */
/**
* \brief   This function initializes the OSAL event required by NxpNfcRdLib.
*
* The underlying implementation has to create and initialize an
* event group using the specific RTOS APIs.
*
* \return  Status code
* \retval  #PH_ERR_SUCCESS Operation successful.
* \retval  #PH_ERR_OSAL_ERROR Unable to create the event group.
*/
phStatus_t phOsal_Event_Init(void);

/**
* \brief   This function gives the event handle created and initialized for NxpNfcRdLib.
* This will assign pHandle to the event initialized by \ref phOsal_Event_Init\n
*
* \return  Status code
* \retval  #PH_ERR_SUCCESS Operation successful.
* \retval  #PH_ERR_OSAL_ERROR Event group is null, as \ref phOsal_Event_Init is not called before calling this API.
*/
phStatus_t phOsal_Event_GetEventHandle(
                                       void **pHandle    /**< [Out] Pointer which will point to the event handle. */
                                      );

/**
* \brief   This function will post the event with source and destination information.
*
* \return  Status code
* \retval  #PH_ERR_SUCCESS Operation successful.
* \retval  #PH_ERR_OSAL_ERROR Event group is null, as \ref phOsal_Event_Init is not called before calling this API.
*/
phStatus_t phOsal_Event_Post(
                             phOsal_EventType_t eEvtType,                     /**< [In] Event type. */
                             phOsal_EventSource_t eSrc,                       /**< [In] Event source. */
                             phOsal_EventDestination_t eDest                  /**< [In] Event destination. */
                             );

/**
* \brief   This function will post the event with detail which can be retrieved using \ref phOsal_Event_GetDetail.
*
* \return  Status code
* \retval  #PH_ERR_SUCCESS Operation successful.
* \retval  #PH_ERR_OSAL_ERROR Event group is null, as \ref phOsal_Event_Init is not called before calling this API.
*/
void phOsal_Event_PostwithDetail(
                                 phOsal_EventType_t eEvtType,                 /**< [In] Event type. */
                                 phOsal_EventSource_t eSrc,                   /**< [In] Event source. */
                                 phOsal_EventDestination_t eDest,             /**< [In] Event destination. */
                                 phOsal_EventDetail_t eDetail                 /**< [In] Event detail. */
                                 );

/**
* \brief   This function will consume the events that are passed in the eEvtType parameter.
*
* \return  Status code
* \retval  #PH_ERR_SUCCESS Operation successful.
* \retval  #PH_ERR_OSAL_ERROR Event group is null, as \ref phOsal_Event_Init is not called before calling this API.
*/
phStatus_t phOsal_Event_Consume(
                                phOsal_EventType_t eEvtType,                  /**< [In] Event type. */
                                phOsal_EventSource_t eSrc                     /**< [In] Event source. */
                                );

/**
* \brief   This function will wait for any given event that is specified in eEvtTpype parameter.
*
* To set a maximum delay dwTimeoutCount should be assigned to #PH_MAX_DELAY \n
* else any time in milliseconds can be provided. A value of dwTimeoutCount=0 means
* this returns immediately.
*
*
* \return  Status code
* \retval  #PH_ERR_SUCCESS Operation successful.
* \retval  #PH_ERR_OSAL_ERROR Event group is null, as \ref phOsal_Event_Init is not called before calling this API.
* \retval  #PH_ERR_IO_TIMEOUT    No event occurred.
*/
phStatus_t phOsal_Event_WaitAny(
                                phOsal_EventType_t eEvtType,                  /**< [In] Event type. */
                                uint32_t dwTimeoutCount,                      /**< [In] Timeout value(ms). */
                                phOsal_EventType_t *pRcvdEvt                  /**< [Out] Received events. */
                                );

/**
* \brief   This function will wait for all the given events that are specified in eEvtType parameter.
*
* To set a maximum delay dwTimeoutCount should be assigned to #PH_MAX_DELAY \n
* else any time in milliseconds can be provided. A value of dwTimeoutCount=0 means
* this returns immediately.
*
* \return  Status code
* \retval  #PH_ERR_SUCCESS Operation successful.
* \retval  #PH_ERR_OSAL_ERROR Event group is null, as \ref phOsal_Event_Init is not called before calling this API.
* \retval  #PH_ERR_IO_TIMEOUT    No event occurred.
*/
phStatus_t phOsal_Event_WaitAll(
                                phOsal_EventType_t eEvtType,                  /**< [In] Event type. */
                                uint32_t dwTimeoutCount,                      /**< [In] Timeout value(ms). */
                                phOsal_EventType_t *pRcvdEvt                  /**< [Out] Received events. */
                                );

/**
* \brief This function will set the details of the event
* This has to be used before posting the event.
*/
void phOsal_Event_SetDetail(phOsal_EventDetail_t eEvtDetail);                 /**< [Out] Event detail. */

/**
* \brief   This function assigns the event detail to the event.
*/
void phOsal_Event_GetDetail(phOsal_EventDetail_t * eEvent_Detail);            /**< [Out] Pointer which will point to the detailed event variable. */

/**
* \brief   This function de-initializes the event.
* The underlying implementation using specific RTOS APIs is expected to
* delete the event group.
*
* \return  Status code
* \retval  #PH_ERR_SUCCESS Operation successful.
* \retval  #PH_ERR_OSAL_ERROR Event group is null, as \ref phOsal_Event_Init is not called before calling this API.
*/
phStatus_t phOsal_Event_Deinit(void);

/**
* \brief Create and initialize a semaphore object to manage resources.
* The underlying implementation should use specific RTOS APIs to
* create a semaphore.
* In order to create a binary semaphore, the dwInitialCount should be zero,
* the dwMaxCount should be set to One.
*
* \return Status
* \retval Handle to the created semaphore. NULL if the semaphore could not be created.
*/
phOsal_SemaphoreHandle_t phOsal_Semaphore_Create(
                                                 uint32_t dwMaxCount,                            /**< [In] The maximum count value that can be reached. */
                                                 uint32_t dwInitialCount                         /**< [In] The count value assigned to the semaphore when it is created. */
                                                 );

/**
* \brief Release a Semaphore token.
*
* \return Status code
* \retval #PH_ERR_SUCCESS           If the semaphore was released.
* \retval #PH_ERR_OSAL_ERROR        If an error occurred.
* \retval #PH_ERR_INVALID_PARAMETER If input parameter is not valid.
*/
phStatus_t phOsal_Semaphore_Give(
                                 phOsal_SemaphoreHandle_t semHandle                              /**< [In] Handle to the semaphore being released. */
                                 );

/**
* \brief To obtain a semaphore.
*
* \return Status code
* \retval #PH_ERR_SUCCESS           If the semaphore was obtained.
* \retval #PH_ERR_OSAL_ERROR        If dwBlockTime expired without the semaphore becoming available.
* \retval #PH_ERR_INVALID_PARAMETER If input parameter is not valid.
*/
phStatus_t phOsal_Semaphore_Take(
                                 phOsal_SemaphoreHandle_t semHandle,                             /**< [In] Handle to the semaphore being taken. */
                                 uint32_t dwBlockTime                                            /**< [In] The time in milli-seconds to wait for the semaphore to become available. */
                                 );

/**
* \brief Delete a semaphore.
*
* \return Status code
* \retval #PH_ERR_SUCCESS           If the semaphore was obtained.
* \retval #PH_ERR_INVALID_PARAMETER If input parameter is not valid.
*/
phStatus_t phOsal_Semaphore_Delete(
                                   phOsal_SemaphoreHandle_t semHandle                            /**< [In] Handle to the semaphore to be deleted. */
                                   );

/**
* \brief Create and initialize a mutex object.
*
* \return Status
* \retval Handle to the created mutex. NULL if the mutex could not be created.
*/
phOsal_MutexHandle_t phOsal_Mutex_Create(void);

/**
* \brief To obtain a mutex.
*
* \return Status code
* \retval #PH_ERR_SUCCESS           If the mutex was obtained.
* \retval #PH_ERR_OSAL_ERROR        If dwBlockTime expired without the mutex becoming available.
* \retval #PH_ERR_INVALID_PARAMETER If input parameter is not valid.
*/
phStatus_t phOsal_Mutex_Take(
                             phOsal_MutexHandle_t mutexHandle,                                   /**< [In] Handle to the Mutex being taken. */
                             uint32_t dwBlockTime                                                /**< [In] The time in milli-seconds to wait for the Mutex to become available. */
                             );

/**
* \brief Release an acquired Mutex.
*
* \return Status code
* \retval #PH_ERR_SUCCESS           If the mutexHandle was released.
* \retval #PH_ERR_OSAL_ERROR        If an error occurred.
* \retval #PH_ERR_INVALID_PARAMETER If input parameter is not valid.
*/
phStatus_t phOsal_Mutex_Give(
                             phOsal_MutexHandle_t mutexHandle                                    /**< [In] Handle to the Mutex being released. */
                             );

/**
* \brief Delete a mutex.
*
* \return Status code
* \retval #PH_ERR_SUCCESS           If the mutex is deleted.
* \retval #PH_ERR_OSAL_ERROR        If an error occurred.
* \retval #PH_ERR_INVALID_PARAMETER If input parameter is not valid.
*/
phStatus_t phOsal_Mutex_Delete(
                               phOsal_MutexHandle_t mutexHandle                                  /**< [In] Handle to the Mutex to be deleted. */
                               );

/**
* \brief Creates a new timer instance.
* Returns a handle using which the timer can be referenced.
*
* \return Status
* \retval If the timer is successfully created then a handle to the newly created timer is returned. If the timer cannot be created then NULL is returned.
*/
phOsal_TimerHandle_t phOsal_Timer_Create(
                                         uint32_t dwTimerID,                                     /**< [In] User assigned timer Id. */
                                         pphOsal_TimerCallback_t fpCallbackFunction              /**< [In] Function to call when the timer expires. */
                                         );

/**
* \brief Starts a timer that was previously created using the phOsal_TimerCreate() API function.
*
* \return Status code
* \retval #PH_ERR_SUCCESS           If the timer is started successfully.
* \retval #PH_ERR_OSAL_ERROR        If an error occurred.
* \retval #PH_ERR_INVALID_PARAMETER If input parameter is not valid.
*/
phStatus_t phOsal_Timer_Start(
                              phOsal_TimerHandle_t timerHandle,                                  /**< [In] Handle of the timer to be started. */
                              uint32_t dwTimeMs                                                  /**< [In] The timer period in Milli-Seconds (ms). */
                              );

/**
* \brief Stops a timer that was previously started using phOsal_TimerStart().
*
* \retval #PH_ERR_SUCCESS           If the timer is stopped successfully.
* \retval #PH_ERR_OSAL_ERROR        If an error occurred.
* \retval #PH_ERR_INVALID_PARAMETER If input parameter is not valid.
*/
phStatus_t phOsal_Timer_Stop(
                             phOsal_TimerHandle_t timerHandle                                    /**< [In] Handle of the timer to be stopped. */
                             );

/**
* \brief Re-starts a timer that was previously created using the phOsal_TimerCreate() API function.
*
* \retval #PH_ERR_SUCCESS           If the timer is reset/restarted successfully.
* \retval #PH_ERR_OSAL_ERROR        If an error occurred.
* \retval #PH_ERR_INVALID_PARAMETER If input parameter is not valid.
*/
phStatus_t phOsal_Timer_Reset(
                              phOsal_TimerHandle_t timerHandle                                   /**< [In] Handle of the timer to be reset/restarted. */
                              );

/**
* \brief Deletes a timer that was previously created using the phOsal_TimerCreate() API function.
*
* \retval #PH_ERR_SUCCESS           If the timer is deleted successfully.
* \retval #PH_ERR_OSAL_ERROR        If an error occurred.
* \retval #PH_ERR_INVALID_PARAMETER If input parameter is not valid.
*/
phStatus_t phOsal_Timer_Delete(
                               phOsal_TimerHandle_t timerHandle                                  /**< [In] Handle of the timer to be deleted. */
                               );

/** @} */
#endif /* NXPBUILD__PH_OSAL */

#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* PHOSAL_H */
