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
* $Author: Santosh D Araballi (ing04223) $
* $Revision: 3239 $
* $Date: 2015-09-04 07:48:31 +0200 (Fr, 04 Sep 2015) $
*
* History:
*
*
*/

#ifndef PH_NXPBUILD_APP_H
#define PH_NXPBUILD_APP_H


/** \defgroup ph_NxpBuild NXP Build
* \brief Controls build behavior of components.
* @{
*/

/* NXPBUILD_DELETE: included code lines should be always removed from code */

/* NXP Build defines                            */
/* use #define to include components            */
/* comment out #define to exclude components    */

/* DEBUG build mode */
/*#define NXPBUILD__PH_DEBUG*/                      /**< DEBUG build definition */

#define NXPRDLIB_REM_GEN_INTFS
#define NXPBUILD__PH_PLATFORM

/* BAL components */
#define NXPBUILD__PHBAL_REG_STUB                /**< Stub BAL definition */

/* HAL components */
//#define NXPBUILD__PHHAL_HW_RC523                /**< Rc523 HAL definition */
//#define NXPBUILD__PHHAL_HW_RC663               /**< Rc663 HAL definition */
//#define NXPBUILD__PHHAL_HW_PN5180                /**< PN5180 HAL definition */

/* PAL ISO 14443-3A components */
#define NXPBUILD__PHPAL_I14443P3A_SW            /**< Software PAL ISO 14443-3A definition */

/* PAL ISO 14443-3B components */
#define NXPBUILD__PHPAL_I14443P3B_SW            /**< Software PAL ISO 14443-3B definition */

/* PAL ISO 14443-4A components */
#define NXPBUILD__PHPAL_I14443P4A_SW            /**< Software PAL ISO 14443-4A definition */

/* PAL ISO 14443-4 components */
#define NXPBUILD__PHPAL_I14443P4_SW             /**< Software PAL ISO 14443-4 definition */

/* PAL MIFARE components */
#define NXPBUILD__PHPAL_MIFARE_SW               /**< Software PAL MIFARE */

/* PAL Felica components */
#define NXPBUILD__PHPAL_FELICA_SW               /**< Software PAL Felica */

/* Protocol components that are not supported in Rc523 */
#ifndef NXPBUILD__PHHAL_HW_RC523

    /* PAL ICode EPC/UID components */
#define NXPBUILD__PHPAL_EPCUID_SW               /**< Software ICode EPC/UID */

    /* PAL ISO15693 ICode SLI components */
#define NXPBUILD__PHPAL_SLI15693_SW             /**< Software PAL Sli 15693 */

    /* PAL I18000p3m3 components */
#define NXPBUILD__PHPAL_I18000P3M3_SW           /**< Software I18000p3m3 */

#endif


/* PAL ISO 18092 components */
#define NXPBUILD__PHPAL_I18092MPI_SW            /**< Software PAL ISO 18092 definition */


/* Protocol components that are not supported in Rc663 */
#ifndef NXPBUILD__PHHAL_HW_RC663

#define NXPBUILD__PHAC_DISCLOOP_TARGET
     /* PAL ISO 14443p4 Card protocol component */
#define NXPBUILD__PHPAL_I14443P4MC_SW              /**< Software implementation of ISO 18092 target mode */

     /* PAL ISO 18092 Target component */
#define NXPBUILD__PHPAL_I18092MT_SW              /**< Software implementation of ISO 18092 target mode */

     /* CE T4AT Card Emulation components */
#define NXPBUILD__PHCE_T4T_SW                  /**< Software Tag OPerations */
//#define NXPBUILD__PHCE_T4T_PROPRIETARY         /**< Proprietary file support */

#endif

#define NXPBUILD__PHAC_DISCLOOP_SW              /**< Discovery loop software component */
#define NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P3_TAGS     /**< Detect type A I3p3 cards */
#define NXPBUILD__PHAC_DISCLOOP_TYPEA_JEWEL_TAGS    /**< Detect type A jewel cards */
#define NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS     /**< Detect type A I3p4 cards */
#define NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS      /**< Detect type A P2P tags */
#define NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS          /**< Detect Type F tags */
#define NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS      /**< Detect Type F P2P tags */
#define NXPBUILD__PHAC_DISCLOOP_TYPEB_I3P3B_TAGS    /**< Detect TypeB, I3P3B tags */
#define NXPBUILD__PHAC_DISCLOOP_TYPEB_I3P4B_TAGS    /**< Detect TypeB, I3P4B tags */
#define NXPBUILD__PHAC_DISCLOOP_TYPEB_BPRIME_TAGS   /**< Detect TypeB, B Prime tags */

/* Tags that are not supported in Rc523 */
#ifndef NXPBUILD__PHHAL_HW_RC523
#define NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS  /**< Detect TypeV Tags */

#define NXPBUILD__PHAC_DISCLOOP_I18000P3M3_TAGS /**< Detect 18000p3m3 Tags */

/* AL ISO 18000P3M3 components */
#define NXPBUILD__PHAL_I18000P3M3_SW            /**< Software AL I18000p3m3 */

/* AL I15693 components */
#define NXPBUILD__PHAL_I15693_SW                /**< Software AL I15693 */

/* AL Sli components */
#define NXPBUILD__PHAL_SLI_SW                   /**< Software AL Sli */
#endif /* NXPBUILD__PHHAL_HW_RC523 */

/* OSAL components */
#define  NXPBUILD__PH_OSAL_POSIX             /**< Software OSAL functionality with RTOS */
#define NXPBUILD__PH_OSAL

/* AL Felica components */
#define NXPBUILD__PHAL_FELICA_SW                /**< Software AL Felica */

/* AL MIFARE Classic components */
#define NXPBUILD__PHAL_MFC_SW                   /**< Software MIFARE Classic */

#define NXPBUILD__PHAL_MFDF_SW                  /**< Software MIFARE DESFire */

/* AL MIFARE Ultralight components */
#define NXPBUILD__PHAL_MFUL_SW                  /**< Software MIFARE Ultralight */

/* AL T1T components */
#define NXPBUILD__PHAL_T1T_SW                  /**< Software Type 1 Tag */

/* AL TOP components */
#define NXPBUILD__PHAL_TOP_SW                  /**< Software Tag OPerations */

/* CID Manager components */
//#define NXPBUILD__PH_CIDMANAGER_SW              /**< Software CID Manager */

/* KeyStore components */
#define NXPBUILD__PH_KEYSTORE_SW                /**< Software KeyStore */
/*#define NXPBUILD__PH_KEYSTORE_RC663 */            /**< RC663 KeyStore */

/* LN components */
#define NXPBUILD__PHLN_LLCP_SW                  /**< Software Link layer */

/* SNEP components */
#define NXPBUILD__PHNP_SNEP_SW                  /**< SNEP  definition */


/** @}
* end of ph_NxpBuild
*/
#endif /* PH_NXPBUILD_APP_H */

