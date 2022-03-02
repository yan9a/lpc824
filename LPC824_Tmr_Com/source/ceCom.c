// Description: Ring buffered USART communication for LPC824
// File: ceCom.h
// Author: Yan Naing Aye
// References:
//    http://cool-emerald.blogspot.com/2021/07/software-timers-and-ring-buffered-uart.html
//    http://cool-emerald.blogspot.com/2010/06/circular-buffered-uart-com-module-for.html

#include "ceCom.h"
#include "fsl_common.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "ceRingBuf.h"
#define WEAK __attribute__ ((weak))
//-----------------------------------------------------------------------------
USART_Type* _ceusart[] = {USART0, USART1, USART2};
IRQn_Type _ce_usart_interrupt[] = {USART0_IRQn, USART1_IRQn, USART2_IRQn};
ceCom_RxCB_t _ce_rx_cb[] = {NULL, NULL, NULL};
cerb_t* rRB[] = {NULL, NULL, NULL};
cerb_t* tRB[] = {NULL, NULL, NULL};
//-----------------------------------------------------------------------------
// init
void ceComInit(uint32_t uartNo, usart_config_t config, size_t bufSize, ceCom_RxCB_t cbFuncOnRx)
{
	rRB[uartNo] = cerb_Create(bufSize);
	tRB[uartNo] = cerb_Create(bufSize);

	USART_Init(_ceusart[uartNo], &config, CECOM_CLK_FREQ);
	/* Enable RX interrupt. */
	USART_EnableInterrupts(_ceusart[uartNo], kUSART_RxReadyInterruptEnable);
	EnableIRQ(_ce_usart_interrupt[uartNo]);
	_ce_rx_cb[uartNo] = cbFuncOnRx;
}

