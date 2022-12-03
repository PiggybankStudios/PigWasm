/*
File:   memory_arena.cpp
Author: Taylor Robbins
Date:   11\26\2022
Description: 
	** Memory Arenas are a structure that holds the state necassary to manage
	** allocating and freeing chunks of memory inside a pre-allocated space
*/

#include "common.h"
#include "memory_arena.h"

#include "memory.h"
#include "debug.h"
#include "intrinsics.h"

// +--------------------------------------------------------------+
// |                       Helper Functions                       |
// +--------------------------------------------------------------+
#define PackAllocPrefixSize(used, size)   ((used) ? HEAP_ALLOC_FILLED_FLAG : 0ULL) | ((size) & HEAP_ALLOC_SIZE_MASK)
#define IsAllocPrefixFilled(packedSize)   IsFlagSet((packedSize), HEAP_ALLOC_FILLED_FLAG)
#define UnpackAllocPrefixSize(packedSize) ((packedSize) & HEAP_ALLOC_SIZE_MASK)

bool IsAlignedTo(const void* memoryPntr, AllocAlignment_t alignment)
{
	if (alignment == AllocAlignment_None) { return true; }
	u64 address = (u64)(memoryPntr);
	return ((address % (u64)alignment) == 0);
}
u8 OffsetToAlign(const void* memoryPntr, AllocAlignment_t alignment)
{
	if (alignment == AllocAlignment_None) { return 0; }
	u64 address = (u64)(memoryPntr);
	if ((address % (u64)alignment) == 0) { return 0; }
	else { return (u8)alignment - (u8)(address % (u64)alignment); }
}

bool IsPntrInsideRange(const void* testPntr, const void* rangeBase, u64 rangeSize)
{
	if (((const u8*)testPntr) < ((const u8*)rangeBase)) { return false; }
	if (((const u8*)testPntr) >= ((const u8*)rangeBase) + rangeSize) { return false; }
	return true;
}

// +--------------------------------------------------------------+
// |                        Init Functions                        |
// +--------------------------------------------------------------+
void InitMemArena_Redirect(MemArena_t* arena, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc)
{
	NotNull(arena);
	NotNull(allocFunc);
	ClearPointer(arena);
	arena->type = MemArenaType_Redirect;
	arena->allocFunc = allocFunc;
	arena->freeFunc = freeFunc;
	arena->used = 0; //NOTE: This only tracks allocations, not deallocations, so it only goes up
	arena->numAllocations = 0;
	
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highAllocMark = arena->numAllocations;
}
void InitMemArena_Alias(MemArena_t* arena, MemArena_t* sourceArena)
{
	NotNull(arena);
	NotNull(sourceArena);
	ClearPointer(arena);
	arena->type = MemArenaType_Alias;
	arena->sourceArena = sourceArena;
	arena->used = 0; //NOTE: This MAY not track deallocations, it depends on if the sourceArena supports returning the size on FreeMem
	arena->numAllocations = 0;
	
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highUsedMark = arena->used;
	arena->highAllocMark = arena->numAllocations;
}
void InitMemArena_StdHeap(MemArena_t* arena)
{
	NotNull(arena);
	ClearPointer(arena);
	arena->type = MemArenaType_StdHeap;
	arena->used = 0;
	arena->size = (u64)((currentWasmPageCount * WASM_MEM_PAGE_SIZE) - currentHeapHeadAddress);
	arena->numAllocations = 0;
	arena->otherPntr = nullptr; //used to keep track of the last allocation that was made, to make some kinds of Realloc calls possible
	// PrintLine_D("There are %u pages allocated (0x%08X) and heap starts at 0x%08X", currentWasmPageCount, (u32)currentWasmPageCount * WASM_MEM_PAGE_SIZE, currentHeapHeadAddress);
	
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highAllocMark = arena->numAllocations;
}
void InitMemArena_FixedHeap(MemArena_t* arena, u64 size, void* memoryPntr, AllocAlignment_t alignment)
{
	NotNull(arena);
	u64 prefixSize = sizeof(HeapAllocPrefix_t);
	Assert(size > prefixSize);
	NotNull(memoryPntr);
	Assert(IsAlignedTo(memoryPntr, alignment));
	ClearPointer(arena);
	arena->type = MemArenaType_FixedHeap;
	arena->alignment = alignment;
	arena->mainPntr = memoryPntr;
	arena->size = size;
	HeapAllocPrefix_t* firstAlloc = (HeapAllocPrefix_t*)arena->mainPntr;
	ClearPointer(firstAlloc);
	firstAlloc->size = PackAllocPrefixSize(false, arena->size);
	arena->used = prefixSize;
	arena->numAllocations = 0;
	
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highUsedMark = arena->used;
	arena->highAllocMark = arena->numAllocations;
}
void InitMemArena_PagedHeapFuncs(MemArena_t* arena, u64 pageSize, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc, u64 maxNumPages, AllocAlignment_t alignment)
{
	ClearPointer(arena);
	arena->type = MemArenaType_PagedHeap;
	arena->alignment = alignment;
	arena->pageSize = pageSize;
	arena->maxNumPages = maxNumPages;
	arena->allocFunc = allocFunc;
	arena->freeFunc = freeFunc;
	FlagSet(arena->flags, MemArenaFlag_AutoFreePages);
	arena->size = 0;
	arena->used = 0;
	arena->numPages = 0;
	arena->numAllocations = 0;
	arena->headerPntr = nullptr;
	arena->mainPntr = nullptr;
	arena->otherPntr = nullptr;
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highUsedMark = 0;
	arena->highAllocMark = 0;
}
void InitMemArena_PagedHeapArena(MemArena_t* arena, u64 pageSize, MemArena_t* sourceArena, u64 maxNumPages, AllocAlignment_t alignment)
{
	UNUSED(arena);
	UNUSED(pageSize);
	UNUSED(sourceArena);
	UNUSED(maxNumPages);
	UNUSED(alignment);
	ClearPointer(arena);
	arena->type = MemArenaType_PagedHeap;
	arena->alignment = alignment;
	arena->pageSize = pageSize;
	arena->maxNumPages = maxNumPages;
	arena->sourceArena = sourceArena;
	arena->size = 0;
	arena->used = 0;
	arena->numPages = 0;
	arena->numAllocations = 0;
	arena->headerPntr = nullptr;
	arena->mainPntr = nullptr;
	arena->otherPntr = nullptr;
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highUsedMark = 0;
	arena->highAllocMark = 0;
}
void InitMemArena_MarkedStack(MemArena_t* arena, u64 size, void* memoryPntr, u64 maxNumMarks, AllocAlignment_t alignment)
{
	NotNull(arena);
	NotNull(memoryPntr);
	Assert(size > 0);
	Assert(maxNumMarks > 0);
	Assert(size > sizeof(MarkedStackArenaHeader_t) + (maxNumMarks * sizeof(u64)));
	
	ClearPointer(arena);
	arena->type = MemArenaType_MarkedStack;
	arena->alignment = alignment;
	arena->headerPntr = (((u8*)memoryPntr) + 0);
	arena->otherPntr  = (((u8*)memoryPntr) + sizeof(MarkedStackArenaHeader_t));
	arena->mainPntr   = (((u8*)memoryPntr) + sizeof(MarkedStackArenaHeader_t) + (maxNumMarks * sizeof(u64)));
	arena->size = size - (sizeof(MarkedStackArenaHeader_t) + (maxNumMarks * sizeof(u64)));
	arena->used = 0;
	arena->numAllocations = 0;
	
	MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
	ClearPointer(stackHeader);
	stackHeader->maxNumMarks = maxNumMarks;
	stackHeader->numMarks = 0;
	
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highUsedMark = 0;
	stackHeader->highMarkCount = 0;
}
void InitMemArena_Buffer(MemArena_t* arena, u64 bufferSize, void* bufferPntr, bool singleAlloc, AllocAlignment_t alignment)
{
	NotNull(arena);
	NotNull(bufferPntr);
	ClearPointer(arena);
	arena->type = MemArenaType_Buffer;
	arena->alignment = alignment;
	FlagSetTo(arena->flags, MemArenaFlag_SingleAlloc, singleAlloc);
	arena->mainPntr = bufferPntr;
	arena->size = bufferSize;
	arena->used = 0;
	arena->numAllocations = 0;
	
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highUsedMark = arena->used;
	arena->highAllocMark = arena->numAllocations;
}

void UpdateMemArenaFuncPntrs(MemArena_t* arena, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc)
{
	Assert(arena->type == MemArenaType_Redirect || MemArenaType_PagedHeap);
	arena->allocFunc = allocFunc;
	arena->freeFunc = freeFunc;
}

// +--------------------------------------------------------------+
// |                    Information Functions                     |
// +--------------------------------------------------------------+
bool DoesMemArenaSupportFreeing(MemArena_t* arena)
{
	NotNull(arena);
	switch (arena->type)
	{
		case MemArenaType_MarkedStack: return false;
		default: return true;
	}
}

