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
* Internal functions of Software implementation of MIFARE (R) Ultralight application layer.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 2673 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  CHu: Generated 05. October 2009
*
*/

#ifndef PHALMFUL_INT_H
#define PHALMFUL_INT_H

#include <ph_Status.h>

/** \addtogroup ph_Private
* @{
*/

#define PHAL_MFUL_RESP_ACK      0x0AU   /**< MIFARE Ultralight ACK response code */
#define PHAL_MFUL_RESP_NAK0     0x00U   /**< MIFARE Ultralight NAK0 response code */
#define PHAL_MFUL_RESP_NAK1     0x01U   /**< MIFARE Ultralight NAK1 response code */
#define PHAL_MFUL_RESP_NAK4     0x04U   /**< MIFARE Ultralight NAK4 response code */
#define PHAL_MFUL_RESP_NAK5     0x05U   /**< MIFARE Ultralight NAK5 response code */

#define PHAL_MFUL_CMD_READ      0x30U   /**< MIFARE Ultralight Read command byte */
#define PHAL_MFUL_CMD_COMPWRITE 0xA0U   /**< MIFARE Ultralight Compatibility Write command byte */
#define PHAL_MFUL_CMD_WRITE     0xA2U   /**< MIFARE Ultralight Write command byte */
#define PHAL_MFUL_CMD_AUTH      0x1AU   /**< MIFARE Ultralight Authenticate command byte */
#define PHAL_MFUL_CMD_INCR_CNT  0xA5U   /**< MIFARE Ultralight Increment count command byte */
#define PHAL_MFUL_CMD_READ_CNT  0x39U   /**< MIFARE Ultralight Read counter command byte */
#define PHAL_MFUL_CMD_PWD_AUTH  0x1BU   /**< MIFARE Ultralight Password Auth command byte */
#define PHAL_MFUL_CMD_GET_VER   0x60U   /**< MIFARE Ultralight Get version command byte */
#define PHAL_MFUL_CMD_FAST_READ 0x3AU   /**< MIFARE Ultralight Fast read command byte */
#define PHAL_MFUL_CMD_READ_SIG  0x3CU   /**< MIFARE Ultralight Read signature command byte */
#define PHAL_MFUL_CMD_CHK_TRG_EVT   0x3EU   /**< MIFARE Ultralight Check tearing event command byte */
#define PHAL_MFUL_CMD_SECTOR_SELECT 0xC2U   /**< Type 2 tag sector select command byte */

#define PHAL_MFUL_PREAMBLE_TX   0xAFU   /**< MIFARE Ultralight preamble byte (tx) for authentication. */
#define PHAL_MFUL_PREAMBLE_RX   0x00U   /**< MIFARE Ultralight preamble byte (rx) for authentication. */

#define PHAL_MFUL_COMPWRITE_BLOCK_LENGTH    16U /**< Length of a compatibility write MIFARE(R) Ultralight data block. */

/** @} */

phStatus_t phalMful_Int_Read(
                             void * pPalMifareDataParams,
                             uint8_t bAddress,
                             uint8_t * pData
                             );

phStatus_t phalMful_Int_SectorSelect(
                                     void * pPalMifareDataParams,
                                     uint8_t bSecNo
                                     );

phStatus_t phalMful_Int_Write(
                              void * pPalMifareDataParams,
                              uint8_t bAddress,
                              uint8_t * pData
                              );

phStatus_t phalMful_Int_CompatibilityWrite(
    void * pPalMifareDataParams,
    uint8_t bAddress,
    uint8_t * pData
    );

phStatus_t phalMful_Int_IncrCnt(
                                void * pPalMifareDataParams,
                                uint8_t bCntNum,
                                uint8_t * pCnt
                                );

phStatus_t phalMful_Int_ReadCnt(
                                void * pPalMifareDataParams,
                                uint8_t bCntNum,
                                uint8_t * pCntValue
                                );

phStatus_t phalMful_Int_PwdAuth(
                                void * pPalMifareDataParams,
                                uint8_t * pPwd,
                                uint8_t * pPack
                                );

phStatus_t phalMful_Int_GetVersion(
                                   void * pPalMifareDataParams,
                                   uint8_t * pVersion
                                   );

phStatus_t phalMful_Int_FastRead(
                                 void * pPalMifareDataParams,
                                 uint8_t  bStartAddr,
                                 uint8_t bEndAddr,
                                 uint8_t ** pData,
                                 uint16_t * pNumBytes
                                 );

phStatus_t phalMful_Int_ReadSign(
                                 void * pPalMifareDataParams,
                                 uint8_t bAddr,
                                 uint8_t ** pSignature
                                 );

phStatus_t phalMful_Int_ChkTearingEvent(
                                        void * pPalMifareDataParams,
                                        uint8_t bCntNum,
                                        uint8_t * pValidFlag
                                        );

#endif /* PHALMFUL_INT_H */
