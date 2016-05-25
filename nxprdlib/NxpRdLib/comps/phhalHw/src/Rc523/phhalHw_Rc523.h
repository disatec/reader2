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
* RC523 specific HAL-Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 4093 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  CHu: Generated 19. May 2009
*
*/

#ifndef PHHALHW_RC523_H
#define PHHALHW_RC523_H

#include <ph_Status.h>
#include <phhalHw.h>

#define PHHAL_HW_RC523_FIFOSIZE                           64U         /**< Size of RC internal FiFo buffer */
#define PHHAL_HW_RC523_DEFAULT_TIMEOUT                    150U        /**< Default timeout in microseconds */
#define PHHAL_HW_RC523_DEFAULT_TIMEOUT_MILLI              50U         /**< Default timeout in milliseconds */
#define PHPAL_HW_RC523_I18092MT_START_BYTE                0xF0U       /**< Start Byte to be added to the protocol header in case protocol ISO18092. */
#define PHHAL_HW_RC523_ACTIVE_RF_OFF_INTEROP_TOLERANCE    300U        /**< Time Delay in us used for external peer to turn OFF RF in Active mode. */
#define PHHAL_HW_RC523_ACTIVE_INTERNAL_RF_OFF_TOLERANCE   20U         /**< Time Delay in us used to turn off Internal RF in Active mode (during buffer overflow scenario). */

phStatus_t phhalHw_Rc523_Exchange(
    phhalHw_Rc523_DataParams_t * pDataParams,
    uint16_t wOption,
    uint8_t * pTxBuffer,
    uint16_t wTxLength,
    uint8_t ** ppRxBuffer,
    uint16_t * pRxLength
    );

phStatus_t phhalHw_Rc523_Transmit(
    phhalHw_Rc523_DataParams_t * pDataParams,
    uint16_t wOption,
    uint8_t * pTxBuffer,
    uint16_t wTxLength
    );

phStatus_t phhalHw_Rc523_Receive(
    phhalHw_Rc523_DataParams_t * pDataParams,
    uint16_t wOption,
    uint8_t ** ppRxBuffer,
    uint16_t * pRxLength
    );

phStatus_t phhalHw_Rc523_ApplyProtocolSettings(
    phhalHw_Rc523_DataParams_t * pDataParams,
    uint8_t bCardType
    );

phStatus_t phhalHw_Rc523_MfcAuthenticateKeyNo(
    phhalHw_Rc523_DataParams_t * pDataParams,
    uint8_t bBlockNo,
    uint8_t bKeyType,
    uint16_t wKeyNo,
    uint16_t wKeyVersion,
    uint8_t * pUid
    );

phStatus_t phhalHw_Rc523_MfcAuthenticate(
    phhalHw_Rc523_DataParams_t * pDataParams,
    uint8_t bBlockNo,
    uint8_t bKeyType,
    uint8_t * pKey,
    uint8_t * pUid
    );

phStatus_t phhalHw_Rc523_SetConfig(
    phhalHw_Rc523_DataParams_t * pDataParams,
    uint16_t wConfig,
    uint16_t wValue
    );

phStatus_t phhalHw_Rc523_GetConfig(
    phhalHw_Rc523_DataParams_t * pDataParams,
    uint16_t wConfig,
    uint16_t * pValue
    );

phStatus_t phhalHw_Rc523_FieldOn(
    phhalHw_Rc523_DataParams_t * pDataParams
    );

phStatus_t phhalHw_Rc523_FieldOff(
    phhalHw_Rc523_DataParams_t * pDataParams
    );

phStatus_t phhalHw_Rc523_FieldReset(
    phhalHw_Rc523_DataParams_t * pDataParams
    );

phStatus_t phhalHw_Rc523_Wait(
    phhalHw_Rc523_DataParams_t * pDataParams,
    uint8_t bUnit,
    uint16_t wTimeout
    );

phStatus_t phhalHw_Rc523_SetMinFDT(
    phhalHw_Rc523_DataParams_t * pDataParams,
    uint16_t wValue
    );

/**
* Used to configure the IC to handle Felica polling or MIFARE anti-collision in Target/Card Mode.
* Parameter wMode option is irrelevant in case of Pn512 as only NFC Target mode is supported and cannot be
* individually configured to either Type A or Type F target.
*
* Note: Ensure that the correct configuration has been loaded using the phhalHw_Rc523_SetListenParameters command.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval #PH_ERR_IO_TIMEOUT No response received.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phhalHw_Rc523_Autocoll(
    phhalHw_Rc523_DataParams_t * pDataParams,
    uint16_t wMode,
    uint8_t ** ppRxBuffer,
    uint16_t * pRxLength,
    uint16_t * pProtParams
    );

phStatus_t phhalHw_Rc523_AsyncAbort(
    phhalHw_Rc523_DataParams_t * pDataParams
    );

phStatus_t phhalHw_Rc523_DeInit(
    phhalHw_Rc523_DataParams_t * pDataParams
    );

#endif /* PHHALHW_RC523_H */
