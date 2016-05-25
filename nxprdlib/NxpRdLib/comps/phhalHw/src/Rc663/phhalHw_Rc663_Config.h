/*
*         Copyright (c), NXP Semiconductors Gratkorn / Austria
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
* Internal definitions for Rc663 specific HAL-Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 2673 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  CHu: Generated 19. May 2009
*
*/

#ifndef PHHALHW_RC663_CONFIG_H
#define PHHALHW_RC663_CONFIG_H

#include <ph_Status.h>
#include <phhalHw.h>

/** \defgroup phhalHw_Rc663_Config Configuration
* \brief Custom configuration related to NFC IC Hardware settings.
* @{
*/

/** \name Memory related constants
*/
/*@{*/
#define PHHAL_HW_RC663_PRECACHED_BYTES  0xFFU   /**< Maximum number of bytes to precache into FIFO before triggering a command. */
/*@}*/

/** \name Modulation indices
*/
/*@{*/
#define PHHAL_HW_RC663_MODINDEX_I14443B     0xCCU   /**< Default modulation index for ISO14443B. */
#define PHHAL_HW_RC663_MODINDEX_FELICA      0x17U   /**< Default modulation index for Felica. */
#define PHHAL_HW_RC663_MODINDEX_I15693      0x10U   /**< Default modulation index for ISO15693.  */
#define PHHAL_HW_RC663_MODINDEX_EPCUID      0x10U   /**< Default modulation index for EPC/UID. */
#define PHHAL_HW_RC663_MODINDEX_I18000P3M3  0x10U   /**< Default modulation index for ISO18000-3 Mode3. */
/*@}*/

/** \name RxThreshold values
*/
/*@{*/
#define PHHAL_HW_RC663_RXTHRESHOLD_I14443A_106          0x3FU   /**< RxThreshold for ISO14443A @ 106kbit/s. */
#define PHHAL_HW_RC663_RXTHRESHOLD_I14443A_212          0x3FU   /**< RxThreshold for ISO14443A @ 212kbit/s. */
#define PHHAL_HW_RC663_RXTHRESHOLD_I14443A_424          0x3FU   /**< RxThreshold for ISO14443A @ 424kbit/s. */
#define PHHAL_HW_RC663_RXTHRESHOLD_I14443A_848          0x3FU   /**< RxThreshold for ISO14443A @ 848kbit/s. */
#define PHHAL_HW_RC663_RXTHRESHOLD_I14443B_106          0x3FU   /**< RxThreshold for ISO14443B @ 106kbit/s. */
#define PHHAL_HW_RC663_RXTHRESHOLD_I14443B_212          0x3FU   /**< RxThreshold for ISO14443B @ 212kbit/s. */
#define PHHAL_HW_RC663_RXTHRESHOLD_I14443B_424          0x3FU   /**< RxThreshold for ISO14443B @ 424kbit/s. */
#define PHHAL_HW_RC663_RXTHRESHOLD_I14443B_848          0x3FU   /**< RxThreshold for ISO14443B @ 848kbit/s. */
#define PHHAL_HW_RC663_RXTHRESHOLD_FELICA_212           0x3FU   /**< RxThreshold for Felica @ 212kbit/s. */
#define PHHAL_HW_RC663_RXTHRESHOLD_FELICA_424           0x3FU   /**< RxThreshold for Felica @ 424kbit/s. */
#define PHHAL_HW_RC663_RXTHRESHOLD_I15693_FAST          0x44U   /**< RxThreshold for ISO15693 @ High. */
#define PHHAL_HW_RC663_RXTHRESHOLD_I15693_FAST_HIGH     0x44U   /**< RxThreshold for ISO15693 @ Fast_High. */
#define PHHAL_HW_RC663_RXTHRESHOLD_EPCUID               0x4EU   /**< RxThreshold for EPC/UID. */
#define PHHAL_HW_RC663_RXTHRESHOLD_I18000P3M3           0x36U   /**< RxThreshold for ISO18000-3 Mode3. */
/*@}*/

/** \name LPCD min. timer reload values
*/
/*@{*/
#define PHHAL_HW_RC663_LPCD_T3_RELOAD_MIN   0x0010U /**< Default minimum for T3 reload value (used only for #PHHAL_HW_RC663_CMD_LPCD_MODE_DEFAULT mode). */
#define PHHAL_HW_RC663_LPCD_T4_RELOAD_MIN   0x0005U /**< Default minimum for T4 reload value (used only for #PHHAL_HW_RC663_CMD_LPCD_MODE_DEFAULT mode). */
/*@}*/

/** \name Feature selection
*/
/*@{*/
#define PHHAL_HW_RC663_FEATURE_FIFO_UNDERFLOW_CHECK /**< Checks for FIFO underflow during data transmission if defined. */
/*@}*/

/** @}
* end of phhalHw_Rc663_Config group
*/

#endif /* PHHALHW_RC663_CONFIG_H */
