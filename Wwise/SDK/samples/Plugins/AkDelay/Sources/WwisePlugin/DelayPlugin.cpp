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

#include "stdafx.h"
#include "DelayPlugin.h"

// Delay property names
const char* const szDelayTime = "DelayTime";
const char* const szFeedbackEnabled = "FeedbackEnabled";
const char* const szFeedback = "Feedback";
const char* const szWetDryMix = "WetDryMix";
const char* const szOutputLevel = "OutputLevel";
const char* const szProcessLFE = "ProcessLFE";

// Store current plugin settings into banks when asked to.
bool Delay::GetBankParameters( const GUID & in_guidPlatform, AK::Wwise::Plugin::DataWriter& in_dataWriter ) const
{
	// Pack parameters in bank
	// IMPORTANT NOTE: they need to be written and read on the AudioEngine side in the same order
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szDelayTime ) );
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szFeedback ) );
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szWetDryMix ) );
	in_dataWriter.WriteReal32( m_propertySet.GetReal32( in_guidPlatform, szOutputLevel ) );
	in_dataWriter.WriteBool( m_propertySet.GetBool( in_guidPlatform, szFeedbackEnabled ) );
	in_dataWriter.WriteBool( m_propertySet.GetBool( in_guidPlatform, szProcessLFE ) );

	return true;
}

AK_DEFINE_PLUGIN_CONTAINER(AkDelay);
AK_EXPORT_PLUGIN_CONTAINER(AkDelay);
AK_ADD_PLUGIN_CLASS_TO_CONTAINER(AkDelay, Delay, AkDelayFX);

DEFINE_PLUGIN_REGISTER_HOOK;
DEFINEDUMMYASSERTHOOK;
