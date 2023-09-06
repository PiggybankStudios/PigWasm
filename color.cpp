/*
File:   color.cpp
Author: Taylor Robbins
Date:   11\27\2022
Description: 
	** None 
*/

#include "common.h"
#include "color.h"

#include "intrinsics.h"

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
u8 ColorChannelToU8(r32 valueR32)
{
	return ClampI32toU8(RoundR32i(valueR32 * 255.0f));
}
r32 ColorChannelToR32(u8 valueU8)
{
	return (r32)valueU8 / 255.0f;
}
