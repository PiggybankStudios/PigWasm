/*
File:   var_array.h
Author: Taylor Robbins
Date:   12\01\2022
*/

#ifndef _VAR_ARRAY_H
#define _VAR_ARRAY_H

// +--------------------------------------------------------------+
// |                          Structure                           |
// +--------------------------------------------------------------+
struct VarArray_t
{
	MemArena_t* allocArena;
	u32 itemSize;
	u32 allocChunkSize;
	bool exponentialChunkSize;
	MyStr_t name;
	
	u32 length;
	u32 allocLength;
	void* items;
	bool wasExpanded; //feel free to clear this in usage code
	u32 numExpansions;
};

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
void FreeVarArray(VarArray_t* array);
void CreateVarArray(VarArray_t* array, MemArena_t* memArena, u32 itemSize, u32 initialRequiredCapacity = 0, bool exponentialChunkSize = true, u32 allocChunkSize = 8);

bool VarArrayExpand(VarArray_t* array, u32 capacityRequired);
void VarArrayClear(VarArray_t* array, bool deallocate = false);

void* VarArrayGet_(VarArray_t* array, u32 index, u32 itemSize, bool assertOnFailure);
const void* VarArrayGet_(const VarArray_t* array, u32 index, u32 itemSize, bool assertOnFailure); //const-variant
#define VarArrayGetHard(array, index, type) ((type*)VarArrayGet_((array), (index), sizeof(type), true))
#define VarArrayGetSoft(array, index, type) ((type*)VarArrayGet_((array), (index), sizeof(type), false))
#define VarArrayGet(array, index, type)     VarArrayGetHard((array), (index), type)

#define VarArrayGetFirstHard(array, type) VarArrayGetHard((array), 0, type)
#define VarArrayGetFirstSoft(array, type) (((array)->length > 0) ? VarArrayGetSoft((array), 0, type) : nullptr)
#define VarArrayGetFirst(array, type)     VarArrayGetFirstHard((array), type) 

#define VarArrayGetLastHard(array, type) VarArrayGetHard((array), (array)->length - 1, type)
#define VarArrayGetLastSoft(array, type) (((array)->length > 0) ? VarArrayGetSoft((array), (array)->length - 1, type) : nullptr)
#define VarArrayGetLast(array, type)     VarArrayGetLastHard((array), type) 

#define VarArrayLoop(arrayPntr, indexName)               for (u32 indexName = 0; indexName < (arrayPntr)->length; indexName++)
#define VarArrayLoopGet(type, varName, arrayPntr, index) type* varName = VarArrayGet((arrayPntr), (index), type)

void* VarArrayAdd_(VarArray_t* array, u32 itemSize);
#define VarArrayAdd(array, type)  (type*)VarArrayAdd_((array), sizeof(type))
#define VarArrayPush(array, type) VarArrayAdd((array), (type))

void* VarArrayInsert_(VarArray_t* array, u32 index, u32 itemSize);
#define VarArrayInsert(array, index, type) (type*)VarArrayInsert_((array), (index), sizeof(type))
#define VarArrayPushFront(array, type)     (type*)VarArrayInsert_((array), 0, sizeof(type))

void VarArrayRemove_(VarArray_t* array, u32 index, u32 itemSize);
#define VarArrayRemove(array, index, type) VarArrayRemove_((array), (index), sizeof(type))
#define VarArrayPop(array, type)           VarArrayRemove_((array), (array)->length - 1, sizeof(type))
#define VarArrayPopFront(array, type)      VarArrayRemove_((array), 0, sizeof(type))

#endif //  _VAR_ARRAY_H
