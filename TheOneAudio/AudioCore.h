#ifndef __AUDIOCORE_H__
#define __AUDIOCORE_H__
#pragma once

#include "AudioUtility.h"

//max audioevents at the same time, number can be changed if needed
#define MAX_AUDIO_EVENTS 20

class AudioEvent
{
public:
	AudioEvent();

	bool IsEventPlaying();

	AkPlayingID playing_id;					// When Event is played, is different from 0L
	AkCallbackFunc event_call_back;			// Call back function
};

enum struct EngineState
{
	STOPPED = 0,
	PLAYING = 1,
	PAUSED = 2
};

class AudioCore
{
public:
	AudioCore();

	void Awake();

	void Update(double dt);

	void CleanUp();

	//set default listener
	void SetDefaultListener(AkGameObjectID goID);

	//register a wwise game object and return its id or return -1 if failed
	AkGameObjectID RegisterGameObject(std::string name);
	//function to play an event
	void PlayEvent(AkUniqueID event, AkGameObjectID goID);
	//function to stop event
	void StopEvent(AkUniqueID event, AkGameObjectID goID);
	//function to pause event
	void PauseEvent(AkUniqueID event, AkGameObjectID goID);
	//function to resume the event if it has been paused
	void ResumeEvent(AkUniqueID event, AkGameObjectID goID);

	//audio engine functions
	void PlayEngine();
	void PauseEngine();
	void StopEngine();

	//volume functions
	//percentage, from 0 (mute) to 100 (max)
	void SetGlobalSound(float volume);

	//transform the game object that events are attached to
	void SetAudioGameObjectTransform(AkGameObjectID goID, float posx, float posy, float posz, float ofx, float ofy, float ofz, float otx, float oty, float otz);

	//transform the position and reset the orientation to the game object that events are attached to
	void SetAudioGameObjectPosition(AkGameObjectID goID, float posx, float posy, float posz);

	EngineState state = EngineState::STOPPED;

	float globalVolume = 100.0f;


	//function called when an event finishes, to make AudioEvent know it ended
	static void EventCallBack(AkCallbackType in_eType, AkCallbackInfo* in_pCallbackInfo);

private:
	bool InitEngine();
	bool InitMemoryManager();
	bool InitStreamingManager();
	bool InitSoundEngine();
	bool InitMusicEngine();
	bool InitSpatialAudio();
	bool InitCommunication();

	//vector of all game object ids
	std::vector<AkGameObjectID> gameObjectIDs;

	//vector of all audio events that must be used
	//WHEN ITERATING IT USE AS MAX VALUE MAX_AUDIO_EVENTS
	std::vector<AudioEvent*> audioEvents;

	//true: music1
	//false: music2
	bool nextSong;

public:
	// We're using the default Low-Level I/O implementation that's part
	// of the SDK's sample code, with the file package extension
	CAkFilePackageLowLevelIODeferred g_lowLevelIO;
};

#endif // !__AUDIOCORE_H__