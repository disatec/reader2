/*
 * phhwConfig.c
 *
 *  Created on: 21 Jan 2016
 *      Author: donatiengarnier
 */


/*
 *         Copyright (c), NXP Semiconductors
 *
 *       All rights are reserved. Reproduction in whole or in part is
 *      prohibited without the written consent of the copyright owner.
 *  NXP reserves the right to make changes without notice at any time.
 * NXP makes no warranty, expressed, implied or statutory, including but
 * not limited to any implied warranty of merchantability or fitness for any
 *particular purpose, or that the use will not infringe any third party patent,
 * copyright or trademark. NXP must not be liable for any loss or damage
 *                          arising from its use.
 */

/*------------------------------------------------------------------------------
 * Includes
 -----------------------------------------------------------------------------*/
#include <ph_TypeDefs.h>
#include <ph_Status.h>
#include <phbalReg.h>
#include <ph_Status.h>
#include "phhwConfig.h"

#include <stdio.h>
#include <pthread.h>
#include <poll.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "phbalReg_Stub_Utils.h"

/*------------------------------------------------------------------------------
 * Local macros and definitions
 -----------------------------------------------------------------------------*/
#if defined (NXPBUILD__PHHAL_HW_RC523) && defined (NXPBUILD__PHHAL_HW_RC663)
#error ** Mismatch reader device - PN512 and RC663 are enabled together **
#endif

static void* irq_pin_helper(void*);

static struct
{
#if defined NXPBUILD__PHHAL_HW_RC523
	int irq;
	int nrst;
#endif

#if defined NXPBUILD__PHHAL_HW_PN5180
	int irq;
	int nrst;
#endif

#if defined NXPBUILD__PHHAL_HW_RC663
	int irq;
	int nrst;
	int ifsel0;
	int ifsel1;
#endif

	volatile bool irq_pending;

	pthread_t irqThread;
} _data;

/* =============================================================================
 * Function:    Set_Interface_Link
 *
 * brief:
 *   Initialize interface link
 *
 * ---------------------------------------------------------------------------*/
int Set_Interface_Link(void)
{
	int ret = 0;
#if defined NXPBUILD__PHHAL_HW_RC523
	//Configure all these pins
	_data.irq = _data.nrst = -1;

	//Export them
	ret = gpio_export(PIN_CS_IN);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	ret = gpio_export(PIN_IRQ);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	ret = gpio_export(PIN_NRST);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	//Set pin directions
	ret = gpio_set_direction(PIN_CS_IN, false);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	ret = gpio_set_direction(PIN_IRQ, false);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	ret = gpio_set_direction(PIN_NRST, true);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	_data.irq = gpio_open(PIN_IRQ, O_RDONLY);
	if(_data.irq < 0)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	_data.nrst = gpio_open(PIN_NRST, O_WRONLY);
	if(_data.nrst < 0)
	{
		Cleanup_Interface_Link();
		return -1;
	}

#endif /* NXPBUILD__PHHAL_HW_RC523 */

#if defined NXPBUILD__PHHAL_HW_RC663
	//Configure all these pins
	_data.irq = _data.nrst = -1;

	//Export them
	ret = gpio_export(PIN_CS_IN);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	ret = gpio_export(PIN_IRQ);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	ret = gpio_export(PIN_NRST);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	ret = gpio_export(PIN_IFSEL0);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}
	
	ret = gpio_export(PIN_IFSEL1);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}


	//Set pin directions
	ret = gpio_set_direction(PIN_CS_IN, false);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	ret = gpio_set_direction(PIN_IRQ, false);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	ret = gpio_set_direction(PIN_NRST, true);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	ret = gpio_set_direction(PIN_IFSEL0, true);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}
	
	ret = gpio_set_direction(PIN_IFSEL1, true);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}


	_data.irq = gpio_open(PIN_IRQ, O_RDONLY);
	if(_data.irq < 0)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	_data.nrst = gpio_open(PIN_NRST, O_WRONLY);
	if(_data.nrst < 0)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	_data.ifsel0 = gpio_open(PIN_IFSEL0, O_WRONLY);
	if(_data.ifsel0 < 0)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	_data.ifsel1 = gpio_open(PIN_IFSEL1, O_WRONLY);
	if(_data.ifsel1 < 0)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	write(_data.ifsel0, "0", 1);
	write(_data.ifsel1, "1", 1);
	

