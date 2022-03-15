/*
 * main.c
 *
 *  Description: I2C Slave
 *      Author: yanna
 */

#include "pin_mux.h"
#include "board.h"
#include "ceTmr.h"
#include "ceCom.h"
#include "ceI2cSlave.h"

#define BOARD_LED_PORT 0U
#define BOARD_LED_PIN  12U

void sby_led_to(void){
	GPIO_PortToggle(GPIO, BOARD_LED_PORT, 1u << BOARD_LED_PIN);
}

void ceComOnRx0(uint8_t b)
{
	ceComPutch(0,b);
}

void ceI2cSlaveCB(void* p)
{
	// ceComPutch(0,*(char*)p);
	ceComPrintHex(0,*(char*)p);
}

int main(void)
{
    GPIO_PortInit(GPIO, BOARD_LED_PORT);
    BOARD_InitPins();
    BOARD_BootClockIRC12M();
    // BOARD_InitDebugConsole();
    //-----------------------------------------------------------------------------
	// Init software timer
	// Set systick reload value to generate 1ms interrupt
	if (SysTick_Config(SystemCoreClock / 1000U)) { while (1) { } }
	// create timer with interval 500 ms, not oneshot, call back function 'sby_led_to'
	ceTmr_t* sbyLED = ceTmrCreate(500,false,sby_led_to);
	ceTmrStart(sbyLED); // start the standby led timer
	//-----------------------------------------------------------------------------
	ceComInitialize();// Init UART
	ceComPrint(0,"I2c slave demo\n");
	//-----------------------------------------------------------------------------
	ceI2cSlaveInit(ceI2cSlaveCB); // Init I2c slave
	//-----------------------------------------------------------------------------
    while (1)
    {
    	ceTmrTask();
    	ceComTask();
    	ceI2cSlaveTask();
    }
}
