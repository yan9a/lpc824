// Description: I2C Slave for LPC824
// File: ceI2cSlave.h
// Author: Yan Naing Aye
// References:
//    http://cool-emerald.blogspot.com

#ifndef CEI2CSLAVE_H_
#define CEI2CSLAVE_H_

#include <stdbool.h>
#include <stdint.h>
#include "fsl_i2c.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_swm.h"
#include "fsl_iocon.h"

#define IOCON_PIO_I2CMODE_FAST 0x00u /*!<@brief Standard/Fast mode */

#define CE_I2C_SLAVE_BASE (I2C0_BASE)
#define I2C_SLAVE_CLOCK_FREQUENCY (12000000)
#define CE_I2C_SLAVE ((I2C_Type *)CE_I2C_SLAVE_BASE)
#define I2C_MASTER_SLAVE_ADDR_7BIT (0x22U)
#define I2C_DATA_LENGTH (4) /* MAX is 256 */

typedef void (*ceI2cSlave_CB_t)(void* p);

int ceI2cSlaveInit(ceI2cSlave_CB_t cbFunc);
void ceI2cSlaveTask();

#endif /* CEI2CSLAVE_H_ */
