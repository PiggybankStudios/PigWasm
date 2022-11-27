/*
File:   helpers.cpp
Author: Taylor Robbins
Date:   11\26\2022
Description: 
	** Helpers are small functions that are used all throughout the codebase and don't
	** really belong in a file of their own
*/

#include "common.h"
#include "helpers.h"

r32 OscillateBy(u64 timeSource, r32 min, r32 max, u64 periodMs, u64 offset)
{
	r32 lerpValue = (SinR32((((timeSource + offset) % periodMs) / (r32)periodMs) * 2*Pi32) + 1.0f) / 2.0f;
	return min + (max - min) * lerpValue;
}
