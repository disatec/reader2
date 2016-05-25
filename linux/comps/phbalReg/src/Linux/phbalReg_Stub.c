/*
 * phbalReg_Stub.c
 *
 *  Created on: 14 Jan 2016
 *      Author: donatiengarnier
 */

#include <ph_Status.h>
#include <phbalReg.h>
#include <ph_RefDefs.h>

#ifdef NXPBUILD__PHBAL_REG_STUB

#include <stdint.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <poll.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "phbalReg_Stub.h"
#include "phbalReg_Stub_Utils.h"

#define CFG_SPI_DIR "/dev/spidev"
#define CFG_DEFAULT_SPI_BUS  0 //"/dev/spidev0.0"
#define CFG_DEFAULT_SPI_CS_PIN  0 //"/dev/spidev0.0"
#define CFG_DEFAULT_BUSY_PIN 25 //"/sys/class/gpio/gpio25/"

#define CFG_SPI_MODE SPI_MODE_0
#define CFG_SPI_BITS_PER_WORD 8
#define CFG_SPI_FREQ 5000000

struct data
{
	//Pin/bus numbers
	size_t spiBus;
	size_t spiCsPinNum;
	size_t busyPinNum;

	//File descriptors
	int spiFd;
	int i2cFd;
	int busyPinFd;
};

static struct data _data;

//Definitions of internal functions

static int spi_transfer(const uint8_t* out, uint8_t* in, size_t length);
static int wait_not_busy();

//Pin definitions
phStatus_t phbalReg_Stub_Init(
                                    phbalReg_Stub_DataParams_t * pDataParams,
                                    uint16_t wSizeOfDataParams
                                    )
{
    if (sizeof(phbalReg_Stub_DataParams_t) != wSizeOfDataParams)
    {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_DATA_PARAMS, PH_COMP_BAL);
    }

    PH_ASSERT_NULL (pDataParams);

    pDataParams->wId = PH_COMP_BAL | PHBAL_REG_STUB_ID;
    pDataParams->bMultiRegRW = PH_OFF;
    _data.spiFd = -1;

    //Set default bus/pin numbers
    _data.spiBus = CFG_DEFAULT_SPI_BUS;
    _data.spiCsPinNum = CFG_DEFAULT_SPI_CS_PIN;
    _data.busyPinNum = CFG_DEFAULT_BUSY_PIN;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_BAL);
}

phStatus_t phbalReg_Stub_GetPortList(
    phbalReg_Stub_DataParams_t * pDataParams,
    uint16_t wPortBufSize,
    uint8_t * pPortNames,
    uint16_t * pNumOfPorts
    )
{
    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_BAL);
}

phStatus_t phbalReg_Stub_SetPort(
                            phbalReg_Stub_DataParams_t * pDataParams,
                            uint8_t * pPortName
                            )
{
	//Data should be "X:Y:Z" where X is the SPI bus, Y is the CS pin number on the bus and Z the busy pin number
	int ret = 0;
	size_t x, y, z;

	ret = sscanf(pPortName, "%lu:%lu:%lu", &x, &y, &z);
	if( ret != 3 )
	{
        return PH_ADD_COMPCODE(PH_ERR_INVALID_DATA_PARAMS, PH_COMP_BAL);
	}

	_data.spiBus = x;
	_data.spiCsPinNum = y;
	_data.busyPinNum = z;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_BAL);
}

phStatus_t phbalReg_Stub_OpenPort(
                                  phbalReg_Stub_DataParams_t * pDataParams
                                  )
{
	int ret = 0;
	int fd = 0;
	char spi[16];

	uint32_t mode = CFG_SPI_MODE;
	uint8_t bits = CFG_SPI_BITS_PER_WORD;
	uint32_t speed = CFG_SPI_FREQ;

	snprintf(spi, sizeof(spi), CFG_SPI_DIR "%lu.%lu", _data.spiBus, _data.spiCsPinNum);

	_data.spiFd = open(spi, O_RDWR);
	if(_data.spiFd < 0)
	{
		return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_BAL);
	}

	//If successful, configure SPI bus using ioctls

	//Set mode
	ret = ioctl(_data.spiFd, SPI_IOC_WR_MODE, &mode);
	if( ret < 0 )
	{
		goto cleanup;
	}

	//Set
	ret = ioctl(_data.spiFd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if( ret < 0 )
	{
		goto cleanup;
	}

	ret = ioctl(_data.spiFd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if( ret < 0 )
	{
		goto cleanup;
	}

	if( pDataParams->wHalType == PHBAL_REG_HAL_HW_PN5180 )
	{
		//Export GPIO function
		ret = gpio_export(_data.busyPinNum);
		if( ret < 0 )
		{
			goto cleanup;
		}

		//Configure as input
		ret = gpio_set_direction(_data.busyPinNum, false);
		if( ret < 0 )
		{
			goto cleanup;
		}

		//Watch falling edge
		ret = gpio_set_edge(_data.busyPinNum, false, true);
		if( ret < 0 )
		{
			goto cleanup;
		}

		_data.busyPinFd = gpio_open(_data.busyPinNum, O_RDONLY);
		if( _data.busyPinFd < 0 )
		{
			goto cleanup;
		}
	}

    return PH_ERR_SUCCESS;

	cleanup:

	phbalReg_Stub_ClosePort(pDataParams);

	return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_BAL);
}

