// Description: SPI Slave for LPC824
// File: ceSpiSlave.h
// Author: Yan Naing Aye
// References:
//    http://cool-emerald.blogspot.com

#ifndef CESPISLAVE_H_
#define CESPISLAVE_H_

#include <stdbool.h>
#include <stdint.h>
#include "fsl_spi.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_swm.h"
#include "fsl_iocon.h"

#define CE_SPI_SLAVE SPI0
#define CE_SPI_SLAVE_IRQ SPI0_IRQn
#define SPI_SLAVE_IRQHandler SPI0_IRQHandler

typedef void (*ceSpiSlave_RxCB_t)(uint8_t b);

void ceSpiSlaveInit(ceSpiSlave_RxCB_t cbFuncOnRx);
void ceSpiSlaveTask();

#endif /* CESPISLAVE_H_ */