bool MemArenaVerify(MemArena_t* arena, bool assertOnFailure)
{
	NotNull(arena);
	if (arena->type == MemArenaType_None)
	{
		AssertIfMsg(assertOnFailure, false, "Tried to verify uninitialized arena");
		return false;
	}
	
	switch (arena->type)
	{
		// +==============================+
		// |    MemArenaType_Redirect     |
		// +==============================+
		// case MemArenaType_Redirect:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +==============================+
		// |      MemArenaType_Alias      |
		// +==============================+
		case MemArenaType_Alias:
		{
			//TODO: Implement me!
		} break;
		
		// +==============================+
		// |     MemArenaType_StdHeap     |
		// +==============================+
		// case MemArenaType_StdHeap:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +==============================+
		// |    MemArenaType_FixedHeap    |
		// +==============================+
		case MemArenaType_FixedHeap:
		{
			if (arena->mainPntr == nullptr)
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap mainPntr is null");
				return false;
			}
			if (arena->used >= arena->size)
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap used is larger than size");
				return false;
			}
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled) && arena->used > arena->highUsedMark)
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap used is higher than high used mark");
				return false;
			}
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled) && arena->numAllocations > arena->highAllocMark)
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap allocation count is higher than high allocation mark");
				return false;
			}
			if (!IsAlignedTo(arena->mainPntr, arena->alignment))
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap main memory not aligned to alignment setting");
				return false;
			}
			if (IsFlagSet(arena->flags, MemArenaFlag_SingleAlloc) && arena->numAllocations > 1)
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap single alloc doesn't match allocation count");
				return false;
			}
			if (arena->used < sizeof(HeapAllocPrefix_t))
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap used is smaller than 1 prefix size");
				return false;
			}
			
			u64 numFilledSections = 0;
			bool lastSectionWasEmpty = false;
			u64 sectionIndex = 0;
			u64 totalUsed = 0;
			
			u64 allocOffset = 0;
			u8* allocBytePntr = (u8*)arena->mainPntr;
			HeapAllocPrefix_t* prevPrefixPntr = nullptr;
			while (allocOffset < arena->size)
			{
				HeapAllocPrefix_t* allocPntr = (HeapAllocPrefix_t*)allocBytePntr;
				u8* allocAfterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
				UNUSED(allocAfterPrefixPntr);
				bool isAllocFilled = IsAllocPrefixFilled(allocPntr->size);
				u64 allocSize = UnpackAllocPrefixSize(allocPntr->size);
				if (allocSize < sizeof(HeapAllocPrefix_t))
				{
					AssertIfMsg(assertOnFailure, false, "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
					return false;
				}
				u64 allocAfterPrefixSize = allocSize - sizeof(HeapAllocPrefix_t);
				UNUSED(allocAfterPrefixSize);
				if (isAllocFilled)
				{
					if (numFilledSections + 1 > arena->numAllocations)
					{
						AssertIfMsg(assertOnFailure, false, "FixedHeap numAllocations doesn't match actual number of filled sections");
						return false;
					}
					numFilledSections++;
					if (totalUsed + allocSize > arena->used)
					{
						AssertIfMsg(assertOnFailure, false, "FixedHeap used doesn't match actual total used area");
						return false;
					}
					totalUsed += allocSize;
					lastSectionWasEmpty = false;
				}
				else //not filled
				{
					if (lastSectionWasEmpty)
					{
						AssertIfMsg(assertOnFailure, false, "FixedHeap two empty sections in a row");
						return false;
					}
					if (totalUsed + sizeof(HeapAllocPrefix_t) > arena->used)
					{
						AssertIfMsg(assertOnFailure, false, "FixedHeap used doesn't match actual total used area");
						return false;
					}
					totalUsed += sizeof(HeapAllocPrefix_t);
					lastSectionWasEmpty = true;
				}
				if (allocOffset + allocSize > arena->size)
				{
					AssertIfMsg(assertOnFailure, false, "FixedHeap corrupt section size stepping us past the end of the arena memory");
					return false;
				}
				prevPrefixPntr = allocPntr;
				allocOffset += allocSize;
				allocBytePntr += allocSize;
				sectionIndex++;
			}
			UNUSED(sectionIndex); //used for debug purposes
			UNUSED(prevPrefixPntr); //used for debug purposes
			AssertMsg(allocOffset == arena->size, "Somehow stepped past end in ArenaVerify on FixedHeap despite the in-loop check");
			
			if (totalUsed != arena->used)
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap used is higher than actual used amount");
				return false;
			}
			if (numFilledSections != arena->numAllocations)
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap numAllocations is higher than actual used section count");
				return false;
			}
			
			return true;
		} break;
		
		// +==============================+
		// |    MemArenaType_PagedHeap    |
		// +==============================+
		case MemArenaType_PagedHeap:
		{
			if (arena->headerPntr == nullptr && arena->numPages > 0)
			{
				AssertIfMsg(assertOnFailure, false, "headerPntr was empty but numPages > 0! Has this arena been initialized??");
				return false;
			}
			if ((arena->sourceArena == nullptr) && (arena->allocFunc == nullptr || arena->freeFunc == nullptr))
			{
				AssertIfMsg(assertOnFailure, false, "PagedHeap doesn't have a allocFunc/freeFun nor a sourceArena to allocate new pages from");
				return false;
			}
			if (arena->mainPntr != nullptr)
			{
				AssertIfMsg(assertOnFailure, false, "mainPntr was filled when it shouldn't be!");
				return false;
			}
			if (arena->otherPntr != nullptr)
			{
				AssertIfMsg(assertOnFailure, false, "otherPntr was filled when it shouldn't be!");
				return false;
			}
			if (arena->pageSize == 0)
			{
				AssertIfMsg(assertOnFailure, false, "pageSize was zero! That's invalid!");
				return false;
			}
			if (arena->alignment > AllocAlignment_Max)
			{
				AssertIfMsg(assertOnFailure, false, "Invalid alignment value!");
				return false;
			}
			
			u64 numAllocations = 0;
			u64 totalNumSections = 0;
			
			HeapPageHeader_t* pageHeader = (HeapPageHeader_t*)arena->headerPntr;
			u64 pageIndex = 0;
			while (pageHeader != nullptr)
			{
				if (pageHeader->size == 0)
				{
					AssertIfMsg(assertOnFailure, false, "Page had a size of 0!");
					return false;
				}
				if (pageHeader->size < arena->pageSize)
				{
					AssertIfMsg(assertOnFailure, false, "Page size was less than arena->pageSize!");
					return false;
				}
				if (pageIndex+1 < arena->numPages && pageHeader->next == nullptr)
				{
					AssertIfMsg(assertOnFailure, false, "Page next pntr was nullptr too soon! We expected more pages in the chain!");
					return false;
				}
				if (pageHeader->used > pageHeader->size)
				{
					AssertIfMsg(assertOnFailure, false, "Page used is higher than size! That's not right!");
					return false;
				}
				if (pageIndex >= arena->numPages)
				{
					AssertIfMsg(assertOnFailure, false, "The numPages in this paged heap is off. We have too many pages or the last pointer to a page was corrupt!");
					return false;
				}
				
				u8* pageBase = (u8*)(pageHeader + 1);
				u64 allocOffset = 0;
				u8* allocBytePntr = pageBase;
				u64 sectionIndex = 0;
				HeapAllocPrefix_t* prevPrefixPntr = nullptr;
				while (allocOffset < pageHeader->size)
				{
					HeapAllocPrefix_t* prefixPntr = (HeapAllocPrefix_t*)allocBytePntr;
					// u8* afterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
					bool isSectionFilled = IsAllocPrefixFilled(prefixPntr->size);
					u64 sectionSize = UnpackAllocPrefixSize(prefixPntr->size);
					if (sectionSize < sizeof(HeapAllocPrefix_t))
					{
						AssertIfMsg(assertOnFailure, false, "Found an allocation header that claimed to be smaller than the header itself in Paged Heap");
						return false;
					}
					u64 afterPrefixSize = sectionSize - sizeof(HeapAllocPrefix_t);
					if (afterPrefixSize == 0)
					{
						AssertIfMsg(assertOnFailure, false, "Found an empty section that was only big enough to contain the allocation header");
						return false;
					}
					if (allocOffset + sectionSize > pageHeader->size)
					{
						AssertIfMsg(assertOnFailure, false, "Found a corrupt allocation header size. It would step us past the end of a page!");
						return false;
					}
					
					if (isSectionFilled)
					{
						numAllocations++;
					}
					
					prevPrefixPntr = prefixPntr;
					allocOffset += sectionSize;
					allocBytePntr += sectionSize;
					totalNumSections++;
					sectionIndex++;
				}
				
				pageHeader = pageHeader->next;
				pageIndex++;
			}
			
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled) && numAllocations != arena->numAllocations)
			{
				AssertIfMsg(assertOnFailure, false, "Actual allocation count in paged heap did not match tracked numAllocations");
				return false;
			}
			
			return true;
		} break;
		
		// +==============================+
		// |   MemArenaType_MarkedStack   |
		// +==============================+
		// case MemArenaType_MarkedStack:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +==============================+
		// |     MemArenaType_Buffer      |
		// +==============================+
		// case MemArenaType_Buffer:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +====================================+
		// | Unsupported or Corrupt Arena Type  |
		// +====================================+
		default:
		{
			AssertIfMsg(assertOnFailure, false, "Unsupported or corrupt arena type found in MemArenaVerify");
			return false;
		} break;
	}
	
	return true;
}

u64 GetNumMemMarks(MemArena_t* arena)
{
	NotNull(arena);
	Assert(arena->type == MemArenaType_MarkedStack);
	NotNull(arena->headerPntr);
	MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
	Assert(stackHeader->maxNumMarks > 0);
	Assert(stackHeader->numMarks <= stackHeader->maxNumMarks);
	return stackHeader->numMarks;
}

