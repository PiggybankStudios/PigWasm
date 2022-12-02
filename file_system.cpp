/*
File:   file_system.cpp
Author: Taylor Robbins
Date:   12\01\2022
Description: 
	** Holds some functionst that handle calling out to javascript to request files
	** and wait for the their asynchronous download before giving callbacks to the
	** code that requested the file
*/

#include "common.h"
#include "file_system.h"

#include "intrinsics.h"
#include "memory.h"
#include "var_array.h"

void FreeFileRequest(FileRequest_t* request)
{
	NotNull(request);
	if (!IsEmptyStr(request->filePath))
	{
		FreeString(request->allocArena, &request->filePath);
	}
	ClearPointer(request);
}

void InitializeFileSystemManager(FileSystemManager_t* manager)
{
	NotNull(manager);
	ClearPointer(manager);
	manager->nextFileRequestId = 1;
	CreateVarArray(&manager->activeFileRequests, mainHeap, sizeof(FileRequest_t));
}

FileRequest_t* GetFileRequestById(FileSystemManager_t* manager, u32 requestId, u32* requestIndexOut)
{
	VarArrayLoop(&manager->activeFileRequests, rIndex)
	{
		VarArrayLoopGet(FileRequest_t, request, &manager->activeFileRequests, rIndex);
		if (request->id == requestId)
		{
			if (requestIndexOut != nullptr) { *requestIndexOut = rIndex; }
			return request;
		}
	}
	return nullptr;
}

void FileSystemHandleFileReady(FileSystemManager_t* manager, u32 requestId, u32 fileSize, const void* fileData)
{
	u32 requestIndex = 0;
	FileRequest_t* request = GetFileRequestById(manager, requestId, &requestIndex);
	if (request != nullptr)
	{
		if (request->callback != nullptr)
		{
			request->callback(request->filePath, fileSize, fileData, request->callbackContext);
		}
		VarArrayRemove(&manager->activeFileRequests, requestIndex, FileRequest_t);
	}
	else
	{
		PrintLine_E("Unknown request for file completed. ID %u (we have %u active requests)", requestId, manager->activeFileRequests.length);
	}
}

void FileSystemRequestAsync(FileSystemManager_t* manager, MyStr_t filePath, FileReadyCallback_f* callback, void* callbackContext)
{
	FileRequest_t* newRequest = VarArrayAdd(&manager->activeFileRequests, FileRequest_t);
	NotNull(newRequest);
	ClearPointer(newRequest);
	newRequest->id = manager->nextFileRequestId;
	manager->nextFileRequestId++;
	newRequest->allocArena = mainHeap;
	newRequest->filePath = AllocString(newRequest->allocArena, &filePath);
	newRequest->callbackContext = callbackContext;
	newRequest->callback = callback;
	
	RequestFileAsync(newRequest->id, newRequest->filePath.chars);
}
