/*
File:   random_num_gen.h
Author: Taylor Robbins
Date:   11\27\2022
*/

#ifndef _RANDOM_NUM_GEN_H
#define _RANDOM_NUM_GEN_H

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
enum RandomAlg_t
{
	RandomAlg_None = 0,
	RandomAlg_LinearCongruential32,
	RandomAlg_NumAlgs,
};

struct RandomSeries_t
{
	RandomAlg_t alg;
	bool isSeeded;
	u32 state;
};

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
void CreateRandomSeries(RandomSeries_t* seriesOut, RandomAlg_t algorithm);
void RandomSeriesSeed(RandomSeries_t* series, u32 seed);
void RandomSeriesStep(RandomSeries_t* series, u32 numberOfSteps = 1);
u32 RandomSeriesGetU32(RandomSeries_t* series);
r32 RandomSeriesGetR32(RandomSeries_t* series);
r32 RandomSeriesGetR32(RandomSeries_t* series, r32 min, r32 max);

#endif //  _RANDOM_NUM_GEN_H
