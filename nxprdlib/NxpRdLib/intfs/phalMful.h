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
* Generic MIFARE(R) Ultralight Application Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 2673 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  CHu: Generated 05. October 2009
*
*/

#ifndef PHALMFUL_H
#define PHALMFUL_H

#include <ph_Status.h>
#include <phpalMifare.h>
#include <phhalHw.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef NXPBUILD__PHAL_MFUL_SW

/** \defgroup phalMful_Sw Component : Software
* @{
*/

#define PHAL_MFUL_SW_ID         0x01    /**< ID for Software MIFARE Ultralight layer */

/**
* \brief Private parameter structure
*/
typedef struct
{
    uint16_t wId;                   /**< Layer ID for this component, NEVER MODIFY! */
    void * pPalMifareDataParams;    /**< Pointer to palMifare parameter structure. */
    void * pKeyStoreDataParams;     /**< Pointer to phKeystore parameter structure. */
    void * pCryptoDataParams;       /**< Pointer to phCrypto data parameters structure. */
    void * pCryptoRngDataParams;    /**< Pointer to the parameter structure of the CryptoRng layer. */
} phalMful_Sw_DataParams_t;

/**
* \brief Initialise this layer.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
*/
phStatus_t phalMful_Sw_Init(
                            phalMful_Sw_DataParams_t * pDataParams,     /**< [In] Pointer to this layer's parameter structure. */
                            uint16_t wSizeOfDataParams,                 /**< [In] Specifies the size of the data parameter structure. */
                            void * pPalMifareDataParams,                /**< [In] Pointer to palMifare parameter structure. */
                            void * pKeyStoreDataParams,                 /**< [In] Pointer to phKeystore parameter structure. */
                            void * pCryptoDataParams,                   /**< [In] Pointer to phCrypto data parameters structure. */
                            void * pCryptoRngDataParams                 /**< [In] Pointer to the parameter structure of the CryptoRng layer. */
                            );

/** @} */
#endif /* NXPBUILD__PHAL_MFUL_SW */



#ifdef NXPBUILD__PHAL_MFUL

/** \defgroup phalMful MIFARE(R) Ultralight
* \brief These Components implement the MIFARE(R) Ultralight, C, Ultralight EV1 commands.
* @{
*/

/**
* \name Block definitions
*/
/*@{*/
#define PHAL_MFUL_READ_BLOCK_LENGTH         16U    /**< Length of a read MIFARE(R) Ultralight data block. */
#define PHAL_MFUL_WRITE_BLOCK_LENGTH        4U     /**< Length of a write MIFARE(R) Ultralight data block. */
#define PHAL_MFUL_COMPWRITE_BLOCK_LENGTH    16U    /**< Length of a compatibility write MIFARE(R) Ultralight data block. */
#define PHAL_MFUL_COUNTER_RD_VALUE_LENGTH   3U     /**< Length of a counter MIFARE(R) Ultralight Read data block. */
#define PHAL_MFUL_COUNTER_WR_VALUE_LENGTH   4U     /**< Length of a counter MIFARE(R) Ultralight Write data block. */
#define PHAL_MFUL_PACK_LENGTH               2U     /**< Length of a password ack MIFARE(R) Ultralight data block. */
#define PHAL_MFUL_VERSION_LENGTH            8U     /**< Length of a Version MIFARE(R) Ultralight data block. */
#define PHAL_MFUL_SIG_LENGTH                32U    /**< Length of a Version MIFARE(R) Ultralight data block. */
/*@}*/

/**
* \name Crypto definitions
*/
/*@{*/
#define PHAL_MFUL_DES_BLOCK_SIZE    8U      /**< Length of an MIFARE(R) Ultralight DES block. */
#define PHAL_MFUL_DES_KEY_LENGTH    16U     /**< Length of an MIFARE(R) Ultralight DES key. */
/*@}*/

#ifdef NXPRDLIB_REM_GEN_INTFS
#include "../comps/phalMful/src/Sw/phalMful_Sw.h"

