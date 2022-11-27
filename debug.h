/*
File:   debug.h
Author: Taylor Robbins
Date:   11\22\2022
*/

#ifndef _DEBUG_H
#define _DEBUG_H

enum DbgLevel_t
{
	DbgLevel_None,
	DbgLevel_Debug = 0,
	DbgLevel_Log,
	DbgLevel_Info,
	DbgLevel_Notify,
	DbgLevel_Other,
	DbgLevel_Warn,
	DbgLevel_Error,
	DbgLevel_NumLevels,
};

void DebugOutputPrint(DbgLevel_t dbgLevel, const char* formatString, ...);

#define WriteLine_D(message) DebugOutput((int)DbgLevel_Debug,  (message))
#define WriteLine_L(message) DebugOutput((int)DbgLevel_Log,    (message))
#define WriteLine_I(message) DebugOutput((int)DbgLevel_Info,   (message))
#define WriteLine_N(message) DebugOutput((int)DbgLevel_Notify, (message))
#define WriteLine_O(message) DebugOutput((int)DbgLevel_Other,  (message))
#define WriteLine_W(message) DebugOutput((int)DbgLevel_Warn,   (message))
#define WriteLine_E(message) DebugOutput((int)DbgLevel_Error,  (message))

#define PrintLine_D(message, ...) DebugOutputPrint(DbgLevel_Debug,  (message), __VA_ARGS__)
#define PrintLine_L(message, ...) DebugOutputPrint(DbgLevel_Log,    (message), __VA_ARGS__)
#define PrintLine_I(message, ...) DebugOutputPrint(DbgLevel_Info,   (message), __VA_ARGS__)
#define PrintLine_N(message, ...) DebugOutputPrint(DbgLevel_Notify, (message), __VA_ARGS__)
#define PrintLine_O(message, ...) DebugOutputPrint(DbgLevel_Other,  (message), __VA_ARGS__)
#define PrintLine_W(message, ...) DebugOutputPrint(DbgLevel_Warn,   (message), __VA_ARGS__)
#define PrintLine_E(message, ...) DebugOutputPrint(DbgLevel_Error,  (message), __VA_ARGS__)

#endif //  _DEBUG_H
