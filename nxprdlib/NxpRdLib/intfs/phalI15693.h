/*
*         Copyright(c), NXP Semiconductors Gratkorn / Austria
*
*                  (C)NXP Semiconductors
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
* Generic ISO 15693 Application Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 2673 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  CHu: Generated 10. November 2009
*
*/

#ifndef PHALI15693_H
#define PHALI15693_H

#include <ph_Status.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef NXPBUILD__PHAL_I15693_SW

/** \defgroup phalI15693_Sw Component : Software
* @{
*/

#define PHAL_I15693_SW_ID       0x01    /**< ID for Software I15693 layer */

/**
* \brief Private parameter structure
*/
typedef struct
{
    uint16_t wId;                   /**< Layer ID for this component, NEVER MODIFY! */
    void * pPalSli15693DataParams;  /**< Pointer to palSli15693 parameter structure. */
} phalI15693_Sw_DataParams_t;

/**
* \brief Initialize this layer.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
*/
phStatus_t phalI15693_Sw_Init(
    phalI15693_Sw_DataParams_t * pDataParams, /**< [In] Pointer to this layers parameter structure. */
    uint16_t wSizeOfDataParams,               /**< [In] Specifies the size of the data parameter structure. */
    void * pPalSli15693DataParams             /**< [In] Pointer to palSli15693 parameter structure. */
    );

/** @} */
#endif /* NXPBUILD__PHAL_I15693_SW */

#ifdef NXPBUILD__PHAL_I15693

/** \defgroup phalI15693 ISO/IEC 15693
* \brief These Components implement the ISO/IEC 15693 commands.
* @{
*/

/**
* \name Options
*/
/*@{*/
#define PHAL_I15693_OPTION_OFF      PH_OFF                  /**< Option Bit is cleared. */
#define PHAL_I15693_OPTION_ON       PH_ON                   /**< Option Bit is set. */
#define PHAL_I15693_OPTION_DEFAULT  PHAL_I15693_OPTION_OFF  /**< Default Option. */
/*@}*/

#ifdef NXPRDLIB_REM_GEN_INTFS
#include "../comps/phalI15693/src/Sw/phalI15693_Sw.h"

#define phalI15693_ReadSingleBlock(pDataParams, bOption, bBlockNo, ppRxBuffer, pRxLength) \
        phalI15693_Sw_ReadSingleBlock((phalI15693_Sw_DataParams_t *)pDataParams, bOption, bBlockNo, ppRxBuffer, pRxLength)

#define phalI15693_ExtendedReadSingleBlock(pDataParams, bOption, wBlockNo, ppRxBuffer, pRxLength) \
        phalI15693_Sw_ExtendedReadSingleBlock((phalI15693_Sw_DataParams_t *)pDataParams, bOption, wBlockNo, ppRxBuffer, pRxLength)

#define phalI15693_ReadMultipleBlocks(pDataParams, bOption, bBlockNo, wNumBlocks, ppRxBuffer, pRxLength) \
        phalI15693_Sw_ReadMultipleBlocks((phalI15693_Sw_DataParams_t *)pDataParams, bOption, bBlockNo, wNumBlocks, ppRxBuffer, pRxLength)

#define phalI15693_ExtendedReadMultipleBlocks(pDataParams, bOption, wBlockNo, wNumBlocks, ppRxBuffer, pRxLength) \
        phalI15693_Sw_ExtendedReadMultipleBlocks((phalI15693_Sw_DataParams_t *)pDataParams, bOption, wBlockNo, wNumBlocks, ppRxBuffer, pRxLength)

#define phalI15693_WriteSingleBlock(pDataParams, bOption, bBlockNo, pTxBuffer, wTxLength) \
        phalI15693_Sw_WriteSingleBlock((phalI15693_Sw_DataParams_t *)pDataParams, bOption, bBlockNo, pTxBuffer, wTxLength)

#define phalI15693_ExtendedWriteSingleBlock(pDataParams, bOption, wBlockNo, pTxBuffer, wTxLength) \
        phalI15693_Sw_ExtendedWriteSingleBlock((phalI15693_Sw_DataParams_t *)pDataParams, bOption, wBlockNo, pTxBuffer, wTxLength)

#define phalI15693_WriteMultipleBlocks(pDataParams, bOption, bBlockNo, wNumBlocks, pTxBuffer, wTxLength) \
        phalI15693_Sw_WriteMultipleBlocks((phalI15693_Sw_DataParams_t *)pDataParams, bOption, bBlockNo, wNumBlocks, pTxBuffer, wTxLength)

#define phalI15693_ExtendedWriteMultipleBlocks(pDataParams, bOption, wBlockNo, wNumBlocks, pTxBuffer, wTxLength) \
        phalI15693_Sw_ExtendedWriteMultipleBlocks((phalI15693_Sw_DataParams_t *)pDataParams, bOption, wBlockNo, wNumBlocks, pTxBuffer, wTxLength)

