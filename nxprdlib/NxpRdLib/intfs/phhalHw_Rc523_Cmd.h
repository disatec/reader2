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
* Command functions for Rc523 specific HAL-Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 2673 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  CHu: Generated 19. April 2010
*
*/

#ifndef PHHALHW_RC523_CMD_H
#define PHHALHW_RC523_CMD_H

#include <ph_Status.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef  NXPBUILD__PHHAL_HW_RC523

/** \defgroup phhalHw_Rc523_Cmd Commands
* \brief Supported ReaderIC commands.
* @{
*/

/**
* \brief Transfers either 25 bytes into internal buffer or returns 25 bytes from internal buffer.
*
* If \c bDataLength equals zero, 25 bytes from the internal buffer are put into \c pDataOut.\n
* If \c bDataLength is 25, 25 bytes from \c pDataIn are copied into the internal buffer.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval #PH_ERR_INVALID_PARAMETER \c bDataLength is neither zero nor 25.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phhalHw_Rc523_Cmd_Mem(
                                 phhalHw_Rc523_DataParams_t * pDataParams,  /**< [In] Pointer to this layer's parameter structure. */
                                 uint8_t * pDataIn,                         /**< [In] 25 bytes In-data or ignored if \c bDataLength equals zero. */
                                 uint8_t bDataLength,                       /**< [In] Data length. Either zero or 25. */
                                 uint8_t * pDataOut                         /**< [Out] 25 bytes Out-data or ignored if \c bDataLength is nonzero. */
                                 );

/**
* \brief Generates a 10 byte random ID.
*
* The generated RandomId is stored in the internal buffer (the first 10 bytes get overridden).\n
* It can be retrieved using the function \ref phhalHw_Rc523_Cmd_Mem.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phhalHw_Rc523_Cmd_GenerateRandomId(
    phhalHw_Rc523_DataParams_t * pDataParams    /**< [In] Pointer to this layer's parameter structure. */
    );

/**
* \brief Calculates a CRC over the input data and returns the result.
*
* Note: The CRC calculation parameters are taken from the respective registers.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phhalHw_Rc523_Cmd_CalcCrc(
                                     phhalHw_Rc523_DataParams_t * pDataParams,  /**< [In] Pointer to this layer's parameter structure. */
                                     uint8_t * pData,                           /**< [In] CRC input data. */
                                     uint16_t wDataLength,                      /**< [In] Length of input data. */
                                     uint16_t * pCrcResult                      /**< [Out] Calculated CRC. */
                                     );

/**
* \brief Performs a Softreset.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phhalHw_Rc523_Cmd_SoftReset(
                                       phhalHw_Rc523_DataParams_t * pDataParams /**< [In] Pointer to this layer's parameter structure. */
                                      );

/**
* \brief Stores data for \ref phhalHw_Autocoll (i.e. phhalHw_Rc523_Autocoll) command into internal buffer.
*
* \b Note: Only applicable if in Target mode.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval #PH_ERR_INVALID_PARAMETER \c bDataLength is neither zero nor 25.
* \retval Other Depending on implementation and under-laying component.
*/
phStatus_t phhalHw_Rc523_SetListenParameters(
                                    phhalHw_Rc523_DataParams_t * pDataParams,   /**< [In] Pointer to this layer's parameter structure. */
                                    uint8_t * pSensRes,                         /**< [In] 2 bytes. */
                                    /** [In] 3 bytes; The first NfcId1 byte is fixed to \c 08h and the check byte is calculated automatically. */
                                    uint8_t * pNfcId1,
                                    uint8_t SelRes,                             /**< [In] 1 byte. */
                                    /** [In] 2 bytes (shall be 01h, FEh) + 6 bytes NfcId2 + 8 bytes Pad + 2 bytes SystemCode. */
                                    uint8_t * pPollingResp,
                                    uint8_t bNfcId3                             /**< [In] 1 byte. */
                                       );

/** @} */
#endif /* NXPBUILD__PHHAL_HW_RC523 */

#ifdef __cplusplus
} /* Extern C */
#endif

#endif /* PHHALHW_RC523_CMD_H */
