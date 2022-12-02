/*
File:   memory.h
Author: Taylor Robbins
Date:   11\26\2022
*/

#include "memory_arena.h"

#ifndef _MEMORY_H
#define _MEMORY_H

// +--------------------------------------------------------------+
// |                   Enumeration and Typedefs                   |
// +--------------------------------------------------------------+
enum ArenaName_t
{
	ArenaName_MainHeap = 0,
	ArenaName_TempArena,
	ArenaName_NumNames,
};
inline const char* GetArenaNameStr(ArenaName_t name)
{
	switch (name)
	{
		case ArenaName_MainHeap: return "MainHeap";
		case ArenaName_TempArena: return "TempArena";
		default: return "Unknown";
	}
}

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
extern u32 currentWasmPageCount;
extern u32 currentHeapHeadAddress;
extern MemArena_t* wasmMemory;
extern MemArena_t* mainHeap;
extern MemArena_t* mainHeapDebug;
extern MemArena_t* tempArena;

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
void* GetHeapBasePntr();
u32 GetHeapBaseAddress();

void InitializeMemory(u32 startingWasmPageCount);
MemArena_t* GetMemArenaByName(ArenaName_t arenaName);

#endif //  _MEMORY_H