#define  phalI15693_LockBlock(pDataParams, bOption, bBlockNo) \
         phalI15693_Sw_LockBlock((phalI15693_Sw_DataParams_t *)pDataParams, bOption, bBlockNo)

#define phalI15693_ExtendedLockBlock(pDataParams, bOption, wBlockNo) \
        phalI15693_Sw_ExtendedLockBlock((phalI15693_Sw_DataParams_t *)pDataParams, bOption, wBlockNo)

#define phalI15693_WriteAFI(pDataParams, bOption, bAfi) \
        phalI15693_Sw_WriteAFI((phalI15693_Sw_DataParams_t *)pDataParams, bOption, bAfi)

#define phalI15693_LockAFI(pDataParams, bOption) \
        phalI15693_Sw_LockAFI((phalI15693_Sw_DataParams_t *)pDataParams, bOption)

#define phalI15693_WriteDSFID(pDataParams, bOption, bDsfid) \
        phalI15693_Sw_WriteDSFID ((phalI15693_Sw_DataParams_t *)pDataParams, bOption, bDsfid)

#define phalI15693_LockDSFID(pDataParams, bOption) \
        phalI15693_Sw_LockDSFID((phalI15693_Sw_DataParams_t *)pDataParams, bOption)

#define phalI15693_GetSystemInformation(pDataParams, ppRxBuffer, pRxLength) \
        phalI15693_Sw_GetSystemInformation((phalI15693_Sw_DataParams_t *)pDataParams, ppRxBuffer, pRxLength)

#define phalI15693_ExtendedGetSystemInformation(pDataParams, ppRxBuffer, pRxLength) \
        phalI15693_Sw_ExtendedGetSystemInformation((phalI15693_Sw_DataParams_t *)pDataParams, ppRxBuffer, pRxLength)

#define phalI15693_GetMultipleBlockSecurityStatus(pDataParams, bBlockNo, wNumBlocks, ppRxBuffer, pRxLength) \
        phalI15693_Sw_GetMultipleBlockSecurityStatus((phalI15693_Sw_DataParams_t *)pDataParams, bBlockNo, wNumBlocks, ppRxBuffer, pRxLength)

#define phalI15693_ExtendedGetMultipleBlockSecurityStatus(pDataParams, wBlockNo, wNumBlocks, ppRxBuffer, pRxLength) \
        phalI15693_Sw_ExtendedGetMultipleBlockSecurityStatus((phalI15693_Sw_DataParams_t *)pDataParams, wBlockNo, wNumBlocks, ppRxBuffer, pRxLength)


#else
/**
* \brief When receiving the Read single block command, the VICC shall read
* the requested block and send back its value in pRxBuffer.
*
* Option_flag can be set by using \ref phpalSli15693_SetConfig command
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_ReadSingleBlock(
    void * pDataParams,       /**< [In] Pointer to this layers parameter structure. */
    uint8_t bOption,          /**< [In] Option flag; \c #PHAL_I15693_OPTION_OFF: disable; \c #PHAL_I15693_OPTION_ON: enable. */
    uint8_t bBlockNo,         /**< [In] Block number. */
    uint8_t ** ppRxBuffer,    /**< [Out] Pointer to received data. */
    uint16_t * pRxLength      /**< [Out] number of received data bytes. */
    );

/**
* \brief When receiving the Extended read single block command, the VICC shall read
* the requested block and send back its value in pRxBuffer.
*
* This command shall be used for extended memory tags.
* Option_flag can be set by using \ref phpalSli15693_SetConfig command
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_ExtendedReadSingleBlock (
    void * pDataParams,      /**< [In] Pointer to this layers parameter structure. */
    uint8_t bOption,         /**< [In] Option flag; \c #PHAL_I15693_OPTION_OFF: disable; \c #PHAL_I15693_OPTION_ON: enable. */
    uint16_t wBlockNo,       /**< [In] Block number. */
    uint8_t ** ppRxBuffer,   /**< [Out] Pointer to received data. */
    uint16_t * pRxLength     /**< [Out] number of received data bytes. */
    );

/**
* \brief When receiving the Read multiple blocks command, the VICC shall read
* the requested block(s) and send back their value in pRxBuffer.
*
* Option_flag can be set by using \ref phpalSli15693_SetConfig command
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_ReadMultipleBlocks(
    void * pDataParams,     /**< [In] Pointer to this layers parameter structure. */
    uint8_t bOption,        /**< [In] Option flag; \c #PHAL_I15693_OPTION_OFF: disable; \c #PHAL_I15693_OPTION_ON: enable. */
    uint8_t bBlockNo,       /**< [In] First Block number. */
    uint16_t wNumBlocks,    /**< [In] Number of blocks. */
    uint8_t ** ppRxBuffer,  /**< [Out] Pointer to received data. */
    uint16_t * pRxLength    /**< [Out] number of received data bytes. */
    );

