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
#include "DemoOptionsUtil.h"
#include "../MenuSystem/ToggleControl.h"

#include <AK/SoundEngine/Common/AkSoundEngine.h>

const char* s_labelAVX = "Use AVX:";
const char* s_labelAudioObjects = "Max System Audio Objects:";

namespace demoOptionsUtil
{	
	void PreInitWwise() {}
	void PostInitWwise() {}

	AkUInt32 GetDefaultSpeakerConfigs(const char**& out_pNames, AkChannelConfig*& out_pConfigs)
	{
		static const char* names[] = { "Automatic", "Mono", "Stereo", "5.1", "7.1", "7.1.4" };
		const size_t uNumNames = sizeof(names) / sizeof(names[0]);

		int i = 0;
		static AkChannelConfig configs[uNumNames];
		configs[i++].SetStandard(0);
		configs[i++].SetStandard(AK_SPEAKER_SETUP_MONO);
		configs[i++].SetStandard(AK_SPEAKER_SETUP_STEREO);
		configs[i++].SetStandard(AK_SPEAKER_SETUP_5POINT1);
		configs[i++].SetStandard(AK_SPEAKER_SETUP_7POINT1);
		configs[i++].SetStandard(AK_SPEAKER_SETUP_DOLBY_7_1_4);

		out_pNames = names;
		out_pConfigs = configs;
		
		return i;
	}

	void InitPlatformSpecificControls(std::vector<Control*>& in_Controls, Page* in_pParent)
	{
		int idx;
		//Add platform specific controls
		AK::IAkGlobalPluginContext* pGlobalCtx = AK::SoundEngine::GetGlobalPluginContext();
		const AkPlatformInitSettings* pd_settings = pGlobalCtx->GetPlatformInitSettings();
		
		ToggleControl* newToggle = new ToggleControl(*in_pParent);
		AddSampleRateOption(newToggle, pd_settings->uSampleRate);
		in_Controls.push_back(newToggle);

		newToggle = new ToggleControl(*in_pParent);
		newToggle->SetLabel(s_labelAVX);
		newToggle->AddOption("No", (void*)0);
		newToggle->AddOption("Yes", (void*)1);		
		idx = (int)pd_settings->bEnableAvxSupport;
		newToggle->SetSelectedIndex(idx);
		in_Controls.push_back(newToggle);

		newToggle = new ToggleControl(*in_pParent);
		newToggle->SetLabel(s_labelAudioObjects);
		newToggle->AddOption("0", (void*)0);
		newToggle->AddOption("5", (void*)5);
		newToggle->AddOption("30", (void*)30);
		idx = pd_settings->uMaxSystemAudioObjects == 0 ? 0 : 1;
		newToggle->SetSelectedIndex(idx);
		in_Controls.push_back(newToggle);
	}

	void FillPlatformSpecificSettings(std::vector<Control*>& in_Controls, AkPlatformInitSettings* io_settings)
	{
		for (size_t i = 0; i < in_Controls.size(); i++)
		{
			if (in_Controls[i]->GetLabel() == s_labelRate)
				io_settings->uSampleRate = (AkUInt32)(AkUIntPtr)((ToggleControl*)in_Controls[i])->SelectedValue();
			else if (in_Controls[i]->GetLabel() == s_labelAVX)
				io_settings->bEnableAvxSupport = (AkUIntPtr)((ToggleControl*)in_Controls[i])->SelectedValue() != 0;
			else if (in_Controls[i]->GetLabel() == s_labelAudioObjects)
				io_settings->uMaxSystemAudioObjects = (AkUInt32)(AkUIntPtr)((ToggleControl*)in_Controls[i])->SelectedValue();
		}
	}

	void UpdatePlatformSpecificControls(std::vector<Control*>& in_Controls)
	{

	}

}