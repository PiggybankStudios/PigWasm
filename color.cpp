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

Color_t ColorLerp(Color_t leftColor, Color_t rightColor, r32 amount)
{
	Color_t result = {};
	result.r = ClampI32toU8(RoundR32i(LerpR32((r32)leftColor.r, (r32)rightColor.r, amount)));
	result.g = ClampI32toU8(RoundR32i(LerpR32((r32)leftColor.g, (r32)rightColor.g, amount)));
	result.b = ClampI32toU8(RoundR32i(LerpR32((r32)leftColor.b, (r32)rightColor.b, amount)));
	result.a = ClampI32toU8(RoundR32i(LerpR32((r32)leftColor.a, (r32)rightColor.a, amount)));
	return result;
}
