/*
File:   memory.cpp
Author: Taylor Robbins
Date:   11\26\2022
Description: 
	** None 
*/

#include "common.h"
#include "memory.h"

#include "memory_arena.h"

// +--------------------------------------------------------------+
// |                          Heap Base                           |
// +--------------------------------------------------------------+
extern unsigned char __heap_base;

__attribute__ ((visibility("default"))) 
void* GetHeapBasePntr()
{
	return &__heap_base;
}
u32 GetHeapBaseAddress()
{
	void* basePntr = GetHeapBasePntr();
	return (u32)basePntr;
}

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
u32 currentWasmPageCount = 0;
u32 currentHeapHeadAddress = 0;
MemArena_t* wasmMemory = nullptr;
MemArena_t* mainHeap = nullptr;
MemArena_t* tempArena = nullptr;

// +--------------------------------------------------------------+
// |                       Private Globals                        |
// +--------------------------------------------------------------+
static MemArena_t wasmMemoryNonPointer;
static MemArena_t mainHeapNonPointer;
static MemArena_t tempArenaNonPointer;

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
void InitializeMemory(u32 startingWasmPageCount)
{
	currentWasmPageCount = startingWasmPageCount;
	currentHeapHeadAddress = GetHeapBaseAddress();
	
	ClearStruct(wasmMemoryNonPointer);
	ClearStruct(mainHeapNonPointer);
	ClearStruct(tempArenaNonPointer);
	wasmMemory = &wasmMemoryNonPointer;
	mainHeap = &mainHeapNonPointer;
	tempArena = &tempArenaNonPointer;
	
	InitMemArena_StdHeap(wasmMemory);
	InitMemArena_PagedHeapArena(mainHeap, MAIN_HEAP_PAGE_SIZE, wasmMemory);
	FlagUnset(mainHeap->flags, MemArenaFlag_AutoFreePages);
	void* tempArenaSpace = AllocMem(wasmMemory, TEMP_ARENA_SIZE);
	NotNull(tempArenaSpace);
	InitMemArena_MarkedStack(tempArena, TEMP_ARENA_SIZE, tempArenaSpace, TEMP_ARENA_MAX_NUM_MARKS);
}
