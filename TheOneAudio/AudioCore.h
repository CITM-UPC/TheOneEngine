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

	//return -1 if failed
	AkGameObjectID RegisterGameObject(std::string name);
	void PlayEvent(AkUniqueID eventToPlay, AkGameObjectID goID, AudioEvent* audioEvent);

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

	//
	std::vector<AkGameObjectID> audioEventsArray;

	// Camera
	AkGameObjectID GAME_OBJECT_ID_BACKGROUNDMUSIC;
	// Static
	AkGameObjectID GAME_OBJECT_ID_SPATIALSOUND1;
	// Moving
	AkGameObjectID GAME_OBJECT_ID_SPATIALSOUND2;

	// 1st music background
	AudioEvent* music1;
	// 2nd music background
	AudioEvent* music2;
	// Static
	AudioEvent* spatial1;
	// Moving
	AudioEvent* spatial2;

	//true: music1
	//false: music2
	bool nextSong;

public:
	// We're using the default Low-Level I/O implementation that's part
	// of the SDK's sample code, with the file package extension
	CAkFilePackageLowLevelIODeferred g_lowLevelIO;
};

#endif // !__AUDIOCORE_H__