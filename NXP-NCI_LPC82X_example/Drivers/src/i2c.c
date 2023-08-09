/*
*         Copyright (c), NXP Semiconductors Caen / France
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

#include <stdint.h>
#include <board.h>
#include <tool.h>

/* I2CM transfer record */
static I2CM_XFER_T  i2cmXferRec;
/* I2C clock is set to 1.8MHz */
#define I2C_CLK_DIVIDER         (40)
/* 100KHz I2C bit-rate */
#define I2C_BITRATE         (100000)

void i2c_Init(void)
{
	SystemCoreClockUpdate();

	/* Enable I2C clock and reset I2C peripheral - the boot ROM does not do this */
	Chip_I2C_Init(LPC_I2C);

	/* Setup clock rate for I2C */
	Chip_I2C_SetClockDiv(LPC_I2C, I2C_CLK_DIVIDER);

	/* Setup I2CM transfer rate */
	Chip_I2CM_SetBusSpeed(LPC_I2C, I2C_BITRATE);

	/* Enable Master Mode */
	Chip_I2CM_Enable(LPC_I2C);

	/* Disable the interrupt for the I2C */
	NVIC_DisableIRQ(I2C_IRQn);
}

uint8_t i2c_Read(uint8_t add, uint8_t *pRsp, uint16_t Rsp_size)
{
	/* Workaround of an issue preventing read after write */
	for(uint8_t i=0; i<100; i++) asm("nop");

	i2cmXferRec.slaveAddr = add;
	i2cmXferRec.status = 0;
	i2cmXferRec.txBuff = NULL;
	i2cmXferRec.txSz = 0;
	i2cmXferRec.rxBuff = pRsp;
	i2cmXferRec.rxSz = Rsp_size;

	Chip_I2CM_XferBlocking(LPC_I2C, &i2cmXferRec);

	return (i2cmXferRec.status == 0 ? SUCCESS : ERROR);
}

uint8_t i2c_Write(uint8_t add, uint8_t *pCmd, uint16_t Cmd_size)
{
	/* Workaround of an issue preventing write after read */
	for(uint8_t i=0; i<100; i++) asm("nop");

	i2cmXferRec.slaveAddr = add;
	i2cmXferRec.status = 0;
	i2cmXferRec.txBuff = pCmd;
	i2cmXferRec.txSz = Cmd_size;
	i2cmXferRec.rxBuff = NULL;
	i2cmXferRec.rxSz = 0;

	Chip_I2CM_XferBlocking(LPC_I2C, &i2cmXferRec);

	return (i2cmXferRec.status == 0 ? SUCCESS : ERROR);
}
