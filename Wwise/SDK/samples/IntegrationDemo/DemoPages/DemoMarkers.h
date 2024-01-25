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
/// Contains the declaration for the DemoMarkers class.

#pragma once

#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/AkCallback.h>
#include "Page.h"
#include <string>

/// Class representing the Markers Demo page. This page demonstrates how to use the markers
/// in a wav file to synchronize events in the game with the audio being played.
class DemoMarkers : public Page
{
public:

	/// DemoMarkers class constructor
	DemoMarkers( Menu& in_ParentMenu );

	/// Initializes the demo.
	/// \return True if successful and False otherwise.
	virtual bool Init();

	/// Releases resources used by the demo.
	virtual void Release();

	/// Override of the Page::Draw() method.
	virtual void Draw();

private:

	/// Initializes the controls on the page.
	virtual void InitControls();

	/// Delegate function for the "Play Markers" button.
	void PlayMarkersButton_Pressed( void* in_pSender, ControlEvent* in_pEvent );

	/// Callback method for the events raised by Wwise while playing the markers demo audio.
	static void MarkersCallback( 
		AkCallbackType in_eType,			///< - The type of the callback
		AkCallbackInfo* in_pCallbackInfo	///< - Structure containing info about the callback
		);

	/// Tracks the current line during audio playback to sync audio with captions.
	std::string m_strCurrentLine;

	/// Whether the audio is currently playing or not.
	bool m_bPlayingMarkers;

	/// Holds the playing ID of the launched PLAY_MARKERS event.
	AkPlayingID m_iPlayingID;

	/// Game Object ID for the "Narrator".
	static const AkGameObjectID GAME_OBJECT_NARRATOR = 100;
};
