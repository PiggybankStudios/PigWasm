/*
File:   file_system.h
Author: Taylor Robbins
Date:   12\01\2022
*/

#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

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
// |                          Functions                           |
// +--------------------------------------------------------------+
void FreeFileRequest(FileRequest_t* request);
void InitializeFileSystemManager(FileSystemManager_t* manager);
FileRequest_t* GetFileRequestById(FileSystemManager_t* manager, u32 requestId, u32* requestIndexOut = nullptr);
void FileSystemHandleFileReady(FileSystemManager_t* manager, u32 requestId, u32 fileSize, const void* fileData);

void FileSystemRequestAsync(FileSystemManager_t* manager, MyStr_t filePath, FileReadyCallback_f* callback, void* callbackContext = nullptr);

#endif //  _FILE_SYSTEM_H
