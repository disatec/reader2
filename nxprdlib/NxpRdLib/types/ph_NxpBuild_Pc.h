/*
*         Copyright (c), NXP Semiconductors Gratkorn / Austria
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
* particular purpose, or that the use will not infringe any third party patent,
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

#ifndef PH_NXPBUILD_PC_H
#define PH_NXPBUILD_PC_H

#ifdef _WIN32 /* For compilation on PC */

/** \defgroup ph_NxpBuild NXP Build
* \brief Controls build behaviour of components.
* @{
*/

/* NXPBUILD_DELETE: included code lines should be always removed from code */

/* NXP Build defines                            */
/* use #define to include components            */
/* comment out #define to exclude components    */

/* DEBUG build mode */

#define NXPBUILD__PH_DEBUG                      /**< DEBUG build definition */

/* BAL components */

/*#define NXPBUILD__PHBAL_REG_STUB*/                /**< Stub BAL definition */
#define NXPBUILD__PHBAL_REG_SERIALWIN           /**< SerialWin BAL definition */
#define NXPBUILD__PHBAL_REG_ABEND               /**< PN5180 via ABEND */
#define NXPBUILD__PHBAL_REG_PCSCWIN             /**< PcscWin BAL definition */
#define NXPBUILD__PHBAL_REG_RD70XUSBWIN         /**< Rd70X_UsbWin BAL definition */
#define NXPBUILD__PHBAL_REG_SOCKETWIN
/* Commenting as this BAL would be available only for LPC platform */
/*#define NXPBUILD__PHBAL_REG_LPC1768SPI*/          /**< Lpc17xx BAL definition */
/*#define NXPBUILD__PHBAL_REG_LPC1768I2C*/

/* HAL components */

#define NXPBUILD__PHHAL_HW_RC523                /**< Rc523 HAL definition */
#define NXPBUILD__PHHAL_HW_RD70X                /**< Rd70X HAL definition */
#define NXPBUILD__PHHAL_HW_RC632                /**< Rc632 HAL definition */
#define NXPBUILD__PHHAL_HW_PN5180               /**< PN5180 HAL definition */
#define NXPBUILD__PHHAL_HW_CALLBACK             /**< Callback HAL definition */
#define NXPBUILD__PHHAL_HW_RC663                /**< Rc663 HAL definition */
#define NXPBUILD__PHHAL_HW_RD710                /**< Rd710 HAL definition */
#define NXPBUILD__PHHAL_HW_UHOST                /**< UHOST HAL. */
#define NXPBUILD__PHHAL_HW_DUT                  /**< DUT HAL. */
#define NXPBUILD__PHHAL_HW_MFCCRYPTO            /**Pallavi: < MfcCrypto HAL. */
#define NXPBUILD__PHHAL_HW_SAMAV2


/* PAL ISO 14443-3A components */

#define NXPBUILD__PHPAL_I14443P3A_SW            /**< Software PAL ISO 14443-3A definition */
#define NXPBUILD__PHPAL_I14443P3A_RD70X         /**< Rd70X PAL ISO 14443-3A definition */
#define NXPBUILD__PHPAL_I14443P3A_RD710         /**< Rd710 PAL ISO 14443-3A definition */
#define NXPBUILD__PHPAL_I14443P3A_DUT


/* PAL ISO 14443-3B components */

#define NXPBUILD__PHPAL_I14443P3B_SW            /**< Software PAL ISO 14443-3B definition */
#define NXPBUILD__PHPAL_I14443P3B_DUT           /**< Rd710 PAL ISO 14443-3B definition */


/* PAL ISO 14443-4A components */

#define NXPBUILD__PHPAL_I14443P4A_SW            /**< Software PAL ISO 14443-4A definition */
#define NXPBUILD__PHPAL_I14443P4A_RD710         /**< Rd710 PAL ISO 14443-4A definition */
#define NXPBUILD__PHPAL_I14443P4A_DUT           /**< DUT PAL ISO 14443-4A definition */

/* PAL ISO 14443-4 components */

#define NXPBUILD__PHPAL_I14443P4_SW             /**< Software PAL ISO 14443-4 definition */
#define NXPBUILD__PHPAL_I14443P4_RD710          /**< Rd710 PAL ISO 14443-4 definition */
#define NXPBUILD__PHPAL_I14443P4_DUT