// +--------------------------------------------------------------+
// |                    GetAllocSize Function                     |
// +--------------------------------------------------------------+
u64 GetAllocSize(MemArena_t* arena, const void* allocPntr)
{
	NotNull(arena);
	AssertMsg(arena->type != MemArenaType_None, "Tried to GetAllocSize from uninitialized arena");
	
	u64 result = 0;
	switch (arena->type)
	{
		// +==============================+
		// |      MemArenaType_Alias      |
		// +==============================+
		case MemArenaType_Alias:
		{
			NotNull(arena->sourceArena);
			result = GetAllocSize(arena->sourceArena, allocPntr);
		} break;
		
		// +==============================+
		// |    MemArenaType_FixedHeap    |
		// +==============================+
		case MemArenaType_FixedHeap:
		{
			NotNull(arena->mainPntr);
			
			u64 allocOffset = 0;
			u8* allocBytePntr = (u8*)arena->mainPntr;
			u64 sectionIndex = 0;
			while (allocOffset < arena->size)
			{
				HeapAllocPrefix_t* prefixPntr = (HeapAllocPrefix_t*)allocBytePntr;
				u8* afterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
				bool isSectionFilled = IsAllocPrefixFilled(prefixPntr->size);
				u64 sectionSize = UnpackAllocPrefixSize(prefixPntr->size);
				AssertMsg(sectionSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
				u64 afterPrefixSize = sectionSize - sizeof(HeapAllocPrefix_t);
				
				if ((u8*)allocPntr >= allocBytePntr && (u8*)allocPntr < allocBytePntr + sectionSize)
				{
					AssertMsg((u8*)allocPntr >= afterPrefixPntr, "Tried to GetAllocSize on a pointer that pointed into a Fixed Heap header. This is a corrupt pointer!");
					//TODO: Check if the allocation was actually aligned. Be more strict if it wasn't aligned
					AssertMsg((u8*)allocPntr <= afterPrefixPntr + OffsetToAlign(afterPrefixPntr, AllocAlignment_Max), "Tried to GetAllocSize on a pointer that pointed to the middle of a Fixed Heap section. This is a corrupt pointer!");
					AssertMsg(isSectionFilled, "Tried to GetAllocSize on section in Fixed Heap that has already been freed. This is a memory management bug");
					result = afterPrefixSize;
					break;
				}
				
				allocOffset += sectionSize;
				allocBytePntr += sectionSize;
				sectionIndex++;
			}
			UNUSED(sectionIndex); //used for debug purposes
			AssertMsg(result != 0, "Tried to GetAllocSize on an unknown pointer from Fixed Heap. The pointer must be corrupt or was asked about in the wrong memArena. This is a memory management bug");
		} break;
		
		// +==============================+
		// |    MemArenaType_PagedHeap    |
		// +==============================+
		case MemArenaType_PagedHeap:
		{
			HeapPageHeader_t* pageHeader = (HeapPageHeader_t*)arena->headerPntr;
			u64 pageIndex = 0;
			bool foundAlloc = false;
			while (pageHeader != nullptr && !foundAlloc)
			{
				u8* pageBase = (u8*)(pageHeader + 1);
				if (allocPntr >= pageBase && allocPntr < pageBase + pageHeader->size)
				{
					u64 allocOffset = 0;
					u8* allocBytePntr = pageBase;
					u64 sectionIndex = 0;
					HeapAllocPrefix_t* prevPrefixPntr = nullptr;
					while (allocOffset < pageHeader->size)
					{
						HeapAllocPrefix_t* prefixPntr = (HeapAllocPrefix_t*)allocBytePntr;
						u8* afterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
						bool isSectionFilled = IsAllocPrefixFilled(prefixPntr->size);
						u64 sectionSize = UnpackAllocPrefixSize(prefixPntr->size);
						AssertMsg(sectionSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Paged Heap");
						u64 afterPrefixSize = sectionSize - sizeof(HeapAllocPrefix_t);
						
						if ((u8*)allocPntr >= allocBytePntr && (u8*)allocPntr < allocBytePntr + sectionSize)
						{
							AssertMsg((u8*)allocPntr >= afterPrefixPntr, "Tried to GetAllocSize on a pointer that pointed into a Paged Heap header. This is a corrupt pointer!");
							//TODO: Check if the allocation was actually aligned. Be more strict if it wasn't aligned
							AssertMsg((u8*)allocPntr <= afterPrefixPntr + OffsetToAlign(afterPrefixPntr, AllocAlignment_Max), "Tried to GetAllocSize on a pointer that pointed to the middle of a Paged Heap section. This is a corrupt pointer!");
							AssertMsg(isSectionFilled, "Tried to GetAllocSize on section in Paged Heap that has already been freed. This is a memory management bug");
							result = afterPrefixSize;
							foundAlloc = true;
							break;
						}
						
						prevPrefixPntr = prefixPntr;
						allocOffset += sectionSize;
						allocBytePntr += sectionSize;
						sectionIndex++;
					}
					AssertMsg(foundAlloc, "We have a bug in our GetAllocSize walk. Couldn't find section that contained the pntr in this page!");
					break;
				}
				
				pageHeader = pageHeader->next;
				pageIndex++;
			}
			AssertMsg(foundAlloc, "Tried to GetAllocSize on pntr that isn't in any of the pages of this arena!");
		} break;
		
		// +==============================+
		// |    Unsupported Arena Type    |
		// +==============================+
		default:
		{
			PrintLine_E("Unsupported arena type in GetAllocSize: %s", GetMemArenaTypeStr(arena->type));
			AssertMsg(false, "Tried to GetAllocSize on arena type that doesn't support giving that information!");
		} break;
	}
	
	return result;
}

// +--------------------------------------------------------------+
// |                      Allocate Function                       |
// +--------------------------------------------------------------+
void* AllocMem(MemArena_t* arena, u64 numBytes, AllocAlignment_t alignOverride)
{
	NotNull(arena);
	AssertMsg(arena->type != MemArenaType_None, "Tried to allocate from uninitialized arena");
	
	if (IsFlagSet(arena->flags, MemArenaFlag_BreakOnAlloc) && (arena->debugBreakThreshold == 0 || numBytes >= arena->debugBreakThreshold))
	{
		MyDebugBreak();
	}
	if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput)) { PrintLine_D("AllocMem on %s for %u bytes (arena is %u/%u)", GetMemArenaTypeStr(arena->type), (u32)numBytes, (u32)arena->used, (u32)arena->size); }
	
	if (numBytes == 0) { return nullptr; }
	if (IsFlagSet(arena->flags, MemArenaFlag_SingleAlloc) && arena->numAllocations > 0)
	{
		PrintLine_W("Attempted second allocation of %llu out of single alloc arena (type: %s, size: %llu, used: %llu)", numBytes, GetMemArenaTypeStr(arena->type), arena->size, arena->used);
		return nullptr;
	}
	AllocAlignment_t alignment = (alignOverride != AllocAlignment_None) ? alignOverride : arena->alignment;
	
	u8* result = nullptr;
	switch (arena->type)
	{
		// +==============================+
		// |    MemArenaType_Redirect     |
		// +==============================+
		case MemArenaType_Redirect:
		{
			AssertMsg(alignment == AllocAlignment_None, "Tried to align memory in Redirect arena type");
			NotNull(arena->allocFunc);
			result = (u8*)arena->allocFunc(numBytes);
			if (result == nullptr) { break; }
			IncrementU64(arena->numAllocations);
			arena->used += numBytes;
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
			}
		} break;
		
		// +==============================+
		// |      MemArenaType_Alias      |
		// +==============================+
		case MemArenaType_Alias:
		{
			NotNull(arena->sourceArena);
			result = (u8*)AllocMem(arena->sourceArena, numBytes, alignment);
			if (result == nullptr) { break; }
			IncrementU64(arena->numAllocations);
			arena->size = arena->sourceArena->size;
			arena->used = arena->sourceArena->used;
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
				if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
			}
		} break;
		
		// +==============================+
		// |     MemArenaType_StdHeap     |
		// +==============================+
		case MemArenaType_StdHeap:
		{
			AssertMsg(alignment == AllocAlignment_None, "Tried to align memory in StdHeap arena type");
			if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput)) { PrintLine_D("Moving currenHeapAddress 0x%08X->0x%08X", currentHeapHeadAddress, currentHeapHeadAddress + (u32)numBytes); }
			if (currentHeapHeadAddress + numBytes > currentWasmPageCount * WASM_MEM_PAGE_SIZE)
			{
				u32 numPagesNeeded = CeilDivU32(currentHeapHeadAddress + numBytes, WASM_MEM_PAGE_SIZE) - currentWasmPageCount;
				if (PRINT_WASM_PAGE_EXPANSIONS || IsFlagSet(arena->flags, MemArenaFlag_DebugOutput)) { PrintLine_D("Expending WASM memory by %u pages (%u total)", numPagesNeeded, currentWasmPageCount + numPagesNeeded); }
				RequestMoreMemoryPages(numPagesNeeded);
				currentWasmPageCount += numPagesNeeded;
				arena->size += (numPagesNeeded * WASM_MEM_PAGE_SIZE);
			}
			result = (u8*)currentHeapHeadAddress;
			arena->otherPntr = (void*)result;
			currentHeapHeadAddress += numBytes;
			IncrementU64(arena->numAllocations);
			arena->used += numBytes;
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
			}
		} break;
		
		// +==============================+
		// |    MemArenaType_FixedHeap    |
		// +==============================+
		case MemArenaType_FixedHeap:
		{
			NotNull(arena->mainPntr);
			
			//TODO: Assert a maximum sized based off the fact that our top bit stores filled info
			
			u64 allocOffset = 0;
			u8* allocBytePntr = (u8*)arena->mainPntr;
			u64 sectionIndex = 0;
			while (allocOffset < arena->size)
			{
				HeapAllocPrefix_t* allocPntr = (HeapAllocPrefix_t*)allocBytePntr;
				u8* allocAfterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
				bool isAllocFilled = IsAllocPrefixFilled(allocPntr->size);
				u64 allocSize = UnpackAllocPrefixSize(allocPntr->size);
				AssertMsg(allocSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
				AssertMsg(allocOffset + allocSize <= arena->size, "Found an allocation header with invalid size. Would extend past the end of the arena!");
				u64 allocAfterPrefixSize = allocSize - sizeof(HeapAllocPrefix_t);
				if (!isAllocFilled)
				{
					u8 alignOffset = OffsetToAlign(allocAfterPrefixPntr, alignment);
					if (allocAfterPrefixSize >= alignOffset + numBytes)
					{
						result = allocAfterPrefixPntr + alignOffset;
						if (allocAfterPrefixSize > alignOffset + numBytes + sizeof(HeapAllocPrefix_t))
						{
							//Split the section into 2 (one filled and one empty)
							allocPntr->size = PackAllocPrefixSize(true, sizeof(HeapAllocPrefix_t) + alignOffset + numBytes);
							HeapAllocPrefix_t* newSection = (HeapAllocPrefix_t*)(allocAfterPrefixPntr + alignOffset + numBytes);
							newSection->size = PackAllocPrefixSize(false, allocAfterPrefixSize - (alignOffset + numBytes));
							arena->used += alignOffset + numBytes + sizeof(HeapAllocPrefix_t);
							Assert(arena->used <= arena->size);
						}
						else
						{
							//This entire section is getting used (or there's not enough extra room to make another empty section)
							allocPntr->size = PackAllocPrefixSize(true, allocSize);
							arena->used += allocSize - sizeof(HeapAllocPrefix_t);
							Assert(arena->used <= arena->size);
						}
						arena->numAllocations++;
						if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
						{
							if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
							if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
						}
						break;
					}
				}
				allocOffset += allocSize;
				allocBytePntr += allocSize;
				sectionIndex++;
			}
			UNUSED(sectionIndex); //used for debug purposes
			AssertIfMsg(result == nullptr, allocOffset == arena->size, "A Fixed Heap is corrupt. The last allocation size does not perfectly match the size of the arena");
			
			// MemArenaVerify(arena, true); //TODO: Remove me when not debugging
		} break;
		
		// +==============================+
		// |    MemArenaType_PagedHeap    |
		// +==============================+
		case MemArenaType_PagedHeap:
		{
			HeapPageHeader_t* pageHeader = (HeapPageHeader_t*)arena->headerPntr;
			u64 pageIndex = 0;
			while (pageHeader != nullptr)
			{
				if (pageHeader->size - pageHeader->used < numBytes)
				{
					pageHeader = pageHeader->next;
					pageIndex++;
					continue;
				}
				
				u64 allocOffset = 0;
				u8* allocBytePntr = (u8*)(pageHeader + 1);
				u64 sectionIndex = 0;
				while (allocOffset < pageHeader->size)
				{
					HeapAllocPrefix_t* allocPntr = (HeapAllocPrefix_t*)allocBytePntr;
					u8* allocAfterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
					bool isAllocFilled = IsAllocPrefixFilled(allocPntr->size);
					u64 allocSize = UnpackAllocPrefixSize(allocPntr->size);
					AssertMsg(allocSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
					AssertMsg(allocOffset + allocSize <= pageHeader->size, "Found an allocation header with invalid size. Would extend past the end of a page!");
					u64 allocAfterPrefixSize = allocSize - sizeof(HeapAllocPrefix_t);
					if (!isAllocFilled)
					{
						u8 alignOffset = OffsetToAlign(allocAfterPrefixPntr, alignment);
						if (allocAfterPrefixSize >= alignOffset + numBytes)
						{
							if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput)) { PrintLine_D("Found space in page %llu/%llu section[%llu]", pageIndex+1, arena->numPages, sectionIndex); }
							result = allocAfterPrefixPntr + alignOffset;
							if (allocAfterPrefixSize > alignOffset + numBytes + sizeof(HeapAllocPrefix_t))
							{
								//Split the section into 2 (one filled and one empty)
								allocPntr->size = PackAllocPrefixSize(true, sizeof(HeapAllocPrefix_t) + alignOffset + numBytes);
								HeapAllocPrefix_t* newSection = (HeapAllocPrefix_t*)(allocAfterPrefixPntr + alignOffset + numBytes);
								newSection->size = PackAllocPrefixSize(false, allocAfterPrefixSize - (alignOffset + numBytes));
								pageHeader->used += alignOffset + numBytes + sizeof(HeapAllocPrefix_t);
								arena->used += alignOffset + numBytes + sizeof(HeapAllocPrefix_t);
								Assert(pageHeader->used <= pageHeader->size);
								Assert(arena->used <= arena->size);
							}
							else
							{
								//This entire section is getting used (or there's not enough extra room to make another empty section)
								allocPntr->size = PackAllocPrefixSize(true, allocSize);
								pageHeader->used += allocSize - sizeof(HeapAllocPrefix_t);
								arena->used += allocSize - sizeof(HeapAllocPrefix_t);
								Assert(pageHeader->used <= pageHeader->size);
								Assert(arena->used <= arena->size);
							}
							arena->numAllocations++;
							if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
							{
								if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
								if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
							}
							break;
						}
					}
					
					allocBytePntr += allocSize;
					allocOffset += allocSize;
					sectionIndex++;
				}
				UNUSED(sectionIndex); //used for debug purposes
				
				pageHeader = pageHeader->next;
				pageIndex++;
			}
			
			// +==============================+
			// |      Allocate New Page       |
			// +==============================+
			if (result == nullptr)
			{
				u64 maxNeededSize = sizeof(HeapAllocPrefix_t) + numBytes + AllocAlignment_Max;
				u64 newPageSize = arena->pageSize;
				if (newPageSize < maxNeededSize)
				{
					newPageSize = maxNeededSize;
				}
				if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput)) { PrintLine_D("Allocating new %llu byte page (now %llu pages)", newPageSize, arena->numPages+1); }
				
				HeapPageHeader_t* newPageHeader = nullptr;
				if (arena->sourceArena != nullptr)
				{
					DebugAssert(arena->sourceArena != arena);
					newPageHeader = (HeapPageHeader_t*)AllocMem(arena->sourceArena, sizeof(HeapPageHeader_t) + newPageSize, alignment);
					// NotNullMsg(newPageHeader, "Failed to allocate new page from arena for paged heap");
					if (newPageHeader == nullptr) { return nullptr; }
				}
				else if (arena->allocFunc != nullptr)
				{
					newPageHeader = (HeapPageHeader_t*)arena->allocFunc(sizeof(HeapPageHeader_t) + newPageSize);
					// NotNullMsg(newPageHeader, "Failed to allocate new page for paged heap");
					if (newPageHeader == nullptr) { return nullptr; }
				}
				NotNullMsg(newPageHeader, "sourceArena and allocFunc are both not filled!");
				
				arena->size += newPageSize;
				arena->used += sizeof(HeapAllocPrefix_t);
				
				ClearPointer(newPageHeader);
				newPageHeader->next = nullptr;
				newPageHeader->size = newPageSize;
				newPageHeader->used = sizeof(HeapAllocPrefix_t);
				
				u8* pageBase = (u8*)(newPageHeader + 1);
				HeapAllocPrefix_t* allocPntr = (HeapAllocPrefix_t*)pageBase;
				u8* allocAfterPrefixPntr = (pageBase + sizeof(HeapAllocPrefix_t));
				u64 allocAfterPrefixSize = newPageSize - sizeof(HeapAllocPrefix_t);
				u8 alignOffset = OffsetToAlign(allocAfterPrefixPntr, alignment);
				AssertMsg(allocAfterPrefixSize >= alignOffset + numBytes, "Paged heap has a bug where we didn't allocate enough space in the new page to fit the allocation");
				result = allocAfterPrefixPntr + alignOffset;
				if (allocAfterPrefixSize > alignOffset + numBytes + sizeof(HeapAllocPrefix_t))
				{
					//Split the section into 2 (one filled and one empty)
					allocPntr->size = PackAllocPrefixSize(true, sizeof(HeapAllocPrefix_t) + alignOffset + numBytes);
					HeapAllocPrefix_t* newSection = (HeapAllocPrefix_t*)(allocAfterPrefixPntr + alignOffset + numBytes);
					newSection->size = PackAllocPrefixSize(false, allocAfterPrefixSize - (alignOffset + numBytes));
					newPageHeader->used += alignOffset + numBytes + sizeof(HeapAllocPrefix_t);
					arena->used += alignOffset + numBytes + sizeof(HeapAllocPrefix_t);
					Assert(newPageHeader->used <= newPageHeader->size);
					Assert(arena->used <= arena->size);
				}
				else
				{
					//This entire section is getting used (or there's not enough extra room to make another empty section)
					allocPntr->size = PackAllocPrefixSize(true, newPageSize);
					newPageHeader->used += newPageSize - sizeof(HeapAllocPrefix_t);
					arena->used += newPageSize - sizeof(HeapAllocPrefix_t);
					Assert(newPageHeader->used <= newPageHeader->size);
					Assert(arena->used <= arena->size);
				}
				arena->numAllocations++;
				
				if (arena->numPages == 0)
				{
					Assert(arena->headerPntr == nullptr);
					arena->headerPntr = newPageHeader;
				}
				else
				{
					NotNull(arena->headerPntr);
					HeapPageHeader_t* walkPntr = (HeapPageHeader_t*)arena->headerPntr;
					for (u64 pIndex = 0; pIndex < arena->numPages-1; pIndex++)
					{
						walkPntr = walkPntr->next;
					}
					NotNull(walkPntr);
					Assert(walkPntr->next == nullptr);
					walkPntr->next = newPageHeader;
				}
				arena->numPages++;
				
				if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
				{
					if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
					if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
				}
				break;
			}
		} break;
		
		// +==============================+
		// |   MemArenaType_MarkedStack   |
		// +==============================+
		case MemArenaType_MarkedStack:
		{
			NotNull(arena->headerPntr);
			NotNull(arena->otherPntr);
			u8 alignOffset = OffsetToAlign(((u8*)arena->mainPntr) + arena->used, alignment);
			if (arena->used + alignOffset + numBytes > arena->size) { return nullptr; }
			result = ((u8*)arena->mainPntr) + arena->used + alignOffset;
			arena->used += alignOffset + numBytes;
			arena->numAllocations++;
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
			}
		} break;
		
		// +==============================+
		// |     MemArenaType_Buffer      |
		// +==============================+
		case MemArenaType_Buffer:
		{
			u8* basePntr = (u8*)arena->mainPntr;
			u64 neededSize = numBytes;
			result = basePntr + arena->used;
			u8 alignOffset = OffsetToAlign(result, alignment);
			neededSize += alignOffset;
			if (arena->size - arena->used < neededSize) { result = nullptr; break; }
			result += alignOffset;
			IncrementU64(arena->numAllocations);
			arena->used += neededSize;
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
				if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
			}
		} break;
		
		// +==============================+
		// |    Unsupported Arena Type    |
		// +==============================+
		default:
		{
			PrintLine_E("Unsupported arena type in AllocMem: %u (size: %llu, used: %llu)", arena->type, arena->size, arena->used);
			AssertMsg(false, "Unsupported arena type in AllocMem. Maybe the arena is corrupted?");
		} break;
	}
	
	if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput)) { PrintLine_D("AllocMem returned 0x%08X (arena is %llu/%llu)", (u32)result, arena->used, arena->size); }
	AssertMsg(IsAlignedTo(result, alignment), "An arena has a bug where it tried to return mis-aligned memory");
	return (void*)result;
}

