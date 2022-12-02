/*
File:   memory_arena.h
Author: Taylor Robbins
Date:   11\26\2022
This is #included by common.h
*/

#ifndef _MEMORY_ARENA_H
#define _MEMORY_ARENA_H

// +--------------------------------------------------------------+
// |                      Enums and Typedefs                      |
// +--------------------------------------------------------------+
enum MemArenaType_t
{
	MemArenaType_None = 0,
	MemArenaType_Redirect,
	MemArenaType_Alias,
	MemArenaType_StdHeap,
	MemArenaType_FixedHeap,
	MemArenaType_PagedHeap,
	MemArenaType_MarkedStack,
	MemArenaType_Buffer,
	MemArenaType_WasmMemory,
	MemArenaType_NumTypes,
};
inline const char* GetMemArenaTypeStr(MemArenaType_t arenaType)
{
	switch (arenaType)
	{
		case MemArenaType_None:        return "None";
		case MemArenaType_Redirect:    return "Redirect";
		case MemArenaType_Alias:       return "Alias";
		case MemArenaType_StdHeap:     return "StdHeap";
		case MemArenaType_FixedHeap:   return "FixedHeap";
		case MemArenaType_PagedHeap:   return "PagedHeap";
		case MemArenaType_MarkedStack: return "MarkedStack";
		case MemArenaType_Buffer:      return "Buffer";
		default: return "Unknown";
	}
}

typedef void* AllocationFunction_f(u64 numBytes);
typedef void FreeFunction_f(void* memPntr);

enum AllocAlignment_t
{
	AllocAlignment_None    = 0,
	AllocAlignment_4Bytes  = 4,
	AllocAlignment_8Bytes  = 8,
	AllocAlignment_16Bytes = 16,
	AllocAlignment_64Bytes = 64,
	AllocAlignment_Max = AllocAlignment_64Bytes,
};

enum MemArenaFlag_t
{
	MemArenaFlag_TelemetryEnabled = 0x0001,
	MemArenaFlag_SingleAlloc      = 0x0002,
	MemArenaFlag_AutoFreePages    = 0x0004,
	// MemArenaFlag_Unused           = 0x0008,
	MemArenaFlag_BreakOnAlloc     = 0x0010,
	MemArenaFlag_BreakOnFree      = 0x0020,
	MemArenaFlag_BreakOnRealloc   = 0x0040,
	MemArenaFlag_DebugOutput      = 0x0080,
	MemArenaFlag_NumFlags         = 6,
};

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
#define HEAP_ALLOC_FILLED_FLAG 0x8000000000000000ULL
#define HEAP_ALLOC_SIZE_MASK   0x7FFFFFFFFFFFFFFFULL
struct HeapAllocPrefix_t
{
	u64 size; //top-bit is used as "filled" flag, includes prefix size
};

struct HeapPageHeader_t
{
	HeapPageHeader_t* next;
	u64 size;
	u64 used;
};

struct MarkedStackArenaHeader_t
{
	u64 maxNumMarks;
	u64 numMarks;
	u64 highMarkCount;
};

struct MemArena_t
{
	MemArenaType_t type;
	u16 flags;
	AllocAlignment_t alignment;
	u64 pageSize;
	u64 maxSize;
	u64 maxNumPages;
	u64 debugBreakThreshold;
	
	u64 size;
	u64 used;
	u64 numPages;
	u64 numAllocations;
	u64 highUsedMark;
	u64 highAllocMark;
	
	void* headerPntr;
	void* mainPntr;
	void* otherPntr;
	AllocationFunction_f* allocFunc;
	FreeFunction_f* freeFunc;
	MemArena_t* sourceArena;
};

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
// +==============================+
// |            Helper            |
// +==============================+
bool IsAlignedTo(const void* memoryPntr, AllocAlignment_t alignment);
u8 OffsetToAlign(const void* memoryPntr, AllocAlignment_t alignment);
bool IsPntrInsideRange(const void* testPntr, const void* rangeBase, u64 rangeSize);

