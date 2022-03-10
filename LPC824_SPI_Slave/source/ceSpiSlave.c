// Description: SPI Slave for LPC824
// File: ceSpiSlave.h
// Author: Yan Naing Aye
// References:
//    http://cool-emerald.blogspot.com

#include "ceSpiSlave.h"
#include "ceRingBuf.h"

cerb_t* ssRB = NULL;
ceSpiSlave_RxCB_t _ce_spi_slave_cb = NULL;

void SPI_SLAVE_IRQHandler(void)
{
	uint8_t a;
	a = (uint8_t)(CE_SPI_SLAVE->RXDAT & 0x000000FFU);
	cerb_Push(ssRB,a);
	CE_SPI_SLAVE->TXDAT = a;
    __DSB();
}

void ceSpiSlaveInit(ceSpiSlave_RxCB_t cbFuncOnRx)
{
	ssRB = cerb_Create(16);
	_ce_spi_slave_cb = cbFuncOnRx;
	/* Enable clock of spi0. */
	CLOCK_EnableClock(kCLOCK_Spi0);

	/* Enables clock for IOCON block.: 0x01u */
	CLOCK_EnableClock(kCLOCK_Iocon);
	/* Enables clock for switch matrix.: 0x01u */
	CLOCK_EnableClock(kCLOCK_Swm);

	const uint32_t pio10_config = (/* Selects pull-up function */
								   IOCON_PIO_MODE_PULLUP |
								   /* Enable hysteresis */
								   IOCON_PIO_HYS_EN |
								   /* Input not invert */
								   IOCON_PIO_INV_DI |
								   /* Disables Open-drain function */
								   IOCON_PIO_OD_DI |
								   /* Bypass input filter */
								   IOCON_PIO_SMODE_BYPASS |
								   /* IOCONCLKDIV0 */
								   IOCON_PIO_CLKDIV0);
	/* SSEL p0_15 (iocon index 10) is configured as  */
	IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_15, pio10_config);

	const uint32_t pio22_config = (/* Selects pull-up function */
								   IOCON_PIO_MODE_PULLUP |
								   /* Enable hysteresis */
								   IOCON_PIO_HYS_EN |
								   /* Input not invert */
								   IOCON_PIO_INV_DI |
								   /* Disables Open-drain function */
								   IOCON_PIO_OD_DI |
								   /* Bypass input filter */
								   IOCON_PIO_SMODE_BYPASS |
								   /* IOCONCLKDIV0 */
								   IOCON_PIO_CLKDIV0);
	/* MOSI p0_26 (iocon index 22) is configured as  */
	IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_26, pio22_config);

	const uint32_t pio23_config = (/* Selects pull-up function */
								   IOCON_PIO_MODE_PULLUP |
								   /* Enable hysteresis */
								   IOCON_PIO_HYS_EN |
								   /* Input not invert */
								   IOCON_PIO_INV_DI |
								   /* Disables Open-drain function */
								   IOCON_PIO_OD_DI |
								   /* Bypass input filter */
								   IOCON_PIO_SMODE_BYPASS |
								   /* IOCONCLKDIV0 */
								   IOCON_PIO_CLKDIV0);
	/* MISO p0_25 (iocon index 23) is configured as  */
	IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_25, pio23_config);

	const uint32_t pio24_config = (/* Selects pull-up function */
								   IOCON_PIO_MODE_PULLUP |
								   /* Enable hysteresis */
								   IOCON_PIO_HYS_EN |
								   /* Input not invert */
								   IOCON_PIO_INV_DI |
								   /* Disables Open-drain function */
								   IOCON_PIO_OD_DI |
								   /* Bypass input filter */
								   IOCON_PIO_SMODE_BYPASS |
								   /* IOCONCLKDIV0 */
								   IOCON_PIO_CLKDIV0);
	/* SCK p0_24 (iocon index 24) is configured as  */
	IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_24, pio24_config);

	/* SPI0_SCK connect to P0_24 */
	SWM_SetMovablePinSelect(SWM0, kSWM_SPI0_SCK, kSWM_PortPin_P0_24);

	/* SPI0_MOSI connect to P0_26 */
	SWM_SetMovablePinSelect(SWM0, kSWM_SPI0_MOSI, kSWM_PortPin_P0_26);

	/* SPI0_MISO connect to P0_25 */
	SWM_SetMovablePinSelect(SWM0, kSWM_SPI0_MISO, kSWM_PortPin_P0_25);

	/* SPI0_SSEL0 connect to P0_15 */
	SWM_SetMovablePinSelect(SWM0, kSWM_SPI0_SSEL0, kSWM_PortPin_P0_15);

	/* Disable clock for switch matrix. */
	CLOCK_DisableClock(kCLOCK_Swm);


	spi_slave_config_t slaveConfig = {0};

	/* Default configuration for slave:
	 * userConfig.enableSlave = true;
	 * userConfig.polarity = kSPI_ClockPolarityActiveHigh;
	 * userConfig.phase = kSPI_ClockPhaseFirstEdge;
	 * userConfig.direction = kSPI_MsbFirst;
	 * userConfig.dataWidth = kSPI_Data8Bits;
	 * userConfig.sselPol = kSPI_SpolActiveAllLow;
	 */
	SPI_SlaveGetDefaultConfig(&slaveConfig);
	SPI_SlaveInit(CE_SPI_SLAVE, &slaveConfig);

	/* Write data to TXDAT register to trigger receive interrupt. */
	SPI_WriteData(CE_SPI_SLAVE, 0x00);
	/* Enable SPI RX ready interrupt. */
	EnableIRQ(CE_SPI_SLAVE_IRQ);
	SPI_EnableInterrupts(CE_SPI_SLAVE, kSPI_RxReadyInterruptEnable);
}

void ceSpiSlaveTask()
{
	//while rx buf is not empty
	while(!cerb_IsEmpty(ssRB)) _ce_spi_slave_cb(cerb_Pop(ssRB));
}
