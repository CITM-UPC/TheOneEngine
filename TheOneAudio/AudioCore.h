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

class AudioCore
{
public:
	AudioCore();

	void Awake();

	void Update(double dt);

	void CleanUp();

	//set default listener
	void SetDefaultListener(AkGameObjectID goID);

	//return -1 if failed
	AkGameObjectID RegisterGameObject(std::string name);
	//function to play an event
	void PlayEvent(AkUniqueID eventToPlay, AkGameObjectID goID);
	//function to stop event
	void StopEvent(AkGameObjectID goID);
	//function to pause event
	void PauseEvent(AkGameObjectID goID);
	//function to resume the event if it has been paused
	void ResumeEvent(AkGameObjectID goID);

	void PlayEngine();
	void PauseEngine();

	//transform the game object that events are attached to
	void SetAudioGameObjectTransform(AkGameObjectID goID, float posx, float posy, float posz, float ofx, float ofy, float ofz, float otx, float oty, float otz);

	//transform the position and reset the orientation to the game object that events are attached to
	void SetAudioGameObjectPosition(AkGameObjectID goID, float posx, float posy, float posz);

	bool isGameOn;

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