MemArena_t AllocBufferArena(MemArena_t* sourceArena, u64 numBytes, AllocAlignment_t alignOverride)
{
	MemArena_t result;
	void* allocatedMemory = AllocMem(sourceArena, numBytes, alignOverride);
	NotNull(allocatedMemory);
	InitMemArena_Buffer(&result, numBytes, allocatedMemory, true);
	return result;
}

char* AllocCharsAndFill(MemArena_t* arena, u64 numChars, const char* dataForFill, bool addNullTerm)
{
	NotNull(arena);
	Assert(dataForFill != nullptr || numChars == 0);
	if (numChars == 0 && !addNullTerm) { return nullptr; }
	char* result = (char*)AllocMem(arena, numChars + (addNullTerm ? 1 : 0));
	if (result == nullptr) { return nullptr; }
	if (numChars > 0)
	{
		memcpy(result, dataForFill, numChars);
	}
	if (addNullTerm)
	{
		result[numChars] = '\0';
	}
	return result;
}
char* AllocCharsAndFillNt(MemArena_t* arena, const char* nullTermStr, bool addNullTerm)
{
	NotNull(arena);
	NotNull(nullTermStr);
	u64 numChars = strlen(nullTermStr);
	return AllocCharsAndFill(arena, numChars, nullTermStr, addNullTerm);
}