#endif /* NXPBUILD__PHHAL_HW_RC663 */



#if defined NXPBUILD__PHHAL_HW_PN5180
	//Configure all these pins
	_data.irq = _data.nrst = -1;

	//Export them

	ret = gpio_export(PIN_IRQ);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	ret = gpio_export(PIN_NRST);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	//Set pin directions
	ret = gpio_set_direction(PIN_IRQ, false);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	ret = gpio_set_direction(PIN_NRST, true);
	if(ret)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	_data.irq = gpio_open(PIN_IRQ, O_RDONLY);
	if(_data.irq < 0)
	{
		Cleanup_Interface_Link();
		return -1;
	}

	_data.nrst = gpio_open(PIN_NRST, O_WRONLY);
	if(_data.nrst < 0)
	{
		Cleanup_Interface_Link();
		return -1;
	}

#endif /* NXPBUILD__PHHAL_HW_PN5180 */

	//Perform reset
	Reset_reader_device();

	return 0;
}

void Cleanup_Interface_Link(void)
{
#if defined NXPBUILD__PHHAL_HW_RC523
	//Close open file descriptors
	if(_data.irq >= 0)
	{
		close(_data.irq);
		_data.irq = -1;
	}
	if(_data.nrst >= 0)
	{
		close(_data.nrst);
		_data.nrst = -1;
	}

	//Unexport all
	gpio_unexport(PIN_CS_IN);
	gpio_unexport(PIN_IRQ);
	gpio_unexport(PIN_NRST);
#endif

#if defined NXPBUILD__PHHAL_HW_RC663
	//Close open file descriptors
	if(_data.irq >= 0)
	{
		close(_data.irq);
		_data.irq = -1;
	}
	if(_data.nrst >= 0)
	{
		close(_data.nrst);
		_data.nrst = -1;
	}
	if(_data.ifsel0 >= 0)
	{
		close(_data.ifsel0);
		_data.ifsel0 = -1;
	}
	if(_data.ifsel1 >= 0)
	{
		close(_data.ifsel1);
		_data.ifsel1 = -1;
	}

	//Unexport all
	gpio_unexport(PIN_CS_IN);
	gpio_unexport(PIN_IRQ);
	gpio_unexport(PIN_NRST);
	gpio_unexport(PIN_IFSEL0);
	gpio_unexport(PIN_IFSEL1);
#endif



#if defined NXPBUILD__PHHAL_HW_PN5180
	//Close open file descriptors
	if(_data.irq >= 0)
	{
		close(_data.irq);
		_data.irq = -1;
	}
	if(_data.nrst >= 0)
	{
		close(_data.nrst);
		_data.nrst = -1;
	}

	//Unexport all
	gpio_unexport(PIN_IRQ);
	gpio_unexport(PIN_NRST);
#endif
}

/*==============================================================================
 * Function:    Reset_reader_device
 *
 * brief:   This function resets an attached reader IC. In PIN_RESET macro is a
 *          correct pin .
 *
 * ---------------------------------------------------------------------------*/
void Reset_reader_device(void)
{
#if defined NXPBUILD__PHHAL_HW_RC523
	write(_data.nrst, "0", 1);
	usleep(10000); //Wait 10 ms
	write(_data.nrst, "1", 1);
	usleep(10000); //Wait 10 ms
#endif

#if defined NXPBUILD__PHHAL_HW_RC663
	write(_data.nrst, "1", 1);
	usleep(10000); //Wait 10 ms
	write(_data.nrst, "0", 1);
	usleep(10000); //Wait 10 ms
#endif


#if defined NXPBUILD__PHHAL_HW_PN5180
	write(_data.nrst, "0", 1);
	usleep(10000); //Wait 10 ms
	write(_data.nrst, "1", 1);
	usleep(10000); //Wait 10 ms
#endif
}

