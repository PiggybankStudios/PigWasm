/*
File:   vectors.cpp
Author: Taylor Robbins
Date:   11\23\2022
Description: 
	** Holds the functions that help manipulate v2, v3, and v4 structures
*/

#include "common.h"
#include "vectors.h"

#include "intrinsics.h"

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
r32 Vec2Length(v2 vector)
{
	return SqrtR32(vector.x * vector.x + vector.y * vector.y);
}
v2 Vec2Normalize(v2 vector)
{
	if (vector.x == 0 && vector.y == 0) { return vector; }
	r32 length = Vec2Length(vector);
	v2 result;
	result.x = vector.x / length;
	result.y = vector.y / length;
	return result;
}
