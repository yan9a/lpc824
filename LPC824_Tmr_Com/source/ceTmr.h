// Description: Simple software timer
// File: ceTmr.c
// Author: Yan Naing Aye
// References:
//    http://cool-emerald.blogspot.com/2010/06/soft-timer-module-for-8051.html

#ifndef CE_TMR_H
#define CE_TMR_H
#include <stdint.h>
#include <stdbool.h>

#define CE_TMR_MAX_NUMBER 10 // maximum number of timer
#define ceTmrTick SysTick_Handler // define timer tick function
// to be called periodically
//-----------------------------------------------------------------------------
typedef void (*ceTmr_CB_t)(void);
typedef struct {
                    bool en;
                    bool oneshot;
                    uint32_t tick;//to keep track elapsed time
                    uint32_t interval;//time out interval
                    ceTmr_CB_t cbFunc;
                } ceTmr_t;
//-----------------------------------------------------------------------------
ceTmr_t* ceTmrCreate(uint32_t interval, bool oneshot, ceTmr_CB_t cbFunc);
void ceTmrTask();
void ceTmrSetInterval(ceTmr_t *ptmr,uint32_t Interval);
void ceTmrReset(ceTmr_t *ptmr);
void ceTmrStart(ceTmr_t *ptmr);
void ceTmrStop(ceTmr_t *ptmr);
void ceTmrTick(void);
//-----------------------------------------------------------------------------

#endif /* CE_TMR_H */
