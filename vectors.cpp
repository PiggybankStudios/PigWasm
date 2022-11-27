/*
File:   vectors.cpp
Author: Taylor Robbins
Date:   11\23\2022
Description: 
	** Holds the functions that help manipulate v2, v3, and v4 structures
*/

#include "common.h"
#include "shared/vectors.h"

inline v2 NewVec2(r32 x, r32 y)
{
	v2 result;
	result.x = x;
	result.y = y;
	return result;
}
inline v3 NewVec3(r32 x, r32 y, r32 z)
{
	v3 result;
	result.x = x;
	result.y = y;
	result.z = z;
	return result;
}
inline v4 NewVec4(r32 x, r32 y, r32 z, r32 w)
{
	v4 result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;
	return result;
}
