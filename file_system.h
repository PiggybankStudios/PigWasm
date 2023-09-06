/*
File:   file_system.h
Author: Taylor Robbins
Date:   12\01\2022
*/

#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

#include "var_array.h"

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
typedef void FileReadyCallback_f(MyStr_t filePath, u32 fileSize, const void* fileData, void* contextPntr);

struct FileRequest_t
{
	u32 id;
	MemArena_t* allocArena;
	MyStr_t filePath;
	void* callbackContext;
	FileReadyCallback_f* callback;
};

struct FileSystemManager_t
{
	u32 nextFileRequestId;
	VarArray_t activeFileRequests; //FileRequest_t
};

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
extern FileSystemManager_t* fileSystem;

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
void FreeFileRequest(FileRequest_t* request);
void InitializeFileSystemManager(FileSystemManager_t* fileSystemManagerPntr);
FileRequest_t* GetFileRequestById(u32 requestId, u32* requestIndexOut = nullptr);
void FileSystemHandleFileReady(u32 requestId, u32 fileSize, const void* fileData);

void FileSystemRequestAsync(MyStr_t filePath, FileReadyCallback_f* callback, void* callbackContext = nullptr);

#endif //  _FILE_SYSTEM_H
