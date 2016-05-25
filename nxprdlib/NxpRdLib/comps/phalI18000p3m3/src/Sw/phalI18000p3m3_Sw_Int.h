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
* Software ISO18000-3 Mode3 Application Layer (Internals).
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 2673 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  CHu: Generated 20. July 2010
*
*/

#ifndef PHALI18000P3M3_SW_INT_H
#define PHALI18000P3M3_SW_INT_H

#include <ph_Status.h>
#include <phalI18000p3m3.h>

/** \addtogroup ph_Private
* @{
*/

/**
* \name Command Codes
*/
/*@{*/
#define PHAL_I18000P3M3_CMD_REQ_RN          0xC1    /**< ReqRn command code. */
#define PHAL_I18000P3M3_CMD_READ            0xC2    /**< Read command code. */
#define PHAL_I18000P3M3_CMD_WRITE           0xC3    /**< Write command code. */
#define PHAL_I18000P3M3_CMD_KILL            0xC4    /**< Kill command code. */
#define PHAL_I18000P3M3_CMD_LOCK            0xC5    /**< Lock command code. */
#define PHAL_I18000P3M3_CMD_ACCESS          0xC6    /**< Access command code. */
#define PHAL_I18000P3M3_CMD_BLOCKWRITE      0xC7    /**< BlockWrite command code. */
#define PHAL_I18000P3M3_CMD_BLOCKERASE      0xC8    /**< BlockErase command code. */
#define PHAL_I18000P3M3_CMD_BLOCKPERMALOCK  0xC9    /**< BlockPermaLock command code. */
/** @} */

/** \name I180000 Error Codes
*/
/*@{*/
#define PHAL_I18000P3M3_ERRCODE_OTHER               0x00U   /**< Other Error. */
#define PHAL_I18000P3M3_ERRCODE_MEMORY_OVERRUN      0x03U   /**< Memory overrun. */
#define PHAL_I18000P3M3_ERRCODE_MEMORY_LOCKED       0x04U   /**< Memory locked. */
#define PHAL_I18000P3M3_ERRCODE_INSUFFICIENT_POWER  0x0BU   /**< Insufficient power. */
#define PHAL_I18000P3M3_ERRCODE_NON_SPECIFIC        0x0FU   /**< Non-specific error. */
/*@}*/

/**
* \brief Parse received error frame.
*
* \return Status code
* \retval #PHAL_I18000P3M3_ERR_OTHER
* \retval #PHAL_I18000P3M3_ERR_MEMORY_OVERRUN
* \retval #PHAL_I18000P3M3_ERR_MEMORY_LOCKED
* \retval #PHAL_I18000P3M3_ERR_INSUFFICIENT_POWER
* \retval #PHAL_I18000P3M3_ERR_NON_SPECIFIC
* \retval #PH_ERR_PROTOCOL_ERROR Either length or Handle does not match.
*/
phStatus_t phalI18000p3m3_Sw_ParseError(
                                        phalI18000p3m3_Sw_DataParams_t * pDataParams,   /**< [In] Pointer to this layer's parameter structure. */
                                        uint8_t * pRxData,                              /**< [In] Received data. */
                                        uint16_t wRxDataLength,                         /**< [In] Length of received data in bytes. */
                                        uint8_t bRxLastBits                             /**< [In] Valid bits of last byte of RxData. */
                                        );

/** @}
* end of ph_Private
*/

#endif /* PHALI18000P3M3_SW_INT_H_ */
