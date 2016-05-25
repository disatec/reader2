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
* Generic ICode SLI Application Component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 2673 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  CHu: Generated 10. November 2009
*
*/

#ifndef PHALSLI_H
#define PHALSLI_H

#include <ph_Status.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef NXPBUILD__PHAL_SLI_SW

/** \defgroup phalSli_Sw Component : Software
* @{
*/

#define PHAL_SLI_SW_ID  0x01    /**< ID for Software ICode SLI layer. */

/**
* \brief Private parameter structure
*/
typedef struct
{
    uint16_t wId;                   /**< Layer ID for this component, NEVER MODIFY! */
    void * pPalSli15693DataParams;  /**< Pointer to palSli15693 parameter structure. */
} phalSli_Sw_DataParams_t;

/**
* \brief Initialise this layer.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
*/
phStatus_t phalSli_Sw_Init(
    phalSli_Sw_DataParams_t * pDataParams,   /**< [In] Pointer to this layer's parameter structure. */
    uint16_t wSizeOfDataParams,              /**< [In] Specifies the size of the data parameter structure. */
    void * pPalSli15693DataParams            /**< [In] Pointer to palSli15693 parameter structure. */
    );

/** @} */
#endif /* NXPBUILD__PHAL_SLI_SW */

#ifdef NXPBUILD__PHAL_SLI

/** \defgroup phalSli ICode SLI
* \brief These Components implement the ICode SLI commands.
* @{
*/

/**
* \name Options
*/
/*@{*/
#define PHAL_SLI_OPTION_OFF         PH_OFF              /**< Option Bit is cleared. */
#define PHAL_SLI_OPTION_ON          PH_ON               /**< Option Bit is set. */
#define PHAL_SLI_OPTION_DEFAULT     PHAL_SLI_OPTION_OFF /**< Default Option. */
/*@}*/

/**
* \name Block definitions
*/
/*@{*/
#define PHAL_SLI_BLOCK_SIZE     4U  /**< Length of an ICode SLI data block. */
/*@}*/

/**
* \name Password identifiers
*/
/*@{*/
#define PHAL_SLI_PWD_READ       0x01U   /**< Read Password. */
#define PHAL_SLI_PWD_WRITE      0x02U   /**< Write Password. */
#define PHAL_SLI_PWD_PRIVACY    0x04U   /**< Privacy Password. */
#define PHAL_SLI_PWD_DESTROY    0x08U   /**< Destroy Password. */
#define PHAL_SLI_PWD_EAS        0x10U   /**< EAS Password. */
/*@}*/

/**
* \name Protection status
*/
/*@{*/
#define PHAL_SLI_PROTECTION_PUBLIC              0x00U   /**< No protection. */
#define PHAL_SLI_PROTECTION_READ_PWD_ONLY       0x01U   /**< Read and Write protected by the Read password. */
#define PHAL_SLI_PROTECTION_WRITE_PWD_ONLY      0x10U   /**< Write protected by the Write password. */
#define PHAL_SLI_PROTECTION_READ_AND_WRITE_PWD  0x11U   /**< Read protected by the Read password, Write protected by the Write password. */
/*@}*/

/**
* \name Block Protection bits
*/
/*@{*/
#define PHAL_SLI_PROTECTION_BIT_BLOCK_LOCK      0x01U   /**< Block is locked. */
#define PHAL_SLI_PROTECTION_BIT_READ            0x02U   /**< Block is read protected. */
#define PHAL_SLI_PROTECTION_BIT_WRITE           0x04U   /**< Block is write protected. */
#define PHAL_SLI_PROTECTION_BIT_PROT_LOCK       0x08U   /**< Page protection is locked. */
/*@}*/

#ifdef NXPRDLIB_REM_GEN_INTFS
#include "../comps/phalSli/src/Sw/phalSli_Sw.h"

#define phalSli_GetRandomNumber(pDataParams, pRnd) \
        phalSli_Sw_GetRandomNumber((phalSli_Sw_DataParams_t *)pDataParams, pRnd)

#define phalSli_SetPWD(pDataParams, bPwdIdentifier, pXorPwd) \
        phalSli_Sw_SetPWD((phalSli_Sw_DataParams_t *)pDataParams, bPwdIdentifier, pXorPwd)

#define phalSli_WritePWD(pDataParams, bPwdIdentifier, pPwd) \
        phalSli_Sw_WritePWD((phalSli_Sw_DataParams_t *)pDataParams, bPwdIdentifier, pPwd)

