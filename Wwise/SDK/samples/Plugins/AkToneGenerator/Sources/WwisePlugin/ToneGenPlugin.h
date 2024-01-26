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
// ToneGenPlugin.h
//
// Tone generator Wwise plugin implementation.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <AK/Wwise/Plugin.h>

extern const char* const szStartFreq;
extern const char* const szStartFreqRandMin;
extern const char* const szStartFreqRandMax;
extern const char* const szSweepFreq;
extern const char* const szSweepFreqType;
extern const char* const szStopFreq;
extern const char* const szStopFreqRandMin;
extern const char* const szStopFreqRandMax;
extern const char* const szWaveType;
extern const char* const szWaveGain;
extern const char* const szDurMode;
extern const char* const szFixDur;
extern const char* const szAttackTime;
extern const char* const szDecayTime;
extern const char* const szSustainTime;
extern const char* const szSustainLevel;
extern const char* const szReleaseTime;
extern const char* const szChannelMask;

enum DurMode
{
	DurMode_Fixed = 0,
	DurMode_Envelope
};

class ToneGen
	: public AK::Wwise::Plugin::AudioPlugin
	, public AK::Wwise::Plugin::RequestHost
	, public AK::Wwise::Plugin::Source
{
public:
	ToneGen() {}

	virtual bool GetBankParameters( const GUID & in_guidPlatform, AK::Wwise::Plugin::DataWriter& in_dataWriter ) const override;
	virtual bool GetSourceDuration( double& out_dblMinDuration, double& out_dblMaxDuration ) const override;
};

AK_DECLARE_PLUGIN_CONTAINER(AkToneGen);