// +--------------------------------------------------------------+
// |                        Free Function                         |
// +--------------------------------------------------------------+
//NOTE: oldSizeOut may not be written. In some cases it is written to allocSize if provided and no sanity checks are done
bool FreeMem(MemArena_t* arena, void* allocPntr, u64 allocSize, bool ignoreNullptr, u64* oldSizeOut)
{
	NotNull(arena);
	AssertMsg(arena->type != MemArenaType_None, "Tried to free from uninitialized arena");
	Assert(ignoreNullptr || allocPntr != nullptr);
	if (allocPntr == nullptr) { return 0; }
	
	if (IsFlagSet(arena->flags, MemArenaFlag_BreakOnFree) && (arena->debugBreakThreshold == 0 || allocSize >= arena->debugBreakThreshold))
	{
		MyDebugBreak();
	}
	if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput)) { PrintLine_D("FreeMem on %s 0x%08X (%llu bytes) (arena is %llu/%llu)", GetMemArenaTypeStr(arena->type), (u32)allocPntr, allocSize, arena->used, arena->size); }
	
	bool result = false;
	switch (arena->type)
	{
		// +==============================+
		// |    MemArenaType_Redirect     |
		// +==============================+
		case MemArenaType_Redirect:
		{
			NotNull(arena->freeFunc);
			arena->freeFunc(allocPntr);
			result = true;
			Decrement(arena->numAllocations);
			DecrementBy(arena->used, allocSize);
			if (oldSizeOut != nullptr && allocSize != 0) { *oldSizeOut = allocSize; }
		} break;
		
		// +==============================+
		// |      MemArenaType_Alias      |
		// +==============================+
		case MemArenaType_Alias:
		{
			NotNull(arena->sourceArena);
			result = FreeMem(arena->sourceArena, allocPntr, allocSize, ignoreNullptr, oldSizeOut);
			Decrement(arena->numAllocations);
			arena->size = arena->sourceArena->size;
			arena->used = arena->sourceArena->used;
		} break;
		
		// +==============================+
		// |     MemArenaType_StdHeap     |
		// +==============================+
		case MemArenaType_StdHeap:
		{
			AssertMsg(false, "MemArenaType_StdHeap does not support freeing in web assembly!!!");
		} break;
		
		// +==============================+
		// |    MemArenaType_FixedHeap    |
		// +==============================+
		case MemArenaType_FixedHeap:
		{
			NotNull(arena->mainPntr);
			
			u64 allocOffset = 0;
			u8* allocBytePntr = (u8*)arena->mainPntr;
			u64 sectionIndex = 0;
			HeapAllocPrefix_t* prevPrefixPntr = nullptr;
			while (allocOffset < arena->size)
			{
				HeapAllocPrefix_t* prefixPntr = (HeapAllocPrefix_t*)allocBytePntr;
				u8* afterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
				bool isSectionFilled = IsAllocPrefixFilled(prefixPntr->size);
				u64 sectionSize = UnpackAllocPrefixSize(prefixPntr->size);
				AssertMsg(sectionSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
				u64 afterPrefixSize = sectionSize - sizeof(HeapAllocPrefix_t);
				
				if ((u8*)allocPntr >= allocBytePntr && (u8*)allocPntr < allocBytePntr + sectionSize)
				{
					AssertMsg((u8*)allocPntr >= afterPrefixPntr, "Tried to free a pointer that pointed into a Fixed Heap header. This is a corrupt pointer!");
					//TODO: Check if the allocation was actually aligned. Be more strict if it wasn't aligned
					AssertMsg((u8*)allocPntr <= afterPrefixPntr + OffsetToAlign(afterPrefixPntr, AllocAlignment_Max), "Tried to free a pointer that pointed to the middle of a Fixed Heap section. This is a corrupt pointer!");
					AssertMsg(isSectionFilled, "Tried to double free section in Fixed Heap. This is a memory management bug");
					if (allocSize != 0)
					{
						//TODO: Handle scenarios where the alignment offset or bad-fit scenarios caused the section to be slightly larger than the requested allocation size
						//NOTE: Right now we allow for slop in both the alignment offset and scenarios where the section that was used was only slighly larger than needed
						//      and a second section couldn't be created because there wasn't even enough room for a HeapAllocPrefix_t
						u64 allowedSlop = OffsetToAlign(afterPrefixPntr, AllocAlignment_Max) + sizeof(HeapAllocPrefix_t);
						AssertMsg(AbsDiffU64(allocSize, afterPrefixSize) <= allowedSlop, "Given size did not match actual allocation size in Fixed Heap during FreeMem. This is a memory management bug");
					}
					
					result = true;
					prefixPntr->size = PackAllocPrefixSize(false, sectionSize);
					AssertMsg(arena->used >= afterPrefixSize, "Fixed Heap used tracker was corrupted. Reached 0 too soon!");
					arena->used -= afterPrefixSize;
					AssertMsg(arena->numAllocations > 0, "Fixed Heap numAllocations was corrupted. Reached 0 too soon!");
					arena->numAllocations--;
					
					if (allocOffset + sectionSize < arena->size)
					{
						Assert(allocOffset + sectionSize + sizeof(HeapAllocPrefix_t) <= arena->size);
						HeapAllocPrefix_t* nextPrefixPntr = (HeapAllocPrefix_t*)(allocBytePntr + sectionSize);
						if (!IsAllocPrefixFilled(nextPrefixPntr->size))
						{
							// Merge the next section with this one by making this section bigger
							sectionSize += UnpackAllocPrefixSize(nextPrefixPntr->size);
							prefixPntr->size = PackAllocPrefixSize(false, sectionSize);
							AssertMsg(arena->used >= sizeof(HeapAllocPrefix_t), "Fixed Heap used tracker was corrupted. Reached 0 too soon.");
							arena->used -= sizeof(HeapAllocPrefix_t);
						}
					}
					if (prevPrefixPntr != nullptr && !IsAllocPrefixFilled(prevPrefixPntr->size))
					{
						// Merge the previous section with this one by making it's sectionSize bigger
						prevPrefixPntr->size = PackAllocPrefixSize(false, UnpackAllocPrefixSize(prevPrefixPntr->size) + sectionSize);
						AssertMsg(arena->used >= sizeof(HeapAllocPrefix_t), "Fixed Heap used tracker was corrupted. Reached 0 too soon.");
						arena->used -= sizeof(HeapAllocPrefix_t);
					}
					
					break;
				}
				
				prevPrefixPntr = prefixPntr;
				allocOffset += sectionSize;
				allocBytePntr += sectionSize;
				sectionIndex++;
			}
			UNUSED(sectionIndex); //used for debug purposes
			AssertMsg(result == true, "Tried to free an unknown pointer from Fixed Heap. The pointer must be corrupt or was freed from the wrong heap. This is a memory management bug");
			
			// MemArenaVerify(arena, true); //TODO: Remove me when not debugging
		} break;
		
		// +==============================+
		// |    MemArenaType_PagedHeap    |
		// +==============================+
		case MemArenaType_PagedHeap:
		{
			HeapPageHeader_t* prevPageHeader = nullptr;
			HeapPageHeader_t* pageHeader = (HeapPageHeader_t*)arena->headerPntr;
			u64 pageIndex = 0;
			bool foundAlloc = false;
			while (pageHeader != nullptr && !foundAlloc)
			{
				u8* pageBase = (u8*)(pageHeader + 1);
				if (allocPntr >= pageBase && allocPntr < pageBase + pageHeader->size)
				{
					u64 allocOffset = 0;
					u8* allocBytePntr = pageBase;
					u64 sectionIndex = 0;
					HeapAllocPrefix_t* prevPrefixPntr = nullptr;
					while (allocOffset < pageHeader->size)
					{
						HeapAllocPrefix_t* prefixPntr = (HeapAllocPrefix_t*)allocBytePntr;
						u8* afterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
						bool isSectionFilled = IsAllocPrefixFilled(prefixPntr->size);
						u64 sectionSize = UnpackAllocPrefixSize(prefixPntr->size);
						AssertMsg(sectionSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Paged Heap");
						u64 afterPrefixSize = sectionSize - sizeof(HeapAllocPrefix_t);
						
						if ((u8*)allocPntr >= allocBytePntr && (u8*)allocPntr < allocBytePntr + sectionSize)
						{
							AssertMsg((u8*)allocPntr >= afterPrefixPntr, "Tried to free a pointer that pointed into a Paged Heap header. This is a corrupt pointer!");
							//TODO: Check if the allocation was actually aligned. Be more strict if it wasn't aligned
							AssertMsg((u8*)allocPntr <= afterPrefixPntr + OffsetToAlign(afterPrefixPntr, AllocAlignment_Max), "Tried to free a pointer that pointed to the middle of a Paged Heap section. This is a corrupt pointer!");
							AssertMsg(isSectionFilled, "Tried to double free section in Paged Heap. This is a memory management bug");
							if (allocSize != 0)
							{
								//TODO: Handle scenarios where the alignment offset or bad-fit scenarios caused the section to be slightly larger than the requested allocation size
								//NOTE: Right now we allow for slop in both the alignment offset and scenarios where the section that was used was only slighly larger than needed
								//      and a second section couldn't be created because there wasn't even enough room for a HeapAllocPrefix_t
								u64 allowedSlop = OffsetToAlign(afterPrefixPntr, AllocAlignment_Max) + sizeof(HeapAllocPrefix_t);
								AssertMsg(AbsDiffU64(allocSize, afterPrefixSize) <= allowedSlop, "Given size did not match actual allocation size in Paged Heap during FreeMem. This is a memory management bug");
							}
							
							result = true;
							foundAlloc = true;
							if (oldSizeOut != nullptr) { *oldSizeOut = afterPrefixSize; }
							
							// +==============================+
							// |   Free Paged Heap Section    |
							// +==============================+
							prefixPntr->size = PackAllocPrefixSize(false, sectionSize);
							AssertMsg(pageHeader->used >= afterPrefixSize, "Paged Heap used tracker was corrupted. Reached 0 too soon!");
							AssertMsg(arena->used >= afterPrefixSize, "Paged Heap used tracker was corrupted. Reached 0 too soon!");
							pageHeader->used -= afterPrefixSize;
							arena->used -= afterPrefixSize;
							AssertMsg(arena->numAllocations > 0, "Paged Heap numAllocations was corrupted. Reached 0 too soon!");
							arena->numAllocations--;
							
							// +==============================+
							// | Merge Sections After Freeing |
							// +==============================+
							if (allocOffset + sectionSize < pageHeader->size)
							{
								Assert(allocOffset + sectionSize + sizeof(HeapAllocPrefix_t) <= pageHeader->size);
								HeapAllocPrefix_t* nextPrefixPntr = (HeapAllocPrefix_t*)(allocBytePntr + sectionSize);
								if (!IsAllocPrefixFilled(nextPrefixPntr->size))
								{
									// Merge the next section with this one by making this section bigger
									sectionSize += UnpackAllocPrefixSize(nextPrefixPntr->size);
									prefixPntr->size = PackAllocPrefixSize(false, sectionSize);
									AssertMsg(pageHeader->used >= sizeof(HeapAllocPrefix_t), "Paged Heap page->used tracker was corrupted. Reached 0 too soon.");
									AssertMsg(arena->used >= sizeof(HeapAllocPrefix_t), "Paged Heap used tracker was corrupted. Reached 0 too soon.");
									pageHeader->used -= sizeof(HeapAllocPrefix_t);
									arena->used -= sizeof(HeapAllocPrefix_t);
								}
							}
							if (prevPrefixPntr != nullptr && !IsAllocPrefixFilled(prevPrefixPntr->size))
							{
								// Merge the previous section with this one by making it's sectionSize bigger
								prevPrefixPntr->size = PackAllocPrefixSize(false, UnpackAllocPrefixSize(prevPrefixPntr->size) + sectionSize);
								AssertMsg(pageHeader->used >= sizeof(HeapAllocPrefix_t), "Paged Heap page->used tracker was corrupted. Reached 0 too soon.");
								AssertMsg(arena->used >= sizeof(HeapAllocPrefix_t), "Paged Heap used tracker was corrupted. Reached 0 too soon.");
								pageHeader->used -= sizeof(HeapAllocPrefix_t);
								arena->used -= sizeof(HeapAllocPrefix_t);
							}
							
							// +==============================+
							// |       Free Empty Page        |
							// +==============================+
							if (pageHeader->used <= sizeof(HeapAllocPrefix_t) && IsFlagSet(arena->flags, MemArenaFlag_AutoFreePages) && pageIndex > 0)
							{
								prevPageHeader->next = pageHeader->next;
								arena->size -= pageHeader->size;
								arena->used -= sizeof(HeapAllocPrefix_t);
								if (arena->freeFunc != nullptr)
								{
									arena->freeFunc(pageHeader);
								}
								else if (arena->sourceArena != nullptr)
								{
									FreeMem(arena->sourceArena, pageHeader, sizeof(HeapPageHeader_t) + pageHeader->size);
								}
								arena->numPages--;
							}
							
							break;
						}
						
						prevPrefixPntr = prefixPntr;
						allocOffset += sectionSize;
						allocBytePntr += sectionSize;
						sectionIndex++;
					}
					AssertMsg(foundAlloc, "We have a bug in our freeing walk. Couldn't find section that contained the pntr in this page!");
					break;
				}
				
				prevPageHeader = pageHeader;
				pageHeader = pageHeader->next;
				pageIndex++;
			}
			AssertMsg(foundAlloc, "Tried to free pntr that isn't in any of the pages of this arena!");
		} break;
		
		// +==============================+
		// |   MemArenaType_MarkedStack   |
		// +==============================+
		case MemArenaType_MarkedStack:
		{
			AssertMsg(false, "FreeMem is not supported for a MarkedStack. Are you trying to free memory allocated on the TempArena");
		} break;
		
		// +==============================+
		// |     MemArenaType_Buffer      |
		// +==============================+
		case MemArenaType_Buffer:
		{
			NotNull(arena->mainPntr);
			u8* basePntr = (u8*)arena->mainPntr;
			Assert(IsPntrInsideRange(allocPntr, arena->mainPntr, arena->size));
			if (allocSize == 0 && allocPntr == arena->mainPntr && arena->numAllocations == 1) //the one scenario where allocSize can be assumed
			{
				allocSize = arena->used;
			}
			AssertMsg(allocSize != 0, "Tried to deallocate from Buffer type arena without specifying allocSize and old size could not be assumed");
			AssertMsg(((u8*)allocPntr + allocSize) == (basePntr + arena->used), "Tried to deallocate out of order in Buffer type arena. Ordered frees only please!");
			arena->used -= allocSize;
			Decrement(arena->numAllocations);
			result = true;
		} break;
		
		// +==============================+
		// |    Unsupported Arena Type    |
		// +==============================+
		default:
		{
			PrintLine_E("Unsupported arena type in FreeMem: %u (size: %llu, used: %llu)", arena->type, arena->size, arena->used);
			AssertMsg(false, "Unsupported arena type in FreeMem. Maybe the arena is corrupted?");
		} break;
	}
	
	if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput)) { PrintLine_D("After FreeMem (arena is %llu/%llu)", arena->used, arena->size); }
	return result;
}

