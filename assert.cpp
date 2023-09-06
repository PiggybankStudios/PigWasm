/*
File:   assert.cpp
Author: Taylor Robbins
Date:   12\04\2022
Description: 
	** Holds the AsserttionHandler function that gets called when an Assertion fails
*/

#include "common.h"
#include "assert.h"

#include "debug.h"

bool insideAssertion = false;

void AssertionHandler(const char* conditionStr, const char* messageStr, const char* functionName, const char* filePath, u32 lineNumber, bool isLowLevel)
{
	if (insideAssertion) { return; }
	insideAssertion = true;
	
	if (!isLowLevel)
	{
		if (messageStr != nullptr)
		{
			PrintLine_E("Assertion Failure! %s (%s) in %s %s:%u",
				messageStr,
				conditionStr,
				functionName,
				filePath, lineNumber
			);
		}
		else
		{
			PrintLine_E("Assertion Failure! (%s) in %s %s:%u",
				conditionStr,
				functionName,
				filePath, lineNumber
			);
		}
	}
	
	HandleAssertionInJs(conditionStr, messageStr, functionName, filePath, lineNumber, isLowLevel);
	
	insideAssertion = false;
}