#define phalSli_LockPWD(pDataParams, bPwdIdentifier) \
        phalSli_Sw_LockPWD((phalSli_Sw_DataParams_t *)pDataParams, bPwdIdentifier)

#define phalSli_64BitPWDProtection(pDataParams) \
        phalSli_Sw_64BitPWDProtection((phalSli_Sw_DataParams_t *)pDataParams)

#define phalSli_ProtectPage(pDataParams, bPageNo, bProtectionStatus) \
        phalSli_Sw_ProtectPage((phalSli_Sw_DataParams_t *)pDataParams, bPageNo, bProtectionStatus)

#define phalSli_LockPageProtectionCondition(pDataParams, bPageNo) \
        phalSli_Sw_LockPageProtectionCondition((phalSli_Sw_DataParams_t *)pDataParams, bPageNo)

#define phalSli_GetMultipleBlockProtStatus(pDataParams, bBlockNo, wNumBlocks, pProtectionStates, pNumReceivedStates) \
        phalSli_Sw_GetMultipleBlockProtStatus((phalSli_Sw_DataParams_t *)pDataParams, bBlockNo, wNumBlocks, pProtectionStates, pNumReceivedStates)

#define phalSli_Destroy(pDataParams) \
        phalSli_Sw_Destroy((phalSli_Sw_DataParams_t *)pDataParams)

#define  phalSli_EnablePrivacy(pDataParams) \
         phalSli_Sw_EnablePrivacy((phalSli_Sw_DataParams_t *)pDataParams)

#define phalSli_ReadSignature(pDataParams, pSignature) \
        phalSli_Sw_ReadSignature((phalSli_Sw_DataParams_t *)pDataParams, pSignature)

#define  phalSli_SetEAS(pDataParams) \
         phalSli_Sw_SetEAS((phalSli_Sw_DataParams_t *)pDataParams)

#define phalSli_ResetEAS(pDataParams) \
        phalSli_Sw_ResetEAS((phalSli_Sw_DataParams_t *)pDataParams)

#define phalSli_LockEAS(pDataParams) \
        phalSli_Sw_LockEAS((phalSli_Sw_DataParams_t *)pDataParams)

#define phalSli_EASAlarm(pDataParams, bOption, pEasIdValue, bEasIdMaskLength, pEas) \
        phalSli_Sw_EASAlarm((phalSli_Sw_DataParams_t *)pDataParams, bOption, pEasIdValue, bEasIdMaskLength, pEas)

#define phalSli_PwdProtectEAS(pDataParams) \
        phalSli_Sw_PwdProtectEAS((phalSli_Sw_DataParams_t *)pDataParams)

#define phalSli_WriteEAS_ID(pDataParams, pEasIdValue) \
        phalSli_Sw_WriteEAS_ID((phalSli_Sw_DataParams_t *)pDataParams, pEasIdValue)

#define phalSli_ReadEPC(pDataParams, pEpc) \
        phalSli_Sw_ReadEPC((phalSli_Sw_DataParams_t *)pDataParams, pEpc)

#define phalSli_GetNxpSystemInfo(pDataParams, pProtectionPointer, pProtectionCondition, pLockBits, pFeatureFlags) \
        phalSli_Sw_GetNxpSystemInfo((phalSli_Sw_DataParams_t *)pDataParams, pProtectionPointer, pProtectionCondition, pLockBits, pFeatureFlags)

#define phalSli_GetRandomNumberSetPWD(pDataParams, bPwdIdentifier, pPwd) \
        phalSli_Sw_GetRandomNumberSetPWD((phalSli_Sw_DataParams_t *)pDataParams, bPwdIdentifier, pPwd)

#else


/**
* \brief This command generates a random number from the label IC.
*
* The received random number shall be used to diversify the
* password for the \ref phalSli_SetPWD command.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_GetRandomNumber(
    void * pDataParams,  /**< [In] Pointer to this layer's parameter structure. */
    uint8_t * pRnd       /**< [Out] Random number; 16 bits. */
    );

/**
* \brief This command transmits different passwords to the label to get access to protected functionalities.
*
* This command has to be executed just once for the related passwords if the label is powered.
* The different passwords are addressed with the Password Identifier.
\verbatim
[XOR password calculation example]
pXorPwd[0] = pPassword[0] ^ pRnd[0];
pXorPwd[1] = pPassword[1] ^ pRnd[1];
pXorPwd[2] = pPassword[2] ^ pRnd[0];
pXorPwd[3] = pPassword[3] ^ pRnd[1];
\endverbatim
*
* \b Remark: This command can only be executed in addressed or selected mode except of Privay Password.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_SetPWD(
    void * pDataParams,       /**< [In] Pointer to this layer's parameter structure. */
    uint8_t bPwdIdentifier,   /**< [In] Password Identifier; Refer to the PHAL_SLI_PWD_* definitions. */
    uint8_t * pXorPwd         /**< [In] XOR Password; 32 bits. */
    );