// +--------------------------------------------------------------+
// |                     Reallocate Function                      |
// +--------------------------------------------------------------+
void* ReallocMem(MemArena_t* arena, void* allocPntr, u64 newSize, u64 oldSize, AllocAlignment_t alignOverride, bool ignoreNullptr, u64* oldSizeOut)
{
	NotNull(arena);
	AssertMsg(arena->type != MemArenaType_None, "Tried to realloc from uninitialized arena");
	Assert(ignoreNullptr || allocPntr != nullptr);
	
	if (IsFlagSet(arena->flags, MemArenaFlag_BreakOnRealloc) && (arena->debugBreakThreshold == 0 || newSize >= arena->debugBreakThreshold || oldSize >= arena->debugBreakThreshold))
	{
		MyDebugBreak();
	}
	if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput))
	{
		PrintLine_D("ReallocMem on %s 0x%08X (%llu bytes to %llu bytes) (arena is %llu/%llu)", GetMemArenaTypeStr(arena->type), (u32)allocPntr, oldSize, newSize, arena->used, arena->size);
		// if (allocPntr != nullptr)
		// {
		// 	PrintLine_D("First bytes before: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X",
		// 		((u8*)allocPntr)[0],
		// 		((u8*)allocPntr)[1],
		// 		((u8*)allocPntr)[2],
		// 		((u8*)allocPntr)[3],
		// 		((u8*)allocPntr)[4],
		// 		((u8*)allocPntr)[5],
		// 		((u8*)allocPntr)[6],
		// 		((u8*)allocPntr)[7]
		// 	);
		// }
	}
	
	AllocAlignment_t alignment = (alignOverride != AllocAlignment_None) ? alignOverride : arena->alignment;
	if (newSize == oldSize && (allocPntr != nullptr || oldSize != 0) && IsAlignedTo(allocPntr, alignment)) //not resizing, just keep the memory where it's at
	{
		return allocPntr;
	}
	if (newSize == 0) //Resizing to 0 is basically freeing
	{
		bool freeSuccess = FreeMem(arena, allocPntr, oldSize, ignoreNullptr);
		AssertMsg(freeSuccess, "Failed attempt to free memory in arena when Realloc'd to 0 bytes");
		return nullptr;
	}
	
	bool knownOldSize = (oldSize != 0 || allocPntr == nullptr);
	bool isRealigning = !IsAlignedTo(allocPntr, alignment);
	bool increasingSize = (knownOldSize && newSize > oldSize);
	bool decreasingSize = (knownOldSize && newSize < oldSize);
	u64 sizeChangeAmount = (newSize >= oldSize) ? (newSize - oldSize) : (oldSize - newSize);
	
	UNUSED(isRealigning); //TODO: Remove me!
	u8* result = nullptr;
	switch (arena->type)
	{
		// +======================================+
		// | Temporary AllocMem+FreeMem Solution  |
		// +======================================+
		case MemArenaType_Redirect:
		case MemArenaType_FixedHeap:
		case MemArenaType_Buffer:
		{
			result = (u8*)AllocMem(arena, newSize, alignOverride);
			if (result == nullptr)
			{
				if (allocPntr != nullptr)
				{
					u64 reportedOldSize = oldSize;
					bool freeSuccess = FreeMem(arena, allocPntr, oldSize, ignoreNullptr, &reportedOldSize);
					AssertMsg(freeSuccess, "Failed to FreeMem after a failed AllocMem in ReallocMem! Something is probably wrong with this arena");
					if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput) && oldSize != reportedOldSize)
					{
						PrintLine_D("Allocation was actually %llu bytes", reportedOldSize);
					}
					Assert(oldSize == 0 || oldSize == reportedOldSize);
					oldSize = reportedOldSize;
					increasingSize = (newSize > oldSize);
					decreasingSize = (newSize < oldSize);
					sizeChangeAmount = (newSize >= oldSize) ? (newSize - oldSize) : (oldSize - newSize);
				}
				if (oldSizeOut != nullptr) { *oldSizeOut = oldSize; }
				break;
			}
			if (allocPntr != nullptr)
			{
				if (oldSize == 0) { oldSize = GetAllocSize(arena, allocPntr); }
				Assert(oldSize != 0);
				memcpy(result, allocPntr, oldSize);
			}
			{
				u64 reportedOldSize = oldSize;
				bool freeSuccess = FreeMem(arena, allocPntr, oldSize, ignoreNullptr, &reportedOldSize);
				AssertMsg(freeSuccess, "Failed to FreeMem in ReallocMem! Does this arena type support freeing memory?");
				if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput) && oldSize != reportedOldSize)
				{
					PrintLine_D("Allocation was actually %llu bytes", reportedOldSize);
				}
				Assert(oldSize == 0 || oldSize == reportedOldSize);
				oldSize = reportedOldSize;
				increasingSize = (newSize > oldSize);
				decreasingSize = (newSize < oldSize);
				sizeChangeAmount = (newSize >= oldSize) ? (newSize - oldSize) : (oldSize - newSize);
				if (oldSizeOut != nullptr) { *oldSizeOut = oldSize; }
			}
		} break;
		
		// +==============================+
		// |    MemArenaType_Redirect     |
		// +==============================+
		// case MemArenaType_Redirect:
		// {
		// 	//TODO: Implement me, and remove from above!
		// } break;
		
		// +==============================+
		// |      MemArenaType_Alias      |
		// +==============================+
		case MemArenaType_Alias:
		{
			NotNull(arena->sourceArena);
			u64 reportedOldSize = oldSize;
			result = (u8*)ReallocMem(arena->sourceArena, allocPntr, newSize, oldSize, alignment, ignoreNullptr, &reportedOldSize);
			if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput) && oldSize != reportedOldSize)
			{
				PrintLine_D("Allocation was actually %llu bytes", reportedOldSize);
			}
			Assert(oldSize == 0 || oldSize == reportedOldSize);
			oldSize = reportedOldSize;
			increasingSize = (newSize > oldSize);
			decreasingSize = (newSize < oldSize);
			sizeChangeAmount = (newSize >= oldSize) ? (newSize - oldSize) : (oldSize - newSize);
			if (oldSizeOut != nullptr) { *oldSizeOut = reportedOldSize; }
			if (result == nullptr)
			{
				Decrement(arena->numAllocations);
				arena->size = arena->sourceArena->size;
				arena->used = arena->sourceArena->used;
				break;
			}
			arena->size = arena->sourceArena->size;
			arena->used = arena->sourceArena->used;
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
				if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
			}
		} break;
		
		// +==============================+
		// |     MemArenaType_StdHeap     |
		// +==============================+
		case MemArenaType_StdHeap:
		{
			if (allocPntr == arena->otherPntr)
			{
				u64 reportedOldSize = (u64)(currentHeapHeadAddress - (u32)arena->otherPntr);
				if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput) && oldSize != reportedOldSize)
				{
					PrintLine_D("Allocation was actually %llu bytes", reportedOldSize);
				}
				Assert(oldSize == 0 || oldSize == reportedOldSize);
				
				u64 difference = newSize - reportedOldSize;
				if (difference == 0) { result = (u8*)allocPntr; break; }
				
				void* newSpace = AllocMem(arena, difference);
				arena->otherPntr = allocPntr; //undo what was done by AllocMem. Treat these two allocations as one further expandable one
				if (newSpace == nullptr) { break; }
				
				//The allocation can stay right where it's at
				result = (u8*)allocPntr;
			}
			else
			{
				Assert(oldSize != 0);
				PrintLine_W("WARNING: ReallocMem on StdHeap was not able to grow (0x%08X != 0x%08X). We are wasting %llu bytes of memory!", (u32)allocPntr, (u32)arena->otherPntr, oldSize);
				void* newSpace = AllocMem(arena, newSize);
				if (allocPntr != nullptr && oldSize > 0)
				{
					memcpy(newSpace, allocPntr, oldSize);
				}
				//no need to call FreeMem because we can't free in web assembly memory model
				result = (u8*)newSpace;
				arena->otherPntr = newSpace;
			}
		} break;
		
		// +==============================+
		// |    MemArenaType_FixedHeap    |
		// +==============================+
		// case MemArenaType_FixedHeap:
		// {
		// 	//TODO: Implement me, and remove from above!
		// } break;
		
		// +==============================+
		// |    MemArenaType_PagedHeap    |
		// +==============================+
		case MemArenaType_PagedHeap:
		{
			if (allocPntr == nullptr && ignoreNullptr)
			{
				result = (u8*)AllocMem(arena, newSize);
			}
			else
			{
				Assert(allocPntr != nullptr);
				HeapPageHeader_t* prevPageHeader = nullptr;
				HeapPageHeader_t* pageHeader = (HeapPageHeader_t*)arena->headerPntr;
				u64 pageIndex = 0;
				bool foundAlloc = false;
				while (pageHeader != nullptr && !foundAlloc)
				{
					u8* pageBase = (u8*)(pageHeader + 1);
					if (allocPntr >= pageBase && allocPntr < pageBase + pageHeader->size)
					{
						u64 allocOffset = 0;
						u8* allocBytePntr = pageBase;
						u64 sectionIndex = 0;
						HeapAllocPrefix_t* prevPrefixPntr = nullptr;
						while (allocOffset < pageHeader->size)
						{
							HeapAllocPrefix_t* prefixPntr = (HeapAllocPrefix_t*)allocBytePntr;
							u8* afterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
							bool isSectionFilled = IsAllocPrefixFilled(prefixPntr->size);
							u64 sectionSize = UnpackAllocPrefixSize(prefixPntr->size);
							AssertMsg(sectionSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Paged Heap");
							u64 afterPrefixSize = sectionSize - sizeof(HeapAllocPrefix_t);
							
							if ((u8*)allocPntr >= allocBytePntr && (u8*)allocPntr < allocBytePntr + sectionSize)
							{
								AssertMsg((u8*)allocPntr >= afterPrefixPntr, "Tried to realloc a pointer that pointed into a Paged Heap header. This is a corrupt pointer!");
								//TODO: Check if the allocation was actually aligned. Be more strict if it wasn't aligned
								AssertMsg((u8*)allocPntr <= afterPrefixPntr + OffsetToAlign(afterPrefixPntr, AllocAlignment_Max), "Tried to realloc a pointer that pointed to the middle of a Paged Heap section. This is a corrupt pointer!");
								AssertMsg(isSectionFilled, "Tried to double realloc section in Paged Heap. This is a memory management bug");
								if (oldSize != 0)
								{
									//TODO: Handle scenarios where the alignment offset or bad-fit scenarios caused the section to be slightly larger than the requested allocation size
									//NOTE: Right now we allow for slop in both the alignment offset and scenarios where the section that was used was only slighly larger than needed
									//      and a second section couldn't be created because there wasn't even enough room for a HeapAllocPrefix_t
									u64 allowedSlop = OffsetToAlign(afterPrefixPntr, AllocAlignment_Max) + sizeof(HeapAllocPrefix_t);
									AssertMsg(AbsDiffU64(oldSize, afterPrefixSize) <= allowedSlop, "Given size did not match actual allocation size in Paged Heap during ReallocMem. This is a memory management bug");
								}
								
								bool reallocSucceeded = false;
								//TODO: Uncomment this when we are ready to implement it!
								/*if (allocOffset + sectionSize + sizeof(HeapAllocPrefix_t) <= pageHeader->size)
								{
									HeapAllocPrefix_t* nextPrefix = (HeapAllocPrefix_t*)(allocBytePntr + sectionSize);
									//TODO: Figure out if we can grow into the following section!
								}
								else */if (allocOffset == 0 && arena->sourceArena != nullptr)
								{
									// This section takes the entire page. Let's try ReallocMem out of our sourceArena
									Assert(sectionSize == pageHeader->size);
									u64 numNewBytes = newSize - pageHeader->size;
									u64 newPageSize = sizeof(HeapAllocPrefix_t) + newSize;
									if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput)) { PrintLine_D("Reallocating page %llu/%llu from %llu->%llu bytes", pageIndex+1, arena->numPages, pageHeader->size, newPageSize); }
									u8* newPageSpace = (u8*)ReallocMem(arena->sourceArena, pageHeader, sizeof(HeapPageHeader_t) + newPageSize, sizeof(HeapPageHeader_t) + pageHeader->size);
									if (newPageSpace == (u8*)pageHeader)
									{
										if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput)) { PrintLine_D("Page expanded without moving at 0x%08X", (u32)newPageSpace); }
										pageHeader->size = newPageSize;
										prefixPntr->size = PackAllocPrefixSize(true, newSize);
										result = (u8*)allocPntr;
										reallocSucceeded = true;
										arena->size += numNewBytes;
										arena->used += numNewBytes;
									}
									else if (newPageSpace != nullptr)
									{
										if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput)) { PrintLine_D("Page moved from 0x%08X -> 0x%08X", (u32)pageHeader, (u32)newPageSpace); }
										pageHeader = (HeapPageHeader_t*)newPageSpace;
										if (prevPageHeader != nullptr)
										{
											prevPageHeader->next = pageHeader;
										}
										else
										{
											Assert(pageIndex == 0);
											arena->headerPntr = pageHeader;
										}
										prefixPntr = (HeapAllocPrefix_t*)(newPageSpace + sizeof(HeapPageHeader_t));
										afterPrefixPntr = (u8*)(prefixPntr + 1);
										prefixPntr->size = PackAllocPrefixSize(true, newSize);
										result = afterPrefixPntr;
										reallocSucceeded = true;
										arena->size += numNewBytes;
										arena->used += numNewBytes;
									}
									else
									{
										reallocSucceeded = false;
									}
								}
								
								if (!reallocSucceeded)
								{
									//Fallback to our simple method of AllocMem, memcpy, FreeMem
									if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput)) { WriteLine_D("Realloc failed. Default to AllocMem,memcpy,FreeMem method!"); }
									result = (u8*)AllocMem(arena, newSize, alignOverride);
									if (result == nullptr)
									{
										if (allocPntr != nullptr)
										{
											u64 reportedOldSize = oldSize;
											bool freeSuccess = FreeMem(arena, allocPntr, oldSize, ignoreNullptr, &reportedOldSize);
											AssertMsg(freeSuccess, "Failed to FreeMem after a failed AllocMem in ReallocMem! Something is probably wrong with this arena");
											if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput) && oldSize != reportedOldSize)
											{
												PrintLine_D("Allocation was actually %llu bytes", reportedOldSize);
											}
											Assert(oldSize == 0 || oldSize == reportedOldSize);
											oldSize = reportedOldSize;
											increasingSize = (newSize > oldSize);
											decreasingSize = (newSize < oldSize);
											sizeChangeAmount = (newSize >= oldSize) ? (newSize - oldSize) : (oldSize - newSize);
										}
										if (oldSizeOut != nullptr) { *oldSizeOut = oldSize; }
										break;
									}
									if (allocPntr != nullptr)
									{
										if (oldSize == 0) { oldSize = GetAllocSize(arena, allocPntr); }
										Assert(oldSize != 0);
										memcpy(result, allocPntr, oldSize);
									}
									{
										u64 reportedOldSize = oldSize;
										bool freeSuccess = FreeMem(arena, allocPntr, oldSize, ignoreNullptr, &reportedOldSize);
										AssertMsg(freeSuccess, "Failed to FreeMem in ReallocMem! Does this arena type support freeing memory?");
										if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput) && oldSize != reportedOldSize)
										{
											PrintLine_D("Allocation was actually %llu bytes", reportedOldSize);
										}
										Assert(oldSize == 0 || oldSize == reportedOldSize);
										oldSize = reportedOldSize;
										increasingSize = (newSize > oldSize);
										decreasingSize = (newSize < oldSize);
										sizeChangeAmount = (newSize >= oldSize) ? (newSize - oldSize) : (oldSize - newSize);
										if (oldSizeOut != nullptr) { *oldSizeOut = oldSize; }
									}
								}
								
								foundAlloc = true;
								break;
							}
							
							prevPrefixPntr = prefixPntr;
							allocOffset += sectionSize;
							allocBytePntr += sectionSize;
							sectionIndex++;
						}
						AssertMsg(foundAlloc, "We have a bug in our reallocing walk. Couldn't find section that contained the pntr in this page!");
						break;
					}
					
					prevPageHeader = pageHeader;
					pageHeader = pageHeader->next;
					pageIndex++;
				}
				AssertMsg(foundAlloc, "Tried to realloc pntr that isn't in any of the pages of this arena!");
			}
		} break;
		
		// +==============================+
		// |   MemArenaType_MarkedStack   |
		// +==============================+
		// case MemArenaType_MarkedStack:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +==============================+
		// |     MemArenaType_Buffer      |
		// +==============================+
		// case MemArenaType_Buffer:
		// {
		// 	//TODO: Implement me, and remove from above!
		// } break;
		
		// +==============================+
		// |    Unsupported Arena Type    |
		// +==============================+
		default:
		{
			PrintLine_E("Unsupported arena type in ReallocMem: %u (size: %llu, used: %llu)", arena->type, arena->size, arena->used);
			AssertMsg(false, "Unsupported arena type in ReallocMem. Maybe the arena is corrupted?");
		} break;
	}
	
	if (IsFlagSet(arena->flags, MemArenaFlag_DebugOutput))
	{
		// if (result != nullptr)
		// {
		// 	PrintLine_D("First bytes after: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X",
		// 		((u8*)result)[0],
		// 		((u8*)result)[1],
		// 		((u8*)result)[2],
		// 		((u8*)result)[3],
		// 		((u8*)result)[4],
		// 		((u8*)result)[5],
		// 		((u8*)result)[6],
		// 		((u8*)result)[7]
		// 	);
		// }
		PrintLine_D("ReallocMem 0x%08X->0x%08X (arena is %llu/%llu)", (u32)allocPntr, (u32)result, arena->used, arena->size);
	}
	return result;
}