/**
* \brief When receiving the Extended read multiple blocks command, the VICC shall read
* the requested block(s) and send back their value in pRxBuffer.
*
* This command shall be used for extended memory tags.
* Option_flag can be set by using \ref phpalSli15693_SetConfig command
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_ExtendedReadMultipleBlocks (
    void * pDataParams,     /**< [In] Pointer to this layers parameter structure. */
    uint8_t bOption,        /**< [In] Option flag; \c #PHAL_I15693_OPTION_OFF: disable; \c #PHAL_I15693_OPTION_ON: enable. */
    uint16_t wBlockNo,      /**< [In] First Block number. */
    uint16_t wNumBlocks,    /**< [In] Number of blocks. */
    uint8_t ** ppRxBuffer,  /**< [Out] Pointer to received data. */
    uint16_t * pRxLength    /**< [Out] number of received data bytes. */
    );

/**
* \brief When receiving the Write single block command, the VICC shall
* write the requested block with the data contained in the pTxBuffer
*
* Option_flag can be set by using \ref phpalSli15693_SetConfig command
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_WriteSingleBlock(
    void * pDataParams,  /**< [In] Pointer to this layers parameter structure. */
    uint8_t bOption,     /**< [In] Option flag; \c #PHAL_I15693_OPTION_OFF: disable; \c #PHAL_I15693_OPTION_ON: enable. */
    uint8_t bBlockNo,    /**< [In] Block number. */
    uint8_t * pTxBuffer, /**< [In] Data to transmit. */
    uint16_t wTxLength   /**< [In] number of bytes to send */
    );

/**
* \brief When receiving the Extended write single block command, the VICC shall
* write the requested block with the data contained in the pTxBuffer
*
* This command shall be used for extended memory tags.
* Option_flag can be set by using \ref phpalSli15693_SetConfig command
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_ExtendedWriteSingleBlock(
    void * pDataParams,  /**< [In] Pointer to this layers parameter structure. */
    uint8_t bOption,     /**< [In] Option flag; \c #PHAL_I15693_OPTION_OFF: disable; \c #PHAL_I15693_OPTION_ON: enable. */
    uint16_t wBlockNo,   /**< [In] Block number. */
    uint8_t * pTxBuffer, /**< [In] Data to transmit. */
    uint16_t wTxLength   /**< [In] number of bytes to send */
    );

/**
* \brief When receiving the Write multiple blocks command, the VICC shall
* write the requested block(s) with the data contained in the pTxBuffer
*
* Option_flag can be set by using \ref phpalSli15693_SetConfig command
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_WriteMultipleBlocks(
    void * pDataParams,     /**< [In] Pointer to this layers parameter structure. */
    uint8_t bOption,        /**< [In] Option flag; \c #PHAL_I15693_OPTION_OFF: disable; \c #PHAL_I15693_OPTION_ON: enable. */
    uint8_t bBlockNo,       /**< [In] First Block number. */
    uint16_t wNumBlocks,    /**< [In] Number of blocks (real number, is automatically decremented by one). */
    uint8_t * pTxBuffer,    /**< [In] Data to transmit. */
    uint16_t wTxLength      /**< [In] number of bytes to send */
    );

/**
* \brief When receiving the Extended write multiple blocks command, the VICC shall
* write the requested block(s) with the data contained in the pTxBuffer
*
* This command shall be used for extended memory tags.
* Option_flag can be set by using \ref phpalSli15693_SetConfig command
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_ExtendedWriteMultipleBlocks (
    void * pDataParams,     /**< [In] Pointer to this layers parameter structure. */
    uint8_t bOption,        /**< [In] Option flag; \c #PHAL_I15693_OPTION_OFF: disable; \c #PHAL_I15693_OPTION_ON: enable. */
    uint16_t wBlockNo,      /**< [In] First Block number. */
    uint16_t wNumBlocks,    /**< [In] Number of blocks (real number, is automatically decremented by one). */
    uint8_t * pTxBuffer,    /**< [In] Data to transmit. */
    uint16_t wTxLength      /**< [In] number of bytes to send */
    );

/**
* \brief When receiving the Lock block command, the VICC shall lock
* permanently the requested block.
*
* Option_flag can be set by using \ref phpalSli15693_SetConfig command
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_LockBlock(
    void * pDataParams, /**< [In] Pointer to this layers parameter structure. */
    uint8_t bOption,    /**< [In] Option flag; \c #PHAL_I15693_OPTION_OFF: disable; \c #PHAL_I15693_OPTION_ON: enable. */
    uint8_t bBlockNo    /**< [In] Block number. */
    );

