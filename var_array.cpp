/*
File:   var_array.cpp
Author: Taylor Robbins
Date:   12\01\2022
Description: 
	** The VarArray_t structure acts like a stretchy buffer, resizing as needed to hold as many fixed size elements as needed
*/

#include "common.h"
#include "var_array.h"

#include "debug.h"
#include "intrinsics.h"

// +--------------------------------------------------------------+
// |                       Create and Free                        |
// +--------------------------------------------------------------+
void FreeVarArray(VarArray_t* array)
{
	NotNull(array);
	if (array->items != nullptr)
	{
		NotNull(array->allocArena);
		FreeMem(array->allocArena, array->items, array->itemSize * array->allocLength);
	}
	if (!IsEmptyStr(array->name))
	{
		NotNull(array->allocArena);
		FreeString(array->allocArena, &array->name);
	}
	ClearPointer(array);
}

void CreateVarArray(VarArray_t* array, MemArena_t* memArena, u32 itemSize, u32 initialRequiredCapacity, bool exponentialChunkSize, u32 allocChunkSize)
{
	NotNull(array);
	NotNull(memArena);
	Assert(itemSize > 0);
	Assert(allocChunkSize > 0);
	ClearPointer(array);
	array->itemSize = itemSize;
	array->allocArena = memArena;
	array->allocChunkSize = allocChunkSize;
	array->exponentialChunkSize = exponentialChunkSize;
	array->length = 0;
	
	if (exponentialChunkSize)
	{
		array->allocLength = allocChunkSize;
		u32 sanityCounter = 0;
		while (array->allocLength < initialRequiredCapacity)
		{
			Assert(array->allocLength <= UINT32_MAX/2);
			array->allocLength *= 2;
			sanityCounter++;
			Assert(sanityCounter <= initialRequiredCapacity); //Infinite loop sanity check
		}
	}
	else
	{
		array->allocLength = RoundUpToU32(initialRequiredCapacity, allocChunkSize);
	}
	if (array->allocLength > 0)
	{
		array->items = AllocMem(memArena, array->allocLength * itemSize);
		if (array->items == nullptr)
		{
			AssertMsg(false, "Initial Allocation inside CreateVarArray failed!");
			ClearPointer(array);
			return;
		}
	}
}

//Returns if the array actually expanded
bool VarArrayExpand(VarArray_t* array, u32 capacityRequired)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	if (array->allocLength >= capacityRequired) { return false; }
	
	u32 newLength = 0;
	if (array->exponentialChunkSize)
	{
		Assert(array->allocLength > 0);
		newLength = array->allocLength;
		u32 sanityCounter = 0;
		while (newLength < capacityRequired)
		{
			Assert(newLength <= UINT32_MAX/2);
			newLength *= 2;
			sanityCounter++;
			Assert(sanityCounter <= capacityRequired); //Infinite loop sanity check
		}
	}
	else
	{
		newLength = RoundUpToU32(capacityRequired, array->allocChunkSize);
	}
	Assert(newLength >= capacityRequired);
	Assert(newLength <= (UINT32_MAX / array->itemSize)); //u32 overflow would be weird
	
	void* newSpace = AllocMem(array->allocArena, newLength * array->itemSize);
	if (newSpace == nullptr)
	{
		PrintLine_E("Failed to expand variable array %s to %u items at %u bytes each", (array->name.pntr != nullptr) ? array->name.pntr : "[unnamed]", newLength, array->itemSize);
		AssertMsg(false, "Failed to expand variable array");
		return false;
	}
	
	if (array->length > 0)
	{
		memcpy(newSpace, array->items, array->length * array->itemSize);
	}
	if (array->items != nullptr)
	{
		FreeMem(array->allocArena, array->items, array->allocLength * array->itemSize);
	}
	
	array->items = newSpace;
	array->allocLength = newLength;
	array->wasExpanded = true;
	IncrementU32(array->numExpansions);
	return true;
}


