#ifndef __AUDIOCORE_H__
#define __AUDIOCORE_H__
#pragma once

#include "AudioUtility.h"

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

	void PlayEngine();
	void PauseEngine();

	void SetListenerTransform(float posx, float posy, float posz, float ofx, float ofy, float ofz, float otx, float oty, float otz);
	void SetSpatial1Transform(float posx, float posy, float posz);
	void SetSpatial2Transform(float posx, float posy, float posz);

	bool isGameOn;

	static void EventCallBack(AkCallbackType in_eType, AkCallbackInfo* in_pCallbackInfo);

private:
	bool InitEngine();
	bool InitMemoryManager();
	bool InitStreamingManager();
	bool InitSoundEngine();
	bool InitMusicEngine();
	bool InitSpatialAudio();
	bool InitCommunication();

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