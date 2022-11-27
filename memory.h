/*
File:   memory.h
Author: Taylor Robbins
Date:   11\26\2022
*/

#include "shared/memory_arena.h"

#ifndef _MEMORY_H
#define _MEMORY_H

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
extern u32 currentWasmPageCount;
extern u32 currentHeapHeadAddress;
extern MemArena_t* wasmMemory;
extern MemArena_t* mainHeap;
extern MemArena_t* tempArena;

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
void* GetHeapBasePntr();
u32 GetHeapBaseAddress();

void InitializeMemory(u32 startingWasmPageCount);

#endif //  _MEMORY_H
