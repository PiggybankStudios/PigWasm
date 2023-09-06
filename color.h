/*
File:   color.h
Author: Taylor Robbins
Date:   11\27\2022
*/

#ifndef _COLOR_H
#define _COLOR_H

#include "vectors.h"
#include "intrinsics.h"

// +--------------------------------------------------------------+
// |                          Structure                           |
// +--------------------------------------------------------------+
union Color_t
{
	u32 value;
	struct
	{
		u8 r; u8 g; u8 b; u8 a;
	};
};

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
u8 ColorChannelToU8(r32 valueR32);
r32 ColorChannelToR32(u8 valueU8);
Color_t ColorLerp(Color_t leftColor, Color_t rightColor, r32 amount);

inline Color_t NewColor(u32 argbValue)
{
	Color_t result;
	result.value =
		((u8)((argbValue >> 16) & 0xFF) <<  0) | //r
		((u8)((argbValue >>  8) & 0xFF) <<  8) | //g
		((u8)((argbValue >>  0) & 0xFF) << 16) | //b
		((u8)((argbValue >> 24) & 0xFF) << 24);  //a
	return result;
}
inline Color_t NewColor(u8 r, u8 g, u8 b, u8 a)
{
	Color_t result;
	result.r = r;
	result.g = g;
	result.b = b;
	result.a = a;
	return result;
}
inline v4 ToVec4(Color_t color)
{
	v4 result;
	result.x = ColorChannelToR32(color.r);
	result.y = ColorChannelToR32(color.g);
	result.z = ColorChannelToR32(color.b);
	result.w = ColorChannelToR32(color.a);
	return result;
}

inline Color_t ColorLighten(Color_t color, u8 amount)
{
	Color_t result;
	result.r = (u8)MinU32(255, color.r + amount);
	result.g = (u8)MinU32(255, color.g + amount);
	result.b = (u8)MinU32(255, color.b + amount);
	result.a = color.a;
	return result;
}

// +--------------------------------------------------------------+
// |                      Predefined Colors                       |
// +--------------------------------------------------------------+
#define NUM_PREDEF_PURE_COLORS       6
#define NUM_PREDEF_GREYSCALE_COLORS  16

#define NoColor_Value                 0x00000000UL
#define TransparentBlack_Value        0x00000000UL
#define Transparent_Value             0x00FFFFFFUL
#define TransparentWhite_Value        0x00FFFFFFUL

#define Black_Value                   0xFF000000UL
#define Grey1_Value                   0xFF111111UL
#define Grey2_Value                   0xFF222222UL
#define Grey3_Value                   0xFF333333UL
#define Grey4_Value                   0xFF444444UL
#define Grey5_Value                   0xFF555555UL
#define Grey6_Value                   0xFF666666UL
#define Grey7_Value                   0xFF777777UL
#define Grey8_Value                   0xFF888888UL
#define Grey9_Value                   0xFF999999UL
#define Grey10_Value                  0xFFAAAAAAUL
#define Grey11_Value                  0xFFBBBBBBUL
#define Grey12_Value                  0xFFCCCCCCUL
#define Grey13_Value                  0xFFDDDDDDUL
#define Grey14_Value                  0xFFEEEEEEUL
#define White_Value                   0xFFFFFFFFUL

#define PureRed_Value                 0xFFFF0000UL
#define PureOrange_Value              0xFFFFA500UL
#define PureYellow_Value              0xFFFFFF00UL
#define PureGreen_Value               0xFF008000UL
#define PureBlue_Value                0xFF0000FFUL
#define PurePurple_Value              0xFF800080UL

#define NoColor                 NewColor(NoColor_Value)
#define TransparentBlack        NewColor(TransparentBlack_Value)
#define Transparent             NewColor(Transparent_Value)
#define TransparentWhite        NewColor(TransparentWhite_Value)
#define Black                   NewColor(Black_Value)
#define Grey1                   NewColor(Grey1_Value)
#define Grey2                   NewColor(Grey2_Value)
#define Grey3                   NewColor(Grey3_Value)
#define Grey4                   NewColor(Grey4_Value)
#define Grey5                   NewColor(Grey5_Value)
#define Grey6                   NewColor(Grey6_Value)
#define Grey7                   NewColor(Grey7_Value)
#define Grey8                   NewColor(Grey8_Value)
#define Grey9                   NewColor(Grey9_Value)
#define Grey10                  NewColor(Grey10_Value)
#define Grey11                  NewColor(Grey11_Value)
#define Grey12                  NewColor(Grey12_Value)
#define Grey13                  NewColor(Grey13_Value)
#define Grey14                  NewColor(Grey14_Value)
#define White                   NewColor(White_Value)
#define PureRed                 NewColor(PureRed_Value)
#define PureOrange              NewColor(PureOrange_Value)
#define PureYellow              NewColor(PureYellow_Value)
#define PureGreen               NewColor(PureGreen_Value)
#define PureBlue                NewColor(PureBlue_Value)
#define PurePurple              NewColor(PurePurple_Value)

#endif //  _COLOR_H