int Set_Interrupt(void)
{
	//This function will start a thread that will call a callback function whenever IRQ pin drops
	int ret = 0;
    /* configure IRQ pin to react on rising edge only */
	ret = gpio_set_edge(PIN_IRQ, true, false);
	if(ret)
	{
		return -1;
	}
    ret = pthread_create(&_data.irqThread, NULL, irq_pin_helper, NULL);
    if(ret)
    {
        return -1;
    }
    ret = sched_yield();
    if(ret)
    {
        return -1;
    }
	return 0;
}

void Cleanup_Interrupt()
{
	pthread_join(_data.irqThread, NULL);
}

void Configure_Device(phhalHw_Nfc_Ic_DataParams_t* pHal)
{
#ifdef NXPBUILD__PHHAL_HW_RC523
    /* Add DG: Configure IRQ pin - required for Explore-NFC board */
	phhalHw_Rc523_WriteRegister(&pHal->sHal, PHHAL_HW_RC523_REG_COMMIEN, 0);
	phhalHw_Rc523_WriteRegister(&pHal->sHal, PHHAL_HW_RC523_REG_DIVIEN, PHHAL_HW_RC523_BIT_IRQPUSHPULL);
#endif

#ifdef NXPBUILD__PHHAL_HW_PN5180
	uint8_t param = 0;
    phhalHw_Pn5180_Instr_ReadE2Prom(&pHal->sHal, 0x1a, &param, 1);
    if (param != 0x1)
    {
        param = 0x1;
	phhalHw_Pn5180_Instr_WriteE2Prom(&pHal->sHal, 0x1a, &param, 1); //Configure IRQ pin polarity & clearing method (IRQ pin active high, NON auto-clear)
        /* another read required before reset, to ensure parameter is written. Otherwise the reset could happen whilst write operation is still ongoing (takes ~3ms) */
        phhalHw_Pn5180_Instr_ReadE2Prom(&pHal->sHal, 0x1a, &param, 1);
        Reset_reader_device();
    }
#endif

#ifdef NXPBUILD__PHHAL_HW_RC663
    phStatus_t status;
	/*
     * Set irq0en (08h) register bit 8 to 1 to configure interrupt as active low
     * User may change this to active high based on the platform.
     * */
    status = phhalHw_Rc663_WriteRegister(&pHal->sHal, PHHAL_HW_RC663_REG_IRQ0EN, 0x00);
    CHECK_STATUS(status);
#endif
}

/*==============================================================================
 * Function:    CLIF_IRQHandler
 *
 * brief:   Pin of interrupt EINT3 is attached to interrupt output pin of IC.
 *          The purpose of this interrupt handler is to detect RF signal from an
 *          External Peer .
 *
 * ---------------------------------------------------------------------------*/
void* irq_pin_helper(void* param)
{
	int ret = 0;
	struct pollfd pollfd;
	char c = 0;
	phStatus_t status = 0;
	pollfd.fd = _data.irq;
	pollfd.events = POLLPRI;

	//Initial status: If pin is already high, post an event
	read(_data.irq, &c, 1);
	lseek(_data.irq, 0, SEEK_SET);

	if( c == '1' )
	{
		status = phOsal_Event_Post(
				E_PH_OSAL_EVT_RF,
				E_PH_OSAL_EVT_SRC_ISR,
				E_PH_OSAL_EVT_DEST_HAL);
		CHECK_STATUS(status);
	}

	while(true)
	{
		ret = poll( &pollfd, 1, -1 ); //Block forever
		if( ret != 1 )
		{
			return NULL; //Return if thread is joining
		}

		//Do a dummy read to acknowledge the event, before posting the event (to avoid race condition)
		read(_data.irq, &c, 1);
		lseek(_data.irq, 0, SEEK_SET);

		status = phOsal_Event_Post(
				E_PH_OSAL_EVT_RF,
				E_PH_OSAL_EVT_SRC_ISR,
				E_PH_OSAL_EVT_DEST_HAL);
		CHECK_STATUS(status);

	}

	return NULL;
}

/*------------------------------------------------------------------------------
 * End of file
 -----------------------------------------------------------------------------*/

