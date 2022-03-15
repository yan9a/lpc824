// Description: I2C Slave for LPC824
// File: ceI2cSlave.c
// Author: Yan Naing Aye
// References:
//    http://cool-emerald.blogspot.com
#include "ceI2cSlave.h"

ceI2cSlave_CB_t _ce_i2c_slave_cb = NULL;

uint8_t _buff[I2C_DATA_LENGTH];
i2c_slave_handle_t _handle;
volatile bool _fAddr = false;
volatile bool _fTx = false;
volatile bool _fRx = false;
volatile bool _fComplete = false;

static void i2c_slave_callback(I2C_Type *base, volatile i2c_slave_transfer_t *xfer, void *userData)
{
    switch (xfer->event)
    {
        /*  Address match event */
        case kI2C_SlaveAddressMatchEvent:
            xfer->rxData = NULL;
            xfer->rxSize = 0;
            _fAddr=true;
            break;
        /*  Transmit request */
        case kI2C_SlaveTransmitEvent:
            /*  Update information for transmit process */
        	_buff[1] = 1;
            xfer->txData = &_buff[2];
            xfer->txSize = _buff[1];
            _fTx=true;
            break;

        /* Setup the slave receive buffer */
        case kI2C_SlaveReceiveEvent:
            /*  Update information for received process */
            xfer->rxData = _buff;
            xfer->rxSize = I2C_DATA_LENGTH;
            _fRx=true;
            break;

        /* The master has sent a stop transition on the bus */
        case kI2C_SlaveCompletionEvent:
            _fComplete = true;
            break;

        default:
            _fComplete = false;
            break;
    }
}

int ceI2cSlaveInit(ceI2cSlave_CB_t cbFunc){
	_ce_i2c_slave_cb = cbFunc;

	i2c_slave_config_t slaveConfig;
	status_t reVal = kStatus_Fail;

	/* Enable clock of i2c0. */
	CLOCK_EnableClock(kCLOCK_I2c0);
	/* Enables clock for IOCON block.: 0x01u */
	CLOCK_EnableClock(kCLOCK_Iocon);
	/* Enables clock for switch matrix.: 0x01u */
	CLOCK_EnableClock(kCLOCK_Swm);

	const uint32_t pio7_config = (/* Input not invert */
								  IOCON_PIO_INV_DI |
								  /* Standard/Fast mode */
								  IOCON_PIO_I2CMODE_FAST |
								  /* Bypass input filter */
								  IOCON_PIO_SMODE_BYPASS |
								  /* IOCONCLKDIV0 */
								  IOCON_PIO_CLKDIV0);
	/* P0-11 (iocon 7) is configured as SDA */
	IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_11, pio7_config);

	const uint32_t pio8_config = (/* Input not invert */
								  IOCON_PIO_INV_DI |
								  /* Standard/Fast mode */
								  IOCON_PIO_I2CMODE_FAST |
								  /* Bypass input filter */
								  IOCON_PIO_SMODE_BYPASS |
								  /* IOCONCLKDIV0 */
								  IOCON_PIO_CLKDIV0);
	/* P0-10 (iocon 8) is configured as  SCL*/
	IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_10, pio8_config);

	/* I2C0_SDA connect to P0_11 */
	SWM_SetFixedPinSelect(SWM0, kSWM_I2C0_SDA, true);

	/* I2C0_SCL connect to P0_10 */
	SWM_SetFixedPinSelect(SWM0, kSWM_I2C0_SCL, true);

	/* Disable clock for switch matrix. */
	CLOCK_DisableClock(kCLOCK_Swm);

	/* Set up i2c slave */
	I2C_SlaveGetDefaultConfig(&slaveConfig);

	/* Change the slave address */
	slaveConfig.address0.address = I2C_MASTER_SLAVE_ADDR_7BIT;

	/* Initialize the I2C slave peripheral */
	I2C_SlaveInit(CE_I2C_SLAVE, &slaveConfig, I2C_SLAVE_CLOCK_FREQUENCY);

	memset(_buff, 0, sizeof(_buff));

	/* Create the I2C handle for the non-blocking transfer */
	I2C_SlaveTransferCreateHandle(CE_I2C_SLAVE, &_handle, i2c_slave_callback, NULL);

	/* Start accepting I2C transfers on the I2C slave peripheral */
	reVal = I2C_SlaveTransferNonBlocking(CE_I2C_SLAVE, &_handle,
										 kI2C_SlaveAddressMatchEvent | kI2C_SlaveCompletionEvent);

	for(int i=0;i<I2C_DATA_LENGTH;i++){
		_buff[i] = i;
	}

	if (reVal != kStatus_Success)
	{
		return -1;
	}
	return 0;
}

void ceI2cSlaveTask()
{
	char* sa = "A\n";
	char* ra = "R\n";
	char* ta = "T\n";
	char* ca = "C\n";
	int i = 0;
	if(_fAddr){
		_fAddr=false;
		for(i=0;i<2;i++){
			_ce_i2c_slave_cb(sa+i);
		}
	}
	if(_fTx){
		_fTx=false;
		for(i=0;i<2;i++){
			_ce_i2c_slave_cb(ta+i);
		}
	}
	if(_fRx){
		_fRx=false;
		for(i=0;i<2;i++){
			_ce_i2c_slave_cb(ra+i);
		}
		for(i=0;i<I2C_DATA_LENGTH;i++){
			_ce_i2c_slave_cb(_buff+i);
		}
	}
	if(_fComplete){
		_fComplete=false;
		for(i=0;i<2;i++){
			_ce_i2c_slave_cb(ca+i);
		}
	}
}
