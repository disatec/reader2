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
* Example Source for Card product type detection.
* $Author: jenkins_cm (nxp92197) $
* $Revision: 2673 $
* $Date: 2015-07-14 15:12:35 +0530 (Tue, 14 Jul 2015) $
*
* History:
* BK: Generated 25. Sept 2014
*
*/

#include <phhwConfig.h>

/* Status code definitions */
#include "cards.h"

/*******************************************************************************
**   DetectMifare
**   This function recognize which kind of mifare card is in field.
**   Card type will be returned.
**   If 0 returned, card isn't recognized.
*******************************************************************************/
uint32_t DetectMifare(uint8_t *pAtqa, uint8_t *bSak)
{
  uint32_t sak_atqa = 0;
  uint16_t detected_card = 0xFFFF;

  sak_atqa = bSak[0] << 24 | pAtqa[0] << 8 | pAtqa[1];
  sak_atqa &= 0xFFFF0FFF;

  /* Detect mini or classic */
  switch (sak_atqa)
  {
    case sak_mfc_1k << 24 | atqa_mfc:
      DEBUG_PRINTF("Product: MIFARE Classic\n");
      detected_card &= mifare_classic;
      break;
    case sak_mfc_4k << 24 | atqa_mfc:
      DEBUG_PRINTF("Product: MIFARE Classic\n");
      detected_card &= mifare_classic;
      break;
    case sak_mfp_2k_sl1 << 24 | atqa_mfp_s:
      DEBUG_PRINTF("Product: MIFARE Classic\n");
      detected_card &= mifare_classic;
      break;
    case sak_mini << 24 | atqa_mini:
      DEBUG_PRINTF("Product: MIFARE Mini\n");
      detected_card &= mifare_mini;
      break;
    case sak_mfp_4k_sl1 << 24 | atqa_mfp_s:
      DEBUG_PRINTF("Product: MIFARE Classic\n");
      detected_card &= mifare_classic;
      break;
    case sak_mfp_2k_sl1 << 24 | atqa_mfp_x:
      DEBUG_PRINTF("Product: MIFARE Classic\n");
      detected_card &= mifare_classic;
      break;
    case sak_mfp_4k_sl1 << 24 | atqa_mfp_x:
      DEBUG_PRINTF("Product: MIFARE Classic\n");
      detected_card &= mifare_classic;
      break;
    default:
      detected_card = 0xFFFF;
      break;
  }

  if (detected_card == 0xFFFF)
  {
    sak_atqa = bSak[0] << 24 | pAtqa[0] << 8 | pAtqa[1];
    switch (sak_atqa)
    {
      case sak_ul << 24 | atqa_ul:
        DEBUG_PRINTF("Product: MIFARE Ultralight\n");
        detected_card &= mifare_ultralight;
        break;
      case sak_mfp_2k_sl2 << 24 | atqa_mfp_s:
        DEBUG_PRINTF("Product: MIFARE Plus\n");
        detected_card &= mifare_plus;
        break;
      case sak_mfp_2k_sl3 << 24 | atqa_mfp_s:
        DEBUG_PRINTF("Product: MIFARE Plus\n");
        detected_card &= mifare_plus;
        break;
      case sak_mfp_4k_sl2 << 24 | atqa_mfp_s:
        DEBUG_PRINTF("Product: MIFARE Plus\n");
        detected_card &= mifare_plus;
        break;
      case sak_mfp_2k_sl2 << 24 | atqa_mfp_x:
        DEBUG_PRINTF("Product: MIFARE Plus\n");
        detected_card &= mifare_plus;
        break;
      case sak_mfp_2k_sl3 << 24 | atqa_mfp_x:
        DEBUG_PRINTF("Product: MIFARE Plus\n");
        detected_card &= mifare_plus;
        break;
      case sak_mfp_4k_sl2 << 24 | atqa_mfp_x:
        DEBUG_PRINTF("Product: MIFARE Plus\n");
        detected_card &= mifare_plus;
        break;
      case sak_desfire << 24 | atqa_desfire:
        DEBUG_PRINTF("Product: MIFARE DESFire\n");
        detected_card &= mifare_desfire;
        break;
      case sak_jcop << 24 | atqa_jcop:
        DEBUG_PRINTF("Payment card:\n");
        detected_card &= jcop;
        break;
      case sak_layer4 << 24 | atqa_nPA:
        DEBUG_PRINTF("German eID (neuer Personalausweis) detected\n");
        detected_card &= nPA;
        break;
      default:
        return 0;
    }
  }
  return detected_card;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
