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
* Generic HAL component of Reader Library Framework.
* $Author: jenkins_ cm (nxp92197) $
* $Revision: 4258 $ (NFCRDLIB_V4.010.03.001609 : 4184 $ (NFCRDLIB_V4.010.01.001603 : 4115 ))
* $Date: 2016-03-01 16:44:26 +0530 (Tue, 01 Mar 2016) $
*
* History:
*  SA: Generated 10. July 2015
*
*/

#ifndef PHHALHW_H
#define PHHALHW_H

#include <ph_Status.h>
#include <ph_NxpBuild.h>
#include <phbalReg.h>
#include <phOsal.h>

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus */

#ifdef NXPBUILD__PHHAL_HW_RC663

    /** \defgroup phhalHw_Rc663 component : Rc663
    * \brief RC663 Reader HAL
    *
    * \b Note: This HAL takes care the "Request Guard Time".
    * \see phpalI14443p3a_RequestA
    * \see phpalI14443p3a_WakeUpA
    * @{
    */

#define PHHAL_HW_RC663_ID               0x01    /**< ID for Rc663 HAL component. */

#define PHHAL_HW_RC663_DEFAULT_TIMEOUT  150U    /**< Default timeout in microseconds. */
#define PHHAL_HW_RC663_SHADOW_COUNT     0x0010U /**< Number of shadowed configurations. */

    /**
    * \name Custom configuration
    */
    /*@{*/
    /** Configure FIFO-Size. */
#define PHHAL_HW_RC663_CONFIG_FIFOSIZE      (PH_CONFIG_CUSTOM_BEGIN)
    /**
    * \brief Enable (#PH_ON) or disable (#PH_OFF) LoadReg Mode.
    *
    * If this mode is enabled (it's disabled by default) then LoadReg is used\n
    * if an operation mode changes instead of applying library constants.\n
    * This is useful if custom antenna configuration values are required to be applied.\n
    * The parameters for LoadReg are taken from \b pLoadRegConfig which is an input parameter\n
    * of \ref phhalHw_Rc663_Init. Refer there for a detailed description of \b pLoadRegConfig.
    */
#define PHHAL_HW_RC663_CONFIG_LOADREG_MODE  (PH_CONFIG_CUSTOM_BEGIN+1)
    /*@}*/

    /**
    * \name FIFO configuration values
    */
    /*@{*/
#define PHHAL_HW_RC663_VALUE_FIFOSIZE_255   0x0000U /**< Configure FIFO-Size to 256 bytes */
#define PHHAL_HW_RC663_VALUE_FIFOSIZE_512   0x0001U /**< Configure FIFO-Size to 512 bytes */

    /*@}*/

    /**
    * \brief Rc663 HAL parameter structure
    */
    typedef struct
    {
        uint16_t wId;                                       /**< Layer ID for this component, NEVER MODIFY!. */
        void  * pBalDataParams;                             /**< Pointer to the lower layers parameter structure. */
        uint8_t * pLoadRegConfig;                           /**< Pointer to configuration buffer for LoadReg mode; Refer to \ref phhalHw_Rc663_Init. */
        uint8_t * pTxBuffer;                                /**< Pointer to global transmit buffer used by the Exchange() function. */
        uint8_t * pRxBuffer;                                /**< Pointer to global receive buffer used by the Exchange() function. */
        uint32_t dwTimingUs;                                /**< Current timing value. */
        uint16_t wCfgShadow[PHHAL_HW_RC663_SHADOW_COUNT];   /**< Configuration shadow; Stores configuration for current card type. */
        uint16_t wTxBufSize;                                /**< Size of the global transmit buffer. */
        uint16_t wTxBufLen;                                 /**< Number of valid bytes within the transmit buffer. */
        uint16_t wRxBufSize;                                /**< Size of the global receive buffer. */
        uint16_t wRxBufLen;                                 /**< Number of valid bytes within the receive buffer. */
        uint16_t wRxBufStartPos;                            /**< Starting position within the global receive buffer. */
        uint16_t wMaxPrecachedBytes;                        /**< Holds the max. number of bytes which are pre-cached prior to command execution. */
        uint16_t wTimingMode;                               /**< Current timing measurement mode. */
        uint16_t wFieldOffTime;                             /**< Field-Off-Time in milliseconds. */
        uint16_t wFieldRecoveryTime;                        /**< Field-Recovery-Time in milliseconds. */
        uint16_t wAdditionalInfo;                           /**< Storage for additional error information. */
        uint16_t wPowerDownTimeMs;                          /**< Power-Down time in milliseconds if power-down mode is used. */
        uint16_t wDetectionTimeUs;                          /**< Detection time in microseconds if power-down mode is used. */
        uint16_t dwFdtPc;                                   /**< Current timing value backup for PC. */
        uint8_t bLoadRegMode;                               /**< Whether LoadReg-Mode is enabled or not. */
        uint8_t bCardType;                                  /**< Type of card for which the hal is configured for. */
        uint8_t bTimeoutUnit;                               /**< Unit of current timeout value (either #PHHAL_HW_TIME_MICROSECONDS or #PHHAL_HW_TIME_MILLISECONDS). */
        uint8_t bSymbolStart;                               /**< Preamble of a frame. */
        uint8_t bSymbolEnd;                                 /**< Trailer symbol of a frame. */
        uint8_t bFifoSize;                                  /**< Fifo-Size setting. */
        uint8_t bBalConnectionType;                         /**< Type of the underlying BAL connection. Refer to #PHHAL_HW_CONFIG_BAL_CONNECTION. */
        uint8_t bRfResetAfterTo;                            /**< Storage for #PHHAL_HW_CONFIG_RFRESET_ON_TIMEOUT setting. */
        uint8_t bMode;                                      /**< One of the above LPCD modes. */
        uint8_t bI;                                         /**< I-Channel value in case of no card on antenna. */
        uint8_t bQ;                                         /**< Q-Channel value in case of no card on antenna. */
        uint8_t bJewelActivated;                            /**< Set the PH_ON/PH_OFF Jewel exchange mode.*/
        uint8_t bCRC5Enabled;                               /**< Set the PH_ON/PH_OFF for 18000P3M3 CRC5.*/
        uint8_t bOpeMode;                                   /**< Operation mode. One of NFC, EMVCo, ISO.*/
        uint8_t bEmdNoiseMaxDataLen;                        /**< EMVCo EMD noise check length.*/
        uint8_t bRxMultiple;                                /**< Flag indicates whether RX multiple is to be enabled  or not. */
        uint8_t bNfcipMode;                                 /**< Enable/Disable to handle start byte at 106kbps in NFCIP-1 mode. */
        uint8_t bCheckEmdErr;                               /**< Enable/Disable to perform Emvco EMD error check. */
        uint8_t abStoredCRC[2];                             /**< StoredCRC of ISO 18000p3m3 tag. */
        uint16_t wCurrentSlotCount;                         /**< ISO 18000p3m3 slot count at time of buffer overflow (PH_ERR_SUCCESS_CHAINING). */
        uint8_t bSession;                                   /**< Inventory session of ISO 18000p3m3. */
    } phhalHw_Rc663_DataParams_t;

    /**
    * \brief Initialize the HAL component.
    *
    * A note on <b>pLoadRegConfig</b>:\n
    * This is used if #PHHAL_HW_RC663_CONFIG_LOADREG_MODE is enabled.\n
    * It is defined as follows:
    \code
    [MSB]                                              [LSB]
    31...............16..................8.................0
    +------------------+------------------+----------------+
    | wEEAddress (16b) | bRegAddress (8b) | bNumBytes (8b) |
    +------------------+------------------+----------------+
    \endcode
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval Other Depending on implementation and underlying component.
    */
    phStatus_t phhalHw_Rc663_Init(
        phhalHw_Rc663_DataParams_t * pDataParams, /**< [In] Pointer to this layer's parameter structure. */
        uint16_t wSizeOfDataParams,               /**< [In] Specifies the size of the data parameter structure. */
        void * pBalDataParams,                    /**< [In] Pointer to the lower layers parameter structure. */
        uint8_t * pLoadRegConfig,                 /**< [In] Pointer to configuration buffer for LoadReg mode; See description above. */
        uint8_t * pTxBuffer,                      /**< [In] Pointer to global transmit buffer used by the phhalHw_Exchange() function. */
        uint16_t wTxBufSize,                      /**< [In] Size of the global transmit buffer. */
        uint8_t * pRxBuffer,                      /**< [In] Pointer to global receive buffer used by the phhalHw_Exchange() function. */
        uint16_t wRxBufSize                       /**< [In] Size of the global receive buffer. Specify the buffer +1 byte, because one byte is reserved for SPI communication. */
        );

#include <phhalHw_Rc663_Reg.h>
#include <phhalHw_Rc663_Cmd.h>

    /** @} */
#endif /* NXPBUILD__PHHAL_HW_RC663 */

#ifdef NXPBUILD__PHHAL_HW_RC523

    /** \defgroup phhalHw_Rc523 component : Rc523
    * \brief RC523 Reader HAL
    *
    * This HAL supports all the additional features required for Pn512 :\n
    * Pn512 and Rc523 are compatible for the common features that they support.
    *
    * @{
    */

#define PHHAL_HW_RC523_ID               0x02    /**< ID for Rc523 HAL component. */

#define PHHAL_HW_RC523_SHADOW_COUNT     0x000FU /**< Number of shadowed configurations. */

    /**
    * \brief Rc523 HAL parameter structure
    */
    typedef struct
    {
        uint16_t wId;                                       /**< Layer ID for this component, NEVER MODIFY!. */
        void  * pBalDataParams;                             /**< Pointer to the lower layers parameter structure. */
        void  * pKeyStoreDataParams;                        /**< Pointer to the parameter structure of the keyStore layer. */
        uint32_t dwTimingUs;                                /**< Current timing value. */
        uint8_t * pTxBuffer;                                /**< Pointer to global transmit buffer used by the phhalHw_Exchange() function. */
        uint8_t * pRxBuffer;                                /**< Pointer to global receive buffer used by the phhalHw_Exchange() function. */
        uint16_t wTxBufSize;                                /**< Size of the global transmit buffer. */
        uint16_t wTxBufLen;                                 /**< Number of valid bytes within the transmit buffer. */
        uint16_t wRxBufSize;                                /**< Size of the global receive buffer. */
        uint16_t wRxBufLen;                                 /**< Number of valid bytes within the receive buffer. */
        uint16_t wRxBufStartPos;                            /**< Starting position within the global receive buffer. */
        uint16_t wMaxPrecachedBytes;                        /**< Holds the max. number of bytes which are precached prior to command execution. */
        uint16_t wCfgShadow[PHHAL_HW_RC523_SHADOW_COUNT];   /**< Configuration shadow; Stores configuration for current card-type. */
        uint16_t wTimingMode;                               /**< Current timing measurement mode. */
        uint16_t wFieldOffTime;                             /**< Field-Off-Time in milliseconds. */
        uint16_t wFieldRecoveryTime;                        /**< Field-Recovery-Time in milliseconds. */
        uint16_t wAdditionalInfo;                           /**< Storage for additional error information. */
        uint16_t dwFdtPc;                                   /**< Current timing value backup for PC. */
        uint16_t wTargetMode;                               /**< Used to indicate the mode and speed at which target is activated. Lower byte indicates speed and higher byte indicates framing. */
        uint16_t bTxWaitPrescaler;                          /**< Holds pre-scaler value for Tx wait. */
        uint16_t bTimeoutPrescaler;                         /**< Holds pre-scaler value for timeout. */
        uint16_t wTxWaitReloadVal;                          /**< Holds Timer reload value for Tx wait. */
        uint16_t wTimeoutReloadVal;                         /**< Holds Timer reload value for timeout. */
        uint8_t bCardType;                                  /**< Type of card for which the HAL is configured for. */
        uint8_t bTimeoutUnit;                               /**< Unit of current timeout value (either #PHHAL_HW_TIME_MICROSECONDS or #PHHAL_HW_TIME_MILLISECONDS). */
        uint8_t bBalConnectionType;                         /**< Type of the underlying BAL connection. Refer to #PHHAL_HW_CONFIG_BAL_CONNECTION. */
        uint8_t bRfResetAfterTo;                            /**< Storage for #PHHAL_HW_CONFIG_RFRESET_ON_TIMEOUT setting. */
        uint8_t bRxMultiple;                                /**< Flag indicates whether RX multiple is to be enabled or not. */
        uint8_t bActiveMode;                                /**< Flag indicates whether active mode is activated or not. */
        uint8_t bRfca;                                      /**< Flag indicates whether RFCA is to be enabled or not (#PH_ON or #PH_OFF, default - #PH_ON). */
        uint8_t bJewelActivated;                            /**< Set the PH_ON/PH_OFF jewel exchange mode. */
        uint8_t bOpeMode;                                   /**< Operation mode. One of NFC, EMVCo, ISO. */
        uint8_t bNfcipMode;                                 /**< Enable/Disable to handle start byte at 106kbps in NFCIP-1 mode. */
        uint8_t bSkipSend;                                  /**< Flag indicates whether Skip send should be performed or not in active target mode. */
        uint8_t bCheckEmdErr;                               /**< Flag indicates EMD feature is enabled for ISO. */
        uint8_t bEmdNoiseMaxDataLen;                        /**< EMD Max noise data length for ISO. */
    } phhalHw_Rc523_DataParams_t;

    /**
    * \brief Initialize the HAL component.
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval Other Depending on implementation and underlying component.
    */
    phStatus_t phhalHw_Rc523_Init(
        phhalHw_Rc523_DataParams_t * pDataParams, /**< [In] Pointer to this layer's parameter structure. */
        uint16_t wSizeOfDataParams,               /**< [In] Specifies the size of the data parameter structure. */
        void * pBalDataParams,                    /**< [In] Pointer to the lower layers parameter structure. */
        void * pKeyStoreDataParams,               /**< [In] Pointer to the parameter structure of the keyStore layer. */
        uint8_t * pTxBuffer,                      /**< [In] Pointer to global transmit buffer used by the phhalHw_Exchange() function. */
        uint16_t wTxBufSize,                      /**< [In] Size of the global transmit buffer. */
        uint8_t * pRxBuffer,                      /**< [In] Pointer to global receive buffer used by the phhalHw_Exchange() function. */
        uint16_t wRxBufSize                       /**< [In] Size of the global receive buffer. Specify the buffer +1 byte, because one byte is reserved for SPI communication. */
        );

