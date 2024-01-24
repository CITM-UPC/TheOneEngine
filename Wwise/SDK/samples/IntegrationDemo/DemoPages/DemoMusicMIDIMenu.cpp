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

// DemoMusicMIDIMenu.cpp
/// \file 
/// Defines the methods declared in DemoMusicMIDIMenu.h.

#include "stdafx.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine

#include "Menu.h"
#include "DemoMusicMIDIMenu.h"
#include "DemoMusicCallbacks.h"
#include "DemoInteractiveMusic.h"
#include "DemoMIDIMetronome.h"
#include "DemoBGM.h"

/////////////////////////////////////////////////////////////////////
// DemoMusicMIDIMenu Public Methods
/////////////////////////////////////////////////////////////////////

DemoMusicMIDIMenu::DemoMusicMIDIMenu( Menu& in_ParentMenu )
: Page( in_ParentMenu, "Music/MIDI Demos" )
{
}

void DemoMusicMIDIMenu::InitControls()
{
	ButtonControl* newBtn;

	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Music Sync Callback Demo" );
	newBtn->SetDelegate( (PageMFP)&DemoMusicMIDIMenu::MusicSyncCallbackButton_Pressed );
	m_Controls.push_back( newBtn );

	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Music Playlist Callback Demo" );
	newBtn->SetDelegate( (PageMFP)&DemoMusicMIDIMenu::MusicPlaylistCallbackButton_Pressed );
	m_Controls.push_back( newBtn );

	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "MIDI Callback Demo" );
	newBtn->SetDelegate( (PageMFP)&DemoMusicMIDIMenu::MusicMIDICallbackButton_Pressed );
	m_Controls.push_back( newBtn );

	// Add button linking to Interactive Music demo
	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Interactive Music Demo" );
	newBtn->SetDelegate( (PageMFP)&DemoMusicMIDIMenu::OpenInteractiveMusicDemoPage );
	m_Controls.push_back( newBtn );

	// Add button linking to MIDI API demo (Metronome)
	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "MIDI API Demo (Metronome)" );
	newBtn->SetDelegate( (PageMFP)&DemoMusicMIDIMenu::OpenMIDIMetronomeDemoPage );
	m_Controls.push_back( newBtn );

	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Background Music/DVR Demo" );
	newBtn->SetDelegate( (PageMFP)&DemoMusicMIDIMenu::OpenBGMDemoPage );
	m_Controls.push_back( newBtn );
}

void DemoMusicMIDIMenu::MusicSyncCallbackButton_Pressed( void* , ControlEvent* )
{
	DemoMusicCallbacks* pg = new DemoMusicCallbacks( *m_pParentMenu );
	m_pParentMenu->StackPage( pg );
}

void DemoMusicMIDIMenu::MusicPlaylistCallbackButton_Pressed( void* , ControlEvent* )
{
	DemoMusicPlaylistCallbacks* pg = new DemoMusicPlaylistCallbacks( *m_pParentMenu );
	m_pParentMenu->StackPage( pg );
}

void DemoMusicMIDIMenu::MusicMIDICallbackButton_Pressed( void* , ControlEvent* )
{
	DemoMIDICallbacks* pg = new DemoMIDICallbacks( *m_pParentMenu );
	m_pParentMenu->StackPage( pg );
}

void DemoMusicMIDIMenu::OpenInteractiveMusicDemoPage( void*, ControlEvent* )
{
	DemoInteractiveMusic* pg = new DemoInteractiveMusic( *m_pParentMenu );
	m_pParentMenu->StackPage( pg );
}

void DemoMusicMIDIMenu::OpenMIDIMetronomeDemoPage( void*, ControlEvent* )
{
	DemoMIDIMetronome* pg = new DemoMIDIMetronome( *m_pParentMenu );
	m_pParentMenu->StackPage( pg );
}

void DemoMusicMIDIMenu::OpenBGMDemoPage( void*, ControlEvent*  )
{
	DemoBGMusic* pg = new DemoBGMusic( *m_pParentMenu );
	m_pParentMenu->StackPage( pg );
}