/**
* \brief When receiving the Extended lock block command, the VICC shall lock
* permanently the requested block for extended memory tags.
*
* This command shall be used for extended memory tags.
* Option_flag can be set by using \ref phpalSli15693_SetConfig command
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_ExtendedLockBlock (
    void * pDataParams, /**< [In] Pointer to this layers parameter structure. */
    uint8_t bOption,    /**< [In] Option flag; \c #PHAL_I15693_OPTION_OFF: disable; \c #PHAL_I15693_OPTION_ON: enable. */
    uint16_t wBlockNo   /**< [In] Block number. */
    );

/**
* \brief When receiving the Write AFI request, the VICC shall write the
* AFI value into its memory.
*
* Option_flag can be set by using \ref phpalSli15693_SetConfig command
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_WriteAFI(
    void * pDataParams,  /**< [In] Pointer to this layers parameter structure. */
    uint8_t bOption,     /**< [In] Option flag; \c #PHAL_I15693_OPTION_OFF: disable; \c #PHAL_I15693_OPTION_ON: enable. */
    uint8_t bAfi         /**< [In] Value of Application Family Identifier. */
    );

/**
* \brief When receiving the Lock AFI request, the VICC shall lock the
* AFI value permanently into its memory.
*
* Option_flag can be set by using \ref phpalSli15693_SetConfig command
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_LockAFI(
    void * pDataParams,   /**< [In] Pointer to this layers parameter structure. */
    uint8_t bOption       /**< [In] Option flag; \c #PHAL_I15693_OPTION_OFF: disable; \c #PHAL_I15693_OPTION_ON: enable. */
    );

/**
* \brief When receiving the Write DSFID request, the VICC shall write
* the DSFID value into its memory.
*
* Option_flag can be set by using \ref phpalSli15693_SetConfig command
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalI15693_WriteDSFID(
    void * pDataParams,    /**< [In] Pointer to this layers parameter structure. */
    uint8_t bOption,       /**< [In] Option flag; \c #PHAL_I15693_OPTION_OFF: disable; \c #PHAL_I15693_OPTION_ON: enable. */
    uint8_t bDsfid         /**< [In] Value of DSFID(data storage format identifier). */
    );

/**
* \brief When receiving the Lock DSFID request, the VICC shall lock the
* DSFID value permanently into its memory.
*
* Option_flag can be set by using \ref phpalSli15693_SetConfig command
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_LockDSFID(
    void * pDataParams, /**< [In] Pointer to this layers parameter structure. */
    uint8_t bOption     /**< [In] Option flag; \c #PHAL_I15693_OPTION_OFF: disable; \c #PHAL_I15693_OPTION_ON: enable. */
    );

/**
* \brief This command allows for retrieving the system information value
* from the VICC.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_GetSystemInformation(
    void * pDataParams,     /**< [In] Pointer to this layers parameter structure. */
    uint8_t ** ppRxBuffer,  /**< [Out] Pointer to received data. */
    uint16_t * pRxLength    /**< [Out] number of received data bytes. */
    );

/**
* \brief This command allows for retrieving the system information value
* from the VICC for extended memory tags.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_ExtendedGetSystemInformation (
    void * pDataParams,     /**< [In] Pointer to this layers parameter structure. */
    uint8_t ** ppRxBuffer,  /**< [Out] Pointer to received data. */
    uint16_t * pRxLength    /**< [Out] number of received data bytes. */
    );

/**
* \brief When receiving the Get multiple block security status command,
* the VICC shall send back the block security status.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_GetMultipleBlockSecurityStatus(
    void * pDataParams,     /**< [In] Pointer to this layers parameter structure. */
    uint8_t bBlockNo,       /**< [In] Block number. */
    uint16_t wNumBlocks,    /**< [In] Number of blocks. */
    uint8_t ** ppRxBuffer,  /**< [Out] Pointer to received data. */
    uint16_t * pRxLength    /**< [Out] number of received data bytes. */
    );

/**
* \brief When receiving the Extended get multiple block security status command,
* the VICC shall send back the block security status (for extended memory tags).
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalI15693_ExtendedGetMultipleBlockSecurityStatus (
    void * pDataParams,     /**< [In] Pointer to this layers parameter structure. */
    uint16_t wBlockNo,      /**< [In] Block number. */
    uint16_t wNumBlocks,    /**< [In] Number of blocks. */
    uint8_t ** ppRxBuffer,  /**< [Out] Pointer to received data. */
    uint16_t * pRxLength    /**< [Out] number of received data bytes. */
    );

/** @} */

#endif /* NXPRDLIB_REM_GEN_INTFS */

#endif /* NXPBUILD__PHAL_I15693 */

#ifdef __cplusplus
} /* Extern C */
#endif

#endif /* PHALI15693_H */
