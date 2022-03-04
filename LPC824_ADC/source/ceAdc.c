// Description: ADC sampling in sequence for LPC824
// File: ceAdc.
// Author: Yan Naing Aye
// References:
//    http://cool-emerald.blogspot.com

#include "ceAdc.h"
//-----------------------------------------------------------------------------
static adc_result_info_t gAdcResultInfoStruct;
adc_result_info_t *volatile gAdcResultInfoPtr = &gAdcResultInfoStruct;
volatile bool gAdcConvSeqAIntFlag;
uint16_t d[CE_ADC_NUMBER_OF_USED_CHANNELS];
//-----------------------------------------------------------------------------
// functions in main
void  mainAdcEvent(uint16_t* d);
//-----------------------------------------------------------------------------
// Initialize ADC
// return 0 if ok
int ceAdcInit(){
	int ret = 0;
	/* Enables clock for IOCON block.: 0x01u */
	CLOCK_EnableClock(kCLOCK_Iocon);
	/* Enables clock for switch matrix.: 0x01u */
	CLOCK_EnableClock(kCLOCK_Swm);

	const uint32_t adc1_config = (/* Selects pull-up function */
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

	IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_6, adc1_config); // adc1
	IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_14, adc1_config); // adc2
	IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_23, adc1_config); // adc3
	IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_22, adc1_config); // adc4
	IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_21, adc1_config); // adc5
	IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_20, adc1_config); // adc6
	IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_19, adc1_config); // adc7
	IOCON_PinMuxSet(IOCON, IOCON_INDEX_PIO0_17, adc1_config); // adc9

	SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN1, true);/* ADC_CHN1 connect to P0_6 */
	SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN2, true);
	SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN3, true);
	SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN4, true);
	SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN5, true);
	SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN6, true);
	SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN7, true);
	SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN9, true);

	/* Disable clock for switch matrix. */
	CLOCK_DisableClock(kCLOCK_Swm);

	/* Power on ADC0. */
	POWER_DisablePD(kPDRUNCFG_PD_ADC0);

#if !(defined(FSL_FEATURE_ADC_HAS_NO_CALIB_FUNC) && FSL_FEATURE_ADC_HAS_NO_CALIB_FUNC)
#if defined(FSL_FEATURE_ADC_HAS_CALIB_REG) && FSL_FEATURE_ADC_HAS_CALIB_REG
    /* Calibration after power up. */
    if (ADC_DoSelfCalibration(CE_ADC_BASE))
#else
    uint32_t frequency;

#if defined(SYSCON_ADCCLKDIV_DIV_MASK)
    frequency = CLOCK_GetFreq(CE_ADC_CLOCK_SOURCE) / CLOCK_GetClkDivider(kCLOCK_DivAdcClk);
#else
    frequency = CLOCK_GetFreq(CE_ADC_CLOCK_SOURCE);
#endif /* SYSCON_ADCCLKDIV_DIV_MASK */
    /* Calibration after power up. */
    if (ADC_DoSelfCalibration(CE_ADC_BASE, frequency))
#endif /* FSL_FEATURE_ADC_HAS_CALIB_REG */
    {
        // ADC calibration ok
    }
    else
    {
        ret = 1;
    }
