/*
*         Copyright (c), NXP Semiconductors Bangalore / India
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
* This File contains all functions that's needed by Discovery Loop
* for Starting the discovery loop, Set & Get Configuration and Card Activation,
* as part of the Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 2673 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  PC: Generated 23. Aug 2012
*
*/
#ifndef PHACDICLOOP_SW_H
#define PHACDICLOOP_SW_H

#ifdef NXPBUILD__PHAC_DISCLOOP_SW

#define PHAC_DISCLOOP_SW_TYPEF_MAX_TSN                          16U    /**< Maximum time slot number for Type F. */
#define PHAC_DISCLOOP_SW_I3P4_MAX_CID                           14U    /**< Maximum permissible size of CID(Card IDentifier). */
#define PHAC_DISCLOOP_SW_I3P4_MAX_DSI                            3U    /**< Maximum permissible size of DSI(Divisor Send Integer). */
#define PHAC_DISCLOOP_SW_I3P4_MAX_DRI                            3U    /**< Maximum permissible size of DRI(Divisor Receive Integer). */

/* Default device limit for poll technologies */
#define PHAC_DISCLOOP_TYPEA_DEFAULT_DEVICE_LIMIT              0x01U    /**< The default value loaded for device limit for Type A tags. */
#define PHAC_DISCLOOP_TYPEB_DEFAULT_DEVICE_LIMIT              0x01U    /**< Default value for maximum number of TypeB tags that can be detected. */
#define PHAC_DISCLOOP_TYPEF_DEFAULT_DEVICE_LIMIT              0x01U    /**< Default value for maximum number of TypeF tags that can be detected. */
#define PHAC_DISCLOOP_TYPEV_DEFAULT_DEVICE_LIMIT              0x01U    /**< The default value loaded for device limit for Type V tags. */
#define PHAC_DISCLOOP_I18000P3M3_DEFAULT_DEVICE_LIMIT         0x01U    /**< The default value loaded for device limit for 18000p3m3 tags. */

/* Default guard times */
/* As per Nfc Forum Activity Spec 1.1 */
#define PHAC_DISCLOOP_TYPEA_DEFAULT_GT                        5100U    /**< Default guard time for Type A tech in Micro seconds.*/
#define PHAC_DISCLOOP_TYPEB_DEFAULT_GT                        5100U    /**< Default guard time for Type B tech in Micro seconds.*/
#define PHAC_DISCLOOP_TYPEF_DEFAULT_GT                       20400U    /**< Default guard time for Type F tech in Micro seconds.*/
#define PHAC_DISCLOOP_B_TO_F_DEFAULT_GT                      15300U    /**< Default guard time for NFC-F device detection preceded by NFC-B type. */
#define PHAC_DISCLOOP_TYPEV_DEFAULT_GT                        5200U    /**< Default guard time for Type V tech in Micro seconds.*/
#define PHAC_DISCLOOP_I18000P3M3_DEFAULT_GT                  10000U    /**< Default guard time for 18000p3m3 tech in Micro seconds.*/

/* Rf Reset delay */
#define PHAC_DISCLOOP_EMVCO_RESET_DELAY                       5100U    /**< Time during which device stops sending the carrier to perform a PICC Reset.*/
/**
 * \brief Function to start the discovery loop
 *
 * \return Error code
 * \retval #PH_ERR_SUCCESS
 * \retval Other errors depends on component.
 *         For more info about various components present refer
 *         NFCReaderlibrary.chm,
 *         "Component identifiers" under the section "Generic Definitions".
 *         For info about Error Codes Refer "Error Code Collection"
 */
phStatus_t phacDiscLoop_Sw_Run(
    phacDiscLoop_Sw_DataParams_t * pDataParams,            /**< [In] pointer to this layer's parameter structure. */
    uint8_t bEntryPoint                                    /**< [In] Entry point to start Discovery Loop. */
    );

/**
 * \brief   Function to Get the configuration value
 *
 * \return  Error code
 *
 * \retval #PH_ERR_SUCCESS         operation successful
 * \retval #PH_ERR_PROTOCOL_ERROR  Invalid option/response received
 * \retval Other errors depends on component.
 *         For more info about various components present refer
 *         NFCReaderlibrary.chm,
 *         "Component identifiers" under the section "Generic Definitions".
 *         For info about Error Codes Refer "Error Code Collection"
 */
phStatus_t phacDiscLoop_Sw_GetConfig(
    phacDiscLoop_Sw_DataParams_t *  pDataParams,           /**< [In] Pointer to this layer's parameter structure */
    uint16_t wConfig,                                      /**< [In] Configuration identifier */
    uint16_t *pValue                                       /**< [Out] Configuration value */
    );

/**
 * \brief Function to Set the configuration value
 *
 * \return Error code
 * \retval #PH_ERR_SUCCESS            operation successful
 * \retval #PH_ERR_INVALID_PARAMETER  Invalid option/response received
 * \retval Other errors depends on component.
 *         For more info about various components present refer
 *         NFCReaderlibrary.chm,
 *         "Component identifiers" under the section "Generic Definitions".
 *         For info about Error Codes Refer "Error Code Collection"
 */
phStatus_t phacDiscLoop_Sw_SetConfig(
    phacDiscLoop_Sw_DataParams_t *  pDataParams,           /**< [In] Pointer to this layer's parameter structure */
    uint16_t wConfig,                                      /**< [In] Configuration identifier */
    uint16_t wValue                                        /**< [In] Configuration value */
    );

/**
 * \brief Activates the given tag type with given index
 *
 * \return Error code
 * \retval #PH_ERR_SUCCESS      On success
 * \retval #PH_ERR_INVALID_ARG  If invalid index or card type was given
 */
phStatus_t phacDiscLoop_Sw_ActivateCard(
    phacDiscLoop_Sw_DataParams_t *  pDataParams,           /**< [In] Pointer to this layer's parameter structure */
    uint8_t bTagType,                                      /**< [In] Tag type could be Type A,B or F */
    uint8_t bTagIndex                                      /**< [In] The tag which has to be activated */
    );

/**
 * \brief Stop and De-Initializes the discovery loop software component.
 * \return Status code
 * \retval #PH_ERR_SUCCESS if operation was successful.
 * \retval #PH_ERR_INVALID_PARAMETER  Invalid option/response received
 * \retval other Other errors depending on underlying component
 */
phStatus_t phacDiscLoop_Sw_DeInit(
    phacDiscLoop_Sw_DataParams_t * pDataParams             /**< [In] Pointer to this layer's parameter structure */
    );

/**
 * \brief Stop discovery loop
 * \return Status code
 * \retval #PH_ERR_SUCCESS if operation was successful.
 * \retval #PH_ERR_INVALID_PARAMETER  Invalid option/response received
 */
phStatus_t phacDiscLoop_Sw_Stop(
    phacDiscLoop_Sw_DataParams_t * pDataParams             /**< [In] Pointer to this layer's parameter structure */
    );

#endif /* NXPBUILD__PHAC_DISCLOOP_SW */
#endif /* PHACDICLOOP_H */
