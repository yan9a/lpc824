// Description: Ring buffered USART communication for LPC824
// File: ceCom.h
// Author: Yan Naing Aye
// References:
//    http://cool-emerald.blogspot.com/2022/03/software-timers-and-ring-buffered-uart.html

#ifndef	CE_COM_H
	#define	CE_COM_H
//-----------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include "fsl_usart.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_swm.h"
#include "fsl_iocon.h"

#define CECOM0_ENABLE 1
#define CECOM1_ENABLE 0
#define CECOM2_ENABLE 0

#define CECOM_USE_TX_INT 0

#define CECOM_CLKSRC kCLOCK_MainClk
#define CECOM_CLK_FREQ CLOCK_GetFreq(kCLOCK_MainClk)

#if(CECOM0_ENABLE==1)
	#define COM_IRQHandler0 USART0_IRQHandler
	void ceComOnRx0(uint8_t b);
#endif
#if(CECOM1_ENABLE==1)
	#define COM_IRQHandler1 USART1_IRQHandler
	void ceComOnRx1(uint8_t b);
#endif
#if(CECOM2_ENABLE==1)
	#define COM_IRQHandler2 USART2_IRQHandler
	void ceComOnRx2(uint8_t b);
#endif

typedef void (*ceCom_RxCB_t)(uint8_t b);
//-----------------------------------------------------------------------------
void ceComInitialize();
void ceComInit(uint32_t uartNo, usart_config_t config, size_t bufSize, ceCom_RxCB_t cbFuncOnRx);
void ceComTask();
void ceComTaskRx(uint32_t uartNo);
void ceComTaskTx(uint32_t uartNo);
//-----------------------------------------------------------------------------
uint8_t ceComPutch(uint32_t uartNo, uint8_t c);
void ceComPrint(uint32_t uartNo, char *s);
void ceComWrite(uint32_t uartNo, uint8_t *s,size_t n);
void ceComPrintHex(uint32_t uartNo, uint8_t ch);
void ceComPrintHex32(uint32_t uartNo, uint32_t r);
void ceComWUT(uint32_t uartNo);// wait until transmitted
//-----------------------------------------------------------------------------
#endif // CE_COM_H
