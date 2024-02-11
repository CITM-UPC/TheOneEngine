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
#pragma once

#include <AK/Wwise/Plugin.h>

extern const char* const szSampleProp;

class SinkPlugin
	: public AK::Wwise::Plugin::AudioPlugin
	, public AK::Wwise::Plugin::RequestHost
{
public:
	SinkPlugin() {}

	virtual bool GetBankParameters( const GUID & in_guidPlatform, AK::Wwise::Plugin::DataWriter& in_dataWriter ) const override;
};

AK_DECLARE_PLUGIN_CONTAINER(Sink);