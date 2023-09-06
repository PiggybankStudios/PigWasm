/*
File:   easing.h
Author: Taylor Robbins
Date:   11\27\2022
*/

#ifndef _EASING_H
#define _EASING_H

// Modeled after the parabola y = x^2
inline r32 EaseQuadraticIn(r32 p)
{
	return p * p;
}

// Modeled after the parabola y = -x^2 + 2x
inline r32 EaseQuadraticOut(r32 p)
{
	return -(p * (p - 2));
}

// Modeled after the piecewise quadratic
// y = (1/2)((2x)^2)             ; [0, 0.5)
// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
inline r32 EaseQuadraticInOut(r32 p)
{
	if (p < 0.5)
	{
		return 2 * p * p;
	}
	else
	{
		return (-2 * p * p) + (4 * p) - 1;
	}
}

// Modeled after the cubic y = x^3
inline r32 EaseCubicIn(r32 p)
{
	return p * p * p;
}

// Modeled after the cubic y = (x - 1)^3 + 1
inline r32 EaseCubicOut(r32 p)
{
	r32 f = (p - 1);
	return f * f * f + 1;
}

// Modeled after the piecewise cubic
// y = (1/2)((2x)^3)       ; [0, 0.5)
// y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
inline r32 EaseCubicInOut(r32 p)
{
	if(p < 0.5)
	{
		return 4 * p * p * p;
	}
	else
	{
		r32 f = ((2 * p) - 2);
		return 0.5f * f * f * f + 1;
	}
}

// Modeled after the quartic x^4
inline r32 EaseQuarticIn(r32 p)
{
	return p * p * p * p;
}

// Modeled after the quartic y = 1 - (x - 1)^4
inline r32 EaseQuarticOut(r32 p)
{
	r32 f = (p - 1);
	return f * f * f * (1 - p) + 1;
}

// Modeled after the piecewise quartic
// y = (1/2)((2x)^4)        ; [0, 0.5)
// y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
inline r32 EaseQuarticInOut(r32 p) 
{
	if(p < 0.5)
	{
		return 8 * p * p * p * p;
	}
	else
	{
		r32 f = (p - 1);
		return -8 * f * f * f * f + 1;
	}
}

// Modeled after the quintic y = x^5
inline r32 EaseQuinticIn(r32 p) 
{
	return p * p * p * p * p;
}

// Modeled after the quintic y = (x - 1)^5 + 1
inline r32 EaseQuinticOut(r32 p) 
{
	r32 f = (p - 1);
	return f * f * f * f * f + 1;
}

// Modeled after the piecewise quintic
// y = (1/2)((2x)^5)       ; [0, 0.5)
// y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
inline r32 EaseQuinticInOut(r32 p) 
{
	if(p < 0.5)
	{
		return 16 * p * p * p * p * p;
	}
	else
	{
		r32 f = ((2 * p) - 2);
		return  0.5f * f * f * f * f * f + 1;
	}
}


#endif //  _EASING_H