#include <phhalHw_Rc523_Cmd.h>
#include <phhalHw_Rc523_Reg.h>

    /** @} */
#endif /* NXPBUILD__PHHAL_HW_RC523 */












#ifdef NXPBUILD__PHHAL_HW_PN5180

/**
 * \defgroup phhalHw_Pn5180 component : Pn5180
 * \brief Pn5180 HAL component
 *
 * @{
 */
#define PHHAL_HW_PN5180_ID                          0x0E        /**< ID for PN5180 HAL component */
#define PHHAL_HW_PN5180_DEFAULT_TIMEOUT             150U        /**< Default timeout in microseconds. */
#define PHHAL_HW_PN5180_DEFAULT_TIMEOUT_MILLI       50U         /**< Default timeout in milliseconds */
#define PHHAL_HW_PN5180_SHADOW_COUNT                0x10U       /**< Pn5180 Shadow Register count */
#define INSTR_BUFFER_SIZE                           262         /**< Used to form commands 259-Max buf size in writeregmultiple */


/**
 * PN5180 HAL component.
 * This structure holds all the data that are required for the PN5180 HAL
 * operations.
 */
    typedef struct
    {
        uint16_t wId;                                       /**< Layer ID for this component, NEVER MODIFY! */
        uint8_t bBalConnectionType;                         /**< Type of the underlying BAL connection. Refer to #PHHAL_HW_CONFIG_BAL_CONNECTION. */
        uint8_t bRfca;                                      /**< Flag indicates whether RFCA is to be enabled or not (#PH_ON or #PH_OFF, default - #PH_ON). */


        void * pBalDataParams;                              /**< Points to the underlaying layer over which HAL is going to communicate */
        void * pKeyStoreDataParams;                        /**< Pointer to the parameter structure of the keyStore layer. */

        uint8_t * pTxBuffer;                                /**< Pointer to global transmit buffer used by the Exchange() function. */
        uint16_t wTxBufSize;                                /**< Size of the global transmit buffer. */
        uint16_t wTxBufLen;                                 /**< Number of valid bytes within the transmit buffer. */

        uint8_t * pRxBuffer;                                /**< Pointer to global receive buffer used by the Exchange() function. */
        uint16_t wRxBufSize;                                /**< Size of the global receive buffer. */
        uint16_t wRxBufLen;                                 /**< Number of valid bytes within the receive buffer. */

        uint16_t wTxBufStartPos;                            /**< Starting position within the global transmit buffer (used if \b TxBuffer equals \b RxBuffer). */
        uint16_t wRxBufStartPos;                            /**< Starting position within the global receive buffer. */

        uint32_t dwTimingUs;                                /**< Current timing value. */
        uint32_t dwTxWaitMs;                                /**<TxWaitMs shadow*/
        uint16_t wTimingMode;                               /**< Current timing measurement mode. */
        uint16_t dwFdtPc;                                   /**< Current timing value backup for PC*/

        uint16_t wFieldOffTime;                             /**< Field-Off-Time in milliseconds. */
        uint16_t wFieldRecoveryTime;                        /**< Field-Recovery-Time in milliseconds. */

        uint16_t wAdditionalInfo;                           /**< Storage for additional error information. */
        uint16_t wTargetMode;                               /**< Used to indicate the mode and speed at which Target is activated. Lower Byte indicates Speed and Higher Byte indicates Framing. */

        uint16_t wWakeupCounterInMs;                        /**<[In] Used value for wakeup counter in msecs, i.e. after this amount of time PN5180 wakes up from standby. Maximum value is 2690. */
        uint16_t wLpcdReference;                            /**< Reference valu read from AGC_REF_CONFIG (check_card) register. */

        uint8_t bLpcdMode;                                  /**< LPCD Mode options. */
        uint8_t bSymbolStart;                               /**< Preamble of a frame. */
        uint8_t bSymbolEnd;                                 /**< Trailer symbol of a frame. */
        uint8_t bRfResetAfterTo;                            /**< Storage for #PHHAL_HW_CONFIG_RFRESET_ON_TIMEOUT setting. */

        uint8_t bActiveMode;                                /**< Flag indicates whether Active mode is activated or not. */
        uint8_t bJewelActivated;                            /**< Set the PH_ON/PH_OFF Jewel exchange mode.*/
        uint8_t bNfcipMode;                                 /**< Enable/Disable to handle Start Byte at 106kbps in NFCIP-1 mode. */
        uint8_t bCardType;                                  /**< Type of card for which the hal is configured for. */

        uint8_t bTimeoutUnit;                               /**< Unit of current timeout value (either #PHHAL_HW_TIME_MICROSECONDS or #PHHAL_HW_TIME_MILLISECONDS). */
        uint8_t bRxMultiple;                                /**< Flag indicates whether RX multiple is to be enabled  or not. */
        uint8_t bSkipSend;                                  /**< Flag indicates whether Skip send should be performed or not in Active Target Mode. */
        uint8_t bTransmited;                                /**< Flag indicates whether the data is transmitted */

        uint8_t bMfcCryptoEnabled;                          /**< Flag indicates whether the MFC Crypto is enabled or not. */
        uint8_t bCardMode;                                  /**< Flag indicates whether the SYSTEM_CONFIG_AUTOCOLL_STATE_A_MASK bit is set or not. */

    /*EMVCO: variable required for EMVCo */
        uint8_t bOpeMode;                                  /**< Used to differentiate between EMVCO or Normal Mode of this layer */
        uint8_t bEmdFlag;                                  /**< Flag to Indicate Enable EMD Suppression*/


        uint16_t wCfgShadow[PHHAL_HW_PN5180_SHADOW_COUNT];   /**< Configuration shadow; Stores configuration for current cardtype. */

        /*Instruction buffer for forming commands*/
        uint8_t pInstrBuffer[INSTR_BUFFER_SIZE];

    /*end */
    } phhalHw_Pn5180_DataParams_t;

/**
 * \brief Initializes the PN5180 HAL component.
 * This function has to be called as part of the initialization of the
 * HAL layer of the application that intends to use the PN517 as the reader
 * IC front end.
 */

phStatus_t phhalHw_Pn5180_Init(
    phhalHw_Pn5180_DataParams_t * pDataParams,                /**<[In] Pointer to data parameter of this HAL layer */
    uint16_t wSizeOfDataParams,                               /**<[In] Size of the data parameter of the structure that was passed */
    void * pBalDataParams,                                    /**<[In] Pointer to the lower layer parameter(BAL) structure */
    void * pKeyStoreDataParams,                               /**< [In] Pointer to the parameter structure of the keyStore layer. */
    uint8_t * pTxBuffer,                                      /**<[In] Pointer to global transmit buffer that will be used by Exchange() */
    uint16_t wTxBufSize,                                      /**<[In] size of the global transmit buffer */
    uint8_t * pRxBuffer,                                      /**<[In] Pointer to global receive buffer that will be used by Exchange() */
    uint16_t wRxBufSize                                       /**<[In] Size of the global receive buffer  */
    );

/**
 * @}
 */
#endif  /* NXPBUILD__PHHAL_HW_PN5180 */




#ifdef NXPBUILD__PHHAL_HW

    /** \defgroup phhalHw Hardware Abstraction Layer
    * \brief These are the components which are used to abstract the
    * functionality of the physical reader device to a generic interface.
    *
    * HAL API usage in common use cases are described in the sequence diagrams below
    *
    * <strong>Reader mode:</strong> This sequence diagram describes the usage of HAL APIs to
    * implement a simple reader mode application.
    * \image html HAL_ReaderMode.png
    *
    * <strong>Passive initiator mode:</strong> This sequence diagram describes the usage of HAL APIs to
    * implement a simple ISO 18092 Initiator in \n
    * passive communication mode.
    * \image html HAL_PassiveInitiator.png
    *
    * <strong>Card mode:</strong> This sequence diagram describes the usage of HAL APIs to
    * implement a ISO 14443-3A PICC / ISO 18092 passive target.
    * \image html HAL_CardMode.png
    *
    * <strong>Active initiator:</strong> This sequence diagram describes the usage of HAL APIs to
    * implement ISO 18092 initiator in active communication mode.
    * \image html HAL_ActiveInitiator.png
    *
    * <strong>Active target:</strong> This sequence diagram describes the usage of HAL APIs to
    * implement ISO 18092 target in active communication mode.
    * \image html HAL_ActiveTarget.png
    *
    * <strong>Low power card detection(LPCD):</strong> This sequence diagram describes the usage of HAL APIs to
    * implement a simple ISO/IEC 14443-3a \n
    * reader that uses LPCD functionality of reader IC.
    * \image html HAL_Lpcd.png
    *
    * <strong>ISO 18000P3M3 Inventory:</strong> This sequence diagram describes the usage of HAL APIs to
    * implement a simple application that \n
    * inventories and activates an ISO 18000p3m3 tag.
    * \image html HAL_I18000p3m3Inventory.png
    *
    * <strong>ISO 18000P3M3 Resume Inventory:</strong> This sequence diagram describes the usage of HAL APIs to
    * implement a simple application that \n
    * inventories and activates an ISO 18000p3m3 tag.\n
    * In this specific diagram, it is shown how to use \ref phhalHw_I18000p3m3ResumeInventory() API.
    * \image html HAL_I18000p3m3Resume.png
    *
    * <strong>OSAL usage in HAL:</strong> This sequence diagram describes the usage of OS event
    * from within the HAL. \n
    * HAL uses OSAL call to wait for a CLIF event that is triggered by a CLIF interrupt.\n
    * \image html HAL_Osal.png
    *
    * @{
    */


    /**
    * \name Communication configuration parameters \n
    *  These parameters are used to configure hardware/HAL
    *  in a particular manner before performing an operation.
    */
    /*@{*/

    /**
     * Enable (#PH_ON) or Disable (#PH_OFF) Parity.\n
     *
     * */
#define PHHAL_HW_CONFIG_PARITY                    0x0000U

    /**
     * Enable (#PH_ON) or disable (#PH_OFF) transmit CRC.\n
     *
     * */
#define PHHAL_HW_CONFIG_TXCRC                     0x0001U

    /**
     * Enable (#PH_ON) or disable (#PH_OFF) receive CRC.\n
     *
     *
     * */
#define PHHAL_HW_CONFIG_RXCRC                     0x0002U

    /**
     * Set number of valid bits for the last transmitted byte.
     * Valid range is 1 to 7
     * */
#define PHHAL_HW_CONFIG_TXLASTBITS                0x0003U

    /**
     * Get number of valid bits of last received byte.
     * Valid range is 1 to 7
     * */
#define PHHAL_HW_CONFIG_RXLASTBITS                0x0004U

    /**
     * Set Rx-Aligned Bits. Used for reception of bit oriented frames.
     * RxAlign defines the bit position for the first bit received to be stored in the FIFO.
     * Further received bits are stored at the following bit positions.
     * Used only for bitwise anti-collision at 106kbps. For all other modes it is set to logic 0.
     * */
#define PHHAL_HW_CONFIG_RXALIGN                   0x0005U

    /**
     * Configure receiver deaf-time (time after a send where every reception is ignored) in microseconds.
     * This is the time for which the IC cannot receive any data after transmit.
     * */
#define PHHAL_HW_CONFIG_RXWAIT_US                 0x0006U

    /**
     * Set Tx wait time (time between last RxIrq and Tx of succeeding frame) in microseconds.\n
     *
     * */
#define PHHAL_HW_CONFIG_TXWAIT_US                 0x0007U

    /**
     * Configure data-rate and framing for transmission.\n
     * Use LSB for data-rate and MSB to specify framing.\n
     * MSB values can be #PHHAL_HW_RF_TYPE_A_FRAMING, #PHHAL_HW_RF_TYPE_B_FRAMING, #PHHAL_HW_RF_TYPE_F_FRAMING\n
     * LSB values can be #PHHAL_HW_RF_DATARATE_106, #PHHAL_HW_RF_DATARATE_212, #PHHAL_HW_RF_DATARATE_424, #PHHAL_HW_RF_DATARATE_848\n
     *
     * */
#define PHHAL_HW_CONFIG_TXDATARATE_FRAMING        0x0009U

    /**
     * Enable or disable clearing of bits after collision.
     * Deprecated. Not used anymore.
     * */
#define PHHAL_HW_CONFIG_CLEARBITSAFTERCOLL        0x0008U

    /**
     * Configure data-rate and framing for reception.
     * Use LSB for data-rate and MSB to specify framing.\n
     * MSB values can be #PHHAL_HW_RF_TYPE_A_FRAMING, #PHHAL_HW_RF_TYPE_B_FRAMING, #PHHAL_HW_RF_TYPE_F_FRAMING\n
     * LSB values can be #PHHAL_HW_RF_DATARATE_106, #PHHAL_HW_RF_DATARATE_212, #PHHAL_HW_RF_DATARATE_424, #PHHAL_HW_RF_DATARATE_848\n
     * */
#define PHHAL_HW_CONFIG_RXDATARATE_FRAMING        0x000AU

    /**
     * Set modulation index (unit and value are hardware-dependent).\n
     *
     * Example values are \n
     * \li #PHHAL_HW_RC663_MODINDEX_I14443B \n
     * \li #PHHAL_HW_RC663_MODINDEX_I15693  \n
     * \li #PHHAL_HW_RC663_MODINDEX_EPCUID  \n
     * \li #PHHAL_HW_RC663_MODINDEX_I18000P3M3\n
     *
     *
     * */
#define PHHAL_HW_CONFIG_MODINDEX                  0x000BU

    /**
     * Enable (#PH_ON) or disable (#PH_OFF) 100% modulation.\n
     *
     * */