/* PAL ISO 18092 Target component */
#define NXPBUILD__PHPAL_I14443P4MC_SW              /**< Software implementation of ISO 18092 target mode */


/* PAL MIFARE components */

#define NXPBUILD__PHPAL_MIFARE_STUB             /**< Stub PAL MIFARE */
#define NXPBUILD__PHPAL_MIFARE_SW               /**< Software PAL MIFARE */
#define NXPBUILD__PHPAL_MIFARE_RD710            /**< Rd710 PAL MIFARE */
#define NXPBUILD__PHPAL_MIFARE_DUT              /**< DUT PAL MIFARE */
#define NXPBUILD__PHPAL_MIFARE_SAMAV2_X         /**< SAMAV2 */


/* PAL ISO15693 ICode SLI components */

#define NXPBUILD__PHPAL_SLI15693_SW             /**< Software PAL Sli 15693 */
#define NXPBUILD__PHPAL_SLI15693_DUT


/* PAL Felica components */

#define NXPBUILD__PHPAL_FELICA_SW               /**< Software PAL Felica */
/*#define NXPBUILD__PHPAL_FELICA_DUT   */           /**< DUT PAL Felica */

/* PAL ICode EPC/UID components */

#define NXPBUILD__PHPAL_EPCUID_SW               /**< Software ICode EPC/UID */


/* PAL I18000p3m3 components */

#define NXPBUILD__PHPAL_I18000P3M3_SW           /**< Software I18000p3m3 */
#define NXPBUILD__PHPAL_I18000P3M3_DUT          /**< DUT PAL ISO 18000-3M3 definition */


/* PAL ISO 18092 components */

#define NXPBUILD__PHPAL_I18092MPI_SW            /**< Software PAL ISO 18092 definition */
#define NXPBUILD__PHPAL_I18092MPI_DUT           /**< DUT PAL ISO 18092 definition */


/* PAL ISO 18092 Target component */
#define NXPBUILD__PHPAL_I18092MT_SW              /**< Software implementation of ISO 18092 target mode */


/* AC dicover loop component */

#define NXPBUILD__PHAC_DISCLOOP_TARGET

#define NXPBUILD__PHAC_DISCLOOP_SW              /**< Discovery loop software componenet */

#define NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P3_TAGS  /**< Detect type A I3p3 cards */
#define NXPBUILD__PHAC_DISCLOOP_TYPEA_JEWEL_TAGS /**< Detect type A jewel cards */
#define NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS  /**< Detect type A I3p4 cards */
#define NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS   /**< Detect type A P2P tags */


#define NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS /**< Detect Type F tags */

#define NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS      /**< Detect Type F P2P tags */


#define NXPBUILD__PHAC_DISCLOOP_TYPEB_I3P3B_TAGS    /**< Detect TypeB, I3P3B tags */
#define NXPBUILD__PHAC_DISCLOOP_TYPEB_I3P4B_TAGS    /**< Detect TypeB, I3P4B tags */
#define NXPBUILD__PHAC_DISCLOOP_TYPEB_BPRIME_TAGS   /**< Detect TypeB, B Prime tags */

#define NXPBUILD__PHAC_DISCLOOP_TYPEV_TAGS  /**< Detect TypeV Tags */

#define NXPBUILD__PHAC_DISCLOOP_I18000P3M3_TAGS /**< Detect 18000p3m3 Tags */

/* OSAL components */
#define  NXPBUILD__PH_OSAL_WINOS                        /**< Software OSAL for WinOs */

/* AL Felica components */
#define NXPBUILD__PHAL_FELICA_SW                /**< Software AL Felica */

/* AL MIFARE Classic components */

#define NXPBUILD__PHAL_MFC_SW                   /**< Software MIFARE Classic */


/* AL MIFARE Plus components */

#define NXPBUILD__PHAL_MFP_SW                   /**< Software MIFARE Plus */
#define NXPBUILD__PHAL_MFP_SAMAV2_X
#define NXPBUILD__PHAL_MFP_SAMAV2


#define NXPBUILD__PHAL_MFDF_SW                  /**< Software MIFARE DESFire */

