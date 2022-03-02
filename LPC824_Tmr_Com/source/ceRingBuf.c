/*
 * ceRingBuf.c
 *
 *  Created on: 24/06/2021
 *      Author: YanA
 */
#include "ceRingBuf.h"

uint8_t _cerb_hm[CERB_SIZE]; // heap memory
void* _cerb_sp = (_cerb_hm + CERB_SIZE); // stack pointer
//-----------------------------------------------------------------------------
// allocate memory in _cerb_hm
void* cerb_malloc(size_t n)
{
	if((_cerb_sp - n) >= (void*)_cerb_hm){
		_cerb_sp -= n;
		return _cerb_sp;
	}
	return NULL;
}
//-----------------------------------------------------------------------------
// free memory in _cerb_hm
void* cerb_free(size_t n)
{
	if((_cerb_sp + n) <= ((void*)_cerb_hm)+CERB_SIZE){
		_cerb_sp += n;
		return _cerb_sp;
	}
	return NULL;
}
//-----------------------------------------------------------------------------
// create ring buffer
// parameters:
//  size n
cerb_t* cerb_Create(size_t n)
{
	cerb_t* pBuf;
	pBuf = (cerb_t*)cerb_malloc(sizeof(cerb_t));
	if(pBuf==NULL){
		return NULL;// error in allocating
	}
	pBuf->n = n;// init size
	pBuf->in = 0; // in pointer
	pBuf->out = 0; // out pointer
	pBuf->buf = (uint8_t*)cerb_malloc(pBuf->n); // allocate buffer
	if(pBuf->buf == NULL){
		cerb_free(sizeof(cerb_t));
		return NULL; // error in buffer allocating
	}
	memset(pBuf->buf, 0, n);
	return pBuf;
}
//-----------------------------------------------------------------------------
// check whether the buffer is full
bool cerb_IsFull(cerb_t* pBuf)
{
	bool f;
//	SysDI();//Enter critical section
	f = (((pBuf->in+1)& (pBuf->n-1))==pBuf->out);
//	SysEI();//Exit critical section
	return f;
}
//-----------------------------------------------------------------------------
// check whether the buffer is empty
bool cerb_IsEmpty(cerb_t* pBuf)
{
	return (pBuf->in==pBuf->out);
}
//-----------------------------------------------------------------------------
// pop from the buffer
uint8_t cerb_Pop(cerb_t* pBuf)
{
	uint8_t v;
	v=pBuf->buf[pBuf->out++];
	pBuf->out %= pBuf->n;
	return v;
}
//-----------------------------------------------------------------------------
// push to the buffer
void cerb_Push(cerb_t* pBuf, uint8_t v)
{
//	SysDI();//Enter critical section
	pBuf->buf[pBuf->in++]=v;
	pBuf->in %= pBuf->n;
//  SysEI();//Exit critical section
}
//-----------------------------------------------------------------------------
