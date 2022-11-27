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
inline v2 NewVec2(r32 x, r32 y);
inline v3 NewVec3(r32 x, r32 y, r32 z);
inline v4 NewVec4(r32 x, r32 y, r32 z, r32 w);

#endif //  _VECTORS_H
