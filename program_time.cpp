
/*
File:   program_time.cpp
Author: Taylor Robbins
Date:   12\03\2022
Description: 
	** None 
*/

#include "common.h"
#include "program_time.h"

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
u32 ProgramTime    = 0U;
u64 ProgramTime64  = 0.0;
r32 ProgramTimeF   = 0.0f;
r64 ProgramTime64F = 0ULL;

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
void UpdateProgramTime(r64 newProgramTime)
{
	ProgramTime64F = newProgramTime;
	ProgramTimeF = (r32)ProgramTime64F;
	ProgramTime64 = (u64)FloorR64i(ProgramTime64F);
	ProgramTime = (u32)FloorR64i(ProgramTime64F);
}
