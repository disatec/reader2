/*
 * phOsal_Freertos.h
 *
 *  Created on: May 12, 2016
 *      Author: nxp69678
 */

#ifndef PHOSAL_FREERTOS_H
#define PHOSAL_FREERTOS_H

#include <ph_Status.h>

#ifdef NXPBUILD__PH_OSAL_FREERTOS

#include <FreeRTOSConfig.h>
#include <projdefs.h>
#include <portmacro.h>

#define PHOSAL_FREERTOS_ALL_EVENTS      0x00FFFFFF

#define PHOSAL_MAX_DELAY      portMAX_DELAY

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
#define PHOSAL_FREERTOS_STATIC_MEM_ALLOCATION
#endif /* ( configSUPPORT_STATIC_ALLOCATION == 1 ) */
#endif /* NXPBUILD__PH_OSAL_FREERTOS */

#endif /* PHOSAL_FREERTOS_H */