#define PHHAL_HW_CONFIG_ASK100                    0x000CU

    /**
     * Set RC timeout (in [us]).\n
     *
     * */
#define PHHAL_HW_CONFIG_TIMEOUT_VALUE_US          0x000DU

    /**
     * Set RC timeout (in [ms]).\n
     *
     * */
#define PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS          0x000EU

    /**
     * Sub-carrier setting for ISO 15693.
     * Possible values are\n
     *     #PHHAL_HW_SUBCARRIER_SINGLE,\n
     *     #PHHAL_HW_SUBCARRIER_DUAL,\n
     *     #PHHAL_HW_SUBCARRIER_QUAD\n
     * */
#define PHHAL_HW_CONFIG_SUBCARRIER                0x000FU

    /**
     * Set the field off time for field-reset ([ms]); \b Note: A value of \c 0 is not allowed.\n
     * Default value is #PHHAL_HW_FIELD_OFF_DEFAULT
     * */
#define PHHAL_HW_CONFIG_FIELD_OFF_TIME            0x0013U

    /**
     * Set the field recovery time for field-reset ([ms]); \b Note: For ISO15693 operation, this should be at least set to \c 1ms.\n
     * Default value is #PHHAL_HW_FIELD_RECOVERY_DEFAULT
     * */
#define PHHAL_HW_CONFIG_FIELD_RECOVERY_TIME       0x0014U

    /**
     * Disable / Set the SOF symbol of a frame.
     * Sets the start of frame symbol for a frame for ICODE EPIC UID, ISO 15693, ISO 18000p3m3. The values can be one of:\n
     * \li #PH_OFF - Disable SoF\n
     * \li #PHHAL_HW_SYMBOL_ICODEEPCUID_SSOF - Short start of frame\n
     * \li #PHHAL_HW_SYMBOL_ICODEEPCUID_LSOF - Long start of frame\n
     * \li #PHHAL_HW_SYMBOL_ICODEEPCUID_CS - Close slot\n
     * \li #PHHAL_HW_SYMBOL_I15693_SOF - SOF for ISO 15693. Automatically chosen between 1/256 and 1/4\n
     * \li #PHHAL_HW_SYMBOL_I18000P3M3_PREAMBLE - ISO 18000p3m3 Preamble\n
     * \li #PHHAL_HW_SYMBOL_I18000P3M3_FSYNC - ISO 18000p3m3 Frame Sync
     *
     * */
#define PHHAL_HW_CONFIG_SYMBOL_START              0x0015U

    /**
     * Disable / Set the EOF symbol of a frame.
     * Sets the end of frame symbol for a frame for ICODE EPIC UID protocol.\n
     * #PH_OFF - Disable EoF\n
     * #PHHAL_HW_SYMBOL_ICODEEPCUID_CEOF - Common end of frame\n
     *
     * */
#define PHHAL_HW_CONFIG_SYMBOL_END                0x0016U

    /**
     * Sets the CRC type for Tx(#PHHAL_HW_CONFIG_TXCRC) and Rx(#PHHAL_HW_CONFIG_RXCRC).
     * Values can be one of\n
     * #PHHAL_HW_CRCTYPE_CRC5,\n
     * #PHHAL_HW_CRCTYPE_CRC16\n
     * optionally combined with the following inversion option.\n
     * #PHHAL_HW_CRCOPTION_INVERTED.
     *
     * */
#define PHHAL_HW_CONFIG_CRCTYPE                   0x0017U

    /**
     * Set TxWait (time between last RxIrq and transmit start of succeeding frame) in milliseconds.\n
     *
     * */
#define PHHAL_HW_CONFIG_TXWAIT_MS                 0x0018U

    /**
     * Set the #PH_ON/#PH_OFF Jewel exchange mode.
     * This should be done by user before T1T/Jewel functions are called.
     * */
#define PHHAL_HW_CONFIG_JEWEL_MODE                0x0020U

    /**
     * Disable MIFARE(R) Classic Crypto1.\n
     * Disables the MIFARE crypto1 in the IC. Only value PH_OFF is allowed.\n
     * The MIFARE Crypto1 will be enabled by the MFAuthent command. This bit cannot be set by software.
     * */
#define PHHAL_HW_CONFIG_DISABLE_MF_CRYPTO1        0x002EU

    /**
     * Set/Get an offset value\n
     * A value at this offset on HAL Tx Buffer can be read or written by using\n
     * Get/Set(#PHHAL_HW_CONFIG_TXBUFFER) once the offset is set using this parameter.\n
     * Used by Protocol components.
     * */
#define PHHAL_HW_CONFIG_TXBUFFER_OFFSET           0x002FU

    /**
     * Set/Get start position of RX buffer to be used (never changed by HAL).\n
     *
     * */
#define PHHAL_HW_CONFIG_RXBUFFER_STARTPOS         0x0030U

    /**
     * Get size of Rx buffer set at HAL initialization. This is read-only parameter.\n
     * Used by protocols to know the Rx buffer size for chaining purposes.
     * */
#define PHHAL_HW_CONFIG_RXBUFFER_BUFSIZE          0x0031U

    /**
     * Get size of TX buffer set at HAL initialization. This is read-only parameter.\n
     * Used by protocols to know the TxBuffer size.
     * */
#define PHHAL_HW_CONFIG_TXBUFFER_BUFSIZE          0x0032U

    /**
     * Set/Get the amount of valid bytes in Tx buffer. \n
     *
     * */
#define PHHAL_HW_CONFIG_TXBUFFER_LENGTH           0x0033U

    /**
     * Access the transmit buffer(TxBuffer) at the byte offset defined by #PHHAL_HW_CONFIG_TXBUFFER_OFFSET.\n
     * Used to get or set two bytes from/to a particular position in TxBuffer.\n
     * The position in transmit buffer is defined by setting TXBUFFER_OFFSET.\n
     * Used by protocol components.
     * */
#define PHHAL_HW_CONFIG_TXBUFFER                  0x0034U

    /**
     * Set/Reset minimum FDT (only for proximity check).\n
     * Used to set minimum frame delay time (timeout) in microseconds as required for proximity check\n
     * command of MIFARE Plus.
     *
     * */
#define PHHAL_HW_CONFIG_SETMINFDT                 0x0051U

    /**
     * Get the status of external RF field. Returned values are #PH_ON or #PH_OFF.
     * Deprecated.
     * */
#define PHHAL_HW_CONFIG_EXT_RF_ON                 0x0052U

    /**
     * Enable (#PH_ON) or disable (#PH_OFF) multiple reception.\n
     * Please refer to phpalFelica.h and phpalFelica_ReqC function documentation to know the
     * frame format in which the data is returned.
     * */
#define PHHAL_HW_CONFIG_RXMULTIPLE                0x0053U

    /**
     * Get the status if active communication mode is #PH_ON / #PH_OFF.\n
     * Used by protocols to know the mode.
     *
     * */
#define PHHAL_HW_CONFIG_ACTIVEMODE                0x0054U

    /**
     * Set/Reset MFHalted bit in MifNFCReg.\n
     *
     * When set, this indicates that the IC is set to HALT mode in card operation mode at 106 kBit.\n
     * Once set, when \ref phhalHw_Autocoll API is called in card/target mode, will only accept WUPA/ALL_REQ command to respond.
     *
     * */
#define PHHAL_HW_CONFIG_MFHALTED                  0x0055U

    /**
     * Enable/disable RF collision avoidance for passive communication mode. (#PH_ON or #PH_OFF).\n
     * This parameter should be set before \ref phhalHw_FieldOn to perform Initial RFCA.
     * By default after IC HAL initialization, this should be set to #PH_ON
     * */
#define PHHAL_HW_CONFIG_RFCA                      0x0056U

    /**
     * Enable RF-ON interrupt\n
     *
     * This is typically used in card/target mode. When there is no external RF, this configuration is set in the HAL\n
     * and the controller can go into a wait state for the external RF-ON interrupt to occur.
     *
     * */
#define PHHAL_HW_CONFIG_RFON_INTERRUPT            0x0057U

    /**
     * Get the framing and speed at which target is operating.\n
     * Read-only parameter.
     * RF data rate values in the LSB and framing in the MSB are returned.\n
     * MSB values can be #PHHAL_HW_RF_TYPE_A_FRAMING, #PHHAL_HW_RF_TYPE_B_FRAMING, #PHHAL_HW_RF_TYPE_F_FRAMING\n
     * LSB values can be #PHHAL_HW_RF_DATARATE_106, #PHHAL_HW_RF_DATARATE_212, #PHHAL_HW_RF_DATARATE_424\n
     *
     * */
#define PHHAL_HW_CONFIG_TARGET_DATARATE_FRAMING   0x0059U

    /**
     * Configure operation mode for this Layer.\n
     * Values can be #RD_LIB_MODE_NFC or #RD_LIB_MODE_EMVCO
     *
     * */
#define PHHAL_HW_CONFIG_OPE_MODE                  0x005AU

    /**
     * Enable/Disable handling start byte at 106kbps in NFCIP Mode of communication.\n
     * This is to tell the HAL to prepend the start byte as required for passive\n
     * communication mode at 106kbps.
     *
     * */
#define PHHAL_HW_CONFIG_NFCIP_STARTBYTE           0x005CU

    /**
     * Enable symbol send (#PH_ON).\n
     * ISO/IEC 18000p3m3 protocol will use data send (#PH_OFF).
     *
     * */
#define PHHAL_HW_CONFIG_SET_SYMBOL_SEND           0x005DU

    /**
     * Enable (#PH_ON) or Disable (#PH_OFF) EMD error check for Emvco.\n
     * The EMD check should be enabled only after ISO 14443A/B layer 3\n
     * activation.
     *
     * */
#define PHHAL_HW_CONFIG_SET_EMD                   0x005EU

    /*@}*/

    /**
    * \name IC specific configuration parameters
    * These are not applicable to all ICs and
    * are not mandatory to be implemented to
    * comply with this HAL specification.
    */
    /*@{*/

    /**
     * Sets the exchange time measurement mode in the IC. The values can be one of:\n
     * #PHHAL_HW_TIMING_MODE_OFF - Time measurement disabled\n
     * #PHHAL_HW_TIMING_MODE_FDT - Frame delay time to be measured\n
     * #PHHAL_HW_TIMING_MODE_COMM - Overall time to be measured\n
     * #PHHAL_HW_TIMING_MODE_OPTION_MASK - Additional option-mask for time measurement.\n
     * #PHHAL_HW_TIMING_MODE_OPTION_DEFAULT - The timing value is accumulated on each exchange until it gets retrieved by phhalHw_GetConfig.\n
     * #PHHAL_HW_TIMING_MODE_OPTION_AUTOCLEAR - Automatically resets the current timing value prior to any communication.
     *
     * */
#define PHHAL_HW_CONFIG_TIMING_MODE               0x8010U

    /**
     * Retrieve elapsed time of receive timer ([us]).\n
     *
     * */
#define PHHAL_HW_CONFIG_TIMING_US                 0x8011U

    /**
     * Retrieve elapsed time of receive timer ([ms]).\n
     *
     * */
#define PHHAL_HW_CONFIG_TIMING_MS                 0x8012U

    /**
     * Configures the max. number of bytes which are pre-cached prior to command execution.\n
     * Bytes up to the maximum pre-cache limit can be written into the FIFO before the command is started.\n
     * The maximum pre-cache limit depends upon the FIFO size of the IC.\n
     * This will ensure faster transmission in comparison to starting the command and then writing the\n
     * parameters + data to the FIFO. Rc523(Pn512) and Rc663 HALs provide such a functionality.\n
     *
     * */
#define PHHAL_HW_CONFIG_MAX_PRECACHED_BYTES       0x8035U

    /**
     * Set the BAL connection type. The default value is always #PHHAL_HW_BAL_CONNECTION_RS232.\n
     * Other supported values are\n
     * #PHHAL_HW_BAL_CONNECTION_SPI\n
     * #PHHAL_HW_BAL_CONNECTION_I2C\n
     * */
#define PHHAL_HW_CONFIG_BAL_CONNECTION            0x8040U

    /**
     * Bit rate for serial UART communication for Rc523, CLRC663\n
     * valid values range from - #PHHAL_HW_RS232_BITRATE_9600 to\n
     * #PHHAL_HW_RS232_BITRATE_460800
     * */
#define PHHAL_HW_CONFIG_SERIAL_BITRATE            0x8041U


    /**
     * Perform an RF-reset in case of a timeout (only for \ref phhalHw_Exchange)\n
     * Valid values are #PH_ON, #PH_OFF
     * */
#define PHHAL_HW_CONFIG_RFRESET_ON_TIMEOUT        0x8050U

    /**
     * Get any additional information that the HAL might provide
     * For example any specific error information.
     *
     * */
#define PHHAL_HW_CONFIG_ADDITIONAL_INFO           0x802FU

    /*@}*/


    /**
    * \name Card Configuration
    */
    /*@{*/
#define PHHAL_HW_CARDTYPE_CURRENT               0x0000U /**< Re-Apply settings for current communication mode. */
#define PHHAL_HW_CARDTYPE_ISO14443A             0x0001U /**< ISO/IEC 14443A mode. */
#define PHHAL_HW_CARDTYPE_ISO14443B             0x0002U /**< ISO/IEC 14443B mode. */
#define PHHAL_HW_CARDTYPE_FELICA_212            0x0003U /**< Felica (JIS X 6319) mode. */
#define PHHAL_HW_CARDTYPE_ISO15693              0x0004U /**< ISO/IEC 15693 mode. */
#define PHHAL_HW_CARDTYPE_ICODEEPCUID           0x0005U /**< NXP I-Code EPC/UID mode. */
#define PHHAL_HW_CARDTYPE_I18000P3M3            0x0006U /**< ISO/IEC 18000-3 mode3. */
#define PHHAL_HW_CARDTYPE_I18092M_ACTIVE_106    0x0007U /**< ISO/IEC ISO18092 (NFC) initiator mode at 106 baud. */
#define PHHAL_HW_CARDTYPE_I18092M_ACTIVE_212    0x0008U /**< ISO/IEC ISO18092 (NFC) initiator mode at 212 baud. */
#define PHHAL_HW_CARDTYPE_I18092M_ACTIVE_424    0x0009U /**< ISO/IEC ISO18092 (NFC) initiator mode at 424 baud. */
#define PHHAL_HW_CARDTYPE_FELICA_424            0x000AU /**< Felica mode at 424 baud. */
#define PHHAL_HW_CARDTYPE_UNKNOWN               0x00FFU /**< Unknown card type. */
    /*@}*/

    /**
    * \name CRC Values
    */
    /*@{*/