#endif /* FSL_FEATURE_ADC_HAS_NO_CALIB_FUNC */

    adc_config_t adcConfigStruct;
    adc_conv_seq_config_t adcConvSeqConfigStruct;
    /* Configure the converter. */
    #if defined(FSL_FEATURE_ADC_HAS_CTRL_ASYNMODE) & FSL_FEATURE_ADC_HAS_CTRL_ASYNMODE
        adcConfigStruct.clockMode = kADC_ClockSynchronousMode; /* Using sync clock source. */
    #endif                                                     /* FSL_FEATURE_ADC_HAS_CTRL_ASYNMODE */
        adcConfigStruct.clockDividerNumber = CE_ADC_CLOCK_DIVIDER;
    #if defined(FSL_FEATURE_ADC_HAS_CTRL_RESOL) & FSL_FEATURE_ADC_HAS_CTRL_RESOL
        adcConfigStruct.resolution = kADC_Resolution12bit;
    #endif /* FSL_FEATURE_ADC_HAS_CTRL_RESOL */
    #if defined(FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL) & FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL
        adcConfigStruct.enableBypassCalibration = false;
    #endif /* FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL */
    #if defined(FSL_FEATURE_ADC_HAS_CTRL_TSAMP) & FSL_FEATURE_ADC_HAS_CTRL_TSAMP
        adcConfigStruct.sampleTimeNumber = 0U;
    #endif /* FSL_FEATURE_ADC_HAS_CTRL_TSAMP */
    #if defined(FSL_FEATURE_ADC_HAS_CTRL_LPWRMODE) & FSL_FEATURE_ADC_HAS_CTRL_LPWRMODE
        adcConfigStruct.enableLowPowerMode = false;
    #endif /* FSL_FEATURE_ADC_HAS_CTRL_LPWRMODE */
    #if defined(FSL_FEATURE_ADC_HAS_TRIM_REG) & FSL_FEATURE_ADC_HAS_TRIM_REG
        adcConfigStruct.voltageRange = kADC_HighVoltageRange;
    #endif /* FSL_FEATURE_ADC_HAS_TRIM_REG */
        ADC_Init(CE_ADC_BASE, &adcConfigStruct);

    #if !(defined(FSL_FEATURE_ADC_HAS_NO_INSEL) && FSL_FEATURE_ADC_HAS_NO_INSEL)
        /* Use the temperature sensor input to channel 0. */
        ADC_EnableTemperatureSensor(CE_ADC_BASE, true);
    #endif /* FSL_FEATURE_ADC_HAS_NO_INSEL. */

	/* Enable channels in Sequence A. */
	adcConvSeqConfigStruct.channelMask = (CE_ADC_CHANNEL_MASK); /* Includes channels */
	adcConvSeqConfigStruct.triggerMask      = 0U;
	adcConvSeqConfigStruct.triggerPolarity  = kADC_TriggerPolarityPositiveEdge;
	adcConvSeqConfigStruct.enableSingleStep = false;
	adcConvSeqConfigStruct.enableSyncBypass = false;
	adcConvSeqConfigStruct.interruptMode    = kADC_InterruptForEachSequence;
	ADC_SetConvSeqAConfig(CE_ADC_BASE, &adcConvSeqConfigStruct);
	ADC_EnableConvSeqA(CE_ADC_BASE, true); /* Enable the conversion sequence A. */

	/*
	//Clear the result register
	ADC_DoSoftwareTriggerConvSeqA(CE_ADC_BASE);
	while (!ADC_GetChannelConversionResult(CE_ADC_BASE,1, &gAdcResultInfoStruct))
	{
	}
	ADC_GetConvSeqAGlobalConversionResult(CE_ADC_BASE, &gAdcResultInfoStruct);
	*/

    /* Enable the interrupt. */
	/* Enable the interrupt the for sequence A done. */
	ADC_EnableInterrupts(CE_ADC_BASE, kADC_ConvSeqAInterruptEnable);
	NVIC_EnableIRQ(CE_ADC_IRQ_ID);
	return 0;
}
//-----------------------------------------------------------------------------
/*
 * ISR for ADC conversion sequence A done.
 */
void CE_ADC_IRQ_HANDLER_FUNC(void)
{
    if (kADC_ConvSeqAInterruptFlag == (kADC_ConvSeqAInterruptFlag & ADC_GetStatusFlags(CE_ADC_BASE)))
    {
        ADC_ClearStatusFlags(CE_ADC_BASE, kADC_ConvSeqAInterruptFlag);
        gAdcConvSeqAIntFlag = true;
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
//-----------------------------------------------------------------------------
// Start sampling process
void ceAdcStartSampling()
{
	gAdcConvSeqAIntFlag = false;
	ADC_DoSoftwareTriggerConvSeqA(CE_ADC_BASE);
}
//-----------------------------------------------------------------------------
// Adc task
void ceAdcTask()
{
	int i,j;
	if(gAdcConvSeqAIntFlag){
		gAdcConvSeqAIntFlag = false;
		// scan all 12 channels
		for(i=0,j=0;i<12;i++){
			if(CE_ADC_CHANNEL_MASK & (1U<<i)){
				ADC_GetChannelConversionResult(CE_ADC_BASE, i, gAdcResultInfoPtr);
				if(j<CE_ADC_NUMBER_OF_USED_CHANNELS){
					d[j++] = (uint16_t)gAdcResultInfoStruct.result;
				}
			}
		}
		mainAdcEvent(d);
	}
}
//-----------------------------------------------------------------------------