phStatus_t phbalReg_Stub_ClosePort(
                                   phbalReg_Stub_DataParams_t * pDataParams
                                   )
{
	if(_data.spiFd >= 0)
	{
		close(_data.spiFd);
	}
	_data.spiFd = -1;

	if( pDataParams->wHalType == PHBAL_REG_HAL_HW_PN5180 )
	{
		if(_data.busyPinFd >= 0)
		{
			close(_data.busyPinFd);
		}
		_data.busyPinFd = -1;

		gpio_unexport(_data.busyPinNum);
	}

    return PH_ERR_SUCCESS;
}

phStatus_t phbalReg_Stub_Exchange(
                                  phbalReg_Stub_DataParams_t * pDataParams,
                                  uint16_t wOption,
                                  uint8_t * pTxBuffer,
                                  uint16_t wTxLength,
                                  uint16_t wRxBufSize,
                                  uint8_t * pRxBuffer,
                                  uint16_t * pRxLength
                                  )
{
	int ret = 0;
	size_t pos = 0;

	if( (pDataParams->bMultiRegRW == PH_ON) && ( pDataParams->wHalType != PHBAL_REG_HAL_HW_PN5180 ) )
	{
		//"Fast" bundled exchange for EMVCo compatibility -- only necessary for PN512/RC663 derivatives
		if( wTxLength < 1 )
		{
			return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_BAL);
		}
	    if( ((pDataParams->wHalType == PHBAL_REG_HAL_HW_RC663)
	    		&& ( (pTxBuffer[0] & 0x01) == 0x01 ))
	    	||  ((pDataParams->wHalType == PHBAL_REG_HAL_HW_RC523)
		    		&& ( (pTxBuffer[0] & 0x80) == 0x80 )) )
	    {
	    	//Perform a "MultiRegRead" exchange
	    	//This is a read operation
			ret = spi_transfer(pTxBuffer, pRxBuffer, wTxLength);
			if(ret)
			{
				return PH_ADD_COMPCODE(PH_ERR_INTERFACE_ERROR, PH_COMP_BAL);
			}

			if( pRxLength != NULL )
			{
				*pRxLength = wTxLength;
			}

		    return PH_ERR_SUCCESS;
	    }
	    else if( (pDataParams->wHalType == PHBAL_REG_HAL_HW_RC663)
	    		&& ( (pTxBuffer[0] & 0x01) == 0x00 ) )
		{
	    	//Perform a "MultiRegWrite" exchange
	    	if( (wTxLength % 2) != 0 )
			{
				return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_BAL);
			}

			while( pos < wTxLength )
			{
				ret = spi_transfer(pTxBuffer, NULL, 2);
				if(ret)
				{
					return PH_ADD_COMPCODE(PH_ERR_INTERFACE_ERROR, PH_COMP_BAL);
				}

				pTxBuffer += 2;
				pos += 2;
			}

			if( pRxLength != NULL )
			{
				*pRxLength = 0;
			}

		    return PH_ERR_SUCCESS;
		}
	}

	//Standard exchange
	if( pDataParams->wHalType == PHBAL_REG_HAL_HW_PN5180 )
	{
		//If PN5180, wait for busy line to be released
		ret = wait_not_busy();
		if(ret)
		{
			return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_BAL);
		}

		//Transmit first
		ret = spi_transfer(pTxBuffer, NULL, wTxLength);
		if(ret)
		{
			return PH_ADD_COMPCODE(PH_ERR_INTERFACE_ERROR, PH_COMP_BAL);
		}

		//Receive response
		if( pRxBuffer != NULL )
		{
			//Wait again
			ret = wait_not_busy();
			if(ret)
			{
				return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_BAL);
			}

			ret = spi_transfer(NULL, pRxBuffer, wRxBufSize);
			if(ret)
			{
				return PH_ADD_COMPCODE(PH_ERR_INTERFACE_ERROR, PH_COMP_BAL);
			}

			if( pRxLength != NULL )
			{
				*pRxLength = wRxBufSize;
			}
		}
	}
	else
	{
		//Transfer everything at once
		ret = spi_transfer(pTxBuffer, pRxBuffer, wTxLength);
		if(ret)
		{
			return PH_ADD_COMPCODE(PH_ERR_INTERFACE_ERROR, PH_COMP_BAL);
		}

		if( pRxLength != NULL )
		{
			*pRxLength = wTxLength;
		}
	}

    return PH_ERR_SUCCESS;
}