#define phalMful_UlcAuthenticate(pDataParams, wKeyNumber, wKeyVersion) \
        phalMful_Sw_UlcAuthenticate((phalMful_Sw_DataParams_t *)pDataParams, wKeyNumber, wKeyVersion)

#define phalMful_Read(pDataParams, bAddress, pData) \
        phalMful_Sw_Read((phalMful_Sw_DataParams_t *)pDataParams, bAddress, pData)

#define phalMful_SectorSelect(pDataParams, bSecNo) \
        phalMful_Sw_SectorSelect((phalMful_Sw_DataParams_t *)pDataParams, bSecNo)

#define phalMful_Write(pDataParams, bAddress, pData) \
        phalMful_Sw_Write((phalMful_Sw_DataParams_t *)pDataParams, bAddress, pData)

#define phalMful_CompatibilityWrite(pDataParams, bAddress, pData) \
        phalMful_Sw_CompatibilityWrite((phalMful_Sw_DataParams_t *)pDataParams, bAddress, pData)

#define phalMful_IncrCnt(pDataParams, bCntNum, pCnt) \
        phalMful_Sw_IncrCnt((phalMful_Sw_DataParams_t *)pDataParams, bCntNum, pCnt)

#define phalMful_ReadCnt(pDataParams, bCntNum, pCntValue) \
        phalMful_Sw_ReadCnt((phalMful_Sw_DataParams_t *)pDataParams, bCntNum, pCntValue)

#define phalMful_PwdAuth(pDataParams, pPwd, pPack) \
        phalMful_Sw_PwdAuth((phalMful_Sw_DataParams_t *)pDataParams, pPwd, pPack)

#define phalMful_GetVersion(pDataParams, pVersion) \
        phalMful_Sw_GetVersion((phalMful_Sw_DataParams_t *)pDataParams, pVersion)

#define phalMful_FastRead(pDataParams, bStartAddr, bEndAddr,pData, pNumBytes) \
        phalMful_Sw_FastRead((phalMful_Sw_DataParams_t *)pDataParams, bStartAddr, bEndAddr,pData, pNumBytes)

#define phalMful_ReadSign(pDataParams, bAddr, pSignature) \
        phalMful_Sw_ReadSign((phalMful_Sw_DataParams_t *)pDataParams, bAddr, pSignature)

#define phalMful_ChkTearingEvent(pDataParams, bCntNum, pValidFlag) \
        phalMful_Sw_ChkTearingEvent((phalMful_Sw_DataParams_t *)pDataParams, bCntNum, pValidFlag)

#else

/**
* \brief Perform MIFARE(R) Ultralight-C Authenticate command with Picc.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval #PH_ERR_AUTH_ERROR Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalMful_UlcAuthenticate(
                                    void * pDataParams,     /**< [In] Pointer to this layer's parameter structure. */
                                    uint16_t wKeyNumber,    /**< [In] Key number to be used in authentication. */
                                    uint16_t wKeyVersion    /**< [In] Key version to be used in authentication. */
                                    );

/**
* \brief Perform MIFARE(R) Ultralight Read command with Picc.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalMful_Read(
                         void * pDataParams,    /**< [In] Pointer to this layer's parameter structure. */
                         uint8_t bAddress,      /**< [In] Address on Picc to read from. */
                         uint8_t * pData        /**< [Out] pData[#PHAL_MFUL_READ_BLOCK_LENGTH] containing data returned from the Picc. */
                         );
/**
* \brief Perform Type 2 tag Sector Select command with Picc.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalMful_SectorSelect(
                                 void * pDataParams,    /**< [In] Pointer to this layer's parameter structure. */
                                 uint8_t bSecNo         /**< [In] Sector number to be selected. */
                                 );
/**
* \brief Perform MIFARE(R) Ultralight Write command with Picc.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalMful_Write(
                          void * pDataParams,   /**< [In] Pointer to this layer's parameter structure. */
                          uint8_t bAddress,     /**< [In] Address on Picc to write to. */
                          uint8_t * pData       /**< [In] pData[#PHAL_MFUL_WRITE_BLOCK_LENGTH] containing block to be written to the Picc. */
                          );

