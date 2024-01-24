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
#include <vector>
#include <AK/SoundEngine/Common/AkTypes.h>

class Control;
class Page;
class ToggleControl;
struct AkPlatformInitSettings;
struct AkChannelConfig;

namespace demoOptionsUtil
{	
	static const char* s_labelRate = "Sample Rate:";
	static const char* s_labelAudioAPI = "Audio API:";
	static const char* s_labelSampleFormat = "Sample Format:";

	void PreInitWwise();
	void PostInitWwise();

	AkUInt32 GetDefaultSpeakerConfigs(const char**& out_pNames, AkChannelConfig*& out_pConfigs);
	void InitPlatformSpecificControls(std::vector<Control*>& in_Controls, Page* in_pParent);
	void FillPlatformSpecificSettings(std::vector<Control*>& in_Controls, AkPlatformInitSettings* io_settings);
	void UpdatePlatformSpecificControls(std::vector<Control*>& in_Controls);

	void AddSampleRateOption(ToggleControl* in_RateToggle, AkUInt32 in_uCurrentRate);
}