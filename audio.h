/*
File:   audio.h
Author: Taylor Robbins
Date:   12\09\2022
*/

#ifndef _AUDIO_H
#define _AUDIO_H

// +--------------------------------------------------------------+
// |                           Structs                            |
// +--------------------------------------------------------------+
struct AudioSource_t
{
	bool isValid;
	u32 id;
};

struct AudioInstance_t
{
	bool filled;
	AudioSource_t* sourcePntr;
	u32 sourceId;
};

struct AudioLoadContext_t
{
	bool filled;
	AudioSource_t* targetAudioSource;
};

#define MAX_NUM_CONCURRENT_AUDIO_LOADS     16
#define MAX_NUM_CONCURRENT_AUDIO_INSTANCES 32

struct AudioManager_t
{
	bool audioStarted;
	
	AudioLoadContext_t audioLoadContexts[MAX_NUM_CONCURRENT_AUDIO_LOADS];
	AudioInstance_t audioInstances[MAX_NUM_CONCURRENT_AUDIO_INSTANCES];
};

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
extern AudioManager_t* audioManager;

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
void InitAudioManager(AudioManager_t* manager);
void UpdateAudioManager();

void CreateAudioSourceFromFile(AudioSource_t* audioSourceOut, u32 fileSize, const void* filePntr);

bool StartLoadAudioSource(AudioSource_t* targetAudioSource, MyStr_t filePath);

AudioInstance_t* StartAudioInstance(AudioSource_t* audioSource);

#endif //  _AUDIO_H