/**
* \brief Perform MIFARE(R) Ultralight Compatibility-Write command with Picc.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalMful_CompatibilityWrite(
                                       void * pDataParams,  /**< [In] Pointer to this layer's parameter structure. */
                                       uint8_t bAddress,    /**< [In] Address on Picc to write to. */
                                       uint8_t * pData      /**< [In] pData[#PHAL_MFUL_COMPWRITE_BLOCK_LENGTH] containing block to be written to the Picc. */
                                       );

/**
* \brief Perform MIFARE(R) Ultralight EV1 Increment count command with Picc.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalMful_IncrCnt(
                            void * pDataParams, /**< [In] Pointer to this layer's parameter structure. */
                            uint8_t bCntNum,    /**< [In] One byte counter number 00 to 02 */
                            uint8_t * pCnt      /**< [In] pCnt[4] counter value LSB first. Only first three data bytes are used by the PICC. The 4th byte is ignored */
                            );

/**
* \brief Perform MIFARE(R) Ultralight EV1 Read count command with Picc.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalMful_ReadCnt(
                            void * pDataParams, /**< [In] Pointer to this layer's parameter structure. */
                            uint8_t bCntNum,    /**< [In] One byte counter number 00 to 02 */
                            uint8_t * pCntValue /**< [Out] pCntValue[3] counter value. LSB first. Three bytes counter value returned from PICC. */
                            );
/**
* \brief Perform MIFARE(R) Ultralight EV1 Password auth command with Picc.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalMful_PwdAuth(
                            void * pDataParams, /**< [In] Pointer to this layer's parameter structure. */
                            uint8_t * pPwd,     /**< [In] Four byte array of type uint8_t containing four password bytes */
                            uint8_t * pPack     /**< [Out] Two byte array of type uint8_t containing two password acknowledge bytes */
                            );

/**
* \brief Perform MIFARE(R) Ultralight EV1 get version command with Picc.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalMful_GetVersion(
                               void * pDataParams, /**< [In] Pointer to this layer's parameter structure. */
                               uint8_t * pVersion  /**< [Out] An eight byte array containing version information bytes */
                               );
/**
* \brief Perform MIFARE(R) Ultralight EV1 Fast read command with Picc.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalMful_FastRead(
                             void * pDataParams,     /**< [In] Pointer to this layer's parameter structure. */
                             uint8_t  bStartAddr,    /**< [In] One byte start address */
                             uint8_t bEndAddr,       /**< [In] One byte end address */
                             uint8_t ** pData,       /**< [Out] Pointer to the data read from the card. The bytes are stored on the HAL's Rx buffer. */
                             uint16_t * wNumBytes    /**< [Out] Contains number of bytes read from the card */
                             );

/**
* \brief Perform MIFARE(R) Ultralight EV1 read signature command with Picc.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalMful_ReadSign(
                             void * pDataParams,     /**< [In] Pointer to this layer's parameter structure. */
                             uint8_t bAddr,          /**< [In] Value is always 00. Present for forward compatibility reasons */
                             uint8_t ** pSignature   /**< [Out] Pointer to a 32 byte signature read from the card */
                             );

/**
* \brief Perform MIFARE(R) Ultralight EV1 check tearing event command with Picc.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalMful_ChkTearingEvent(
                                    void * pDataParams,     /**< [In] Pointer to this layer's parameter structure. */
                                    uint8_t bCntNum,        /**< [In] Value specifying the counter number 00 to 02 */
                                    uint8_t * pValidFlag    /**< [Out] Address of one byte containing the valid flag byte */
                                    );

/** @} */

#endif /* NXPRDLIB_REM_GEN_INTFS */

#endif /* NXPBUILD__PHAL_MFUL */

#ifdef __cplusplus
} /* Extern C */
#endif

#endif /* PHALMFUL_H */
