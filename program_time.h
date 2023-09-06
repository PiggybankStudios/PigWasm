/*
File:   program_time.h
Author: Taylor Robbins
Date:   12\03\2022
*/

#ifndef _PROGRAM_TIME_H
#define _PROGRAM_TIME_H

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
extern u32 ProgramTime;
extern u64 ProgramTime64;
extern r32 ProgramTimeF;
extern r64 ProgramTime64F;

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
void UpdateProgramTime(r64 newProgramTime);

#endif //  _PROGRAM_TIME_H