// +--------------------------------------------------------------+
// |                 Push And Pop Mark Functions                  |
// +--------------------------------------------------------------+
void PushMemMark(MemArena_t* arena)
{
	NotNull(arena);
	Assert(arena->type == MemArenaType_MarkedStack);
	NotNull(arena->headerPntr);
	NotNull(arena->otherPntr);
	
	MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
	Assert(stackHeader->maxNumMarks > 0);
	Assert(stackHeader->numMarks <= stackHeader->maxNumMarks);
	if (stackHeader->numMarks == stackHeader->maxNumMarks)
	{
		PrintLine_E("Tried to push mark %llu onto marked stack which only has support for %llu marks", stackHeader->numMarks+1, stackHeader->maxNumMarks);
		AssertMsg(false, "Too many marks pushed onto a MarkedStack");
		return;
	}
	
	u64* marksPntr = (u64*)arena->otherPntr;
	marksPntr[stackHeader->numMarks] = arena->used;
	stackHeader->numMarks++;
	
	if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
	{
		if (stackHeader->highMarkCount < stackHeader->numMarks) { stackHeader->highMarkCount = stackHeader->numMarks; }
	}
}

void PopMemMark(MemArena_t* arena)
{
	NotNull(arena);
	Assert(arena->type == MemArenaType_MarkedStack);
	NotNull(arena->headerPntr);
	NotNull(arena->otherPntr);
	
	MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
	Assert(stackHeader->maxNumMarks > 0);
	Assert(stackHeader->numMarks <= stackHeader->maxNumMarks);
	if (stackHeader->numMarks == 0)
	{
		WriteLine_E("Tried to pop stack mark when no marks were left");
		AssertMsg(false, "Tried to pop too many times on a MarkedStack");
		return;
	}
	
	u64* marksPntr = (u64*)arena->otherPntr;
	Assert(marksPntr[stackHeader->numMarks-1] <= arena->used);
	Assert(marksPntr[stackHeader->numMarks-1] <= arena->size);
	arena->used = marksPntr[stackHeader->numMarks-1];
	stackHeader->numMarks--;
}

u64 GetNumMarks(MemArena_t* arena)
{
	NotNull(arena);
	Assert(arena->type == MemArenaType_MarkedStack);
	NotNull(arena->headerPntr);
	NotNull(arena->otherPntr);
	
	MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
	Assert(stackHeader->maxNumMarks > 0);
	Assert(stackHeader->numMarks <= stackHeader->maxNumMarks);
	return stackHeader->numMarks;
}