/**
* \brief With this command, a new password is written into the related memory.
*
* Note that the old password has to be transmitted before with \ref phalSli_SetPWD.
* The new password takes effect immediately which means that the new password has to be
* transmitted with \ref phalSli_SetPWD to get access to protected blocks/pages.
* \b Remark: This command can only be executed in addressed or selected mode.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_WritePWD(
    void * pDataParams,         /**< [In] Pointer to this layer's parameter structure. */
    uint8_t bPwdIdentifier,     /**< [In] Password Identifier; Refer to the PHAL_SLI_PWD_* definitions. */
    uint8_t * pPwd              /**< [In] Plain Password; 32 bits. */
    );

/**
* \brief This command locks the addressed password.
*
* Note that the addressed password has to be transmitted before with \ref phalSli_SetPWD.
* A locked password can not be changed any longer.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_LockPWD(
    void * pDataParams,      /**< [In] Pointer to this layer's parameter structure. */
    uint8_t bPwdIdentifier   /**< [In] Password Identifier; Refer to the PHAL_SLI_PWD_* definitions. */
    );

/**
* \brief This instructs the label that both of the Read and Write passwords are required for protected access.
*
* Note that both the Read and Write passwords have to be
* transmitted before with \ref phalSli_SetPWD.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_64BitPWDProtection(
    void * pDataParams    /**< [In] Pointer to this layer's parameter structure. */
    );

/**
* \brief This command changes the protection status of a page.
*
* Note that the related passwords have to be transmitted before
* with \ref phalSli_SetPWD if the page is not public.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_ProtectPage(
    void * pDataParams,          /**< [In] Pointer to this layer's parameter structure. */
    uint8_t bPageNo,             /**< [In] Page number. */
    uint8_t bProtectionStatus    /**< [In] New protection status; Refer to the PHAL_SLI_PROTECTION_* definitions. */
    );

/**
* \brief This command permanenty locks the protection status of a page.
*
* Note that the related passwords have to be transmitted before
* with \ref phalSli_SetPWD if the page is not public.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_LockPageProtectionCondition(
    void * pDataParams,     /**< [In] Pointer to this layer's parameter structure. */
    uint8_t bPageNo         /**< [In] Page number. */
    );

/**
* \brief This instructs the label to return the block protection status of the requested blocks.
*
* \b Remark: If \c bBlockNo + \c bNumBlocks exceeds the total available number of
* user blocks, the number of received status bytes is less than the requested number.
* This means that the last returned status byte corresponds to the highest available user block.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_GetMultipleBlockProtStatus(
    void * pDataParams,             /**< [In] Pointer to this layer's parameter structure. */
    uint8_t bBlockNo,               /**< [In] First Block number. */
    uint16_t wNumBlocks,            /**< [In] Number of blocks. */
    uint8_t * pProtectionStates,    /**< [Out] Protection states of requested blocks. Refer to the PHAL_SLI_PROTECTION_BIT_* definitions. */
    uint8_t * pNumReceivedStates    /**< [Out] Number of received block protection states. */
    );

/**
* \brief This command permanently destroys the label.
*
* The Destroy password has to be transmitted before with \ref phalSli_SetPWD.
* \b Remark: This command is irreversible and the label will never respond to any command again.
* \b Remark: This command can only be executed in addressed or selected mode.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_Destroy(
    void * pDataParams   /**< [In] Pointer to this layer's parameter structure. */
    );

/**
* \brief This command instructs the label to enter privacy mode.
*
* In privacy mode, the label will only respond to \ref phalSli_GetRandomNumber and \ref phalSli_SetPWD commands.
* To get out of the privacy mode, the Privacy password has to be transmitted before with \ref phalSli_SetPWD.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_EnablePrivacy(
    void * pDataParams     /**< [In] Pointer to this layer's parameter structure. */
    );

/**
* \brief The command returns an IC specific, 32-byte ECC signature, to verify
* NXP Semiconductors as the silicon vendor..
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalSli_ReadSignature(
    void * pDataParams,     /**< [In] Pointer to this layer's parameter structure. */
    uint8_t * pSignature    /**< [Out] Signature; 256 bits. */
    );