phStatus_t phbalReg_Stub_SetConfig(
                                   phbalReg_Stub_DataParams_t * pDataParams,
                                   uint16_t wConfig,
                                   uint16_t wValue
                                   )
{
    switch (wConfig)
    {
        /* EMVCo: Configure to use Multiple Register Read/Write Functionality */
        case PHBAL_CONFIG_RW_MULTI_REG:
        {
            pDataParams->bMultiRegRW = wValue;
            break;
        }

        case PHBAL_REG_CONFIG_HAL_HW_TYPE:
        {
            switch(wValue)
            {
                case PHBAL_REG_HAL_HW_RC523:
                case PHBAL_REG_HAL_HW_RC663:
                case PHBAL_REG_HAL_HW_PN5180:
                    pDataParams->wHalType = wValue;
                    break;

                default:
                    return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_BAL);
            }
            break;
        }

        default:
        {
            return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_PARAMETER, PH_COMP_BAL);
        }
    }

    return PH_ERR_SUCCESS;
}

phStatus_t phbalReg_Stub_GetConfig(
                                   phbalReg_Stub_DataParams_t * pDataParams,
                                   uint16_t wConfig,
                                   uint16_t * pValue
                                   )
{
	   switch (wConfig)
	    {
	        case PHBAL_CONFIG_RW_MULTI_REG:
	        {
	            *pValue = pDataParams->bMultiRegRW;
	            break;
	        }

	        case PHBAL_REG_CONFIG_HAL_HW_TYPE:
	        {
	        	*pValue = pDataParams->wHalType;
	            break;
	        }

	        default:
	        {
	            return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_PARAMETER, PH_COMP_BAL);
	        }
	    }

	    return PH_ERR_SUCCESS;
}

//Internal functions
int spi_transfer(const uint8_t* out, uint8_t* in, size_t length)
{
	int ret = 0;
	struct spi_ioc_transfer transfer = {0};

	if(length == 0)
	{
		return 0; //Avoid asserting /CS line if there is no pending transfer (can wake-up PNxxx even if we don't want to)
	}

	transfer.tx_buf = (uintptr_t)out;
	transfer.rx_buf = (uintptr_t)in;
	transfer.len = length;
	transfer.speed_hz = CFG_SPI_FREQ;
	transfer.bits_per_word = CFG_SPI_BITS_PER_WORD;
	transfer.delay_usecs = 0;

	//Execute the transfer
	ret = ioctl(_data.spiFd, SPI_IOC_MESSAGE(1), &transfer);
	if( ret < 0 )
	{
		return -1;
	}

	return 0;
}

int wait_not_busy()
{
	int ret = 0;
	struct pollfd pollfd;
	char c = 0;

	pollfd.fd = _data.busyPinFd;
	pollfd.events = POLLPRI;

	//Read value -- seek otherwise subsequent reads fail (return '\n' for instance)
	lseek(_data.busyPinFd, 0, SEEK_SET);
	ret = read(_data.busyPinFd, &c, 1);
	if( ret != 1 )
	{
		return -1;
	}

	if( c == '0' ) //Not busy
	{
		return 0;
	}

	ret = poll( &pollfd, 1, 5000 ); //Block for 5 seconds
	if( ret != 1 )
	{
		return -1;
	}

	//Re-Read value
	lseek(_data.busyPinFd, 0, SEEK_SET);
	ret = read(_data.busyPinFd, &c, 1);
	if( ret != 1 )
	{
		return -1;
	}

	return 0;
}




#endif
