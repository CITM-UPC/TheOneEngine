/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2023 Audiokinetic Inc.
*******************************************************************************/

// DemoMarkers.h
/// \file
/// Contains the declaration for the DemoMusicCallbacks class.

#pragma once

#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/AkCallback.h>
#include "Page.h"

/// Class representing the Music Callbacks Demo page. This page demonstrates how to use the music
/// callabacks in the interactive music system.
class DemoMusicCallbacksBase : public Page
{
public:

	/// DemoMarkers class constructor
	DemoMusicCallbacksBase( Menu& in_ParentMenu, const string& in_strPageTitle );

	virtual void InitControls(){}

	/// Initializes the demo.
	/// \return True if successful and False otherwise.
	virtual bool Init();

	/// Releases resources used by the demo.
	virtual void Release();

	/// Override of the Page::Draw() method.
	virtual void Draw();

	/// Game Object ID for the "Narrator".
	static const AkGameObjectID GAME_OBJECT_MUSIC = 100;

protected:

	bool m_bIsPlaying;
	bool m_bStopPlaylist;
	AkPlayingID m_iPlayingID;
};

class DemoMusicCallbacks : public DemoMusicCallbacksBase
{
public:
	DemoMusicCallbacks( Menu& in_ParentMenu );

	virtual bool Init();

	/// Override of the Page::Draw() method.
	virtual void Draw();

	/// Callback method for the events raised by Wwise while playing the markers demo audio.
	static void MusicCallback( 
		AkCallbackType in_eType,			///< - The type of the callback
		AkCallbackInfo* in_pCallbackInfo	///< - Structure containing info about the callback
		);

private:
	AkUInt32 m_uiBeatCount;
	AkUInt32 m_uiBarCount;
};

class DemoMusicPlaylistCallbacks : public DemoMusicCallbacksBase
{
public:
	DemoMusicPlaylistCallbacks( Menu& in_ParentMenu );

	virtual bool Init();

	/// Override of the Page::Draw() method.
	virtual void Draw();

	/// Callback method for the events raised by Wwise while playing the markers demo audio.
	static void MusicCallback( 
		AkCallbackType in_eType,			///< - The type of the callback
		AkCallbackInfo* in_pCallbackInfo	///< - Structure containing info about the callback
		);

private:
	AkUInt32 m_uiPlaylistItem;
};

class DemoMIDICallbacks : public DemoMusicCallbacksBase
{
public:
	DemoMIDICallbacks( Menu& in_ParentMenu );

	virtual bool Init();

	/// Override of the Page::Draw() method.
	virtual void Draw();

	/// Callback method for the events raised by Wwise while playing the markers demo audio.
	static void MusicCallback( 
		AkCallbackType in_eType,			///< - The type of the callback
		AkCallbackInfo* in_pCallbackInfo	///< - Structure containing info about the callback
		);

private:

	AkMidiNoteNo	m_byNote;
	AkUInt8			m_byVelocity;
	AkUInt8			m_byCc;
	AkUInt8			m_byValue;
};
