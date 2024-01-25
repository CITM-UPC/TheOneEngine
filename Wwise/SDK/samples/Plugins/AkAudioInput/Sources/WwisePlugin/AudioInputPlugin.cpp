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
//////////////////////////////////////////////////////////////////////
//
// AudioInputPlugin.cpp
//
// Audio Input Wwise plugin implementation.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AudioInputPlugin.h"
#include "SoundInputMgr.h"
#include "SoundInput.h"
#include "../AudioEngineFX/AkFXSrcAudioInput.h"

namespace
{
	unsigned int numOfInstances = 0;

	void InitSoundInputMgr()
	{
		if (numOfInstances++ == 0)
		{
			SoundInputMgr::Instance().Initialize();
		}
	}

	void TermSoundInputMgr()
	{
		if (--numOfInstances == 0)
		{
			SoundInputMgr::Instance().Term();
		}
	}
}

// Audio input plugin property names
const char* const szAudioInputGain = "AudioInputGain";

AudioInput::AudioInput()
{
	InitSoundInputMgr();
}

AudioInput::~AudioInput()
{
	TermSoundInputMgr();
}

// Store current plugin settings into banks when asked to.
bool AudioInput::GetBankParameters( const GUID & in_guidPlatform, AK::Wwise::Plugin::DataWriter& in_dataWriter ) const
{
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szAudioInputGain ) );

	return true;
}

class AkMicrophoneSource : public CAkFXSrcAudioInput
{
	virtual AKRESULT Init(AK::IAkPluginMemAlloc* in_pAllocator,		// Memory allocator interface.
		AK::IAkSourcePluginContext* in_pSourceFXContext,// Source FX context
		AK::IAkPluginParam* in_pParams,			// Effect parameters.
		AkAudioFormat& io_rFormat			// Supported audio output format.
	)
	{
		m_Input.InputOn();
		return CAkFXSrcAudioInput::Init(in_pAllocator, in_pSourceFXContext, in_pParams, io_rFormat);
	}

	// Effect termination.
	virtual AKRESULT Term(AK::IAkPluginMemAlloc* in_pAllocator)
	{
		m_Input.InputOff();
		return CAkFXSrcAudioInput::Term(in_pAllocator);
	}

	SoundInput m_Input;
};

// Plugin mechanism. FX create function and register its address to the FX manager.
AK::IAkPlugin* CreateMicrophoneSource(AK::IAkPluginMemAlloc* in_pAllocator)
{
	return AK_PLUGIN_NEW(in_pAllocator, AkMicrophoneSource());
}

// Plugin mechanism. Parameter node create function to be registered to the FX manager.
AK::IAkPluginParam* CreateMicrophoneSourceParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
	return AK_PLUGIN_NEW(in_pAllocator, CAkFxSrcAudioInputParams());
}

AK_ATTR_USED AK::PluginRegistration AkMicrophoneSourceRegistration(AkPluginTypeSource, AKCOMPANYID_AUDIOKINETIC, 200, CreateMicrophoneSource, CreateMicrophoneSourceParams);

AK_DEFINE_PLUGIN_CONTAINER(AkAudioInput);
AK_EXPORT_PLUGIN_CONTAINER(AkAudioInput);
AK_ADD_PLUGIN_CLASS_TO_CONTAINER(AkAudioInput, AudioInput, AkMicrophoneSource);

DEFINE_PLUGIN_REGISTER_HOOK;
DEFINEDUMMYASSERTHOOK;
