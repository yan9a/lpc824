// Description: Simple software timer
// File: ceTmr.c
// Author: Yan Naing Aye
// References:
//    http://cool-emerald.blogspot.com/2010/06/soft-timer-module-for-8051.html

//ceTmrTask() is to be polled regularly e.g., by main() loop
//*****************************************************************************
#include "ceTmr.h"
//-----------------------------------------------------------------------------
volatile uint32_t _ceTmrTickCounter = 0;
uint32_t _ceTmr_index = 0;
ceTmr_t T[CE_TMR_MAX_NUMBER];//declare timers array
//-----------------------------------------------------------------------------
// Call ceTmrTick() every tick interval by an interrupt
void ceTmrTick(void)
{
    _ceTmrTickCounter++;
}
//-----------------------------------------------------------------------------
// TmrTask() is to be polled regularly e.g., by main()
void ceTmrTask()
{
    uint32_t i;
    ceTmr_t *ptmr;
    if(_ceTmrTickCounter)
    {
        _ceTmrTickCounter--;//_ceTmrTickCounter increases every interval, decrease it to 0 again
        for(i=0;i<_ceTmr_index;i++,ptmr++)
		{
        	ptmr = &T[i];
			if(ptmr->en)//if it is enabled
			{
				if(--(ptmr->tick)==0)//accuracy <= one tick interval
				{
					ptmr->tick=ptmr->interval;//restart
					if(ptmr->oneshot) ptmr->en = false;
					ptmr->cbFunc();   //call time out function
				}
			}
		}
    }
}
//-----------------------------------------------------------------------------
// create timer
ceTmr_t* ceTmrCreate(uint32_t interval, bool oneshot, ceTmr_CB_t cbFunc)
{
    ceTmr_t *ptmr;
    if(_ceTmr_index<CE_TMR_MAX_NUMBER){
    	ptmr = &T[_ceTmr_index++];
    	ptmr->en = false;
		ptmr->interval = interval;
		ptmr->tick = ptmr->interval;
		ptmr->oneshot = oneshot;
		ptmr->cbFunc = cbFunc;
    }
    else {
    	ptmr = 0;
    }
    return ptmr;
}
//-----------------------------------------------------------------------------
// set timer interval
void ceTmrSetInterval(ceTmr_t *ptmr,uint32_t Interval)
{
    if(ptmr)
    {
        ptmr->interval=Interval;
        ptmr->tick=ptmr->interval;
    }
}
//-----------------------------------------------------------------------------
// start timer
void ceTmrStart(ceTmr_t *ptmr)
{
    if(ptmr)
    {
        ptmr->en=true;
    }
}
//-----------------------------------------------------------------------------
// stop timer
void ceTmrStop(ceTmr_t *ptmr)
{
    if(ptmr)
    {
        ptmr->en=false;
    }
}
//-----------------------------------------------------------------------------
//Reloads a timer with its initial value
void ceTmrReset(ceTmr_t *ptmr)
{
    if(ptmr)
    {
        ptmr->tick=ptmr->interval;
    }
}
//-----------------------------------------------------------------------------

