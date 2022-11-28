/*
File:   vectors.h
Author: Taylor Robbins
Date:   11\23\2022
*/

#ifndef _VECTORS_H
#define _VECTORS_H

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
struct v2
{
	r32 x;
	r32 y;
};
struct v3
{
	r32 x;
	r32 y;
	r32 z;
};
struct v4
{
	r32 x;
	r32 y;
	r32 z;
	r32 w;
};

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
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

r32 Vec2Length(v2 vector);
v2 Vec2Normalize(v2 vector);

#endif //  _VECTORS_H