#define PHHAL_HW_CRCTYPE_CRC5               0x0001U /**< 5 bit length, polynomial x^5+x^3+1, preset value 01001b, residue 00000b. */
#define PHHAL_HW_CRCTYPE_CRC16              0x0002U /**< ISO/IEC 13239, 16 bit length, polynomial x^16+x^12+x^5+1, preset value 0xFFFF, residue 0x1D0F. */
#define PHHAL_HW_CRCOPTION_INVERTED         0x0100U /**< To indicate that the CRC generated should be inverted. This should be ORed with the CRCTYPEs above. */
    /*@}*/

    /**
    * \name RF Data rate values
    */
    /*@{*/
#define PHHAL_HW_RF_DATARATE_106                0x0000U /**< Data rate setting for 106 kBit/s. */
#define PHHAL_HW_RF_DATARATE_212                0x0001U /**< Data rate setting for 212 kBit/s. */
#define PHHAL_HW_RF_DATARATE_424                0x0002U /**< Data rate setting for 424 kBit/s. */
#define PHHAL_HW_RF_DATARATE_848                0x0003U /**< Data rate setting for 848 kBit/s. */

#define PHHAL_HW_RF_TX_DATARATE_1_OUT_OF_256    0x000AU /**< Data rate setting for ISO15693 '1 out of 256' mode. */
#define PHHAL_HW_RF_TX_DATARATE_1_OUT_OF_4      0x000BU /**< Data rate setting for ISO15693 '1 out of 4' mode. */
#define PHHAL_HW_RF_RX_DATARATE_LOW             0x000CU /**< Data rate setting for ISO15693 low data rate. */
#define PHHAL_HW_RF_RX_DATARATE_HIGH            0x000DU /**< Data rate setting for ISO15693 high data rate. */
#define PHHAL_HW_RF_RX_DATARATE_FAST_LOW        0x000EU /**< Data rate setting for ISO15693 fast (low*2) data rate (FAST INVENTORY (PAGE) READ). */
#define PHHAL_HW_RF_RX_DATARATE_FAST_HIGH       0x000FU /**< Data rate setting for ISO15693 fast (high*2) data rate (FAST INVENTORY (PAGE) READ). */

#define PHHAL_HW_RF_DATARATE_EPCUID             0x0010U /**< Data rate setting for EPC/UID. */

#define PHHAL_HW_RF_TX_DATARATE_I18000P3M3      0x0011U    /**< Tx data rate setting for ISO18000-3 mode3. TARI Value = 9.44us*/
#define PHHAL_HW_RF_TX_DATARATE_I18000P3M3_TARI1888      0x0012U    /**< Tx data rate setting for ISO18000-3 mode3. TARI Value = 18.88 us*/

#define PHHAL_HW_RX_I18000P3M3_FL_423_MAN2    0x0013U /**< ICode ISO18000-3 Mode3 424 kBit/s (M=2). */
#define PHHAL_HW_RX_I18000P3M3_FL_847_MAN2    0x0014U /**< ICode ISO18000-3 Mode3 847 kBit/s (M=2). */
#define PHHAL_HW_RX_I18000P3M3_FL_423_MAN4    0x0015U /**< ICode ISO18000-3 Mode3 424 kBit/s (M=4). */
#define PHHAL_HW_RX_I18000P3M3_FL_847_MAN4    0x0016U /**< ICode ISO18000-3 Mode3 847 kBit/s (M=4). */
    /*@}*/

    /**
    * \name RF Framing Mode Values
    */
    /*@{*/
#define PHHAL_HW_RF_TYPE_A_FRAMING              0x0100U /**< Data rate setting for 106 kBit/s. */
#define PHHAL_HW_RF_TYPE_B_FRAMING              0x0200U /**< Data rate setting for 212 kBit/s. */
#define PHHAL_HW_RF_TYPE_F_FRAMING              0x0300U /**< Data rate setting for 424 kBit/s. */
#define PHHAL_HW_RF_TYPE_ACTIVE_FRAMING         0x0400U /**< Data rate setting for 848 kBit/s. */

#define PHHAL_HW_RF_DATARATE_OPTION_MASK        0x00FFU /**< Option-mask for data rate. */
#define PHHAL_HW_RF_FRAMING_OPTION_MASK         0xFF00U /**< Option-mask for framing. */
#define PHHAL_HW_RF_TYPE_A_START_BYTE           0xF0U   /**< Start byte used at passive 106kbps. */
    /*@}*/

    /**
    * \name Helper macros used for \ref phhalHw_Autocoll.
    * Used to extract data rate and framing values.
    */
    /*@{*/
#define PHHAL_HW_GETFRAMING( protParams ) ((protParams) >> 8)
#define PHHAL_HW_GETBITRATE( protParams ) ((protParams) & 0x00FF)
    /*@}*/

    /**
    * \name RF sub carrier Values
    */
    /*@{*/
#define PHHAL_HW_SUBCARRIER_SINGLE  0x0000U /**< Sub carrier setting for ISO15693 single sub carrier. */
#define PHHAL_HW_SUBCARRIER_DUAL    0x0001U /**< Sub carrier setting for ISO15693 / EPC V2 dual sub carrier. */
#define PHHAL_HW_SUBCARRIER_QUAD    0x0002U /**< Sub carrier setting for EPC V2 quad sub carrier. */
    /*@}*/

    /**
    * \name Time units
    */
    /*@{*/
#define PHHAL_HW_TIME_MICROSECONDS  0x0000U /**< Time in microseconds (us). */
#define PHHAL_HW_TIME_MILLISECONDS  0x0001U /**< Time in milliseconds (ms). */
    /*@}*/

    /**
    * \name Timing mode settings
    */
    /*@{*/
#define PHHAL_HW_TIMING_MODE_OFF                0x0000U /**< Time measurement disabled. */
#define PHHAL_HW_TIMING_MODE_FDT                0x0001U /**< Measures time from Tx-end until Rx-begin. */
#define PHHAL_HW_TIMING_MODE_COMM               0x0002U /**< Measures the duration of the complete communication. */
#define PHHAL_HW_TIMING_MODE_OPTION_MASK        0xFF00U /**< Additional option-mask for time measurement. */
#define PHHAL_HW_TIMING_MODE_OPTION_DEFAULT     0x0000U /**< The timing value is accumulated on each exchange until it gets retrieved by \ref phhalHw_GetConfig. */
#define PHHAL_HW_TIMING_MODE_OPTION_AUTOCLEAR   0x0100U /**< Automatically resets the current timing value prior to any communication. */
    /*@}*/

    /**
    * \name Field reset default times
    */
    /*@{*/
#define PHHAL_HW_FIELD_OFF_DEFAULT          0x0005U     /**< Default field-off time. */
#define PHHAL_HW_FIELD_RECOVERY_DEFAULT     0x0005U     /**< Default field-recovery time. */
    /*@}*/

    /**
    * \name Frame Symbols (use #PH_OFF to disable)
    */
    /*@{*/
#define PHHAL_HW_SYMBOL_ICODEEPCUID_SSOF    0x0001U /**< ICode EPC/UID short start of frame. */
#define PHHAL_HW_SYMBOL_ICODEEPCUID_LSOF    0x0002U /**< ICode EPC/UID long start of frame. */
#define PHHAL_HW_SYMBOL_ICODEEPCUID_CEOF    0x0003U /**< ICode EPC/UID common end of frame. */
#define PHHAL_HW_SYMBOL_ICODEEPCUID_CS      0x0004U /**< ICode EPC/UID close slot. */
#define PHHAL_HW_SYMBOL_I15693_SOF          0x0005U /**< ISO15693 SOF (automatically chosen between 1/256 and 1/4). */
#define PHHAL_HW_SYMBOL_I18000P3M3_PREAMBLE 0x0006U /**< ISO18000-3 mode3 preamble. */
#define PHHAL_HW_SYMBOL_I18000P3M3_FSYNC    0x0007U /**< ISO18000-3 mode3 frame-sync. */
    /*@}*/

    /**
    * \name MIFARE Classic authenticate options
    */
    /*@{*/
#define PHHAL_HW_MFC_KEYA               0x0AU   /**< MIFARE Classic key type A. */
#define PHHAL_HW_MFC_KEYB               0x0BU   /**< MIFARE Classic key type B. */
#define PHHAL_HW_MFC_KEY_LENGTH         0x06U   /**< Length of a MIFARE Classic key (for completeness). */
    /*@}*/

    /**
    * \name BAL Connection types
    */
    /*@{*/
#define PHHAL_HW_BAL_CONNECTION_RS232   0x0000U     /**< Reader IC is connected via RS232. */
#define PHHAL_HW_BAL_CONNECTION_SPI     0x0001U     /**< Reader IC is connected via SPI. */
#define PHHAL_HW_BAL_CONNECTION_I2C     0x0002U     /**< Reader IC is connected via I2C. */
    /*@}*/

    /**
    * \name RS232 Bitrate Values
    */
    /*@{*/
#define PHHAL_HW_RS232_BITRATE_7200     0x0000U     /**< 7.200 kBit/s. */
#define PHHAL_HW_RS232_BITRATE_9600     0x0001U     /**< 9.600 kBit/s. */
#define PHHAL_HW_RS232_BITRATE_14400    0x0002U     /**< 14.400 kBit/s. */
#define PHHAL_HW_RS232_BITRATE_19200    0x0003U     /**< 19.200 kBit/s. */
#define PHHAL_HW_RS232_BITRATE_38400    0x0004U     /**< 38.400 kBit/s. */
#define PHHAL_HW_RS232_BITRATE_57600    0x0005U     /**< 57.500 kBit/s. */
#define PHHAL_HW_RS232_BITRATE_115200   0x0006U     /**< 115.200 kBit/s. */
#define PHHAL_HW_RS232_BITRATE_128000   0x0007U     /**< 128.000 kBit/s. */
#define PHHAL_HW_RS232_BITRATE_230400   0x0008U     /**< 230.400 kBit/s. */
#define PHHAL_HW_RS232_BITRATE_460800   0x0009U     /**< 460.800 kBit/s. */
#define PHHAL_HW_RS232_BITRATE_921600   0x000AU     /**< 921.600 kBit/s. */
#define PHHAL_HW_RS232_BITRATE_1228800  0x000BU     /**< 1.228.800 kBit/s. */
    /*@}*/

    /**
    * \name Min FDT values
    */
    /*@{*/
#define PHHAL_HW_MINFDT_106_US        91+10         /**< (n * 128 + 84) / 13.56; n = 9. */
#define PHHAL_HW_MINFDT_212_US        87+40         /**< (n * 128 + 148) / 13.56; n = 8. */
#define PHHAL_HW_MINFDT_424_US        84+35         /**< (n * 128 + 116) / 13.56; n = 8. */
#define PHHAL_HW_MINFDT_848_US        83+45         /**< (n * 128 + 100) / 13.56; n = 8. */
    /*@}*/

    /**
     * \name Framing mode values
     */
    /*@{*/
#define PHHAL_HW_I14443A_106_FRAMING        0x00    /**< ISO/IEC 14443A/MIFARE and passive communication mode 106 kBit. */
#define PHHAL_HW_ACTIVE_MODE_FRAMING        0x01    /**< Active communication mode. */
#define PHHAL_HW_FELICA_FRAMING             0x02    /**< FeliCa and passive communication mode 212 and 424 kBit. */
#define PHHAL_HW_I14443B_FRAMING            0x03    /**< ISO/IEC 14443B. */
        /*@}*/

    /**
    * \name Target Bit Coding
    */
    /*@{*/
#define PHHAL_HW_EMULATE_TYPE_A_TARGET      0x0001U /**< Bit setting to emulate Type A target. */
#define PHHAL_HW_EMULATE_TYPE_B_TARGET      0x0002U /**< Bit setting to emulate Type B target. */
#define PHHAL_HW_EMULATE_TYPE_F_TARGET      0x0004U /**< Bit setting to emulate Type F target. */
    /*@}*/

    /**
     * \name Felica Response Frame Coding \n
     * Applicable when \ref PHHAL_HW_CONFIG_RXMULTIPLE is enable
     */
    /*@{*/
#define PHHAL_HW_REQC_MIN_LEN                        19U  /**< Minimum length of valid ReqC response when multiple reception is enabled. */
#define PHHAL_HW_RESP_FRAME_SIZE                     32U  /**< Frame size of one felica response when multiple reception is enable. */
#define PHHAL_HW_STATUS_FRAME_LEN_BYTE_POS           28U  /**< Byte position where length information in felica response frame. */
#define PHHAL_HW_STATUS_FRAME_ERR_BYTE_POS           29U  /**< Byte position where error information in felica response frame. */

#define PHHAL_HW_STATUS_FRAME_CL_ERR_POS        (1 << 0)  /**< Bit position for contact less Error in felica response frame. */
#define PHHAL_HW_STATUS_FRAME_DATA_ERR_POS      (1 << 1)  /**< Bit position for data(CRC or Parity) error in felica response frame. */
#define PHHAL_HW_STATUS_FRAME_PROT_ERR_POS      (1 << 2)  /**< Bit position for protocol error in felica response frame. */
#define PHHAL_HW_STATUS_FRAME_COLL_ERR_POS      (1 << 3)  /**< Bit position for collision error in felica response frame. */
#define PHHAL_HW_STATUS_FRAME_LEN_ERR_POS       (1 << 4)  /**< Bit position for length error in felica response frame. */

