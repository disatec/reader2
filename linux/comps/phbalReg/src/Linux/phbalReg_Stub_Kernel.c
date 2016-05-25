#include <ph_Status.h>
#include <phbalReg.h>
#include <ph_RefDefs.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#ifdef NXPBUILD__PHBAL_REG_STUB

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "phbalReg_Stub.h"

#define CFG_BAL_DEV "/dev/bal"

static int fp;
//Pin definitions
phStatus_t phbalReg_Stub_Init(
                                    phbalReg_Stub_DataParams_t * pDataParams,
                                    uint16_t wSizeOfDataParams
                                    )
{
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

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_BAL);
}

phStatus_t phbalReg_Stub_OpenPort(
                                  phbalReg_Stub_DataParams_t * pDataParams
                                  )
{
	fp = open(CFG_BAL_DEV, O_RDWR);
	if (!fp) {
		return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_BAL);
	}

    return PH_ERR_SUCCESS;

}

phStatus_t phbalReg_Stub_ClosePort(
                                   phbalReg_Stub_DataParams_t * pDataParams
                                   )
{
	if (fp) {
		close(fp);
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

	int sz;
	sz = write(fp, pTxBuffer, wTxLength);

	if (sz != wTxLength)
	{
		/* this should not happen! */
		return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_BAL);
	}

	if (( pRxBuffer != NULL ) && wRxBufSize) {
		*pRxLength = read(fp, pRxBuffer, wRxBufSize);
	}

    return PH_ERR_SUCCESS;
}


phStatus_t phbalReg_Stub_SetConfig(
                                   phbalReg_Stub_DataParams_t * pDataParams,
                                   uint16_t wConfig,
                                   uint16_t wValue
                                   )
{
	int status = PH_ERR_SUCCESS;
	if (wConfig == 0xabcd) {
		status = ioctl(fp, 0, wValue);
	}
    return status;
}

phStatus_t phbalReg_Stub_GetConfig(
                                   phbalReg_Stub_DataParams_t * pDataParams,
                                   uint16_t wConfig,
                                   uint16_t * pValue
                                   )
{
	    return PH_ERR_SUCCESS;
}
#endif