#define NXPBUILD__PHAL_MFDFEV2_SW                  /**< Software MIFARE DESFire EV2 */


/* AL MIFARE Ultralight components */

#define NXPBUILD__PHAL_MFUL_SW                  /**< Software MIFARE Ultralight */

/* AL T1T components */

#define NXPBUILD__PHAL_T1T_SW                  /**< Software Type 1 Tag */

/* AL TOP components */

#define NXPBUILD__PHAL_TOP_SW                  /**< Software Tag OPerations */


/* AL Virtual Card Architecture components */

#define NXPBUILD__PHAL_VCA_SW                   /**< Software Virtual Card Architecture */

/* DL FPGA Box components */

/* #define NXPBUILD__PHDL_RDFPGA_V6            */    /**< FPGA Box measurement reader */

/* DL Amplifier components */

/*#define NXPBUILD__PHDL_AMP_AR75A250                   **< AR75A250 Amplifier */


/* DL Thermostream components */

/*#define NXPBUILD__PHDL_THSTRM_TP04030                 **< TP04030 Thermostream */


/* DL Oscilloscope components */


/* DL Master Amplifier Oscilloscope components */

/*#define NXPBUILD__PHDL_MSTAMPOSC_AR75A250         **< Software Master Amplifier Oscilloscope */
/*#define NXPBUILD__PHDL_MSTAMPOSC_ISOSETUP  **< ISO Setup Master Amplifier Oscilloscope */


/* DL Stepper components */

/*#define NXPBUILD__PHDL_STEPPER_V1                  **< V1 Stepper */


/* AL I15693 components */

#define NXPBUILD__PHAL_I15693_SW                /**< Software AL I15693 */

/* AL Sli components */

#define NXPBUILD__PHAL_SLI_SW                   /**< Software AL Sli */


/* AL ISO 18000P3M3 components */

#define NXPBUILD__PHAL_I18000P3M3_SW            /**< Software AL I18000p3m3 */


/* AL P40 PrivateCommands components */
#define NXPBUILD__PHAL_P40CMDPRIV_SW        /**< Software AL P40  PrivateCommands*/

/* AL P40 PublicCommand components */
#define NXPBUILD__PHAL_P40CMDPUB_SW     /**< Software AL P40 PublicCommand*/


/* CE T4AT Card Emulation components */
#define NXPBUILD__PHCE_T4T_SW                  /**< Software Tag OPerations */
#define NXPBUILD__PHCE_T4T_PROPRIETARY         /**< Proprietary file support */


/* CID Manager components */

#define NXPBUILD__PH_CIDMANAGER_SW              /**< Software CID Manager */
#define NXPBUILD__PH_CIDMANAGER_RD710           /**< Rd710 CID Manager */

/* Crypto components */

#define NXPBUILD__PH_CRYPTOSYM_SW                  /**< Software Crypto Sym */
#define NXPBUILD__PH_CRYPTOSYM_STUB                /**< Stub Crypto Sym */


/* CryptoRng components */

#define NXPBUILD__PH_CRYPTORNG_SW               /**< Software CryptoRng */
#define NXPBUILD__PH_CRYPTORNG_STUB             /**< Stub CryptoRng */


/* KeyStore components */

#define NXPBUILD__PH_KEYSTORE_SW                /**< Software KeyStore */
#define NXPBUILD__PH_KEYSTORE_RC632             /**< RC632 KeyStore */
#define NXPBUILD__PH_KEYSTORE_RC663             /**< RC663 KeyStore */
#define NXPBUILD__PH_KEYSTORE_RD710             /**< Rd710 KeyStore */

/* LN components */

/* #define NXPBUILD__PHLN_LLCP_FRI     */           /**< Software Link layer */

/* SNEP components */

/* #define NXPBUILD__PHNP_SNEP_FRI    */            /**< SNEP FRI definition */


/* TMIUtils component */
#define NXPBUILD__PH_TMIUTILS
/* Log component */

#define NXPBUILD__PH_LOG

/* Enable/disable Debugging */
/*#define NXPBUILD__PH_DEBUG*/

/** @}
* end of ph_NxpBuild
*/

#endif /* _WIN32 */
#endif /* PH_NXPBUILD_PC_H */
