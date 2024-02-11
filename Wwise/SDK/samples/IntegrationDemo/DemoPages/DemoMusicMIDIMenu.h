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

// DemoMusicMIDIMenu.h
/// \file
/// Contains the declaration for the DemoMusicMIDIMenu class.

#pragma once

#include "Page.h"

/// This class represents the Music and MIDI Demos Menu page. This is simply a submenu to
/// link to various demonstrations involving music and MIDI.
class DemoMusicMIDIMenu : public Page
{
public:
	/// DemoMusicMIDIMenu class constructor
	DemoMusicMIDIMenu(Menu& in_ParentMenu);

private:

	/// Initializes the controls on the page.
	virtual void InitControls();

	/// Delegate functions for Music Callback demos buttons.
	void MusicSyncCallbackButton_Pressed( void* in_pSender, ControlEvent* in_pEvent );
	void MusicPlaylistCallbackButton_Pressed( void* in_pSender, ControlEvent* in_pEvent );
	void MusicMIDICallbackButton_Pressed( void* in_pSender, ControlEvent* in_pEvent );

	/// Delegate function for Interactive Music Demo button.
	void OpenInteractiveMusicDemoPage( void* in_pSender, ControlEvent* in_pEvent );

	/// Delegate function for MIDI API Demo button.
	void OpenMIDIMetronomeDemoPage( void* in_pSender, ControlEvent* in_pEvent );

	/// Delegate function for BGM Demo button.
	void OpenBGMDemoPage( void* in_pSender, ControlEvent* in_pEvent );
};
