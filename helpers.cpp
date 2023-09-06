/*
File:   helpers.cpp
Author: Taylor Robbins
Date:   11\26\2022
Description: 
	** Helpers are small functions that are used all throughout the codebase and don't
	** really belong in a file of their own
*/

#include "common.h"
#include "helpers.h"

#include "intrinsics.h"
#include "memory.h"

r32 OscillateBy(u64 timeSource, r32 min, r32 max, u64 periodMs, u64 offset)
{
	r32 lerpValue = (SinR32((((timeSource + offset) % periodMs) / (r32)periodMs) * 2*Pi32) + 1.0f) / 2.0f;
	return min + (max - min) * lerpValue;
}

bool DoRecsIntersect(v2 pos1, v2 size1, v2 pos2, v2 size2)
{
	if (pos1.x + size1.x < pos2.x) { return false; }
	if (pos1.y + size1.y < pos2.y) { return false; }
	if (pos2.x + size2.x < pos1.x) { return false; }
	if (pos2.y + size2.y < pos1.y) { return false; }
	return true;
}

MyStr_t FormattedMillisecondsU64(u64 milliseconds, MemArena_t* memArena)
{
	u64 millisecondsLeft = milliseconds;
	u64 numDays = (millisecondsLeft / NUM_MILLISECONDS_PER_DAY); millisecondsLeft -= (numDays * NUM_MILLISECONDS_PER_DAY);
	u64 numHours = (millisecondsLeft / NUM_MILLISECONDS_PER_HOUR); millisecondsLeft -= (numHours * NUM_MILLISECONDS_PER_HOUR);
	u64 numMinutes = (millisecondsLeft / NUM_MILLISECONDS_PER_MINUTE); millisecondsLeft -= (numMinutes * NUM_MILLISECONDS_PER_MINUTE);
	u64 numSeconds = (millisecondsLeft / NUM_MILLISECONDS_PER_SECOND); millisecondsLeft -= (numSeconds * NUM_MILLISECONDS_PER_SECOND);
	if (numDays == 0)
	{
		if (numHours == 0)
		{
			if (numMinutes == 0)
			{
				if (numSeconds == 0)
				{
					return PrintInArenaStr(memArena, "%llums", millisecondsLeft);
				}
				else
				{
					return PrintInArenaStr(memArena, "%llus %llums", numSeconds, millisecondsLeft);
				}
			}
			else
			{
				return PrintInArenaStr(memArena, "%llum %llus %llums", numMinutes, numSeconds, millisecondsLeft);
			}
		}
		{
			return PrintInArenaStr(memArena, "%lluh %llum %llus %llums", numHours, numMinutes, numSeconds, millisecondsLeft);
		}
	}
	else
	{
		return PrintInArenaStr(memArena, "%llud %lluh %llum %llus %llums", numDays, numHours, numMinutes, numSeconds, millisecondsLeft);
	}
}
MyStr_t FormattedMillisecondsR64(r64 milliseconds, MemArena_t* memArena, bool includeMicroseconds)
{
	if (includeMicroseconds)
	{
		u64 wholeMilliseconds = (u64)FloorR64i(milliseconds);
		r64 partialMilliseconds = milliseconds - FloorR64(wholeMilliseconds);
		u64 numMicroseconds = (u64)RoundR64i(partialMilliseconds * 1000.0);
		u64 numDays = (wholeMilliseconds / NUM_MILLISECONDS_PER_DAY); wholeMilliseconds -= (numDays * NUM_MILLISECONDS_PER_DAY);
		u64 numHours = (wholeMilliseconds / NUM_MILLISECONDS_PER_HOUR); wholeMilliseconds -= (numHours * NUM_MILLISECONDS_PER_HOUR);
		u64 numMinutes = (wholeMilliseconds / NUM_MILLISECONDS_PER_MINUTE); wholeMilliseconds -= (numMinutes * NUM_MILLISECONDS_PER_MINUTE);
		u64 numSeconds = (wholeMilliseconds / NUM_MILLISECONDS_PER_SECOND); wholeMilliseconds -= (numSeconds * NUM_MILLISECONDS_PER_SECOND);
		if (numDays == 0)
		{
			if (numHours == 0)
			{
				if (numMinutes == 0)
				{
					if (numSeconds == 0)
					{
						return PrintInArenaStr(memArena, "%llums %lluus", wholeMilliseconds, numMicroseconds);
					}
					else
					{
						return PrintInArenaStr(memArena, "%llus %llums %lluus", numSeconds, wholeMilliseconds, numMicroseconds);
					}
				}
				else
				{
					return PrintInArenaStr(memArena, "%llum %llus %llums %lluus", numMinutes, numSeconds, wholeMilliseconds, numMicroseconds);
				}
			}
			{
				return PrintInArenaStr(memArena, "%lluh %llum %llus %llums %lluus", numHours, numMinutes, numSeconds, wholeMilliseconds, numMicroseconds);
			}
		}
		else
		{
			return PrintInArenaStr(memArena, "%llud %lluh %llum %llus %llums %lluus", numDays, numHours, numMinutes, numSeconds, wholeMilliseconds, numMicroseconds);
		}
	}
	else
	{
		return FormattedMillisecondsU64((u64)FloorR64i(milliseconds), memArena);
	}
}
