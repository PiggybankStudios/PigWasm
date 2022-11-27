/*
File:   intrinsics.h
Author: Taylor Robbins
Date:   11\26\2022
*/

#ifndef _INTRINSICS_H
#define _INTRINSICS_H

// +==============================+
// |             Abs              |
// +==============================+
inline r32 AbsR32(r32 value)
{
	return fabsf(value);
}
inline r64 AbsR64(r64 value)
{
	return fabs(value);
}
inline i8 AbsI8(i8 value)
{
	return (value < 0) ? -value : value;
}
inline i16 AbsI16(i16 value)
{
	return (value < 0) ? -value : value;
}
inline i32 AbsI32(i32 value)
{
	return (value < 0 ? -value : value);
}
inline i64 AbsI64(i64 value)
{
	return (value < 0 ? -value : value);
}

// +==============================+
// |            Round             |
// +==============================+
inline r32 RoundR32(r32 value)
{
	return roundf(value);
}
inline i32 RoundR32i(r32 value)
{
	return (i32)roundf(value);
}
inline r64 RoundR64(r64 value)
{
	return round(value);
}
inline i64 RoundR64i(r64 value)
{
	return (i64)round(value);
}

// +==============================+
// |            Floor             |
// +==============================+
inline r32 FloorR32(r32 value)
{
	return floorf(value);
}
inline i32 FloorR32i(r32 value)
{
	return (i32)floorf(value);
}
inline r64 FloorR64(r64 value)
{
	return floor(value);
}
inline i64 FloorR64i(r64 value)
{
	return (i64)floor(value);
}

// +==============================+
// |             Ceil             |
// +==============================+
inline r32 CeilR32(r32 value)
{
	return ceilf(value);
}
inline i32 CeilR32i(r32 value)
{
	return (i32)ceilf(value);
}
inline r64 CeilR64(r64 value)
{
	return ceil(value);
}
inline i64 CeilR64i(r64 value)
{
	return (i64)ceil(value);
}

// +==============================+
// |            CeilTo            |
// +==============================+
inline i32 CeilToI32(i32 value, i32 chunkSize)
{
	if ((value%chunkSize) == 0) { return value; }
	return value + (chunkSize - (value%chunkSize));
}
inline u32 CeilToU32(u32 value, u32 chunkSize)
{
	if ((value%chunkSize) == 0) { return value; }
	return value + (chunkSize - (value%chunkSize));
}

// +==============================+
// |           CeilDiv            |
// +==============================+
inline i32 CeilDivI32(i32 dividend, i32 divisor)
{
	if ((dividend % divisor) == 0) { return dividend / divisor; }
	else { return (dividend / divisor) + 1; }
}
inline u32 CeilDivU32(u32 dividend, u32 divisor)
{
	if ((dividend % divisor) == 0) { return dividend / divisor; }
	else { return (dividend / divisor) + 1; }
}
inline u64 CeilDivU64(u64 dividend, u64 divisor)
{
	if ((dividend % divisor) == 0) { return dividend / divisor; }
	else { return (dividend / divisor) + 1; }
}

// +==============================+
// |           AbsDiff            |
// +==============================+
inline u8 AbsDiffU8(u8 value1, u8 value2)
{
	if (value1 > value2) { return value1 - value2; }
	else { return value2 - value1; }
}
inline u16 AbsDiffU16(u16 value1, u16 value2)
{
	if (value1 > value2) { return value1 - value2; }
	else { return value2 - value1; }
}
inline u32 AbsDiffU32(u32 value1, u32 value2)
{
	if (value1 > value2) { return value1 - value2; }
	else { return value2 - value1; }
}
inline u64 AbsDiffU64(u64 value1, u64 value2)
{
	if (value1 > value2) { return value1 - value2; }
	else { return value2 - value1; }
}

// +==============================+
// |             Sin              |
// +==============================+
inline r32 SinR32(r32 angle)
{
	return sinf(angle);
}
inline r64 SinR64(r64 angle)
{
	return sin(angle);
}

// +==============================+
// |             Cos              |
// +==============================+
inline r32 CosR32(r32 angle)
{
	return cosf(angle);
}
inline r64 CosR64(r64 angle)
{
	return cos(angle);
}

// +==============================+
// |             Tan              |
// +==============================+
inline r32 TanR32(r32 angle)
{
	return tanf(angle);
}
inline r64 TanR64(r64 angle)
{
	return tan(angle);
}

// +==============================+
// |             Asin             |
// +==============================+
inline r32 AsinR32(r32 value)
{
	return asinf(value);
}
inline r64 AsinR64(r64 value)
{
	return asin(value);
}

// +==============================+
// |             Acos             |
// +==============================+
inline r32 AcosR32(r32 value)
{
	return acosf(value);
}
inline r64 AcosR64(r64 value)
{
	return acos(value);
}

// +==============================+
// |             Atan             |
// +==============================+
inline r32 AtanJoinedR32(r32 value)
{
	return atanf(value);
}
inline r64 AtanJoinedR64(r64 value)
{
	return atan(value);
}
inline r32 AtanR32(r32 y, r32 x)
{
	return atan2f(y, x);
}
inline r64 AtanR64(r64 y, r64 x)
{
	return atan2(y, x);
}

// +==============================+
// |             Saw              |
// +==============================+
inline r32 SawR32(r32 angle)
{
	r32 x = (angle-Pi32/2) / (Pi32);
	i32 section = FloorR32i(x);
	r32 offset = x - (r32)section;
	if ((section%2) == 0)
	{
		return -1 + (offset * 2);
	}
	else
	{
		return 1 - (offset * 2);
	}
}
inline r64 SawR64(r64 angle)
{
	r64 x = (angle-Pi64/2) / (Pi64);
	i64 section = FloorR64i(x);
	r64 offset = x - (r64)section;
	if ((section%2) == 0)
	{
		return -1 + (offset * 2);
	}
	else
	{
		return 1 - (offset * 2);
	}
}

#endif //  _INTRINSICS_H
