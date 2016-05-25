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
* Build System Definitions for Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 3073 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  MK: Generated 15. October 2009
*
*/

#ifndef NXPBUILD_CUSTOMER_HEADER_INCLUDED
#   ifdef _WIN32
#       include <ph_NxpBuild_Pc.h>
#   else
#      include <ph_NxpBuild_Lpc.h>
#   endif
#endif

#ifdef NXPBUILD_CUSTOMER_HEADER_INCLUDED
#       include <ph_NxpBuild_App.h>
#endif

#ifndef PH_NXPBUILD_H
#define PH_NXPBUILD_H

/* #define  NXPRDLIB_REM_GEN_INTFS */

/** \defgroup ph_NxpBuild NXP Build
* \brief Controls build behaviour of components.
* @{
*/

/* NXPBUILD_DELETE: included code lines should be always removed from code */

/* NXP Build defines                            */
/* use #define to include components            */
/* comment out #define to exclude components    */

/* DEBUG build mode */

/*#define NXPBUILD__PH_DEBUG*/                      /**< DEBUG build definition */

/* BAL components */

#if defined (NXPBUILD__PHBAL_REG_STUB)
    #define NXPBUILD__PHBAL_REG                 /**< Generic BAL definition */
#endif

/* HAL components */

#if defined (NXPBUILD__PHHAL_HW_RC523) || \
    defined (NXPBUILD__PHHAL_HW_RC663) || \
    defined (NXPBUILD__PHHAL_HW_PN5180)
    #define NXPBUILD__PHHAL_HW                  /**< Generic HAL definition */
#endif

/* PAL ISO 14443-3A components */

#if defined (NXPBUILD__PHPAL_I14443P3A_SW)
    #define NXPBUILD__PHPAL_I14443P3A           /**< Generic PAL ISO 14443-3A definition */
#endif


/* PAL ISO 14443-3B components */

#if defined (NXPBUILD__PHPAL_I14443P3B_SW)
    #define NXPBUILD__PHPAL_I14443P3B           /**< Generic PAL ISO 14443-3B definition */
#endif
/* PAL ISO 14443-4A components */


#if defined (NXPBUILD__PHPAL_I14443P4A_SW)
    #define NXPBUILD__PHPAL_I14443P4A           /**< Generic PAL ISO 14443-4A definition */
#endif

/* PAL ISO 14443-4 components */

#if defined (NXPBUILD__PHPAL_I14443P4_SW)
    #define NXPBUILD__PHPAL_I14443P4            /**< Generic PAL ISO 14443-4 definition */
#endif

/* PAL ISO 18092 Target component */

#if defined (NXPBUILD__PHPAL_I14443P4MC_SW)
    #define NXPBUILD__PHPAL_I14443P4MC             /**< Generic PAL ISO 18092 target definition */
#endif

/* PAL MIFARE components */

#if defined (NXPBUILD__PHPAL_MIFARE_SW)
    #define NXPBUILD__PHPAL_MIFARE              /**< Generic PAL MIFARE definition */
#endif

/* PAL ISO15693 ICode SLI components */

#if defined (NXPBUILD__PHPAL_SLI15693_SW)
    #define NXPBUILD__PHPAL_SLI15693            /**< Generic PAL Sli 15693 definition */
#endif

/* PAL Felica components */

/*#define HANDLE_MULTIPLE_FELICA_CARD_RESP */   /**< Enable handling of multiple Felica card response */

#if defined (NXPBUILD__PHPAL_FELICA_SW)
    #define NXPBUILD__PHPAL_FELICA              /**< Generic PAL Felica definition */
#endif

/* PAL ICode EPC/UID components */

#if defined (NXPBUILD__PHPAL_EPCUID_SW)
    #define NXPBUILD__PHPAL_EPCUID              /**< Generic PAL ICode EPC/UID definition */
#endif

/* PAL I18000p3m3 components */

#if defined (NXPBUILD__PHPAL_I18000P3M3_SW)
    #define    NXPBUILD__PHPAL_I18000P3M3            /**< Generic PAL I18000p3m3 definition */
#endif

/* PAL ISO 18092 components */

#if defined (NXPBUILD__PHPAL_I18092MPI_SW)
    #define NXPBUILD__PHPAL_I18092MPI           /**< Generic PAL ISO 18092 definition */
#endif

/* PAL ISO 18092 Target component */

#if defined (NXPBUILD__PHPAL_I18092MT_SW)
    #define NXPBUILD__PHPAL_I18092MT             /**< Generic PAL ISO 18092 target definition */
#endif

/* AC dicover loop component */

