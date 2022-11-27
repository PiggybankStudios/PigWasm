/*
File:   matrices.h
Author: Taylor Robbins
Date:   11\27\2022
*/

#ifndef _MATRICES_H
#define _MATRICES_H

// +--------------------------------------------------------------+
// |                          Structure                           |
// +--------------------------------------------------------------+
union mat4
{
	//         C  R
	r32 values[4][4];
	struct
	{
		r32 r0c0, r1c0, r2c0, r3c0;
		r32 r0c1, r1c1, r2c1, r3c1;
		r32 r0c2, r1c2, r2c2, r3c2;
		r32 r0c3, r1c3, r2c3, r3c3;
	};
};

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
inline mat4 NewMat4(
	r32 r0c0, r32 r0c1, r32 r0c2, r32 r0c3,
	r32 r1c0, r32 r1c1, r32 r1c2, r32 r1c3,
	r32 r2c0, r32 r2c1, r32 r2c2, r32 r2c3,
	r32 r3c0, r32 r3c1, r32 r3c2, r32 r3c3)
{
	mat4 result;
	result.r0c0 = r0c0;
	result.r0c1 = r0c1;
	result.r0c2 = r0c2;
	result.r0c3 = r0c3;
	result.r1c0 = r1c0;
	result.r1c1 = r1c1;
	result.r1c2 = r1c2;
	result.r1c3 = r1c3;
	result.r2c0 = r2c0;
	result.r2c1 = r2c1;
	result.r2c2 = r2c2;
	result.r2c3 = r2c3;
	result.r3c0 = r3c0;
	result.r3c1 = r3c1;
	result.r3c2 = r3c2;
	result.r3c3 = r3c3;
	return result;
}

#define Mat4_Identity NewMat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)

inline mat4 Mat4Multiply(mat4 left, mat4 right)
{
	mat4 result = {
		left.r0c0*right.r0c0 + left.r0c1*right.r1c0 + left.r0c2*right.r2c0 + left.r0c3*right.r3c0, //r0c0
		left.r1c0*right.r0c0 + left.r1c1*right.r1c0 + left.r1c2*right.r2c0 + left.r1c3*right.r3c0, //r1c0
		left.r2c0*right.r0c0 + left.r2c1*right.r1c0 + left.r2c2*right.r2c0 + left.r2c3*right.r3c0, //r2c0
		left.r3c0*right.r0c0 + left.r3c1*right.r1c0 + left.r3c2*right.r2c0 + left.r3c3*right.r3c0, //r3c0
		
		left.r0c0*right.r0c1 + left.r0c1*right.r1c1 + left.r0c2*right.r2c1 + left.r0c3*right.r3c1, //r0c1
		left.r1c0*right.r0c1 + left.r1c1*right.r1c1 + left.r1c2*right.r2c1 + left.r1c3*right.r3c1, //r1c1
		left.r2c0*right.r0c1 + left.r2c1*right.r1c1 + left.r2c2*right.r2c1 + left.r2c3*right.r3c1, //r2c1
		left.r3c0*right.r0c1 + left.r3c1*right.r1c1 + left.r3c2*right.r2c1 + left.r3c3*right.r3c1, //r3c1
		
		left.r0c0*right.r0c2 + left.r0c1*right.r1c2 + left.r0c2*right.r2c2 + left.r0c3*right.r3c2, //r0c2
		left.r1c0*right.r0c2 + left.r1c1*right.r1c2 + left.r1c2*right.r2c2 + left.r1c3*right.r3c2, //r1c2
		left.r2c0*right.r0c2 + left.r2c1*right.r1c2 + left.r2c2*right.r2c2 + left.r2c3*right.r3c2, //r2c2
		left.r3c0*right.r0c2 + left.r3c1*right.r1c2 + left.r3c2*right.r2c2 + left.r3c3*right.r3c2, //r3c2
		
		left.r0c0*right.r0c3 + left.r0c1*right.r1c3 + left.r0c2*right.r2c3 + left.r0c3*right.r3c3, //r0c3
		left.r1c0*right.r0c3 + left.r1c1*right.r1c3 + left.r1c2*right.r2c3 + left.r1c3*right.r3c3, //r1c3
		left.r2c0*right.r0c3 + left.r2c1*right.r1c3 + left.r2c2*right.r2c3 + left.r2c3*right.r3c3, //r2c3
		left.r3c0*right.r0c3 + left.r3c1*right.r1c3 + left.r3c2*right.r2c3 + left.r3c3*right.r3c3  //r3c3
	};
	return result;
}

inline mat4 Mat4Translate2(r32 x, r32 y)
{
	return NewMat4(
		1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

inline mat4 Mat4Scale2(r32 x, r32 y)
{
	return NewMat4(
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

#endif //  _MATRICES_H
