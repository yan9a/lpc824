/*
 * ceRingBuf.h
 *
 *  Created on: 24/06/2021
 *      Author: YanA
 */

#ifndef CERINGBUF_H_
#define CERINGBUF_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define CERB_SIZE (4*(1024+16))

typedef struct {
	uint32_t in;
	uint32_t out;
	uint8_t* buf;
	size_t n;
} cerb_t;

// allocate memory in _cerb_hm
void* cerb_malloc(size_t n);

// free memory in _cerb_hm
void* cerb_free(size_t n);

// create ring buffer
// parameters:
//  size n
cerb_t* cerb_Create(size_t n);

// check whether the buffer is full
bool cerb_IsFull(cerb_t* pBuf);

// check whether the buffer is empty
bool cerb_IsEmpty(cerb_t* pBuf);

// pop from the buffer
uint8_t cerb_Pop(cerb_t* pBuf);

// push to the buffer
void cerb_Push(cerb_t* pBuf, uint8_t v);

#endif /* CERINGBUF_H_ */
