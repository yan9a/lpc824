// Description: ADC sampling in sequence for LPC824
// File: ceAdc.
// Author: Yan Naing Aye
// References:
//    http://cool-emerald.blogspot.com

#ifndef CEADC_H_
#define CEADC_H_
#include "fsl_device_registers.h"
#include "fsl_adc.h"
#include "fsl_clock.h"
#include "fsl_power.h"
#include "pin_mux.h"
#include <stdbool.h>
#include <stdint.h>
#include "fsl_swm.h"
#include "fsl_iocon.h"

#define CE_ADC_BASE ADC0

// used channels: 1,...7,9
// | 11| 10|  9|  8|  7|  6|  5|  4|  3|  2|  1|  0| // bit index
// |  0|  0|  1|  0|  1|  1|  1|  1|  1|  1|  1|  0| // 1 enabled
// |   |   | D7|   | D2| A5| A4| A3| A2| A1| A0|   | // pin on board

#define CE_ADC_CHANNEL_MASK 0x2FE
#define CE_ADC_NUMBER_OF_USED_CHANNELS 8
#define CE_ADC_CLOCK_SOURCE kCLOCK_Irc
#define CE_ADC_IRQ_ID ADC0_SEQA_IRQn
#define CE_ADC_IRQ_HANDLER_FUNC ADC0_SEQA_IRQHandler
#define CE_ADC_CLOCK_DIVIDER 1U

int ceAdcInit();
void ceAdcStartSampling();
void ceAdcTask();

#endif /* CEADC_H_ */