#define PHHAL_HW_STATUS_FRAME_ERR_MASK             0x1FU  /**< Bit mask for error byte in felica response frame. */
#define PHHAL_HW_STATUS_FRAME_LEN_MASK             0x1FU  /**< Bit mask for Length byte in felica response frame. */
    /*@}*/

    /**
     * \name Parameter values for ISO/IEC 18000p3m3 HAL APIs \n
     *
     */
    /*@{*/
#define PHHAL_HW_I18000P3M3_GET_MAX_RESPS             0          /**< Gets responses from more than one slots limited by the RX buffer size and number of slots. */
#define PHHAL_HW_I18000P3M3_ONE_TS_ONLY               1          /**< Gets response for only one time slot */
#define PHHAL_HW_I18000P3M3_GET_TAG_HANDLE            2          /**< Gets response for only one time slot. Also sends a ReqRN to get the tag handle for this slot. */
#define PHHAL_HW_I18000P3M3_SESSION_S0               0x00U       /**< Session S0. */
#define PHHAL_HW_I18000P3M3_SESSION_S2               0x02U       /**< Session S2. */
#define PHHAL_HW_I18000P3M3_TARGET_INVENTORIED_S0    0x00U       /**< INVENTORIED_S0 target. */
#define PHHAL_HW_I18000P3M3_TARGET_INVENTORIED_S2    0x02U       /**< INVENTORIED_S2 target. */
#define PHHAL_HW_I18000P3M3_TARGET_SL                0x04U       /**< SL target. */
    /*@}*/

#ifndef _WIN32

    /**
    * \brief HAL parameter structure
    */
    typedef struct
    {
#ifdef NXPBUILD__PHHAL_HW_RC523
        phhalHw_Rc523_DataParams_t         sHal;                       /* HAL component holder Rc523 */
#endif
#ifdef NXPBUILD__PHHAL_HW_RC663
        phhalHw_Rc663_DataParams_t         sHal;                       /* HAL component holder for Rc663 */
#endif
#ifdef NXPBUILD__PHHAL_HW_PN5180
        phhalHw_Pn5180_DataParams_t         sHal;                       /* HAL component holder for Pn5180 */
#endif
    } phhalHw_Nfc_Ic_DataParams_t;

    /**
    * \brief Initialize the HAL component.
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval Other Depending on implementation and underlying component.
    */
    phStatus_t phhalHw_Nfc_IC_Init(
                                  phhalHw_Nfc_Ic_DataParams_t * pDataParams,    /**< [In] Pointer to this layer's parameter structure. */
                                  uint16_t wSizeOfDataParams,                   /**< [In] Specifies the size of the data parameter structure. */
                                  void * pBalDataParams,                        /**< [In] Pointer to the lower layers parameter structure. */
                                  void * pKeyStoreDataParams,                   /**< [In] Pointer to configuration buffer for LoadReg mode; See description above. */
                                  uint8_t * pTxBuffer,                          /**< [In] Pointer to global transmit buffer used by the phhalHw_Exchange() function. */
                                  uint16_t wTxBufSize,                          /**< [In] Size of the global transmit buffer. */
                                  uint8_t * pRxBuffer,                          /**< [In] Pointer to global receive buffer used by the phhalHw_Exchange() function. */
                                  uint16_t wRxBufSize                           /**< [In] Size of the global receive buffer. Specify the buffer +1 byte, because one byte is reserved for SPI communication. */
                                  );
#endif

/*
 * Below #defines remove the generic HAL interface file phhalHw.c from compiling there by
 * saving some code space.
 * This is enabled by defining NXPRDLIB_REM_GEN_INTFS in the project settings during compilation.
 */
#if defined  (NXPRDLIB_REM_GEN_INTFS) && \
    defined (NXPBUILD__PHHAL_HW_RC663)
#include "phhalHw_Rc663_Cmd.h"
#include "../comps/phhalHw/src/Rc663/phhalHw_Rc663.h"

#define phhalHw_Exchange(pDataParams,wOption,pTxBuffer,wTxLength,ppRxBuffer,pRxLength) \
        phhalHw_Rc663_Exchange((phhalHw_Rc663_DataParams_t *)pDataParams, wOption, pTxBuffer, wTxLength, ppRxBuffer, pRxLength)

#define phhalHw_ApplyProtocolSettings(pDataParams, bMode) \
        phhalHw_Rc663_ApplyProtocolSettings((phhalHw_Rc663_DataParams_t *)pDataParams, bMode)

#define phhalHw_SetConfig(pDataParams, wConfig, wValue) \
        phhalHw_Rc663_SetConfig((phhalHw_Rc663_DataParams_t *)pDataParams, wConfig, wValue)

#define phhalHw_MfcAuthenticateKeyNo(pDataParams, bBlockNo, bKeyType, wKeyNo, wKeyVersion,pUid) \
        phhalHw_Rc663_MfcAuthenticateKeyNo((phhalHw_Rc663_DataParams_t *)pDataParams,bBlockNo, bKeyType, wKeyNo, wKeyVersion, pUid)

#define phhalHw_MfcAuthenticate(  pDataParams, bBlockNo, bKeyType, pKey, pUid) \
        phhalHw_Rc663_MfcAuthenticate((phhalHw_Rc663_DataParams_t *)pDataParams,bBlockNo,bKeyType, pKey,pUid)

#define phhalHw_GetConfig(pDataParams, wConfig, pValue) \
        phhalHw_Rc663_GetConfig((phhalHw_Rc663_DataParams_t *)pDataParams, wConfig, pValue)

#define phhalHw_FieldOn(pDataParams) \
        phhalHw_Rc663_FieldOn((phhalHw_Rc663_DataParams_t *)pDataParams)

#define phhalHw_FieldOff(pDataParams) \
        phhalHw_Rc663_FieldOff((phhalHw_Rc663_DataParams_t *)pDataParams)

#define phhalHw_FieldReset(pDataParams) \
        phhalHw_Rc663_FieldReset((phhalHw_Rc663_DataParams_t *)pDataParams)

#define phhalHw_Wait(pDataParams, bUnit, wTimeout) \
        phhalHw_Rc663_Wait((phhalHw_Rc663_DataParams_t *)pDataParams, bUnit, wTimeout)

#define phhalHw_Transmit(pDataParams, wOption, pTxBuffer, wTxLength) \
        phhalHw_Rc663_Transmit((phhalHw_Rc663_DataParams_t *)pDataParams, wOption, pTxBuffer, wTxLength)

#define phhalHw_Receive(pDataParams, wOption, ppRxBuffer, pRxLength) \
        phhalHw_Rc663_Receive((phhalHw_Rc663_DataParams_t *)pDataParams, wOption, ppRxBuffer, pRxLength)

#define phhalHw_Lpcd(pDataParams) \
        phhalHw_Rc663_Cmd_Lpcd((phhalHw_Rc663_DataParams_t *)pDataParams)

#define phhalHw_AsyncAbort(pDataParams) \
        phhalHw_Rc663_AsyncAbort((phhalHw_Rc663_DataParams_t *)pDataParams)

#define phhalHw_Autocoll(pDataParams, wMode, ppRxBuffer, pRxLength, pProtParams) PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_COMMAND, PH_COMP_HAL)

#define phhalHw_I18000p3m3Inventory(pDataParams, pSelCmd, bSelCmdLen, bNumValidBitsinLastByte, pBeginRndCmd, bTSprocessing, ppRxBuffer, wRxBufferLen)\
        phhalHw_Rc663_I18000p3m3Inventory((phhalHw_Rc663_DataParams_t *)pDataParams, pSelCmd, bSelCmdLen, bNumValidBitsinLastByte, pBeginRndCmd, bTSprocessing, ppRxBuffer, wRxBufferLen)

#define phhalHw_I18000p3m3ResumeInventory(pDataParams, ppRxBuffer, wRxBufferLen)\
         phhalHw_Rc663_I18000p3m3ResumeInventory((phhalHw_Rc663_DataParams_t *)pDataParams, ppRxBuffer, wRxBufferLen)

#define phhalHw_DeInit(pDataParams) \
         phhalHw_Rc663_DeInit((phhalHw_Rc663_DataParams_t *)pDataParams)
#endif

#if defined  (NXPRDLIB_REM_GEN_INTFS) && \
    defined (NXPBUILD__PHHAL_HW_RC523)
#include "../comps/phhalHw/src/Rc523/phhalHw_Rc523.h"

#define phhalHw_Exchange(pDataParams,wOption,pTxBuffer,wTxLength,ppRxBuffer,pRxLength) \
        phhalHw_Rc523_Exchange((phhalHw_Rc523_DataParams_t *)pDataParams, wOption, pTxBuffer, wTxLength, ppRxBuffer, pRxLength)

#define phhalHw_ApplyProtocolSettings(pDataParams, bMode) \
        phhalHw_Rc523_ApplyProtocolSettings((phhalHw_Rc523_DataParams_t *)pDataParams, bMode)

#define phhalHw_SetConfig(pDataParams, wConfig, wValue) \
        phhalHw_Rc523_SetConfig((phhalHw_Rc523_DataParams_t *)pDataParams, wConfig, wValue)

#define phhalHw_MfcAuthenticateKeyNo(pDataParams, bBlockNo, bKeyType, wKeyNo, wKeyVersion,pUid) \
        phhalHw_Rc523_MfcAuthenticateKeyNo((phhalHw_Rc523_DataParams_t *)pDataParams,bBlockNo, bKeyType, wKeyNo, wKeyVersion, pUid)

#define phhalHw_MfcAuthenticate(  pDataParams, bBlockNo, bKeyType, pKey, pUid) \
        phhalHw_Rc523_MfcAuthenticate((phhalHw_Rc523_DataParams_t *)pDataParams,bBlockNo,bKeyType, pKey,pUid)

#define phhalHw_GetConfig(pDataParams, wConfig, pValue) \
        phhalHw_Rc523_GetConfig((phhalHw_Rc523_DataParams_t *)pDataParams, wConfig, pValue)

#define phhalHw_FieldOn(pDataParams) \
        phhalHw_Rc523_FieldOn((phhalHw_Rc523_DataParams_t *)pDataParams)

#define phhalHw_FieldOff(pDataParams) \
        phhalHw_Rc523_FieldOff((phhalHw_Rc523_DataParams_t *)pDataParams)

#define phhalHw_FieldReset(pDataParams) \
        phhalHw_Rc523_FieldReset((phhalHw_Rc523_DataParams_t *)pDataParams)

#define phhalHw_Wait(pDataParams, bUnit, wTimeout) \
        phhalHw_Rc523_Wait((phhalHw_Rc523_DataParams_t *)pDataParams, bUnit, wTimeout)

#define phhalHw_Transmit(pDataParams, wOption, pTxBuffer, wTxLength) \
        phhalHw_Rc523_Transmit((phhalHw_Rc523_DataParams_t *)pDataParams, wOption, pTxBuffer, wTxLength)

#define phhalHw_Receive(pDataParams, wOption, ppRxBuffer, pRxLength) \
        phhalHw_Rc523_Receive((phhalHw_Rc523_DataParams_t *)pDataParams, wOption, ppRxBuffer, pRxLength)

#define phhalHw_Autocoll(pDataParams, wMode, ppRxBuffer, pRxLength, pProtParams) \
        phhalHw_Rc523_Autocoll((phhalHw_Rc523_DataParams_t *)pDataParams, wMode, ppRxBuffer, pRxLength, pProtParams);

#define phhalHw_AsyncAbort(pDataParams) \
        phhalHw_Rc523_AsyncAbort((phhalHw_Rc523_DataParams_t *)pDataParams)

#define phhalHw_Lpcd(pDataParams) PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_PARAMETER, PH_COMP_HAL)

#define phhalHw_DeInit(pDataParams) \
         phhalHw_Rc523_DeInit((phhalHw_Rc523_DataParams_t *)pDataParams)
#endif


#if defined  (NXPRDLIB_REM_GEN_INTFS) && \
    defined (NXPBUILD__PHHAL_HW_PN5180)
#include "../comps/phhalHw/src/Pn5180/phhalHw_Pn5180.h"
#include "phhalHw_Pn5180_Instr.h"

#define phhalHw_Exchange(pDataParams,wOption,pTxBuffer,wTxLength,ppRxBuffer,pRxLength) \
        phhalHw_Pn5180_Exchange((phhalHw_Pn5180_DataParams_t *)pDataParams, wOption, pTxBuffer, wTxLength, ppRxBuffer, pRxLength)

#define phhalHw_ApplyProtocolSettings(pDataParams, bMode) \
        phhalHw_Pn5180_ApplyProtocolSettings((phhalHw_Pn5180_DataParams_t *)pDataParams, bMode)

#define phhalHw_SetConfig(pDataParams, wConfig, wValue) \
        phhalHw_Pn5180_SetConfig((phhalHw_Pn5180_DataParams_t *)pDataParams, wConfig, wValue)

#define phhalHw_MfcAuthenticateKeyNo(pDataParams, bBlockNo, bKeyType, wKeyNo, wKeyVersion,pUid) \
        phhalHw_Pn5180_MfcAuthenticateKeyNo((phhalHw_Pn5180_DataParams_t *)pDataParams,bBlockNo, bKeyType, wKeyNo, wKeyVersion, pUid)

#define phhalHw_MfcAuthenticate(  pDataParams, bBlockNo, bKeyType, pKey, pUid) \
        phhalHw_Pn5180_MfcAuthenticate((phhalHw_Pn5180_DataParams_t *)pDataParams,bBlockNo,bKeyType, pKey,pUid)

#define phhalHw_GetConfig(pDataParams, wConfig, pValue) \
        phhalHw_Pn5180_GetConfig((phhalHw_Pn5180_DataParams_t *)pDataParams, wConfig, pValue)

#define phhalHw_FieldOn(pDataParams) \
        phhalHw_Pn5180_FieldOn((phhalHw_Pn5180_DataParams_t *)pDataParams)