#if defined (NXPBUILD__PHAC_DISCLOOP_SW)
    #define NXPBUILD__PHAC_DISCLOOP             /**< Generic dicovery loop component */
#endif


#if defined (NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P3_TAGS) || \
    defined (NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS) || \
    defined (NXPBUILD__PHAC_DISCLOOP_TYPEA_JEWEL_TAGS)
    #define NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS
#endif

#if defined (NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS)
   #define NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS
#endif

#if defined (NXPBUILD__PHAC_DISCLOOP_TYPEB_I3P3B_TAGS) || \
    defined (NXPBUILD__PHAC_DISCLOOP_TYPEB_I3P4B_TAGS) || \
    defined (NXPBUILD__PHAC_DISCLOOP_TYPEB_BPRIME_TAGS)
    #define NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS          /**< Detect TypeB Tags */
#endif

#if defined (NXPBUILD__PH_OSAL_FREERTOS) || \
    defined (NXPBUILD__PH_OSAL_NORTOS)
    #define NXPBUILD__PH_OSAL                           /**< Generic OSAL definition */
#endif

/* AL Felica components */

#if defined (NXPBUILD__PHAL_FELICA_SW)
    #define NXPBUILD__PHAL_FELICA               /**< Generic AL Felica definition */
#endif

/* AL MIFARE Classic components */

#if defined (NXPBUILD__PHAL_MFC_SW)
    #define NXPBUILD__PHAL_MFC                  /**< Generic AL MIFARE Classic definition */
#endif

/* AL MIFARE Plus components */


/* AL MIFARE DESFire Components */

#if defined (NXPBUILD__PHAL_MFDF_SW)
    #define NXPBUILD__PHAL_MFDF
#endif



/* AL MIFARE Ultralight components */

#if defined (NXPBUILD__PHAL_MFUL_SW)
    #define NXPBUILD__PHAL_MFUL                 /**< Generic AL MIFARE Ultralight definition */
#endif


/* AL T1T components */

#if defined (NXPBUILD__PHAL_T1T_SW)
    #define NXPBUILD__PHAL_T1T                 /**< Generic AL Type 1 Tag definition */
#endif


/* AL TOP components */

#if defined (NXPBUILD__PHAL_TOP_SW)
    #define NXPBUILD__PHAL_TOP                 /**< Generic AL Tag Operations */
#endif

/* CE T4AT Card Emulation components */

#if defined (NXPBUILD__PHCE_T4T_SW)
    #define NXPBUILD__PHCE_T4T                 /**< Generic HCE */
#endif

/* AL Virtual Card Architecture components */



/* DL FPGA Box components */



/* DL Amplifier components */


/* DL Thermostream components */


/* DL Oscilloscope components */


/* DL Master Amplifier Oscilloscope components */


/* DL Stepper components */


/* AL I15693 components */

#if defined (NXPBUILD__PHAL_I15693_SW)
    #define NXPBUILD__PHAL_I15693               /**< Generic AL I15693 definition */
#endif

/* AL Sli components */

#if defined (NXPBUILD__PHAL_SLI_SW)
    #define NXPBUILD__PHAL_SLI                  /**< Generic AL Sli definition */
#endif

/* AL ISO 18000P3M3 components */

#if defined (NXPBUILD__PHAL_I18000P3M3_SW)
    #define NXPBUILD__PHAL_I18000P3M3           /**< Generic AL I18000p3m3 definition */
#endif

/* AL P40 PrivateCommands components */


/* AL P40 PublicCommand components */


/* CID Manager components */

#if defined (NXPBUILD__PH_CIDMANAGER_SW)
    #define NXPBUILD__PH_CIDMANAGER             /**< Generic CID Manager definition */
#endif


/* Crypto components */


/* CryptoRng components */


/* KeyStore components */

#if defined (NXPBUILD__PH_KEYSTORE_SW) || \
    defined (NXPBUILD__PH_KEYSTORE_RC663)
    #define NXPBUILD__PH_KEYSTORE               /**< Generic KeyStore definition */
#endif


/* LN components */

#if defined (NXPBUILD__PHLN_LLCP_SW)
    #define NXPBUILD__PHLN_LLCP               /**< Generic Link layer definition */
#endif

/* SNEP components */

#if defined (NXPBUILD__PHNP_SNEP_SW)
    #define NXPBUILD__PHNP_SNEP                 /**< Generic HAL definition */
#endif

/* Log component */

/*#define NXPBUILD__PH_LOG*/

/* Enable/disable Debugging */
/*#define NXPBUILD__PH_DEBUG*/

/** @}
* end of ph_NxpBuild
*/

#endif /* PH_NXPBUILD_H */