void VarArrayClear(VarArray_t* array, bool deallocate)
{
	NotNull(array);
	if (deallocate && array->allocLength > 0)
	{
		NotNull(array->allocArena);
		FreeMem(array->allocArena, array->items, array->allocLength * array->itemSize);
		array->items = nullptr;
		array->allocLength = 0;
	}
	array->length = 0;
}


// +--------------------------------------------------------------+
// |                             Get                              |
// +--------------------------------------------------------------+
void* VarArrayGet_(VarArray_t* array, u32 index, u32 itemSize, bool assertOnFailure)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayGet. Make sure you're accessing the VarArray with the correct type!");
	
	if (index >= array->length)
	{
		if (assertOnFailure)
		{
			PrintLine_E("Tried to get item %u/%u in VarArray %s of itemSize %u", index, array->length, (array->name.pntr != nullptr) ? array->name.pntr : "[unnamed]", array->itemSize);
			AssertMsg(false, "VarArrayGet out of bounds!");
		}
		return nullptr;
	}
	
	NotNull(array->items);
	void* result = (((u8*)array->items) + (index * array->itemSize));
	return result;
}
const void* VarArrayGet_(const VarArray_t* array, u32 index, u32 itemSize, bool assertOnFailure) //const-variant
{
	return (const void*)VarArrayGet_((VarArray_t*)array, index, itemSize, assertOnFailure);
}

// +--------------------------------------------------------------+
// |                             Add                              |
// +--------------------------------------------------------------+
//NOTE: This always asserts on failure to allocate since VarArrayExpand defaults to asserting
void* VarArrayAdd_(VarArray_t* array, u32 itemSize)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayAdd. Make sure you're accessing the VarArray with the correct type!");
	
	VarArrayExpand(array, array->length+1);
	Assert(array->allocLength >= array->length+1);
	
	void* result = (((u8*)array->items) + (array->itemSize * array->length));
	#if DEBUG_BUILD
	memset(result, 0xCC, array->itemSize);
	#endif
	array->length++;
	
	return result;
}

// +--------------------------------------------------------------+
// |                            Insert                            |
// +--------------------------------------------------------------+
//NOTE: This always asserts on failure to allocate since VarArrayExpand defaults to asserting
void* VarArrayInsert_(VarArray_t* array, u32 index, u32 itemSize)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayInsert. Make sure you're accessing the VarArray with the correct type!");
	Assert(index <= array->length);
	if (index == array->length) { return VarArrayAdd_(array, itemSize); }
	
	VarArrayExpand(array, array->length+1);
	Assert(array->allocLength >= array->length+1);
	
	//Move all items above the index up by one slot
	u8* itemPntr = (u8*)VarArrayGet_(array, array->length-1, itemSize, true);
	for (u32 iIndex = array->length; iIndex > index; iIndex--)
	{
		memcpy(itemPntr + array->itemSize, itemPntr, array->itemSize);
		itemPntr -= array->itemSize;
	}
	
	void* result = (((u8*)array->items) + (array->itemSize * index));
	#if DEBUG_BUILD
	memset(result, 0xCC, array->itemSize);
	#endif
	array->length++;
	
	return result;
}

// +--------------------------------------------------------------+
// |                            Remove                            |
// +--------------------------------------------------------------+
void VarArrayRemove_(VarArray_t* array, u32 index, u32 itemSize)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayRemove. Make sure you're accessing the VarArray with the correct type!");
	
	if (index >= array->length)
	{
		PrintLine_E("Tried to remove item [%u]/%u in VarArray %s of itemSize %u", index, array->length, (array->name.pntr != nullptr) ? array->name.pntr : "[unnamed]", array->itemSize);
		AssertMsg(false, "VarArrayRemove out of bounds!");
		return;
	}
	
	//move all items above index down by one
	for (u32 iIndex = index; iIndex+1 < array->length; iIndex++)
	{
		memcpy(VarArrayGet_(array, iIndex, itemSize, true), VarArrayGet_(array, iIndex+1, itemSize, true), array->itemSize);
	}
	array->length--;
}