void ceComInitialize()
{
	usart_config_t config;
    /* Default config by using USART_GetDefaultConfig():
     * config.baudRate_Bps = 9600U;
     * config.parityMode = kUSART_ParityDisabled;
     * config.stopBitCount = kUSART_OneStopBit;
     * config.bitCountPerChar = kUSART_8BitsPerChar;
     * config.loopback = false;
     * config.enableRx = false;
     * config.enableTx = false;
     * config.syncMode = kUSART_SyncModeDisabled;
     */
	/* Ser DIV of uart0. */
	CLOCK_SetClkDivider(kCLOCK_DivUsartClk, 1U);
	/* Enables clock for IOCON block.: Enable. */
	CLOCK_EnableClock(kCLOCK_Iocon);
	/* Enables clock for switch matrix.: 0x01u */
	CLOCK_EnableClock(kCLOCK_Swm);
#if(CECOM0_ENABLE==1)
	/* Enable clock of uart0. */
	CLOCK_EnableClock(kCLOCK_Uart0);

    const uint32_t p0_7_io15_config = (/* Selects pull-up function */
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
    /* PORT0 PIN7 (iocon index 15) is configured as  */
    IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_7, p0_7_io15_config);

    const uint32_t p0_18_io30_config = (/* Selects pull-up function */
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
    /* PORT0 PIN18 (iocon index 30) is configured as  */
    IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_18, p0_18_io30_config);

	/* USART0_TXD connect to P0_7 */
	SWM_SetMovablePinSelect(SWM0, kSWM_USART0_TXD, kSWM_PortPin_P0_7);

	/* USART0_RXD connect to P0_18 */
	SWM_SetMovablePinSelect(SWM0, kSWM_USART0_RXD, kSWM_PortPin_P0_18);

	USART_GetDefaultConfig(&config);
	config.baudRate_Bps = 115200U;
	config.enableTx     = true;
	config.enableRx     = true;
	config.parityMode = kUSART_ParityDisabled;
	// uart0, buffer size 1023, call back 'ceComOnRx0' on receiving
	ceComInit(0, config, 1024, ceComOnRx0);
#endif
#if(CECOM1_ENABLE==1)
	/* Enable clock of uart1. */
	CLOCK_EnableClock(kCLOCK_Uart1);
	const uint32_t p0_7_io15_config = (/* Selects pull-up function */
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
	    /* PORT0 PIN7 (iocon index 15) is configured as  */
	    IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_7, p0_7_io15_config);

	    const uint32_t p0_18_io30_config = (/* Selects pull-up function */
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
	    /* PORT0 PIN18 (iocon index 30) is configured as  */
	    IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_18, p0_18_io30_config);

		/* USART1_TXD connect to P0_7 */
		SWM_SetMovablePinSelect(SWM0, kSWM_USART1_TXD, kSWM_PortPin_P0_7);

		/* USART1_RXD connect to P0_18 */
		SWM_SetMovablePinSelect(SWM0, kSWM_USART1_RXD, kSWM_PortPin_P0_18);

		USART_GetDefaultConfig(&config);
		config.baudRate_Bps = 115200U;
		config.enableTx     = true;
		config.enableRx     = true;
		config.parityMode = kUSART_ParityDisabled;
	// uart1, buffer size 1023, call back 'ceComOnRx1' on receiving
	ceComInit(1, config, 1024, ceComOnRx1);
#endif
#if(CECOM2_ENABLE==1)
	/* Enable clock of uart2. */
	CLOCK_EnableClock(kCLOCK_Uart2);

	const uint32_t p0_4_io4_config = (/* Selects pull-up function */
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
	    /* PORT0 PIN4 (iocon index 4) is configured as  */
	    IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_4, p0_4_io4_config);

	    const uint32_t p0_0_io17_config = (/* Selects pull-up function */
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
	    /* PORT0 PIN0 (iocon index 17) is configured as  */
	    IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_0, p0_0_io17_config);

		/* USART2_TXD connect to P0_4 */
		SWM_SetMovablePinSelect(SWM0, kSWM_USART2_TXD, kSWM_PortPin_P0_4);

		/* USART2_RXD connect to P0_0 */
		SWM_SetMovablePinSelect(SWM0, kSWM_USART2_RXD, kSWM_PortPin_P0_0);

		USART_GetDefaultConfig(&config);
		config.baudRate_Bps = 115200U;
		config.enableTx     = true;
		config.enableRx     = true;
		config.parityMode = kUSART_ParityDisabled;
	// uart2, buffer size 1023, call back 'ceComOnRx2' on receiving
	ceComInit(2, config, 1024, ceComOnRx2);
#endif
	/* Disable clock for switch matrix. */
	CLOCK_DisableClock(kCLOCK_Swm);
}
//-----------------------------------------------------------------------------
#if(CECOM0_ENABLE==1)
void COM_IRQHandler0(void)
{
    uint8_t data;
    uint8_t uartNo = 0;
    /* If new data arrived. */
    if ((kUSART_RxReady)&USART_GetStatusFlags(_ceusart[uartNo]))
    {
        data = USART_ReadByte(_ceusart[uartNo]);
        cerb_Push(rRB[uartNo],data);
    }
#if(CECOM_USE_TX_INT==1)
    /* If tx ready */
	if ((kUSART_TxReady)&USART_GetStatusFlags(_ceusart[uartNo]))
	{
		if(!cerb_IsEmpty(tRB[uartNo]))
		{
			USART_WriteByte(_ceusart[uartNo],cerb_Pop(tRB[uartNo]));
		}
		else {
			/* Disable TX ready interrupt. */
			USART_DisableInterrupts(_ceusart[uartNo], kUSART_TxReadyInterruptEnable);
		}
	}
#endif
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#if(CECOM1_ENABLE==1)
void COM_IRQHandler1(void)
{
    uint8_t data;
    uint8_t uartNo = 1;
    /* If new data arrived. */
    if ((kUSART_RxReady)&USART_GetStatusFlags(_ceusart[uartNo]))
    {
        data = USART_ReadByte(_ceusart[uartNo]);
        cerb_Push(rRB[uartNo],data);
    }
#if(CECOM_USE_TX_INT==1)
    /* If tx ready */
	if ((kUSART_TxReady)&USART_GetStatusFlags(_ceusart[uartNo]))
	{
		if(!cerb_IsEmpty(tRB[uartNo]))
		{
			USART_WriteByte(_ceusart[uartNo],cerb_Pop(tRB[uartNo]));
		}
		else {
			/* Disable TX ready interrupt. */
			USART_DisableInterrupts(_ceusart[uartNo], kUSART_TxReadyInterruptEnable);
		}
	}
#endif
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#if(CECOM2_ENABLE==1)
void COM_IRQHandler2(void)
{
    uint8_t data;
    uint8_t uartNo = 2;
    /* If new data arrived. */
    if ((kUSART_RxReady)&USART_GetStatusFlags(_ceusart[uartNo]))
    {
        data = USART_ReadByte(_ceusart[uartNo]);
        cerb_Push(rRB[uartNo],data);
    }
#if(CECOM_USE_TX_INT==1)
    /* If tx ready */
	if ((kUSART_TxReady)&USART_GetStatusFlags(_ceusart[uartNo]))
	{
		if(!cerb_IsEmpty(tRB[uartNo]))
		{
			USART_WriteByte(_ceusart[uartNo],cerb_Pop(tRB[uartNo]));
		}
		else {
			/* Disable TX ready interrupt. */
			USART_DisableInterrupts(_ceusart[uartNo], kUSART_TxReadyInterruptEnable);
		}
	}
#endif
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
//-----------------------------------------------------------------------------
void ceComTask()
{
#if(CECOM0_ENABLE==1)
	ceComTaskTx(0);
	ceComTaskRx(0);
#endif
#if(CECOM1_ENABLE==1)
	ceComTaskTx(1);
	ceComTaskRx(1);
#endif
#if(CECOM2_ENABLE==1)
	ceComTaskTx(2);
	ceComTaskRx(2);
#endif
}
//-----------------------------------------------------------------------------
void ceComTaskTx(uint32_t uartNo)
{
	// Send data only when USART TX register is empty and ring buffer has data to send out.
	while((kUSART_TxReady & USART_GetStatusFlags(_ceusart[uartNo])) && (!cerb_IsEmpty(tRB[uartNo])))
	{
		USART_WriteByte(_ceusart[uartNo],cerb_Pop(tRB[uartNo]));
#if(CECOM_USE_TX_INT==1)
		/* Send back the received characters to terminal in interrupt-mode. */
		USART_EnableInterrupts(_ceusart[uartNo], kUSART_TxReadyInterruptEnable);
#endif
	}
}
//-----------------------------------------------------------------------------
//Polled regularly by main(), check the serial input buffer if
//there is any input char and then pass these chars to corresponding call back func
void ceComTaskRx(uint32_t uartNo)
{
	//while rx buf is not empty
    while(!cerb_IsEmpty(rRB[uartNo])) _ce_rx_cb[uartNo](cerb_Pop(rRB[uartNo]));
}
//-----------------------------------------------------------------------------
//put the char into the transmit buffer and start transmission
uint8_t ceComPutch(uint32_t uartNo, uint8_t c)
{
//  while(cerb_IsFull(tRB[uartNo]));//wait when the buffer is full
//	commenting above line means: Discard Policy=> discard old data
    cerb_Push(tRB[uartNo],c);
    return c;
}
//-----------------------------------------------------------------------------
void ceComPrint(uint32_t uartNo, char *s)
{
    for(;*s;s++) ceComPutch(uartNo,*s);
}
//-----------------------------------------------------------------------------

void ceComWrite(uint32_t uartNo, uint8_t *s,size_t n)
{
	size_t i;
	for(i=0;i<n;i++){
		ceComPutch(uartNo,s[i]);
	}
}
//-----------------------------------------------------------------------------
void ceComPrintHex(uint32_t uartNo, uint8_t ch)
{
    uint8_t a;
    a=(ch >> 4) | 0x30;
    if(a>0x39) a+=7;
    ceComPutch(uartNo,a);
    a=(ch & 0x0F) | 0x30;
    if(a>0x39) a+=7;
    ceComPutch(uartNo,a);
}

//-----------------------------------------------------------------------------
// Print 32 bit number in hex
void ceComPrintHex32(uint32_t uartNo, uint32_t r)
{
	int i;
	uint8_t b[4];
	for(i=0;i<4;i++){
		b[i] = (uint8_t)( (r>>(i*8)) & 0xFF);
	}
	for(i=0;i<4;i++){
		ceComPrintHex(uartNo,b[3-i]);
	}
}
//-----------------------------------------------------------------------------
// ceComWUT waits until transmitted
void ceComWUT(uint32_t uartNo)
{
	while(!cerb_IsEmpty(tRB[uartNo])) ceComTaskTx(uartNo);
}
//-----------------------------------------------------------------------------
#if(CECOM0_ENABLE==1)
WEAK void ceComOnRx0(uint8_t ch)
{
// to do on rx
}
#endif
//-----------------------------------------------------------------------------
#if(CECOM1_ENABLE==1)
WEAK void ceComOnRx1(uint8_t ch)
{
// to do on rx
}
#endif
//-----------------------------------------------------------------------------
#if(CECOM2_ENABLE==1)
WEAK void ceComOnRx2(uint8_t ch)
{
// to do on rx
}
#endif
//-----------------------------------------------------------------------------