#define phhalHw_FieldOff(pDataParams) \
        phhalHw_Pn5180_FieldOff((phhalHw_Pn5180_DataParams_t *)pDataParams)

#define phhalHw_FieldReset(pDataParams) \
        phhalHw_Pn5180_FieldReset((phhalHw_Pn5180_DataParams_t *)pDataParams)

#define phhalHw_Wait(pDataParams, bUnit, wTimeout) \
        phhalHw_Pn5180_Wait((phhalHw_Pn5180_DataParams_t *)pDataParams, bUnit, wTimeout)

#define phhalHw_Transmit(pDataParams, wOption, pTxBuffer, wTxLength) \
        phhalHw_Pn5180_Transmit((phhalHw_Pn5180_DataParams_t *)pDataParams, wOption, pTxBuffer, wTxLength)

#define phhalHw_Receive(pDataParams, wOption, ppRxBuffer, pRxLength) \
        phhalHw_Pn5180_Receive((phhalHw_Pn5180_DataParams_t *)pDataParams, wOption, ppRxBuffer, pRxLength)

#define phhalHw_Lpcd(pDataParams) \
        phhalHw_Pn5180_Lpcd((phhalHw_Pn5180_DataParams_t *)pDataParams)

#define phhalHw_AsyncAbort(pDataParams) \
        phhalHw_Pn5180_AsyncAbort((phhalHw_Pn5180_DataParams_t *)pDataParams)

#define phhalHw_Autocoll(pDataParams, wMode, ppRxBuffer, pRxLength, pProtParams) \
        phhalHw_Pn5180_Autocoll((phhalHw_Pn5180_DataParams_t *)pDataParams, wMode, ppRxBuffer,  pRxLength, pProtParams)

#define phhalHw_I18000p3m3Inventory(pDataParams, pSelCmd, bSelCmdLen, bNumValidBitsinLastByte, pBeginRndCmd, bTSprocessing, ppRxBuffer, wRxBufferLen)\
        phhalHw_Pn5180_I18000p3m3Inventory((phhalHw_Pn5180_DataParams_t *)pDataParams, pSelCmd, bSelCmdLen, bNumValidBitsinLastByte, pBeginRndCmd, bTSprocessing, ppRxBuffer, wRxBufferLen)

#define phhalHw_I18000p3m3ResumeInventory(pDataParams, ppRxBuffer, wRxBufferLen)\
        phhalHw_Pn5180_18000p3m3ResumeInventory((phhalHw_Pn5180_DataParams_t *)pDataParams, ppRxBuffer, wRxBufferLen)

#define phhalHw_DeInit(pDataParams) \
         phhalHw_Pn5180_DeInit((phhalHw_Pn5180_DataParams_t *)pDataParams)
#endif


