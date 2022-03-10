/*
 * main.c
 *
 *  Description: SPI slave example
 *      Author: yanna
 */

#include "pin_mux.h"
#include "board.h"
#include "ceTmr.h"
#include "ceCom.h"
#include "ceSpiSlave.h"

#define BOARD_LED_PORT 0U
#define BOARD_LED_PIN  12U

void sby_led_to(void){
	GPIO_PortToggle(GPIO, BOARD_LED_PORT, 1u << BOARD_LED_PIN);
}

void ceComOnRx0(uint8_t b)
{
	ceComPutch(0,b);
}

void ceSpiSlaveOnRx(uint8_t b)
{
	ceComPutch(0,b);
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
	ceComPrint(0,"SPI Slave Eg\n");
	//-----------------------------------------------------------------------------
	ceSpiSlaveInit(ceSpiSlaveOnRx);// Init SPI slave
	//-----------------------------------------------------------------------------
    while (1)
    {
    	ceTmrTask();
    	ceComTask();
    	ceSpiSlaveTask();
    }
}
