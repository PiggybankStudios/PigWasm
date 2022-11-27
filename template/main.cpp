/*
File:   main.cpp
Author: Taylor Robbins
Date:   11\22\2022
Description: 
	** Holds the main entry point for the wasm binary
*/

#include "common.h"
#include "main.h"

#include "intrinsics.h"
#include "debug.h"
#include "vectors.h"
#include "graphics_base.h"
#include "memory.h"
#include "memory_arena.h"
#include "helpers.h"

u32 ProgramTime;
u64 ProgramTime64;
r32 ProgramTimeF;
r64 ProgramTime64F;

// +--------------------------------------------------------------+
// |                          Initialize                          |
// +--------------------------------------------------------------+
EXPORTED_FUNC(void, Initialize, u32 startingWasmPageCount)
{
	InitializeMemory(startingWasmPageCount);
	
	ProgramTime64F = 0.0;
	ProgramTimeF = (r32)ProgramTime64F;
	ProgramTime64 = (u64)FloorR64i(ProgramTime64F);
	ProgramTime = (u32)FloorR64i(ProgramTime64F);
	
	WriteLine_O("Hello from Web Assembly land!!!");
	
	//TODO: Add your initialization code here!
}

// +--------------------------------------------------------------+
// |                       UpdateAndRender                        |
// +--------------------------------------------------------------+
EXPORTED_FUNC(void, UpdateAndRender, r32 elapsedMs)
{
	ProgramTime64F += elapsedMs;
	ProgramTimeF = (r32)ProgramTime64F;
	ProgramTime64 = (u64)FloorR64i(ProgramTime64F);
	ProgramTime = (u32)FloorR64i(ProgramTime64F);
	
	//TODO: Add your rendering code here!
}
