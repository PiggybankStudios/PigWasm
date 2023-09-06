/*
File:   helpers.h
Author: Taylor Robbins
Date:   11\26\2022
*/

#ifndef _HELPERS_H
#define _HELPERS_H

#include "vectors.h"

// +--------------------------------------------------------------+
// |                         Time Defines                         |
// +--------------------------------------------------------------+
#define NUM_DAYS_PER_NORMAL_YEAR    365
#define NUM_DAYS_PER_WEEK           7
#define NUM_HOURS_PER_DAY           24
#define NUM_MINUTES_PER_HOUR        60
#define NUM_SECONDS_PER_MINUTE      60
#define NUM_MILLISECONDS_PER_SECOND 1000

#define NUM_MILLISECONDS_PER_MINUTE ((u64)NUM_MILLISECONDS_PER_SECOND * (u64)NUM_SECONDS_PER_MINUTE)
#define NUM_MILLISECONDS_PER_HOUR   (NUM_MILLISECONDS_PER_MINUTE * (u64)NUM_MINUTES_PER_HOUR)
#define NUM_MILLISECONDS_PER_DAY    (NUM_MILLISECONDS_PER_HOUR * (u64)NUM_HOURS_PER_DAY)
#define NUM_SECONDS_PER_HOUR        ((u64)NUM_SECONDS_PER_MINUTE * (u64)NUM_MINUTES_PER_HOUR)
#define NUM_SECONDS_PER_DAY         (NUM_SECONDS_PER_HOUR * (u64)NUM_HOURS_PER_DAY)
#define NUM_MINUTES_PER_DAY         ((u64)NUM_MINUTES_PER_HOUR * (u64)NUM_HOURS_PER_DAY)

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
r32 OscillateBy(u64 timeSource, r32 min, r32 max, u64 periodMs, u64 offset = 0);
bool DoRecsIntersect(v2 pos1, v2 size1, v2 pos2, v2 size2);
MyStr_t FormattedMillisecondsU64(u64 milliseconds, MemArena_t* memArena);
MyStr_t FormattedMillisecondsR64(r64 milliseconds, MemArena_t* memArena, bool includeMicroseconds = false);

#endif //  _HELPERS_H
