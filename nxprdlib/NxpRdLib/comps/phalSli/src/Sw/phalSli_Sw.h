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
* Software ICode SLI Application Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 2673 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  CHu: Generated 10. November 2009
*
*/

#ifndef PHALSLI_SW_H
#define PHALSLI_SW_H

#include <ph_Status.h>

phStatus_t phalSli_Sw_GetRandomNumber(
                                      phalSli_Sw_DataParams_t * pDataParams,
                                      uint8_t * pRnd
                                      );

phStatus_t phalSli_Sw_SetPWD(
                             phalSli_Sw_DataParams_t * pDataParams,
                             uint8_t bPwdIdentifier,
                             uint8_t * pXorPwd
                             );

phStatus_t phalSli_Sw_WritePWD(
                               phalSli_Sw_DataParams_t * pDataParams,
                               uint8_t bPwdIdentifier,
                               uint8_t * pPwd
                               );

phStatus_t phalSli_Sw_LockPWD(
                              phalSli_Sw_DataParams_t * pDataParams,
                              uint8_t bPwdIdentifier
                              );

phStatus_t phalSli_Sw_64BitPWDProtection(
    phalSli_Sw_DataParams_t * pDataParams
    );

phStatus_t phalSli_Sw_ProtectPage(
                                  phalSli_Sw_DataParams_t * pDataParams,
                                  uint8_t bPageNo,
                                  uint8_t bProtectionStatus
                                  );

phStatus_t phalSli_Sw_LockPageProtectionCondition(
    phalSli_Sw_DataParams_t * pDataParams,
    uint8_t bPageNo
    );

phStatus_t phalSli_Sw_GetMultipleBlockProtStatus(
    phalSli_Sw_DataParams_t * pDataParams,
    uint8_t bBlockNo,
    uint16_t wNumBlocks,
    uint8_t * pProtectionStates,
    uint8_t * pNumReceivedStates
    );

phStatus_t phalSli_Sw_Destroy(
                              phalSli_Sw_DataParams_t * pDataParams
                              );

phStatus_t phalSli_Sw_EnablePrivacy(
                                    phalSli_Sw_DataParams_t * pDataParams
                                    );

phStatus_t phalSli_Sw_ReadSignature(
                                    phalSli_Sw_DataParams_t * pDataParams,
                                    uint8_t * pSignature
                                    );

phStatus_t phalSli_Sw_SetEAS(
                             phalSli_Sw_DataParams_t * pDataParams
                             );

phStatus_t phalSli_Sw_ResetEAS(
                               phalSli_Sw_DataParams_t * pDataParams
                               );

phStatus_t phalSli_Sw_LockEAS(
                              phalSli_Sw_DataParams_t * pDataParams
                              );

phStatus_t phalSli_Sw_EASAlarm(
                               phalSli_Sw_DataParams_t * pDataParams,
                               uint8_t bOption,
                               uint8_t * pEasIdValue,
                               uint8_t bEasIdMaskLength,
                               uint8_t * pEas
                               );

phStatus_t phalSli_Sw_PwdProtectEAS(
                                    phalSli_Sw_DataParams_t * pDataParams
                                    );

phStatus_t phalSli_Sw_WriteEAS_ID(
                                  phalSli_Sw_DataParams_t * pDataParams,
                                  uint8_t * pEasIdValue
                                  );

phStatus_t phalSli_Sw_ReadEPC(
                              phalSli_Sw_DataParams_t * pDataParams,
                              uint8_t * pEpc
                              );

phStatus_t phalSli_Sw_GetNxpSystemInfo(
                                       phalSli_Sw_DataParams_t * pDataParams,
                                       uint8_t * pProtectionPointer,
                                       uint8_t * pProtectionCondition,
                                       uint8_t * pLockBits,
                                       uint8_t * pFeatureFlags
                                       );

phStatus_t phalSli_Sw_GetRandomNumberSetPWD(
                                            phalSli_Sw_DataParams_t * pDataParams,
                                            uint8_t bPwdIdentifier,
                                            uint8_t * pPwd
                                            );

#endif /* PHALSLI_SW_H */
