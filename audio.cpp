/*
File:   audio.cpp
Author: Taylor Robbins
Date:   12\09\2022
Description: 
	** Holds the functions that help us synthesize audio output
*/

#include "common.h"
#include "audio.h"

#include "memory.h"
#include "intrinsics.h"
#include "mouse_and_keyboard.h"
#include "vectors.h"
#include "file_system.h"

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
AudioManager_t* audioManager = nullptr;

// +--------------------------------------------------------------+
// |                      Private Functions                       |
// +--------------------------------------------------------------+
static AudioLoadContext_t* NewAudioSourceLoadContext(AudioSource_t* targetAudioSource)
{
	for (u64 cIndex = 0; cIndex < MAX_NUM_CONCURRENT_AUDIO_LOADS; cIndex++)
	{
		if (!audioManager->audioLoadContexts[cIndex].filled)
		{
			AudioLoadContext_t* context = &audioManager->audioLoadContexts[cIndex];
			ClearPointer(context);
			context->filled = true;
			context->targetAudioSource = targetAudioSource;
			return context;
		}
	}
	return nullptr;
}
static AudioInstance_t* NewAudioInstance(AudioSource_t* audioSource)
{
	for (u64 iIndex = 0; iIndex < MAX_NUM_CONCURRENT_AUDIO_INSTANCES; iIndex++)
	{
		if (!audioManager->audioInstances[iIndex].filled)
		{
			AudioInstance_t* instance = &audioManager->audioInstances[iIndex];
			ClearPointer(instance);
			instance->filled = true;
			instance->sourcePntr = audioSource;
			instance->sourceId = audioSource->id;
			return instance;
		}
	}
	return nullptr;
}

static void FileLoadingManagerAudioFileReadyCallback(MyStr_t filePath, u32 fileSize, const void* fileData, void* contextPntr)
{
	AudioLoadContext_t* context = (AudioLoadContext_t*)contextPntr;
	NotNull2(context, context->targetAudioSource );
	Assert(context->filled == true);
	CreateAudioSourceFromFile(context->targetAudioSource, fileSize, contextPntr);
	WriteLine_D("Successfully loaded audio source");
	context->filled = false;
}

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
void InitAudioManager(AudioManager_t* manager)
{
	audioManager = manager;
	ClearPointer(audioManager);
	audioManager->audioStarted = false;
}

void UpdateAudioManager()
{
	if (!audioManager->audioStarted)
	{
		if (MousePressed(MouseBtn_Left))
		{
			v2 mousePos = NewVec2(0, 0);
			GetMousePosition(&mousePos.x, &mousePos.y);
			v2 canvasSize = NewVec2(0, 0);
			GetCanvasSize(&canvasSize.x, &canvasSize.y);
			
			if (mousePos.x >= 0 && mousePos.y >= 0 &&
				mousePos.x < canvasSize.x && mousePos.y < canvasSize.y)
			{
				WriteLine_N("Starting audio output...");
				InitializeAudioOutput();
				audioManager->audioStarted = true;
			}
		}
	}
}

void CreateAudioSourceFromFile(AudioSource_t* audioSourceOut, u32 fileSize, const void* filePntr)
{
	NotNull(audioSourceOut);
	Assert(fileSize > 0);
	NotNull(filePntr);
	
	ClearPointer(audioSourceOut);
	audioSourceOut->id = CreateAudioSourceFromFile(fileSize, filePntr);
	audioSourceOut->isValid = true;
}

bool StartLoadAudioSource(AudioSource_t* targetAudioSource, MyStr_t filePath)
{
	AudioLoadContext_t* audioSourceLoadContext = NewAudioSourceLoadContext(targetAudioSource);
	if (audioSourceLoadContext == nullptr) { return false; }
	
	FileSystemRequestAsync(filePath, FileLoadingManagerAudioFileReadyCallback, audioSourceLoadContext);
	return true;
}

AudioInstance_t* StartAudioInstance(AudioSource_t* audioSource)
{
	AudioInstance_t* newInstance = NewAudioInstance(audioSource);
	if (newInstance != nullptr)
	{
		//TODO: Anything we want to do?
	}
	return newInstance;
}
