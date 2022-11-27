/*
File:   stdint.h
Author: Taylor Robbins
Date:   11\23\2022
*/

#ifndef _STDINT_H
#define _STDINT_H

typedef unsigned long size_t;
typedef unsigned long uintptr_t;
typedef long ptrdiff_t;

typedef signed char        int8_t;
typedef signed short       int16_t;
typedef signed int         int32_t;
typedef signed long long   int64_t;
typedef signed long long   intmax_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long long uintmax_t;

typedef __builtin_va_list va_list;
typedef __builtin_va_list __isoc_va_list;

#define INT8_MIN   (-1 - 0x7f)
#define INT16_MIN  (-1 - 0x7fff)
#define INT32_MIN  (-1 - 0x7fffffff)
#define INT64_MIN  (-1 - 0x7fffffffffffffff)

#define INT8_MAX   (0x7f)
#define INT16_MAX  (0x7fff)
#define INT32_MAX  (0x7fffffff)
#define INT64_MAX  (0x7fffffffffffffff)

#define UINT8_MAX  (0xff)
#define UINT16_MAX (0xffff)
#define UINT32_MAX (0xffffffffu)
#define UINT64_MAX (0xffffffffffffffffu)

#define SIZE_MAX     UINT32_MAX
#define UINTPTR_MAX  UINT32_MAX
#define PTRDIFF_MIN  INT32_MIN
#define PTRDIFF_MAX  INT32_MAX

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#endif //  _STDINT_H
