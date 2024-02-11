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
// SinePlugin.cpp
//
// Sine tone Wwise plugin implementation.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SinePlugin.h"

#ifndef FLT_MAX
#define FLT_MAX          3.402823466e+38F        // max value
#endif

// SineTone property names
const char* const szSineFrequency = "SineFrequency";
const char* const szSineGain = "SineGain";
const char* const szSineDuration = "SineDuration";
const char* const szChannelMask = "ChannelMask";

// Store current plugin settings into banks when asked to.
bool SinePlugin::GetBankParameters(const GUID & in_guidPlatform, AK::Wwise::Plugin::DataWriter& in_dataWriter) const
{
	in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, szSineFrequency));
	in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, szSineGain));
	in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, szSineDuration));
	in_dataWriter.WriteInt32(m_propertySet.GetInt32(in_guidPlatform, szChannelMask));

	return true;
}

bool SinePlugin::GetSourceDuration(double& out_dblMinDuration, double& out_dblMaxDuration) const
{
	double dblDuration = 0.;

	if (m_propertySet.PropertyHasRTPC(szSineDuration))
	{
		out_dblMinDuration = 0.0;
		out_dblMaxDuration = FLT_MAX;
		return false;
	}

	dblDuration = m_propertySet.GetReal32(m_host.GetCurrentPlatform(), szSineDuration);

	out_dblMinDuration = dblDuration;
	out_dblMaxDuration = dblDuration;

	return true;
}

AK_DEFINE_PLUGIN_CONTAINER(AkSineTone);
AK_EXPORT_PLUGIN_CONTAINER(AkSineTone);
AK_ADD_PLUGIN_CLASS_TO_CONTAINER(AkSineTone, SinePlugin, SineSource);

DEFINE_PLUGIN_REGISTER_HOOK;
DEFINEDUMMYASSERTHOOK;
