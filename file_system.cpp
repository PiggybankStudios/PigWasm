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

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
FileSystemManager_t* fileSystem = nullptr;

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
void FreeFileRequest(FileRequest_t* request)
{
	NotNull(request);
	if (!IsEmptyStr(request->filePath))
	{
		FreeString(request->allocArena, &request->filePath);
	}
	ClearPointer(request);
}

void InitializeFileSystemManager(FileSystemManager_t* fileSystemManagerPntr)
{
	NotNull(fileSystemManagerPntr);
	fileSystem = fileSystemManagerPntr;
	ClearPointer(fileSystem);
	fileSystem->nextFileRequestId = 1;
	CreateVarArray(&fileSystem->activeFileRequests, mainHeap, sizeof(FileRequest_t), 64);
}

FileRequest_t* GetFileRequestById(u32 requestId, u32* requestIndexOut)
{
	VarArrayLoop(&fileSystem->activeFileRequests, rIndex)
	{
		VarArrayLoopGet(FileRequest_t, request, &fileSystem->activeFileRequests, rIndex);
		if (request->id == requestId)
		{
			if (requestIndexOut != nullptr) { *requestIndexOut = rIndex; }
			return request;
		}
	}
	return nullptr;
}

void FileSystemHandleFileReady(u32 requestId, u32 fileSize, const void* fileData)
{
	u32 requestIndex = 0;
	FileRequest_t* request = GetFileRequestById(requestId, &requestIndex);
	if (request != nullptr)
	{
		if (request->callback != nullptr)
		{
			request->callback(request->filePath, fileSize, fileData, request->callbackContext);
		}
		VarArrayRemove(&fileSystem->activeFileRequests, requestIndex, FileRequest_t);
	}
	else
	{
		PrintLine_E("Unknown request for file completed. ID %u (we have %u active requests)", requestId, fileSystem->activeFileRequests.length);
	}
}

void FileSystemRequestAsync(MyStr_t filePath, FileReadyCallback_f* callback, void* callbackContext)
{
	// PrintLine_D("activeFileRequests: %u/%u (0x%08X)", fileSystem->activeFileRequests.length, fileSystem->activeFileRequests.allocLength, (u32)fileSystem->activeFileRequests.items);
	FileRequest_t* newRequest = VarArrayAdd(&fileSystem->activeFileRequests, FileRequest_t);
	NotNull(newRequest);
	ClearPointer(newRequest);
	newRequest->id = fileSystem->nextFileRequestId;
	fileSystem->nextFileRequestId++;
	newRequest->allocArena = mainHeap;
	newRequest->filePath = AllocString(newRequest->allocArena, &filePath);
	newRequest->callbackContext = callbackContext;
	newRequest->callback = callback;
	
	RequestFileAsync(newRequest->id, newRequest->filePath.chars);
}
