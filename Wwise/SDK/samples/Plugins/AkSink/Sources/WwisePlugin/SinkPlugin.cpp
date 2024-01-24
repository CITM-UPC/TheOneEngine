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
#include "SinkPlugin.h"

// Sink Property names
const char* const szSampleProp = "DirectGain";

bool SinkPlugin::GetBankParameters(const GUID & in_guidPlatform, AK::Wwise::Plugin::DataWriter& in_dataWriter) const
{
	in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, szSampleProp));

	return true;
}

AK_DEFINE_PLUGIN_CONTAINER(Sink);
AK_EXPORT_PLUGIN_CONTAINER(Sink);
AK_ADD_PLUGIN_CLASS_TO_CONTAINER(Sink, SinkPlugin, AkSink);

DEFINE_PLUGIN_REGISTER_HOOK;
DEFINEDUMMYASSERTHOOK;
