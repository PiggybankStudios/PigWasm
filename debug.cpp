/*
File:   debug.cpp
Author: Taylor Robbins
Date:   11\23\2022
Description: 
	** None 
*/

#include "common.h"
#include "debug.h"

static char printBuffer[PRINT_BUFFER_SIZE];

void DebugOutputPrint(DbgLevel_t dbgLevel, const char* formatString, ...)
{
	va_list args;
	va_start(args, formatString);
	
	int length = vsnprintf(&printBuffer[0], ArrayCount(printBuffer), formatString, args); //Measure first
	
	if (length < 0)
	{
		DebugOutput(DbgLevel_Error, "[Print Error]");
		DebugOutput((int)dbgLevel, formatString);
		return;
	}
	if (length > ArrayCount(printBuffer)-1)
	{
		DebugOutput(DbgLevel_Error, "[Print Overflow]");
		length = ArrayCount(printBuffer)-1;
	}
	printBuffer[length] = '\0';
	
	va_end(args);
	
	DebugOutput((int)dbgLevel, &printBuffer[0]);
}