/**
* \brief This command enables the EAS mode if the EAS mode is not locked.
*
* If the EAS mode is password protected the EAS password has to be
* transmitted before with \ref phalSli_SetPWD.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_SetEAS(
    void * pDataParams    /**< [In] Pointer to this layer's parameter structure. */
    );

/**
* \brief This command disables the EAS mode if the EAS mode is not locked.
*
* If the EAS mode is password protected the EAS password has to be
* transmitted before with \ref phalSli_SetPWD.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_ResetEAS(
    void * pDataParams  /**< [In] Pointer to this layer's parameter structure. */
    );

/**
* \brief This command locks the current state of the EAS mode and the EAS ID.
*
* If the EAS mode is password protected the EAS password has to be
* transmitted before with \ref phalSli_SetPWD.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_LockEAS(
    void * pDataParams   /**< [In] Pointer to this layer's parameter structure. */
    );

/**
* \brief This command returns the EAS sequence if the EAS mode is enabled.
*
* \li \c bOption disabled: \c bEasIdMaskLength and \c pEasIdValue are not transmitted, EAS Sequence is returned;
* \li \c bOption enabled and \c bEasIdMaskLength = \c 0: EAS ID is returned;
* \li \c bOption enabled and \c bEasIdMaskLength > \c 0: EAS Sequence is returned by ICs with matching \c pEasIdValue;
*
* If the EAS mode is disabled, the label remains silent.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_EASAlarm(
    void * pDataParams,         /**< [In] Pointer to this layer's parameter structure. */
    uint8_t bOption,            /**< [In] Option flag; \c #PHAL_SLI_OPTION_OFF: disable; \c #PHAL_SLI_OPTION_ON: enable. */
    uint8_t * pEasIdValue,      /**< [In] EAS ID; 0, 8 or 16 bits; optional. */
    uint8_t bEasIdMaskLength,   /**< [In] 8 bits; optional. */
    uint8_t * pEas              /**< [Out] EAS ID (16 bits) or EAS Sequence (256 bits). */
    );

/**
* \brief This command enables the password protection for EAS.
*
* The EAS password has to be transmitted before with \ref phalSli_SetPWD.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_PwdProtectEAS(
    void * pDataParams     /**< [In] Pointer to this layer's parameter structure. */
    );

/**
* \brief With this command, a new EAS identifier is stored in the corresponding configuration memory.
*
* If the EAS mode is password protected the EAS password has to be
* transmitted before with \ref phalSli_SetPWD.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_WriteEAS_ID(
    void * pDataParams,      /**< [In] Pointer to this layer's parameter structure. */
    uint8_t * pEasIdValue    /**< [In] EAS ID; 16 bits. */
    );

/**
* \brief On this command, the label will respond with it's EPC data.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlaying component.
*/
phStatus_t phalSli_ReadEPC(
    void * pDataParams,  /**< [In] Pointer to this layer's parameter structure. */
    uint8_t * pEpc       /**< [Out] EPC data; 96 bits. */
    );

/**
* \brief The command provides information about IC access conditions and supported features.
*
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalSli_GetNxpSystemInfo(
    void * pDataParams,              /**< [In] Pointer to this layer's parameter structure. */
    uint8_t * pProtectionPointer,    /**< [Out] Protection pointer address; 8 bits. */
    uint8_t * pProtectionCondition,  /**< [Out] Protection conditions for the password protection; 8 bits. */
    uint8_t * pLockBits,             /**< [Out] Lock bits settings; 8 bits. */
    uint8_t * pFeatureFlags          /**< [Out] Supported commands and features; 32 bits (+) additional  32 bits if EXT flag is set. */
    );

/**
* \brief Get random number and set password using this random number and provided password
*
* This command combines together \ref phalSli_GetRandomNumber and \ref phalSli_SetPWD commands.
* \return Status code
* \retval #PH_ERR_SUCCESS Operation successful.
* \retval Other Depending on implementation and underlying component.
*/
phStatus_t phalSli_GetRandomNumberSetPWD(
    void * pDataParams,        /**< [In] Pointer to this layer's parameter structure. */
    uint8_t bPwdIdentifier,    /**< [In] Password Identifier; Refer to the PHAL_SLI_PWD_* definitions. */
    uint8_t * pPwd             /**< [In] Password; 32 bits. */
    );

/** @} */

#endif /* NXPRDLIB_REM_GEN_INTFS */

#endif /* NXPBUILD__PHAL_SLI */

#ifdef __cplusplus
} /* Extern C */
#endif

#endif /* PHALSLI_H */
