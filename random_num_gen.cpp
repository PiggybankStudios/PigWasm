/*
File:   random_num_gen.cpp
Author: Taylor Robbins
Date:   11\27\2022
Description: 
	** Holds implementations for a few different kinds of pseudo random number
	** generating algorithms
*/

#include "common.h"
#include "random_num_gen.h"

#include "intrinsics.h"

void CreateRandomSeries(RandomSeries_t* seriesOut, RandomAlg_t algorithm)
{
	ClearPointer(seriesOut);
	seriesOut->alg = algorithm;
	seriesOut->isSeeded = false;
	seriesOut->state = 0;
}

void RandomSeriesSeed(RandomSeries_t* series, u32 seed)
{
	series->state = seed;
	series->isSeeded = true;
}

void RandomSeriesStep(RandomSeries_t* series, u32 numberOfSteps)
{
	Assert(series->isSeeded);
	switch (series->alg)
	{
		case RandomAlg_LinearCongruential32:
		{
			//n(x+1) = n(x) * A + C modulo M
			for (u64 sIndex = 0; sIndex < numberOfSteps; sIndex++)
			{
				u64 newState = (series->state * 1103515245ULL + 12345ULL) & 0xFFFFFFFFFFFFFFFFULL;
				series->state = newState;
			}
		} break;
		
		default: Assert(false); break;
	}
}

u32 RandomSeriesGetU32(RandomSeries_t* series)
{
	RandomSeriesStep(series, 1);
	return series->state;
}
r32 RandomSeriesGetR32(RandomSeries_t* series)
{
	RandomSeriesStep(series, 1);
	return (r32)(series->state % 10000) / 10000.0f;
}
r32 RandomSeriesGetR32(RandomSeries_t* series, r32 min, r32 max)
{
	return LerpR32(min, max, RandomSeriesGetR32(series));
}