#ifndef NXPRDLIB_REM_GEN_INTFS /* Without optimization */
    /**
    * \brief Perform data exchange with target/PICC in initiator/PCD mode.
    *
    * This is a blocking exchange function to be used by all PAL components of reader, initiator.
    * Frame step option required for T1T/Jewel protocol can be set using a SetConfig - #PHHAL_HW_CONFIG_JEWEL_MODE
    * It is recommended that the Tx Buffer pointer supplied to this function is word aligned (word alignment specific to the
    * micro-controller platform where this is used)
    *
    * Before this function is called, it is expected that the user/upper layer has performed\n
    * \li BAL initialization (where applicable) - Example - phbalReg_Serial_Init()
    * \li HAL initialization - Example phhalHw_Rc523_Init();
    * \li Load the protocol -  phhalHw_ApplyProtocolSettings()
    * \li Field ON          -  phhalHw_FieldOn(); or phhalHw_FieldReset()
    * \li Wait for initial guard time using phhalHw_Wait() in case phhalHw_FieldOn() was called earlier
    * \li Set or modify any configuration parameters using phhalHw_SetConfig()
    * \li Call phhalHw_Exchange() with relevant protocol command/data.
    *
    * \b wOption can be one of:\n
    * \li #PH_EXCHANGE_DEFAULT
    *
    * \b wOption can be combined with:\n
    * \li #PH_EXCHANGE_BUFFERED_BIT
    * \li #PH_EXCHANGE_LEAVE_BUFFER_BIT
    *
    * Note: When activated in target mode using \ref phhalHw_Autocoll API, exchange should not be used.\n
    *
    * Please refer to #PHHAL_HW_CONFIG_PARITY from where all the configuration parameters are defined that will
    * affect the behavior of this API.
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_INVALID_PARAMETER \b wOption is invalid.
    * \retval #PH_ERR_SUCCESS_INCOMPLETE_BYTE Operation successful, incomplete byte received; Retrieve number of valid bits of last byte with #PHHAL_HW_CONFIG_RXLASTBITS.
    * \retval #PH_ERR_IO_TIMEOUT No response detected within the configured time frame.
    * \retval #PH_ERR_INTEGRITY_ERROR Response received but CRC or parity is invalid.
    * \retval #PH_ERR_COLLISION_ERROR Response received but a collision occurred; Retrieve number of valid bits of last byte with #PHHAL_HW_CONFIG_RXLASTBITS.
    * \retval #PH_ERR_BUFFER_OVERFLOW Internal receive buffer is too small for transmission or smaller than the response.
    * \retval #PH_ERR_FRAMING_ERROR Frame format is either invalid for configured protocol or corrupted.
    * \retval #PH_ERR_PROTOCOL_ERROR Frame format is definitely invalid for configured protocol or received response by Type 1 Tag is less than 4 Bytes.
    * \retval #PH_ERR_ABORTED Returned when this API exits after \ref phhalHw_AsyncAbort API is called from another thread.
    * \retval #PH_ERR_EXT_RF_ERROR External RF is ON during this exchange or if peer RF did not turn ON within Tadt in Active Mode.
    * \retval #PH_ERR_READ_WRITE_ERROR Hardware problem.
    * \retval #PH_ERR_TEMPERATURE_ERROR Hardware problem.
    * \retval #PH_ERR_INTERFACE_ERROR Hardware problem.
    * \retval #PH_ERR_INTERNAL_ERROR Unexpected Hardware problem or Software logic that should never be executed.
    */
    phStatus_t phhalHw_Exchange(
        void * pDataParams,     /**< [In] Pointer to this layer's parameter structure. */
        uint16_t wOption,       /**< [In] Option parameter. */
        uint8_t * pTxBuffer,    /**< [In] Data to transmit. */
        uint16_t wTxLength,     /**< [In] Number of bytes to transmit. */
        uint8_t ** ppRxBuffer,  /**< [Out] Pointer to received data. */
        uint16_t * pRxLength    /**< [Out] Number of received data bytes. */
        );

    /**
    * \brief Configure reader IC for a particular reader/initiator protocol.
    *
    * Used to apply hardware settings for a particular protocol for reader, initiator modes.
    * Example: ISO 14443-3A, ISO 14443-3B, ISO 18092 initiator etc. For target or card emulation \ref phhalHw_Autocoll
    * should be used. An IC specific configuration for setting antenna type before applying protocol settings
    * may be required during development phase and this should be managed by IC specific HALs.
    *
    *
    * \b bMode can be:\n
    * \li #PHHAL_HW_CARDTYPE_CURRENT
    * \li #PHHAL_HW_CARDTYPE_ISO14443A
    * \li #PHHAL_HW_CARDTYPE_ISO14443B
    * \li #PHHAL_HW_CARDTYPE_FELICA_212
    * \li #PHHAL_HW_CARDTYPE_ISO15693
    * \li #PHHAL_HW_CARDTYPE_ICODEEPCUID
    * \li #PHHAL_HW_CARDTYPE_I18000P3M3
    * \li #PHHAL_HW_CARDTYPE_I18092M_ACTIVE_106
    * \li #PHHAL_HW_CARDTYPE_I18092M_ACTIVE_212
    * \li #PHHAL_HW_CARDTYPE_I18092M_ACTIVE_424
    * \li #PHHAL_HW_CARDTYPE_FELICA_424
    *
    *
    * Before this function is called, it is expected that the user/upper layer has performed\n
    * \li BAL initialization (where applicable) - Example - phbalReg_Serial_Init()
    * \li HAL initialization - Example phhalHw_Rc523_Init();
    *
    * Only after calling this function, the RF field has to be turned ON
    * using phhalHw_FieldOn() or phhalHw_FieldReset()
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_INVALID_PARAMETER \b bCardType is invalid or not supported.
    * \retval #PH_ERR_INTERFACE_ERROR Hardware problem.
    */
    phStatus_t phhalHw_ApplyProtocolSettings(
        void * pDataParams, /**< [In] Pointer to this layer's parameter structure. */
        uint8_t bMode       /**< [In] Mode in which the IC hardware is to be configured. */
        );

    /**
    * \brief Perform MIFARE(R) Classic authentication using a key number.
    *
    * wKeyVersion and wKeyNo parameters are assigned for a particular key.\n
    * When hardware key store is used, the keys are stored in the reader IC.\n
    * The keys are referenced by using key number and key version. \n
    *
    * When software key store is used (please refer to phKeyStore.h), the key number\n
    * and key version are as assigned in the phKeyStore_Sw_KeyVersionPair_t structure.\n
    *
    * Before this function is called, it is expected that the user/upper layer has performed\n
    * \li BAL initialization (where applicable) - Example - phbalReg_Serial_Init()
    * \li HAL initialization - Example phhalHw_Rc523_Init();
    * \li Load the protocol -  phhalHw_ApplyProtocolSettings()
    * \li Field ON          -  phhalHw_FieldOn(); or phhalHw_FieldReset()
    * \li Wait for initial Guard time using phhalHw_Wait()
    * \li Activate the MIFARE Classic / MIFARE Plus SL1 card using ISO14443-3A commands.
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_INVALID_PARAMETER \b bKeyType, \b wKeyNo or \b wKeyVersion is invalid.
    * \retval #PH_ERR_IO_TIMEOUT Error in authentication.
    * \retval #PH_ERR_AUTH_ERROR Error in authentication.
    */
    phStatus_t phhalHw_MfcAuthenticateKeyNo(
        void * pDataParams,     /**< [In] Pointer to this layer's parameter structure. */
        uint8_t bBlockNo,       /**< [In] Block number on card to authenticate to. */
        uint8_t bKeyType,       /**< [In] Either #PHHAL_HW_MFC_KEYA or #PHHAL_HW_MFC_KEYB. */
        uint16_t wKeyNo,        /**< [In] Key number to be used in authentication. */
        uint16_t wKeyVersion,   /**< [In] Key version to be used in authentication. */
        uint8_t * pUid          /**< [In] Serial number of current cascade level; uint8_t[4]. */
        );

    /**
    * \brief Perform MIFARE(R) Classic authentication using a key.
    *
    * Here the 6 byte MIFARE Key is supplied as an argument (pKey).
    *
    * Before this function is called, it is expected that the user/upper layer has performed\n
    * \li BAL initialization (where applicable) - Example - phbalReg_Serial_Init()
    * \li HAL initialization - Example phhalHw_Rc523_Init();
    * \li Load the protocol -  phhalHw_ApplyProtocolSettings();
    * \li Field ON          -  phhalHw_FieldOn();
    * \li Wait for initial Guard time using phhalHw_Wait();
    * \li Activate the MIFARE Classic / MIFARE Plus SL1 card using ISO14443-3A commands
    *
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_INVALID_PARAMETER \b bKeyType is invalid.
    * \retval #PH_ERR_IO_TIMEOUT Error in authentication.
    * \retval #PH_ERR_AUTH_ERROR Error in authentication.
    */
    phStatus_t phhalHw_MfcAuthenticate(
        void * pDataParams,  /**< [In] Pointer to this layer's parameter structure. */
        uint8_t bBlockNo,    /**< [In] Block number on card to authenticate to. */
        uint8_t bKeyType,    /**< [In] Either #PHHAL_HW_MFC_KEYA or #PHHAL_HW_MFC_KEYB */
        uint8_t * pKey,      /**< [In] Key to be used in authentication. */
        uint8_t * pUid       /**< [In] Serial number of current cascade level; uint8_t[4]. */
        );

    /**
    * \brief Set configuration parameter.
    *
    * All mandatory configuration parameters are to be implemented
    * to be compliant to generic HAL specification \n
    * Refer #PHHAL_HW_CONFIG_PARITY and subsequent configuration parameters that can be set using this API.
    *
    * This operation is performed by user application or upper layer to set/change the behavior of the
    * HAL APIs.
    * For example - To enable parity during phhalHw_Exchange(), \n
    * phhalHw_SetConfig(pHal, PHHAL_HW_CONFIG_PARITY, PH_ON); needs to be called.
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_UNSUPPORTED_PARAMETER Configuration is not supported or invalid.
    * \retval #PH_ERR_INVALID_PARAMETER Parameter value is invalid.
    * \retval #PH_ERR_PARAMETER_OVERFLOW Setting the parameter value would lead to an overflow.
    * \retval #PH_ERR_INTERFACE_ERROR Communication error.
    */
    phStatus_t phhalHw_SetConfig(
        void * pDataParams,    /**< [In] Pointer to this layer's parameter structure. */
        uint16_t wConfig,      /**< [In] Configuration identifier. */
        uint16_t wValue        /**< [In] Configuration value. */
        );

    /**
    * \brief Get configuration parameter.
    *
    * All mandatory configuration parameters are to be implemented
    * to be compliant to generic HAL specification. \n
    * Refer #PHHAL_HW_CONFIG_PARITY and subsequent configuration parameters whose value can be read using this API.
    *
    * This operation is performed by user application or upper layer to get the value of a configuration parameter
    * of the HAL.
    * For example - To get the number of last bits received during phhalHw_Exchange(), \n
    * phhalHw_GetConfig(pHal, PHHAL_HW_CONFIG_RXLASTBITS, &wLastBits);  needs to be called.
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_UNSUPPORTED_PARAMETER Configuration is not supported or invalid.
    * \retval #PH_ERR_INVALID_PARAMETER Parameter value is invalid.
    * \retval #PH_ERR_PARAMETER_OVERFLOW The parameter value is larger than the range of \b pValue.
    * \retval #PH_ERR_INTERFACE_ERROR Communication error.
    */
    phStatus_t phhalHw_GetConfig(
        void * pDataParams,    /**< [In] Pointer to this layer's parameter structure. */
        uint16_t wConfig,      /**< [In] Configuration identifier. */
        uint16_t * pValue      /**< [Out] Configuration value. */
        );

    /**
    * \brief Switch ON the RF-field.
    *
    * If the configuration parameter #PHHAL_HW_CONFIG_RFCA is set, it performs
    * initial RF collision avoidance(I-RFCA) in initiator/reader mode before switching ON the field.\n
    * If parameter #PHHAL_HW_CONFIG_RFCA is not set, field ON is done without performing RFCA.\n
    *
    * This API shall be called only after \ref phhalHw_ApplyProtocolSettings.
    * An example sequence is given below \n
    * \li BAL initialization (where applicable) - Example - phbalReg_Serial_Init()
    * \li HAL initialization - Example phhalHw_Rc523_Init();
    * \li Load the protocol -  phhalHw_ApplyProtocolSettings(pHal, PHHAL_HW_CARDTYPE_I18092M_ACTIVE_106);
    * \li Field ON          -  phhalHw_FieldOn();
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_INTERFACE_ERROR Communication error.
    * \retval #PH_ERR_RF_ERROR External RF is detected during RF Collision avoidance when RFCA is enabled.
    */
    phStatus_t phhalHw_FieldOn(
        void * pDataParams   /**< [In] Pointer to this layer's parameter structure. */
        );

    /**
    * \brief Switch off the RF-field.
    *
    * If this API is called when communication mode is "active initiator",
    * this will reset the Auto-RF OFF behavior of the active initiator mode\n
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_INTERFACE_ERROR Communication error.
    */
    phStatus_t phhalHw_FieldOff(
        void * pDataParams  /**< [In] Pointer to this layer's parameter structure. */
        );

    /**
    * \brief Perform RF-Reset.
    *
    * Switches OFF, waits for a configured time, and then turns the RF back to ON.\n
    * This is expected to retain the protocol settings that were applied before calling this function.
    * The time to wait before RF ON is configured using a SetConfig call.
    *
    * #PHHAL_HW_CONFIG_FIELD_OFF_TIME (in milliseconds) can be set to a particular value
    * to keep the field OFF for the configured time value before turning it ON.
    *
    * #PHHAL_HW_CONFIG_FIELD_RECOVERY_TIME (in milliseconds) can be set to a particular value
    * to wait for RF recovery time after field is switched ON.
    *
    * A typical sequence in which this API is called is given below\n
    * \li BAL initialization (where applicable) - Example - phbalReg_Serial_Init()
    * \li HAL initialization - Example phhalHw_Rc523_Init();
    * \li Load the protocol -  phhalHw_ApplyProtocolSettings();
    * \li Field ON          -  phhalHw_FieldOn();
    * \li Wait for initial guard time - phalHw_Wait();
    * \li Send/Receive protocol commands
    * \li Field reset       - phhalHw_FieldReset();
    * \li Restart activation commands followed by other protocol commands.
    * \li Field OFF         - phhalHw_FieldOff();
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_INTERFACE_ERROR Communication error.
    */
    phStatus_t phhalHw_FieldReset(
        void * pDataParams    /**< [In] Pointer to this layer's parameter structure. */
        );

    /**
    * \brief phhalHw_Wait function waits/loops until the input time value is elapsed.\n
    *
    * This function can be configured to generate Time delays in micro or milliseconds.
    * This can be used as initial RF guard time delay in discovery loop
    *
    * A typical sequence in which this API is called is given below\n
    * \li BAL initialization (where applicable) - Example - phbalReg_Serial_Init();
    * \li HAL initialization - Example phhalHw_Rc523_Init();
    * \li Load the protocol -  phhalHw_ApplyProtocolSettings();
    * \li Field ON          -  phhalHw_FieldOn();
    * \li Wait for initial guard time - phalHw_Wait(pHal, 5100, PHHAL_HW_TIME_MICROSECONDS);
    * \li Send/Receive protocol commands
    * \li Field Reset       - phhalHw_FieldReset();
    * \li Restart activation commands followed by other protocol commands.
    * \li Field OFF         - phhalHw_FieldOff();
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_INVALID_PARAMETER \b bUnit is invalid.
    * \retval #PH_ERR_INTERFACE_ERROR Communication error.
    */
    phStatus_t phhalHw_Wait(
        void * pDataParams, /**< [In] Pointer to this layer's parameter structure. */
        uint8_t bUnit,      /**< [In] Unit of given timeout value (either #PHHAL_HW_TIME_MICROSECONDS or #PHHAL_HW_TIME_MILLISECONDS). */
        uint16_t wTimeout   /**< [In] Timeout value. */
        );

    /**
    * \brief Used to transmit data. This is a generic transmit-only function that can be used in initiator/PCD and target modes.
    *
    * In the initiator/reader mode, the FWT timer starts after last bit transmitted to handle timeout and
    * is automatically stopped after first bit is received in a phhalHw_Receive() function call.
    *
    * Note: <em>Special behavior:</em> To use phhalHw_Transmit API in target mode \ref phhalHw_Autocoll
    * API shall be executed before.
    *
    * \b wOption can be one of:\n
    * \li #PH_TRANSMIT_DEFAULT
    * \li #PH_TRANSMIT_PREV_FRAME
    *
    * \b If wOption can be combined with:\n
    * \li #PH_TRANSMIT_BUFFERED_BIT
    * \li #PH_TRANSMIT_LEAVE_BUFFER_BIT
    *
    * NOTE : #PH_TRANSMIT_PREV_FRAME cannot be combined with any of the buffering options above.
    *
    * A typical sequence in which this API is called in initiator mode is given below\n
    * \li BAL initialization (where applicable) - Example - phbalReg_Serial_Init();
    * \li HAL initialization - Example phhalHw_Rc523_Init();
    * \li Load the protocol -  phhalHw_ApplyProtocolSettings(pHal, PHHAL_HW_CARDTYPE_I18092M_ACTIVE_106);
    * \li Field ON          -  phhalHw_FieldOn();
    * \li Wait for initial guard time - phalHw_Wait(pHal, 5100, PHHAL_HW_TIME_MICROSECONDS);
    * \li Transmit the ATR_REQ - phhalHw_Transmit(pHal, PH_EXCHANGE_DEFAULT, pATR_REQ, wATR_len);
    * \li Receive the response - phhalHw_Receive(pHal, 0x00, &pResponse, &wRxLen);
    * \li Other command exchanges
    * \li Field OFF         - phhalHw_FieldOff();
    *
    * A typical sequence in which this API is called in target/PICC mode is given below\n
    * \li BAL initialization (where applicable) - Example - phbalReg_Serial_Init()
    * \li HAL initialization - Example phhalHw_Rc523_Init();
    * \li HAL specific listen mode configuration - Example phhalHw_Rc523_SetListenParameters(pHal, pSENS_RES, pNfcId1, bSelelctResponse, pFelicaResponse, bNfcId3);
    * \li Start target mode -  phhalHw_Autocoll(pHal, 0x05, &pResponse, &wRxlen, &wProtParams);
    * \li if (pResponse is valid ATR_REQ) then phhalHw_Transmit(pHal, PH_TRANSMIT_DEFAULT, pATR_RES, wATR_len);
    * \li Receive the response - phhalHw_Receive(pHal, 0x00, &pResponse, &wRxLen);
    * \li Other response/command exchanges
    * \li Termination of the target flow.
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_INVALID_PARAMETER wOption parameter is invalid.
    * \retval #PH_ERR_INTERFACE_ERROR Communication error.
    * \retval #PH_ERR_EXT_RF_ERROR In active mode if External RF is present during this transmit or in passive target mode
    *                              if external field is not present before initiating this transmit.
    * \retval #PH_ERR_ABORTED Returned when this API exits after \ref phhalHw_AsyncAbort API is called from another thread.
    * \retval #PH_ERR_INTERNAL_ERROR If transmission was not successful due to some Hardware or Memory alignment issues.
    * \retval #PH_ERR_BUFFER_OVERFLOW If the buffer provided by PAL or Application to HAL exceeds the HAL Tx Buffer and other
                                      platform specific reasons.
    */
    phStatus_t phhalHw_Transmit(
        void * pDataParams,   /**< [In] Pointer to this layer's parameter structure. */
        uint16_t wOption,     /**< [In] Option parameter. */
        uint8_t * pTxBuffer,  /**< [In] Data to transmit. */
        uint16_t wTxLength    /**< [In] Number of bytes to transmit. */
        );

    /**
    * \brief Used to receive data. Can be used in initiator/PCD and target/PICC Modes.
    *
    * Note: <em>Special behavior:</em> To use phhalHw_Receive API in target Mode, \ref phhalHw_Autocoll API shall be executed
    * previously. In target mode this API will wait for receive interrupt or external RF OFF interrupt (in case of passive mode).
    *
    * \b wOption is RFU and should be set to 0x00
    *
    * A typical sequence in which this API is called in initiator mode is given below\n
    * \li BAL initialization (where applicable) - Example - phbalReg_Serial_Init()
    * \li HAL initialization - Example phhalHw_Rc523_Init();
    * \li Load the protocol -  phhalHw_ApplyProtocolSettings(pHal, PHHAL_HW_CARDTYPE_I18092M_ACTIVE_106);
    * \li Field ON          -  phhalHw_FieldOn()
    * \li Wait for initial guard time - phalHw_Wait(pHal, 5100, PHHAL_HW_TIME_MICROSECONDS);
    * \li Transmit the ATR_REQ - phhalHw_Transmit(pHal, PH_EXCHANGE_DEFAULT, pATR_REQ, wATR_len);
    * \li Receive the response - phhalHw_Receive(pHal, 0x00, &pResponse, &wRxLen);
    * \li Other command exchanges
    * \li Field OFF         - phhalHw_FieldOff();
    *
    * A typical sequence in which this API is called in target/PICC mode is given below\n
    * \li BAL initialization (where applicable) - Example - phbalReg_Serial_Init()
    * \li HAL initialization - Example phhalHw_Rc523_Init();
    * \li HAL specific listen mode configuration - Example phhalHw_Rc523_SetListenParameters(pHal, pSENS_RES, pNfcId1, bSelelctResponse, pFelicaResponse, bNfcId3);
    * \li Start target mode -  phhalHw_Autocoll(pHal, 0x05, &pResponse, &wRxlen, &wProtParams);
    * \li if (pResponse is valid RATS) then phhalHw_Transmit(pHal, PH_EXCHANGE_DEFAULT, pATS, wATS_len);
    * \li Receive the response - phhalHw_Receive(pHal, 0x00, &pResponse, &wRxLen);
    * \li Other response/command exchanges
    * \li Termination of the target flow.
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_INVALID_PARAMETER The option provided is invalid.
    * \retval #PH_ERR_IO_TIMEOUT No response detected within the configured time frame.
    * \retval #PH_ERR_INTEGRITY_ERROR Response received but CRC or parity is invalid.
    * \retval #PH_ERR_COLLISION_ERROR Response received but a collision occurred; Retrieve number of valid bits of last byte
    *                                 with #PHHAL_HW_CONFIG_RXLASTBITS.
    * \retval #PH_ERR_BUFFER_OVERFLOW Internal receive buffer is smaller than the received response.
    * \retval #PH_ERR_FRAMING_ERROR Frame format is either invalid for configured protocol or corrupted.
    * \retval #PH_ERR_PROTOCOL_ERROR Frame format is definitely invalid for configured protocol.
    * \retval #PH_ERR_ABORTED Returned when this API exits after \ref phhalHw_AsyncAbort API is called from another thread.
    * \retval #PH_ERR_EXT_RF_ERROR In passive target mode, external reader/initiator switched OFF its field before/during reception
    *                              or if peer RF did not turn ON within Tadt in Active Mode.
    * \retval #PH_ERR_READ_WRITE_ERROR Hardware problem.
    * \retval #PH_ERR_TEMPERATURE_ERROR Hardware problem.
    * \retval #PH_ERR_INTERFACE_ERROR Communication error.
    * \retval #PH_ERR_INTERNAL_ERROR Unexpected Hardware problem (like in active mode if modem state is WAIT_RECIEVE but the internal RF field is ON).
    */
    phStatus_t phhalHw_Receive(
        void * pDataParams,     /**< [In]  Pointer to this layer's parameter structure. */
        uint16_t wOption,       /**< [In]  Option parameter. RFU. Should be set to 0x00. */
        uint8_t ** ppRxBuffer,  /**< [Out] Pointer to received data. */
        uint16_t * pRxLength    /**< [Out] Number of received data bytes. */
        );

    /**
    * \brief Used to configure the IC to NFC target or card mode based on the configured option.
    * Before calling this API \ref phhalHw_FieldOff API should be called.
    *
    * This is a blocking call that waits for reception of polling frames for the technologies mentioned in the wMode bit map and
    * terminates with a response having command buffer received from an external reader/initiator.
    * After target / PICC activation, the CRC is validated and removed before handing over the received
    * frame to the caller.
    * If target is activated in Active communication mode, then Auto RF-OFF is enabled by this API.
    * The technologies mentioned in wMode or a combination of these depends on the hardware capability.\n
    *
    * Note: If #PHHAL_HW_CONFIG_MFHALTED configuration parameter is set then this function will only respond to WUPA/ALL_REQ.
    *
    * IC specific phhalHw_XXX_SetListenParameters API has to be called to configure the NFC Type A, Type B(Optional) and
    * Type F responses in the IC before calling this function.
    *
    * The LSB and MSB of wMode has to be coded as follows.\n
    * <pre>
    *      | - - - - - - - - - - - - - - - - - - - - - - - -|
    *  bit |  7 | 6 | 5 | 4 | 3 |   2    |   1     |   0    |
    *      | - - - - - - - - - - - - - - - - - - - - - - - -|
    *  Val |  x | x | x | x | x | Type F |  Type B | Type A |
    *      | - - - - - - - - - - - - - - - - - - - - - - - -|
    * </pre>
    *
    * The LSB codes target/PICC technology supported for NFC passive technology and MSB codes target/PICC technology supported for NFC active technology\n
    *
    *
    * A typical sequence in which this API is called in a target/PICC use case is given below\n
    * \li BAL initialization (where applicable) - Example - phbalReg_Serial_Init()
    * \li HAL initialization - Example phhalHw_Rc523_Init();
    * \li HAL specific listen mode configuration - Example phhalHw_Rc523_SetListenParameters(pHal, pSENS_RES, pNfcId1, bSelelctResponse, pFelicaResponse, bNfcId3);
    * \li Field OFF         - phhalHw_FieldOff();
    * \li Start target mode -  phhalHw_Autocoll(pHal, 0x05, &pResponse, &wRxlen, &wProtParams);
    * \li if (pResponse is valid ATR_REQ) then phhalHw_Transmit(pHal, PH_EXCHANGE_DEFAULT, pATR_RES, wATR_len);
    * \li Receive the next command - phhalHw_Receive(pHal, 0x00, &pResponse, &wRxLen);
    * \li Other response/command exchanges
    * \li Termination of the target flow.
    *
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_IO_TIMEOUT No response received.
    * \retval #PH_ERR_EXT_RF_ERROR External RF was OFF when this API is called or switched OFF before active/passive activation.
    * \retval #PH_ERR_PROTOCOL_ERROR Protocol error.
    * \retval #PH_ERR_INTEGRITY_ERROR Response received but CRC or Parity is invalid.
    * \retval #PH_ERR_BUFFER_OVERFLOW Internal receive buffer is too small for transmission or smaller than the response.
    * \retval #PH_ERR_FRAMING_ERROR Frame format is either invalid for configured protocol or corrupted.
    * \retval #PH_ERR_PROTOCOL_ERROR Frame format is definitely invalid for configured protocol.
    * \retval #PH_ERR_ABORTED Returned when this API exits after \ref phhalHw_AsyncAbort API is called from another thread.
    * \retval #PH_ERR_READ_WRITE_ERROR Hardware problem.
    * \retval #PH_ERR_TEMPERATURE_ERROR Hardware problem.
    * \retval #PH_ERR_INTERFACE_ERROR Communication error.
    */
    phStatus_t phhalHw_Autocoll(
        void * pDataParams,     /**< [In] Pointer to this layer's parameter structure. */
        uint16_t wMode,         /**< [In] Bitmap of the modes that this will support in target/card emulation mode. */
        uint8_t ** ppRxBuffer,  /**< [Out] Pointer to received data after Activation. It can be an error frame or ATR_REQ/RATS. */
        uint16_t * pRxLength,   /**< [Out] Number of received data bytes. */
        uint16_t * pProtParams  /**< [Out] Contains frame mode in the MSB Byte and Rx speed information in the LSB Byte.
                                           MSB value can be one of #PHHAL_HW_RF_TYPE_A_FRAMING, #PHHAL_HW_RF_TYPE_B_FRAMING, #PHHAL_HW_RF_TYPE_F_FRAMING.\n
                                           LSB value can be one of #PHHAL_HW_RF_DATARATE_106, #PHHAL_HW_RF_DATARATE_212, #PHHAL_HW_RF_DATARATE_424, #PHHAL_HW_RF_DATARATE_848. */
        );

    /**
    * \brief Perform Low-Power-Card-Detection.
    *
    * Note: IC specific API has to be called to configure required parameters for LPCD before calling this function.
    * Example - phhalHw_Rc663_Cmd_Lpcd_SetConfig for CLRC663 has to be called to configure LPCD.
    *
    * A typical sequence in which this API is called is given below\n
    * \li BAL initialization (where applicable) - Example - phbalReg_Serial_Init()
    * \li HAL initialization - Example phhalHw_Rc663_Init();
    * \li HAL specific LPCD configuration - phhalHw_Rc663_phhalHw_Rc663_Cmd_Lpcd_SetConfig(pHal, bLPCD_MODE, I_Value, Q_Value, wPowerDownTimeMs, wDetectionTimeUs);
    * \li Start LPCD - phhalHw_Lpcd(pHal); This blocks until a card comes in the field or due to disturbance in the field.
    * \li Upon LPCD return - phhalHw_ApplyProtocolSettings(pHal, PHHAL_HW_CARDTYPE_ISO14443A);
    * \li Turn ON the field - phhalHw_FieldOn();
    * \li Wait for initial guard time - phalHw_Wait(pHal, 5100, PHHAL_HW_TIME_MICROSECONDS);
    * \li Start card activation and subsequent card read/write command exchanges.
    * \li Turn OFF the field - phhalHw_FieldOff();
    * \li Termination of the application flow.
    *
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Card present.
    * \retval #PH_ERR_IO_TIMEOUT No card found.
    * \retval Other Depending on implementation and underlying component.
    */
    phStatus_t phhalHw_Lpcd(
        void * pDataParams      /**< [In] Pointer to this layer's parameter structure. */
        );

    /**
    * \brief Used to asynchronously stop HAL waiting for RF event and exit from HAL.
    *
    * Note: This function will post an 'abort' event which is used by the HAL to move into 'Idle' state and exit from polling for RF Event.
    * The functions that were waiting for a HAL event will return #PH_ERR_ABORTED error code once this function is executed.
    *
    * This function shall be called in a task other than the task where reader library is running.
    * This API can be called at any time after the HAL has been initialized and any of the HAL operations like transmit, receive, exchange,
    * autocoll are in progress.
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    */
    phStatus_t phhalHw_AsyncAbort(
        void * pDataParams      /**< [In] Pointer to this layer's parameter structure. */
        );

    /**
    * \brief ISO 18000p3m3 Inventory command.
    *
    *   Note: This function does
    *   ISO18000p3m3 BeginRound
    *   Sends an Ack
    *   Optionally sends a ReqRN
    *   Stores the information related to tag reply in the HAL RX Buffer
    *   Continues with NextSlot or else pauses and returns based on the option specified
    *   Can return multi card information based on the configuration option and the size of
    *   HAL RX Buffer.
    *   The bTSprocessing (Time slot processing behavior) can take the following values
    *   \li #PHHAL_HW_I18000P3M3_GET_MAX_RESPS - Gets responses from more than one slot limited by the Rx Buffer size and number of slots specified in begin round.
    *   \li #PHHAL_HW_I18000P3M3_ONE_TS_ONLY - Gets response for only one time slot. Here the number of time slots should be zero in the begin round command.
    *   \li #PHHAL_HW_I18000P3M3_GET_TAG_HANDLE - Gets response for only one time slot. Also send a ReqRN to get the tag handle for this slot. Here the number of time slots should be zero in the begin round command.
    *
    *   The response is a Pointer to a buffer containing the result of the inventory.
    *   The buffer may contain the result of one or more timeslots. Each timeslot result has the following structure:
    *   1 Byte time slot status
    *       (
    *        0 ... Tag response available. 'Tag reply length field', 'Valid bits in last byte field' and 'Tag reply field' are present.
    *        1 ... Tag response available. In addition to three field above, 'Tag handle field' also present.
    *        2 ... No tag replied in timeslot. 'Tag reply length field', 'Valid bits in last byte field' and 'Tag reply field', 'Tag Handle' are not present.
    *        3 ... Two or more tags responded in the timeslot. (Collision). Tag Reply Length, valid bits in last byte and tag reply field not present.
    *       )
    *   1 Byte 'Tag reply' Length (1-66)
    *   1 Byte Valid bits in last Byte (0-7, 0 means all bits are valid)
    *   0-66 Bytes Tag reply
    *   0 or 2 Bytes Handle of the tag, in case the field
    *
    * A typical sequence in which this API is called is given below\n
    * \li BAL initialization (where applicable) - Example - phbalReg_Serial_Init()
    * \li HAL initialization - Example phhalHw_Rc663_Init();
    * \li Load ISO 1800p3m3 protocol - phhalHw_ApplyProtocolSettings(pHal, PHHAL_HW_CARDTYPE_I18000P3M3);
    * \li Turn ON the field - phhalHw_FieldOn();
    * \li Wait for initial guard time - phalHw_Wait(pHal, 5100, PHHAL_HW_TIME_MICROSECONDS);
    * \li Start inventory - phhalHw_I18000p3m3Inventory(pHal,pSelectFrame, bSelectFrameLen, bNumValidBits, pBeginRndFrame, bTSProcessing, &pResponse, &wRxLen);
    * \li Use phpalI18000p3m3 and phalI18000p3m3 APIs to further communicate with the tag.
    * \li Turn OFF the field - phhalHw_FieldOff();
    * \li Termination of the application flow.
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_IO_TIMEOUT When there is no tag in the field.
    * \retval #PH_ERR_INVALID_PARAMETER If wrong value specified for bNumValidBits or pBeginRound.
    */
    phStatus_t phhalHw_I18000p3m3Inventory(
        void * pDataParams,                 /**< [In] Pointer to this layer's parameter structure. */
        uint8_t * pSelCmd,                  /**< [In] ISO 18000p3m3 Select command frame. */
        uint8_t bSelCmdLen,                 /**< [In] Select command length in bytes- 1 to 39 bytes. */
        uint8_t bNumValidBitsinLastByte,    /**< [In] Number of valid bits in last byte of pSelCmd. */
        uint8_t * pBeginRndCmd,             /**< [In] ISO 18000p3m3 BeginRound command frame. This is 17bits i.e., 3 bytes are expected. CRC5 should not be provided. */
        uint8_t bTSprocessing,              /**< [In] TimeSlot processing behavior. */
        uint8_t ** ppRxBuffer,              /**< [Out] Pointer to HAL RX Buffer containing response from single/multiple tags. */
        uint16_t * wRxBufferLen             /**< [Out] Length of response in HAL RX Buffer. */
        );


    /**
    * \brief ISO 18000p3m3 resume inventory command.
    *
    * Note: This function is to be called after phhalHw_I18000p3m3Inventory.
    * This internally calls the ISO 18000p3m3 NextSlot command once or multiple times to get responses from
    * remaining slots.
    *
    * Resume inventory also returns the response similar to phhalHw_I18000p3m3Inventory.
    *
    * A typical sequence in which this API is called is given below\n
    * \li BAL initialization (where applicable) - Example - phbalReg_Serial_Init()
    * \li HAL initialization - Example phhalHw_Rc663_Init();
    * \li Load ISO 1800p3m3 protocol - phhalHw_ApplyProtocolSettings(pHal, PHHAL_HW_CARDTYPE_I18000P3M3);
    * \li Turn ON the field - phhalHw_FieldOn();
    * \li Wait for initial guard time - phalHw_Wait(pHal, 5100, PHHAL_HW_TIME_MICROSECONDS);
    * \li Start inventory - phhalHw_I18000p3m3Inventory(pHal,pSelectFrame, bSelectFrameLen, bNumValidBits, pBeginRndFrame, bTSProcessing, &pResponse, &wRxLen);
    * \li Store the responses received for the tags.
    * \li If inventory has to be continued then issue the ResumeInventory command - phhalHw_I18000p3m3ResumeInventory(pHal, &pResponse, &wRxLen);
    * \li Use phpalI18000p3m3 and phalI18000p3m3 APIs to further communicate with the tag.
    * \li Turn OFF the field - phhalHw_FieldOff();
    * \li Termination of the application flow.
    *
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_IO_TIMEOUT When there is no tag in the field.
    * \retval #PH_ERR_USE_CONDITION If called when the phhalHw_I18000p3m3 was not called previously with bTSprocessing = PHHAL_HW_I18000P3M3_GET_MAX_RESPS.
    */
    phStatus_t phhalHw_I18000p3m3ResumeInventory(
        void * pDataParams,                 /**< [In] Pointer to this layer's parameter structure. */
        uint8_t ** ppRxBuffer,              /**< [Out] Pointer to HAL Rx Buffer containing response from single/multiple tags. */
        uint16_t * wRxBufferLen             /**< [Out] Length of response in hal Rx Buffer. */
        );

    /**
     * \brief De-initializes the contactless IC HAL.
     *
     * Note: This API resets the HAL context data.
     * The Tx and Rx buffers are made invalid
     * The connection to BAL is made invalid
     * All contactless IP interrupts are disabled.
     * No other HAL APIs can be called after de-initializing the HAL till the phhalHw_<IC>_Init() function is called.
     *
     * This API shall be called during the application termination sequence to de-initialize the HAL.
     *
     * \return Status code
     * \retval #PH_ERR_SUCCESS Operation successful.
     */
     phStatus_t phhalHw_DeInit(
         void * pDataParams                 /**< [In] Pointer to this layer's parameter structure. */
         );

#ifdef _WIN32
    /**
    * \brief Write value to register address.
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_INTERFACE_ERROR Hardware problem.
    */
    phStatus_t phhalHw_WriteRegister(
        void * pDataParams,    /**< [In] Pointer to this layer's parameter structure. */
        uint8_t bAddress,      /**< [In] Register address. */
        uint8_t bValue         /**< [In] Register value. */
        );

    /**
    * \brief Read value from register address.
    * \return Status code
    * \retval #PH_ERR_SUCCESS Operation successful.
    * \retval #PH_ERR_INTERFACE_ERROR Hardware problem.
    */
    phStatus_t phhalHw_ReadRegister(
        void * pDataParams, /**< [In] Pointer to this layer's parameter structure. */
        uint8_t bAddress,   /**< [In] Register address. */
        uint8_t * pValue    /**< [Out] Register value. */
        );
#endif /* WIN32 */

    /** @} */
#endif /* NXPBUILD__PHHAL_HW */

#endif /* Without optimization */

#ifdef __cplusplus
} /* Extern C */
#endif

#endif /* PHHALHW_H */

