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
// ToneGenPlugin.cpp
//
// Tone generator Wwise plugin implementation.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToneGenPlugin.h"

// Tone generator property names
const char* const szStartFreq = "StartFreq";
const char* const szStartFreqRandMin = "StartFreqRandMin";
const char* const szStartFreqRandMax = "StartFreqRandMax";
const char* const szSweepFreq = "SweepFreq";
const char* const szSweepFreqType = "SweepFreqType";
const char* const szStopFreq = "StopFreq";
const char* const szStopFreqRandMin = "StopFreqRandMin";
const char* const szStopFreqRandMax = "StopFreqRandMax";
const char* const szWaveType = "WaveType";
const char* const szWaveGain = "WaveGain";
const char* const szDurMode = "DurMode";
const char* const szFixDur = "FixDur";
const char* const szAttackTime = "AttackTime";
const char* const szDecayTime = "DecayTime";
const char* const szSustainTime = "SustainTime";
const char* const szSustainLevel = "SustainLevel";
const char* const szReleaseTime = "ReleaseTime";
const char* const szChannelMask = "ChannelMask";

// Store current plugin settings into banks when asked to.
bool ToneGen::GetBankParameters( const GUID & in_guidPlatform, AK::Wwise::Plugin::DataWriter& in_dataWriter ) const
{
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szWaveGain ) );
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szStartFreq ) );
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szStopFreq ) );
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szStartFreqRandMin ) );
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szStartFreqRandMax ) );
	in_dataWriter.WriteBool( m_propertySet.GetBool( in_guidPlatform, szSweepFreq ) );
	in_dataWriter.WriteInt32( m_propertySet.GetInt32( in_guidPlatform, szSweepFreqType ) );
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szStopFreqRandMin ) );
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szStopFreqRandMax ) );
	in_dataWriter.WriteInt32( m_propertySet.GetInt32( in_guidPlatform, szWaveType ) );
	in_dataWriter.WriteInt32( m_propertySet.GetInt32( in_guidPlatform, szDurMode ) );
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szFixDur ) );
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szAttackTime ) );
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szDecayTime ) );
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szSustainTime ) );
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szSustainLevel ) );
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szReleaseTime ) );
	in_dataWriter.WriteInt32( m_propertySet.GetInt32( in_guidPlatform, szChannelMask ) );

	return true;
}

bool ToneGen::GetSourceDuration( double& out_dblMinDuration, double& out_dblMaxDuration ) const
{
	double dblDuration = 0.0;

	if( m_propertySet.GetInt32( m_host.GetCurrentPlatform(), szDurMode ) == DurMode_Fixed )
	{
		dblDuration = m_propertySet.GetReal32( m_host.GetCurrentPlatform(), szFixDur );
	}
	else
	{
		dblDuration += m_propertySet.GetReal32( m_host.GetCurrentPlatform(), szAttackTime );
		dblDuration += m_propertySet.GetReal32( m_host.GetCurrentPlatform(), szDecayTime );
		dblDuration += m_propertySet.GetReal32( m_host.GetCurrentPlatform(), szSustainTime );
		dblDuration += m_propertySet.GetReal32( m_host.GetCurrentPlatform(), szReleaseTime );
	}

	out_dblMinDuration = dblDuration;
	out_dblMaxDuration = dblDuration;

	return true;
}

AK_DEFINE_PLUGIN_CONTAINER(AkToneGen);
AK_EXPORT_PLUGIN_CONTAINER(AkToneGen);
AK_ADD_PLUGIN_CLASS_TO_CONTAINER(AkToneGen, ToneGen, AkToneSource);

DEFINE_PLUGIN_REGISTER_HOOK;
DEFINEDUMMYASSERTHOOK;
