/*
 * main.c
 *
 *  Description: adc sample
 *      Author: yanna
 */
#include <stdbool.h>
#include <stdint.h>
#include "pin_mux.h"
#include "board.h"
#include "ceTmr.h"
#include "ceCom.h"
#include "ceAdc.h"
//-----------------------------------------------------------------------------
#define BOARD_LED_PORT 0U
#define BOARD_LED_PIN  12U
//-----------------------------------------------------------------------------
void  mainAdcEvent(uint16_t* d);
//-----------------------------------------------------------------------------
// handlers
void sby_led_to(void){
	GPIO_PortToggle(GPIO, BOARD_LED_PORT, 1u << BOARD_LED_PIN);
}

void adc_sampling(void){
	ceAdcStartSampling();
}

void ceComOnRx0(uint8_t b)
{
	ceComPutch(0,b);
}
//-----------------------------------------------------------------------------
int main(void)
{
    GPIO_PortInit(GPIO, BOARD_LED_PORT);
    BOARD_InitPins();
    BOARD_BootClockIRC12M();
    // BOARD_InitDebugConsole();
    //-----------------------------------------------------------------------------
	// Init software timer
	// Set systick reload value to generate 100us (10kHz) interrupt
	if (SysTick_Config(SystemCoreClock / 10000U)) { while (1) { } }

	// create timer with interval 500 ms, not oneshot, call back function 'sby_led_to'
	ceTmr_t* sbyLED = ceTmrCreate(5000,false,sby_led_to);
	ceTmrStart(sbyLED); // start the standby led timer

	// create timer with interval 1 seconds, not oneshot, call back function 'adc_sampling'
	ceTmr_t* adcSampling = ceTmrCreate(10000,false,adc_sampling);
	ceTmrStart(adcSampling); // start the adc_sampling timer
	//-----------------------------------------------------------------------------
	ceComInitialize();// Init UART
	ceComPrint(0,"Hello\n");
	//-----------------------------------------------------------------------------
	if(ceAdcInit())// Init ADC
	{
		ceComPrint(0,"ADC init failed\n");
	}
	//-----------------------------------------------------------------------------
    while (1)
    {
    	ceTmrTask();
    	ceComTask();
    	ceAdcTask();
    }
}
//-----------------------------------------------------------------------------
void  mainAdcEvent(uint16_t* d)
{
	int i;
	for(i=0;i<CE_ADC_NUMBER_OF_USED_CHANNELS;i++){
		ceComPrintHex(0,(uint8_t)(d[i]>>4));
		ceComPutch(0,' ');
	}
	ceComPutch(0,'\n');
}