// +==============================+
// |             Init             |
// +==============================+
void InitMemArena_Redirect(MemArena_t* arena, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc);
void InitMemArena_Alias(MemArena_t* arena, MemArena_t* sourceArena);
void InitMemArena_StdHeap(MemArena_t* arena);
void InitMemArena_FixedHeap(MemArena_t* arena, u64 size, void* memoryPntr, AllocAlignment_t alignment = AllocAlignment_None);
void InitMemArena_PagedHeapFuncs(MemArena_t* arena, u64 pageSize, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc, u64 maxNumPages = 0, AllocAlignment_t alignment = AllocAlignment_None);
void InitMemArena_PagedHeapArena(MemArena_t* arena, u64 pageSize, MemArena_t* sourceArena, u64 maxNumPages = 0, AllocAlignment_t alignment = AllocAlignment_None);
void InitMemArena_MarkedStack(MemArena_t* arena, u64 size, void* memoryPntr, u64 maxNumMarks, AllocAlignment_t alignment = AllocAlignment_None);
void InitMemArena_Buffer(MemArena_t* arena, u64 bufferSize, void* bufferPntr, bool singleAlloc = false, AllocAlignment_t alignment = AllocAlignment_None);
#define CreateBufferArenaOnStack(arenaName, bufferName, size) MemArena_t arenaName; u8 bufferName[size]; InitMemArena_Buffer(&arenaName, (size), &bufferName[0])
void UpdateMemArenaFuncPntrs(MemArena_t* arena, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc);

// +==============================+
// |             Info             |
// +==============================+
bool DoesMemArenaSupportFreeing(MemArena_t* arena);
bool MemArenaVerify(MemArena_t* arena, bool assertOnFailure = false);
u64 GetNumMemMarks(MemArena_t* arena);

// +==============================+
// |           Allocate           |
// +==============================+
void* AllocMem(MemArena_t* arena, u64 numBytes, AllocAlignment_t alignOverride = AllocAlignment_None);
#define AllocStruct(arena, structName)          (structName*)AllocMem((arena), sizeof(structName))
#define AllocArray(arena, structName, numItems) (structName*)AllocMem((arena), sizeof(structName) * (numItems))
#define AllocBytes(arena, numBytes)             (u8*)AllocMem((arena), (numBytes))
#define AllocChars(arena, numBytes)             (char*)AllocMem((arena), (numBytes))
MemArena_t AllocBufferArena(MemArena_t* sourceArena, u64 numBytes, AllocAlignment_t alignOverride = AllocAlignment_None);
char* AllocCharsAndFill(MemArena_t* arena, u64 numChars, const char* dataForFill, bool addNullTerm = true);
char* AllocCharsAndFillNt(MemArena_t* arena, const char* nullTermStr, bool addNullTerm = true);

// +==============================+
// |             Free             |
// +==============================+
bool FreeMem(MemArena_t* arena, void* allocPntr, u64 allocSize = 0, bool ignoreNullptr = false, u64* oldSizeOut = nullptr);
#define HardFreeMem(arena, allocPntr) FreeMem((arena), (allocPntr), 0, false)
#define SoftFreeMem(arena, allocPntr) FreeMem((arena), (allocPntr), 0, true)
#define FreeBufferArena(bufferArena, sourceArena) FreeMem((sourceArena), (bufferArena)->mainPntr, (bufferArena)->size)

// +==============================+
// |          Reallocate          |
// +==============================+
void* ReallocMem(MemArena_t* arena, void* allocPntr, u64 newSize, u64 oldSize = 0, AllocAlignment_t alignOverride = AllocAlignment_None, bool ignoreNullptr = false, u64* oldSizeOut = nullptr);
#define HardReallocMem(arena, allocPntr, newSize) ReallocMem((arena), (allocPntr), (newSize), 0, AllocAlignment_None, false)
#define SoftReallocMem(arena, allocPntr, newSize) ReallocMem((arena), (allocPntr), (newSize), 0, AllocAlignment_None, true)

// +==============================+
// |         Push and Pop         |
// +==============================+
void PushMemMark(MemArena_t* arena);
void PopMemMark(MemArena_t* arena);
u64 GetNumMarks(MemArena_t* arena);

#endif //  _MEMORY_ARENA_H
