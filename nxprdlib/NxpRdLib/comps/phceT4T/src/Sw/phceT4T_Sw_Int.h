/*
*             Copyright (c) NXP, 2012
*                  http://www.nxp.com
*
*         Developed by CETRTA POT, d.o.o. Kranj, 2012
*                 http://www.cetrtapot.si
*
* All rights are reserved. Reproduction in whole or in part is
* prohibited without the written consent of the copyright owner.
* NXP reserves the right to make changes without notice at
* any time. NXP makes no warranty, expressed, implied or
* statutory, including but not limited to any implied warranty of
* merchantability or fitness for any particular purpose, or that
* the use will not infringe any third party patent, copyright or
* trademark. NXP must not be liable for any loss or damage
* arising from its use.
*/

/** \file
* Card Emulation component for Type 4A Tag.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 2673 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*/

#ifndef PHCET4T_SW_INT_H
#define PHCET4T_SW_INT_H

#include <phceT4T.h>

/**
* \name Data type to send
*/
#define PHCE_SEND_NO_DATA   0x00U
#define PHCE_SEND_DATA      0x01U
#define PHCE_SEND_WTX       0x02U

/**
* \name Status Word
*/
/*@{*/
/** Unsupported class byte in CPDU */
#define PHCE_T4T_ISO7816_UNSUPPORTED_CLASS            0x6E00
/** Unsupported Instruction byte in CPDU */
#define PHCE_T4T_ISO7816_UNSUPPORTED_INSTRUCTION      0x6D00
/** No precise diagnosis */
#define PHCE_T4T_ISO7816_NO_PRECISE_DIAGNOSIS         0x6F00
/** Incorrect Parameter bytes in CPDU */
#define PHCE_T4T_ISO7816_ERR_PARAMETER                0x6A86
/** Incorrect File Name or File ID in CPDU */
#define PHCE_T4T_ISO7816_FILE_NOT_FOUND               0x6A82
/** Unsupported function */
#define PHCE_T4T_ISO7816_UNSUPPORTED_FUNC             0x6A81
/** Length Error */
#define PHCE_T4T_ISO7816_ERR_LENGTH                   0x6700
/** Memory Failure */
#define PHCE_T4T_ISO7816_ERR_MEMORY                   0x6581
/**Command incompatible with file structure */
#define PHCE_T4T_ISO7816_ERR_INCOMPATIBLE_FILE_STRUCT 0x6981
/** Command completed */
#define PHCE_T4T_ISO7816_SUCCESS                      0x9000
/*@}*/

/**
* \name Instruction Byte
*/
/*@{*/
/**Select Instruction*/
#define PHCE_T4T_INS_SELECT      0xA4
/**Read Instruction*/
#define PHCE_T4T_INS_READ        0xB0
/**Update Instruction*/
#define PHCE_T4T_INS_UPDATE      0xD6
/*@}*/

/**
* \name Command Type
*/
/*@{*/
/**Command with a short Length*/
#define PHCE_T4T_SHORT_LEN_CMD    0x00
/**Command with Extended Length*/
#define PHCE_T4T_EXT_LEN_CMD      0x01
/*@}*/

phStatus_t phceT4T_Sw_Int_SetCcFile(
                                    phceT4T_Sw_DataParams_t * pDataParams,
                                    uint8_t *pCcFile,
                                    uint16_t wCcFileId,
                                    uint16_t wCcFileSize,
                                    uint16_t wCcContentLen
                                    );

phStatus_t phceT4T_Sw_Int_SetNdefFile(
                                      phceT4T_Sw_DataParams_t   *pDataParams,
                                      uint8_t *pNdefFile,
                                      uint16_t wNdefFileId,
                                      uint32_t dwNdefFileSize,
                                      uint32_t dwNdefContentLen
                                      );

#ifdef NXPBUILD__PHCE_T4T_PROPRIETARY
phStatus_t phceT4T_Sw_Int_SetProprietaryFile(
                                             phceT4T_Sw_DataParams_t *pDataParams,
                                             uint8_t *pFile,
                                             uint16_t wFileId,
                                             uint16_t wFileSize,
                                             uint16_t wContentLen
                                             );
#endif /* NXPBUILD__PHCE_T4T_PROPRIETARY */

phStatus_t phceT4T_Sw_Int_Activate(
                                   phceT4T_Sw_DataParams_t *pDataParams
                                   );

void phceT4T_Sw_Int_UpdateFile(
                               phceT4T_Sw_DataParams_t *pDataParams,
                               uint8_t *pData,
                               uint32_t dwDataLen
                               );

phStatus_t phceT4T_Sw_Int_Select(
                                 phceT4T_Sw_DataParams_t *pDataParams,
                                 uint8_t *pRxData,
                                 uint16_t wRxDataLen,
                                 uint16_t *pStatusWord
                                 );

phStatus_t phceT4T_Sw_Int_ReadBinary(
                                     phceT4T_Sw_DataParams_t *pDataParams,
                                     uint8_t *pRxData,
                                     uint16_t wRxDataLen,
                                     uint16_t *pStatusWord,
                                     uint8_t **ppTxData,
                                     uint16_t *pTxDataLen
                                     );

phStatus_t phceT4T_Sw_Int_UpdateBinary(
                                       phceT4T_Sw_DataParams_t *pDataParams,
                                       uint16_t wOption,
                                       uint8_t *pRxData,
                                       uint16_t wRxDataLen,
                                       uint16_t *pStatusWord,
                                       uint8_t **ppTxData,
                                       uint16_t *pTxDataLen
                                       );

#endif /* PHCET4T_SW_INT_H */
