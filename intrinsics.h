/*
File:   intrinsics.h
Author: Taylor Robbins
Date:   11\26\2022
*/

#ifndef _INTRINSICS_H
#define _INTRINSICS_H

inline r32 AbsR32(r32 value);
inline r64 AbsR64(r64 value);
inline i8 AbsI8(i8 value);
inline i16 AbsI16(i16 value);
inline i32 AbsI32(i32 value);
inline i64 AbsI64(i64 value);
inline r32 RoundR32(r32 value);
inline i32 RoundR32i(r32 value);
inline r64 RoundR64(r64 value);
inline i64 RoundR64i(r64 value);
inline r32 FloorR32(r32 value);
inline i32 FloorR32i(r32 value);
inline r64 FloorR64(r64 value);
inline i64 FloorR64i(r64 value);
inline r32 CeilR32(r32 value);
inline i32 CeilR32i(r32 value);
inline r64 CeilR64(r64 value);
inline i64 CeilR64i(r64 value);
inline i32 CeilToI32(i32 value, i32 chunkSize);
inline u32 CeilToU32(u32 value, u32 chunkSize);
inline i32 CeilDivI32(i32 dividend, i32 divisor);
inline u32 CeilDivU32(u32 dividend, u32 divisor);
inline u64 CeilDivU64(u64 dividend, u64 divisor);
inline u8 AbsDiffU8(u8 value1, u8 value2);
inline u16 AbsDiffU16(u16 value1, u16 value2);
inline u32 AbsDiffU32(u32 value1, u32 value2);
inline u64 AbsDiffU64(u64 value1, u64 value2);
inline r32 SinR32(r32 angle);
inline r64 SinR64(r64 angle);
inline r32 CosR32(r32 angle);
inline r64 CosR64(r64 angle);
inline r32 TanR32(r32 angle);
inline r64 TanR64(r64 angle);
inline r32 AsinR32(r32 value);
inline r64 AsinR64(r64 value);
inline r32 AcosR32(r32 value);
inline r64 AcosR64(r64 value);
inline r32 AtanJoinedR32(r32 value);
inline r64 AtanJoinedR64(r64 value);
inline r32 AtanR32(r32 y, r32 x);
inline r64 AtanR64(r64 y, r64 x);
inline r32 SawR32(r32 angle);
inline r64 SawR64(r64 angle);

#endif //  _INTRINSICS_H
