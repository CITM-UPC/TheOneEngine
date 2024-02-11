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
#include "Platform.h"
#include "DemoOptions.h"
#include "DemoOptionsUtil.h"
#include "../MenuSystem/ToggleControl.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine

#include "IntegrationDemo.h"
#include "Menu.h"

// SpatialAudio configuration is disabled in IntegrationDemo because the related plug-ins are optional,
// and may not be present. If you do have the plug-in contents and want to try it, include the relevant
// plug-in factory, and link the relevant lib.
#define SPATIAL_AUDIO_PLUGIN_AVAILABLE 0

// To persist the active device when exiting and reentering the demo page
static bool g_spatialAudioEnabled = false;

const char* s_labelDevice = "Device:";
const char* s_labelPanning = "Speaker Panning:";
const char* s_labelConfig = "Speaker Config:";
const char* s_labelFrameSize = "Frame Size:";
const char* s_labelBuffers = "Refill Buffers:";
const char* s_labelRangeCheckLimit = "Range Check Limit:";
const char* s_labelRangeCheckEnable = "Range Check:";
const char* s_labelMaxMemory = "Memory Limit:";
const char* s_labelMemoryDebug = "Memory Debug Option:";
const char* s_labelMaxThreadAllocSize = "Max Thread-Local Malloc Size:";
const char* s_labelMaxJobWorkers = "Job Workers:";

#define TITLE_POS_Y (GetLineHeight(DrawStyle_Title)*2)

namespace demoOptionsUtil
{
	void AddSampleRateOption(ToggleControl* in_RateToggle, AkUInt32 in_uCurrentRate)
	{
		int idx = 0;
		char buf[20];
		in_RateToggle->SetLabel(s_labelRate);
		in_RateToggle->AddOption("48000", (void*)48000);
		in_RateToggle->AddOption("44100", (void*)44100);
		in_RateToggle->AddOption("32000", (void*)32000);
		in_RateToggle->AddOption("24000", (void*)24000);
		switch (in_uCurrentRate)
		{
		case 48000: idx = 0; break;
		case 44100: idx = 1; break;
		case 32000: idx = 2; break;
		case 24000: idx = 3; break;
		default:
			snprintf(buf, 20, "%i", in_uCurrentRate);
			in_RateToggle->AddOption(buf, (void*)(AkUIntPtr)in_uCurrentRate);
			idx = 4;
			break;
		}
		in_RateToggle->SetSelectedIndex(idx);
	}
}

/// DemoOptions class constructor
DemoOptions::DemoOptions( Menu& in_ParentMenu ) : Page( in_ParentMenu, "Options Menu" )	
	, m_activeDeviceIdx(0)
	, m_activePanningRule(AkPanningRule_Speakers)
	, m_activeChannelConfig()
	, m_spatialAudioRequested(g_spatialAudioEnabled)
	, m_spatialAudioDeviceAvailable(g_spatialAudioEnabled)	
	, m_speakerConfigToggle(NULL)	
{
	m_szHelp = "This page provides options to configure the sound engine for the whole application.";
}

/// Initializes the demo.
/// \return True if successful and False otherwise.
bool DemoOptions::Init()
{
	// Initialize the page
	return Page::Init();		
}

/// Releases resources used by the demo.
void DemoOptions::Release()
{	
	g_spatialAudioEnabled = m_spatialAudioRequested && m_spatialAudioDeviceAvailable;
	Page::Release();
}

void DemoOptions::InitControls()
{	
	PopulateOutputDeviceOptions();

	// Note that despite providing these for all platforms, not all platforms support
	// all of these options equally. For example, Spatial Audio will be requested,
	// but unless the platform/device supports it (see: AK::SoundEngine::GetDeviceSpatialAudioSupport)
	// it will be ignored. Also, Speaker Configuration is handled automatically on most
	// platforms, and will be ignored.

#if SPATIAL_AUDIO_PLUGIN_AVAILABLE
	ToggleControl* newToggle = new ToggleControl(*this);
	newToggle->SetLabel("Spatial Audio:");
	newToggle->AddOption("Disabled", (void*)false);
	newToggle->AddOption("Enabled", (void*)true);
	newToggle->SetSelectedIndex(g_spatialAudioEnabled ? 1 : 0);
	newToggle->SetDelegate((PageMFP)&DemoOptions::spatialAudioOption_Changed);
	m_Controls.push_back(newToggle);
#endif

	InitSpeakerPanning();
	InitSpeakerConfig();
	InitAudioSettings();
	InitJobWorkerSettings();

	size_t curSize = m_Controls.size();
	PositionControls(0, (int)curSize, 0);	//Position all previous controls in the first column.

	InitMemorySettings();

	demoOptionsUtil::InitPlatformSpecificControls(m_Controls, this);

	//All platform specific settings will reset the engine.
	for(size_t i = curSize; i < m_Controls.size(); i++)
		m_Controls[i]->SetDelegate((PageMFP)&DemoOptions::InitSettings_Changed);

	PositionControls((int)curSize, (int)m_Controls.size(), 1);	//Position all previous controls in the second column.
}

void DemoOptions::PopulateOutputDeviceOptions()
{
	ToggleControl* newToggle = NULL;
	newToggle = new ToggleControl(*this);
	newToggle->SetLabel(s_labelDevice);

	newToggle->AddOption("Use Default");
	m_deviceIds.push_back(DemoOptions::DeviceId(0, 0));

	if (AK::SoundEngine::IsInitialized())
	{
		const std::vector<const char*>& supportedSharesets = IntegrationDemo::GetSupportedAudioDeviceSharesets();

		const AkInitSettings* initSettings = AK::SoundEngine::GetGlobalPluginContext()->GetInitSettings();
		AkOutputDeviceID idCurrent = AK::SoundEngine::GetOutputID((AkUniqueID)0, 0 /*Default for this shareset*/);

		for (const auto& sharesetName : supportedSharesets)
		{
			AkUInt32 sharesetId = AK::SoundEngine::GetIDFromString(sharesetName);

			AkUInt32 deviceCount = 0;
			AKRESULT res = AK::SoundEngine::GetDeviceList(sharesetId, deviceCount, nullptr);
			if (deviceCount == 0 || res != AK_Success)
			{
				char name[AK_MAX_PATH] = { 0 };
				snprintf(name, AK_MAX_PATH, "%s - Primary Output", sharesetName);
				newToggle->AddOption(name);
				m_deviceIds.push_back(DemoOptions::DeviceId(sharesetId, 0));
				continue;
			}

			std::vector<AkDeviceDescription> devices(deviceCount);
			AK::SoundEngine::GetDeviceList(sharesetId, deviceCount, devices.data());

			AkUInt32 uRealCount = 0;
			for (AkUInt32 i = 0; i < deviceCount; ++i)
			{
				if (devices[i].deviceStateMask == AkDeviceState_Active)
				{
					char* deviceName = NULL;
					CONVERT_OSCHAR_TO_CHAR(devices[i].deviceName, deviceName);

					char name[AK_MAX_PATH] = { 0 };
					snprintf(name, AK_MAX_PATH, "%s - %s", sharesetName, deviceName);

					newToggle->AddOption(name);
					m_deviceIds.push_back(DemoOptions::DeviceId(sharesetId, devices[i].idDevice));

					if (devices[i].isDefaultDevice && sharesetId == initSettings->settingsMainOutput.audioDeviceShareset)
					{
						//Two possibilities to check: either the soundengine is using this output because we specifically asked it 
						// OR, it is the default.  
						if (initSettings->settingsMainOutput.idDevice == 0)//Picked as default
							m_activeDeviceIdx = 0;
						else if (AK::SoundEngine::GetOutputID(sharesetId, devices[i].idDevice) == idCurrent)	//Specified explicitely
							m_activeDeviceIdx = uRealCount+1; //+1 because it is offset by the "default" option.
					}

					uRealCount++;
				}
			}
		}
	}

	if (m_activeDeviceIdx < m_deviceIds.size())
		newToggle->SetSelectedIndex(m_activeDeviceIdx);
	else
		m_activeDeviceIdx = 0;

	newToggle->SetDelegate((PageMFP)&DemoOptions::outputDeviceOption_Changed);
	m_Controls.push_back(newToggle);
}

void DemoOptions::InitSpeakerPanning()
{
	ToggleControl* newToggle = new ToggleControl(*this);
	newToggle->SetLabel(s_labelPanning);
	newToggle->AddOption("Speakers", (void*)AkPanningRule_Speakers);
	newToggle->AddOption("Headphones", (void*)AkPanningRule_Headphones);
	AK::SoundEngine::GetPanningRule(m_activePanningRule);	//Get the panning rule for the main device.
	newToggle->SetSelectedIndex((int)m_activePanningRule);
	newToggle->SetDelegate((PageMFP)&DemoOptions::speakerPosOption_Changed);
	m_Controls.push_back(newToggle);
}

void DemoOptions::InitSpeakerConfig()
{
	m_speakerConfigToggle = new ToggleControl(*this);
	m_speakerConfigToggle->SetLabel(s_labelConfig);
	const char** pNames;
	AkChannelConfig* pConfigs;
	AkUInt32 uCount = demoOptionsUtil::GetDefaultSpeakerConfigs(pNames, pConfigs);
	int selected = -1;
	AkChannelConfig currentConfig = AK::SoundEngine::GetSpeakerConfiguration();	//Get the panning rule for the main device.	
	for (AkUInt32 i = 0; i < uCount; i++)
	{
		m_speakerConfigToggle->AddOption(*pNames, (void*)(AkUIntPtr)(pConfigs[i].Serialize()));
		if (currentConfig == pConfigs[i])
			selected = i;
		
		pNames++;
	}

	if (selected == -1)
	{
		char szOption[50];
		snprintf(szOption, 50, "Other (%i)", currentConfig.uNumChannels);
		m_speakerConfigToggle->AddOption(szOption, (void*)(AkUIntPtr)currentConfig.Serialize());
		selected = uCount;
	}

	m_speakerConfigToggle->SetSelectedIndex(selected);
	m_speakerConfigToggle->SetDelegate((PageMFP)&DemoOptions::speakerConfigOption_Changed);
	m_Controls.push_back(m_speakerConfigToggle);
}

void DemoOptions::InitAudioSettings()
{
	AK::IAkGlobalPluginContext* pGlobalCtx = AK::SoundEngine::GetGlobalPluginContext();
	const AkInitSettings *settings  = pGlobalCtx->GetInitSettings();	
	const AkPlatformInitSettings* pd_settings = pGlobalCtx->GetPlatformInitSettings();

	ToggleControl* newToggle = new ToggleControl(*this);
	newToggle->SetLabel(s_labelFrameSize);
	newToggle->AddOption("256", (void*)256);
	newToggle->AddOption("512", (void*)512);
	newToggle->AddOption("1024", (void*)1024);
	newToggle->AddOption("2048", (void*)2048);

	char buf[20];
	int idx = 0;
	switch (settings->uNumSamplesPerFrame)
	{	
	case 256: idx = 0; break;
	case 512: idx = 1; break;
	case 1024: idx = 2; break;
	case 2048: idx = 3; break;
	default:
		snprintf(buf, 20, "%i", settings->uNumSamplesPerFrame);
		newToggle->AddOption(buf, (void*)(AkUIntPtr)settings->uNumSamplesPerFrame);
		idx = 5;
		break;
	}

	newToggle->SetSelectedIndex(idx);
	newToggle->SetDelegate((PageMFP)&DemoOptions::InitSettings_Changed);
	m_Controls.push_back(newToggle);

	newToggle = new ToggleControl(*this);
	newToggle->SetLabel(s_labelBuffers);
	newToggle->AddOption("2", (void*)2);
	newToggle->AddOption("3", (void*)3);
	newToggle->AddOption("4", (void*)4);	
	newToggle->AddOption("8", (void*)8);
	newToggle->AddOption("16", (void*)16);
	newToggle->AddOption("32", (void*)32);
	switch (pd_settings->uNumRefillsInVoice)
	{
	case 2: idx = 0; break;
	case 3: idx = 1; break;
	case 4: idx = 2; break;
	case 8: idx = 3; break;
	case 16: idx = 4; break;
	case 32: idx = 5; break;
	default:
		snprintf(buf, 20, "%i", pd_settings->uNumRefillsInVoice);		
		newToggle->AddOption(buf, (void*)(AkUIntPtr)pd_settings->uNumRefillsInVoice);
		idx = 6;
		break;
	}

	newToggle->SetSelectedIndex(idx);
	newToggle->SetDelegate((PageMFP)&DemoOptions::InitSettings_Changed);
	m_Controls.push_back(newToggle);

	newToggle = new ToggleControl(*this);
	newToggle->SetLabel(s_labelRangeCheckLimit);
	newToggle->AddOption("+6dB", (void*)2);
	newToggle->AddOption("+12dB", (void*)4);
	newToggle->AddOption("+24dB", (void*)16);
	newToggle->AddOption("+48dB", (void*)256);
	switch ((int)settings->fDebugOutOfRangeLimit)
	{
	case 2: idx = 0; break;
	case 4: idx = 1; break;
	case 16: idx = 2; break;
	case 256: idx = 3; break;
	default:
		snprintf(buf, 20, "%i", (int)round(log(settings->fDebugOutOfRangeLimit) * 20));		
		newToggle->AddOption(buf, (void*)(AkUIntPtr)settings->fDebugOutOfRangeLimit);
		idx = 4;
		break;
	}

	newToggle->SetSelectedIndex(idx);
	newToggle->SetDelegate((PageMFP)&DemoOptions::InitSettings_Changed);
	m_Controls.push_back(newToggle);

	newToggle = new ToggleControl(*this);
	newToggle->SetLabel(s_labelRangeCheckEnable);
	newToggle->AddOption("Disabled", (void*)0);
	newToggle->AddOption("Enabled", (void*)1);	

	idx = (int)settings->bDebugOutOfRangeCheckEnabled;	
	newToggle->SetSelectedIndex(idx);
	newToggle->SetDelegate((PageMFP)&DemoOptions::InitSettings_Changed);
	m_Controls.push_back(newToggle);
}

void DemoOptions::InitMemorySettings()
{
	AkMemSettings &memSettings = IntegrationDemo::Instance().GetMemSettings();

	ToggleControl * newToggle = new ToggleControl(*this);
	newToggle->SetLabel(s_labelMaxMemory);

	char buf[20];
	int idx = 0;

	newToggle->AddOption("Disabled", (void*)0);		
	newToggle->AddOption("32 MB", (void*)(32 * 1024 * 1024));
	newToggle->AddOption("64 MB", (void*)(64 * 1024 * 1024));
	newToggle->AddOption("128 MB", (void*)(128 * 1024 * 1024));
	switch (memSettings.uMemAllocationSizeLimit)
	{
	case 0: idx = 0; break;
	case (32 * 1024 * 1024): idx = 1; break;
	case (64 * 1024 * 1024): idx = 2; break;
	case (128 * 1024 * 1024): idx = 3; break;
	default:
		snprintf(buf, 20, "%i", (AkUInt32)memSettings.uMemAllocationSizeLimit);		
		size_t len = strlen(buf);
		buf[len] = 'M';
		buf[len + 1] = 'B';
		buf[len + 2] = 0;
		newToggle->AddOption(buf, (void*)(AkUIntPtr)memSettings.uMemAllocationSizeLimit);
		idx = 5;
		break;
	}

	newToggle->SetSelectedIndex(idx);
	newToggle->SetDelegate((PageMFP)&DemoOptions::InitSettings_Changed);
	m_Controls.push_back(newToggle);

	newToggle = new ToggleControl(*this);
	newToggle->SetLabel(s_labelMemoryDebug);
	newToggle->AddOption("Disabled", (void*)0);
	newToggle->AddOption("Leaks", (void*)1);
	newToggle->AddOption("Stomp Allocator", (void*)2);
	newToggle->AddOption("Stomp Allocator and Leaks", (void*)3);
	newToggle->SetSelectedIndex(memSettings.uMemoryDebugLevel);
	newToggle->SetDelegate((PageMFP)&DemoOptions::InitSettings_Changed);
	m_Controls.push_back(newToggle);

	newToggle = new ToggleControl(*this);
	newToggle->SetLabel(s_labelMaxThreadAllocSize);
	newToggle->AddOption("0", (void*)0);
	newToggle->AddOption("128", (void*)128);
	newToggle->AddOption("512", (void*)512);
	newToggle->AddOption("2048", (void*)2048);
	newToggle->AddOption("32768", (void*)32768);
	switch (memSettings.uMaxThreadLocalHeapAllocSize)
	{
	case 0: idx = 0; break;
	case 128: idx = 1; break;
	case 512: idx = 2; break;
	case 2048: idx = 3; break;
	case 32768: idx = 4; break;
	default: idx = 2; break;
	}
	newToggle->SetSelectedIndex(idx);
	newToggle->SetDelegate((PageMFP)&DemoOptions::InitSettings_Changed);
	m_Controls.push_back(newToggle);	
}

void DemoOptions::InitJobWorkerSettings()
{
#if defined(AK_SUPPORT_THREADS)
	AkJobMgrSettings jobMgrSettings = IntegrationDemo::Instance().GetInitSettings().settingsJobManager;
	NumericControl* newNumeric = new NumericControl(*this);
	newNumeric->SetLabel(s_labelMaxJobWorkers);
	newNumeric->SetMinValue(0);
	newNumeric->SetMaxValue(AK_MAX_WORKER_THREADS);
	newNumeric->SetIncrement(1);
	newNumeric->SetValue(jobMgrSettings.uMaxActiveWorkers[AkJobType_AudioProcessing]);
	newNumeric->SetDelegate((PageMFP)&DemoOptions::jobSettings_Changed);
	m_Controls.push_back(newNumeric);
#endif
}

void DemoOptions::UpdateSpeakerConfigForShareset()
{
	if (m_speakerConfigToggle == nullptr)
		return;

	// Custom speaker configurations are only valid for the default sink
	if (m_deviceIds[m_activeDeviceIdx].sharesetId == IntegrationDemo::GetDefaultAudioDeviceSharesetId() || m_deviceIds[m_activeDeviceIdx].sharesetId == 0)
	{
		m_activeChannelConfig.Deserialize((AkUInt32)((uintptr_t)m_speakerConfigToggle->SelectedValue()));
	}
	else
	{
		m_activeChannelConfig = AkChannelConfig();
		m_speakerConfigToggle->SetSelectedIndex(0);
	}
}

void DemoOptions::InitSettings_Changed(void* in_pSender, ControlEvent* in_pControlEvent)
{
	AkMemSettings& memSettings = IntegrationDemo::Instance().GetMemSettings();
	AkInitSettings& initSettings = IntegrationDemo::Instance().GetInitSettings();
	AkPlatformInitSettings&	platformInitSettings = IntegrationDemo::Instance().GetPlatformInitSettings();
	AK::JobWorkerMgr::InitSettings& jobWorkerMgrSettings = IntegrationDemo::Instance().GetJobWorkerSettings();

	// Term first, so the term logic can inspect the previous settings
	IntegrationDemo::Instance().TermWwise();
	
	//Change all settings with our currently selected options	
	FillOutputSettingsFromControl(initSettings.settingsMainOutput);
	initSettings.uNumSamplesPerFrame = (AkUInt32)(AkUIntPtr)((ToggleControl*)FindControl(s_labelFrameSize))->SelectedValue();
	initSettings.bDebugOutOfRangeCheckEnabled = (AkUIntPtr)((ToggleControl*)FindControl(s_labelRangeCheckEnable))->SelectedValue() != 0;
	initSettings.fDebugOutOfRangeLimit = (float)(AkUIntPtr)((ToggleControl*)FindControl(s_labelRangeCheckLimit))->SelectedValue();
	
	platformInitSettings.uNumRefillsInVoice = (AkUInt16)(AkUIntPtr)((ToggleControl*)FindControl(s_labelBuffers))->SelectedValue();
	demoOptionsUtil::FillPlatformSpecificSettings(m_Controls, &platformInitSettings);

#if defined(AK_SUPPORT_THREADS)
	//Update the job worker settings, then re-generate jobmgr settings with this new info
	{
		AkUInt32 uNumJobWorkers = (AkUInt32)((NumericControl*)FindControl(s_labelMaxJobWorkers))->GetValue();;
		// If initializing with jobs, init the workerMgr with as many threads as possible
		jobWorkerMgrSettings.uNumWorkerThreads = uNumJobWorkers > 0 ? AK_MAX_WORKER_THREADS : 0;
		
		AkJobMgrSettings jobMgrSettings = jobWorkerMgrSettings.GetJobMgrSettings();
		for (AkUInt32 uJobType = 0; uJobType < AK_NUM_JOB_TYPES; ++uJobType)
		{
			jobMgrSettings.uMaxActiveWorkers[uJobType] = uNumJobWorkers;
		}
		initSettings.settingsJobManager = jobMgrSettings;
	}
#endif

	memSettings.uMemAllocationSizeLimit = (AkUInt64)(AkUIntPtr)((ToggleControl*)FindControl(s_labelMaxMemory))->SelectedValue();
	memSettings.uMemoryDebugLevel = (AkUInt32)(AkUIntPtr)((ToggleControl*)FindControl(s_labelMemoryDebug))->SelectedValue();	
	memSettings.uMaxThreadLocalHeapAllocSize = (AkUInt32)(AkUIntPtr)((ToggleControl*)FindControl(s_labelMaxThreadAllocSize))->SelectedValue();

	AkOSChar errorMsg[200];	
	demoOptionsUtil::PreInitWwise();
	bool bSuccess = IntegrationDemo::Instance().InitWwise(errorMsg, 200);

	if (!bSuccess)
	{		
		char *charMsg;
		CONVERT_OSCHAR_TO_CHAR(errorMsg, charMsg);
		m_InternalErrorMsg = charMsg;
	}
	else
	{
		demoOptionsUtil::PostInitWwise();
		//Re-load the Init.bnk
		AkBankID bankID;
		AKRESULT res = AK::SoundEngine::LoadBank("Init.bnk", bankID);
		if (res != AK_Success)
			m_InternalErrorMsg += " (Init.bnk)"; //On top of the error reported in local output
		else
			m_InternalErrorMsg.clear();
	}	
}

void DemoOptions::FillOutputSettingsFromControl(AkOutputSettings& newSettings)
{	
	DeviceId newDeviceId = m_activeDeviceIdx < m_deviceIds.size() ?
		m_deviceIds[m_activeDeviceIdx] : DeviceId(IntegrationDemo::GetDefaultAudioDeviceSharesetId(), 0);

	AkChannelConfig newChannelConfig = m_activeChannelConfig;

	m_spatialAudioDeviceAvailable = (AK::SoundEngine::GetDeviceSpatialAudioSupport(newDeviceId.deviceId) == AK_Success);
	
	newSettings.audioDeviceShareset = (m_spatialAudioRequested && m_spatialAudioDeviceAvailable) ?
		IntegrationDemo::GetSpatialAudioSharesetId() : newDeviceId.sharesetId;

	newSettings.idDevice = newDeviceId.deviceId;
	newSettings.ePanningRule = m_activePanningRule;
	if (newChannelConfig.IsValid())
	{
		newSettings.channelConfig = newChannelConfig;
	}
	else
	{
		newSettings.channelConfig.Clear();
	}	
}

Control* DemoOptions::FindControl(const char* in_label)
{
	for (size_t i = 0; i < m_Controls.size(); i++)
	{
		if (m_Controls[i]->GetLabel() == in_label)
			return m_Controls[i];
	}
	return NULL;
}

void DemoOptions::outputDeviceOption_Changed(void* in_pSender, ControlEvent* in_pControlEvent)
{
	ToggleControl& toggleControl = *((ToggleControl*)in_pSender);
	m_activeDeviceIdx = toggleControl.SelectedIndex();
	UpdateSpeakerConfigForShareset();
	updateOutputDevice();
}

void DemoOptions::spatialAudioOption_Changed(void* in_pSender, ControlEvent* in_pControlEvent)
{
	ToggleControl& toggleControl = *((ToggleControl*)in_pSender);
	m_spatialAudioRequested = ((uintptr_t)toggleControl.SelectedValue()) != 0;
	updateOutputDevice();
}

void DemoOptions::speakerPosOption_Changed(void* in_pSender, ControlEvent* in_pControlEvent)
{
	ToggleControl& toggleControl = *((ToggleControl*)in_pSender);
	m_activePanningRule = (AkPanningRule)((uintptr_t)toggleControl.SelectedValue());
	updateOutputDevice();
}

void DemoOptions::speakerConfigOption_Changed(void* in_pSender, ControlEvent* in_pControlEvent)
{
	UpdateSpeakerConfigForShareset();
	updateOutputDevice();
}

void DemoOptions::jobSettings_Changed(void* in_pSender, ControlEvent* in_pControlEvent)
{
	// If we already have the job system set up, and we're going to a worker count that's still valid,
	// then just call SetJobMgrMaxActiveWorkers
	AkUInt32 uNumJobWorkers = (AkUInt32)((NumericControl*)FindControl(s_labelMaxJobWorkers))->GetValue();
	if (IntegrationDemo::Instance().GetJobWorkerSettings().uNumWorkerThreads > 0 &&
		uNumJobWorkers > 0)
	{
		for (AkUInt32 uJobType = 0; uJobType < AK_NUM_JOB_TYPES; ++uJobType)
		{
			IntegrationDemo::Instance().GetInitSettings().settingsJobManager.uMaxActiveWorkers[uJobType] = uNumJobWorkers;
			AK::SoundEngine::SetJobMgrMaxActiveWorkers(uJobType, uNumJobWorkers);
		}
	}
	else // otherwise fully reinitialize the soundengine
	{
		InitSettings_Changed(in_pSender, in_pControlEvent);
	}
}

void DemoOptions::updateOutputDevice()
{
	if (!AK::SoundEngine::IsInitialized())
	{
		InitSettings_Changed(nullptr, nullptr);
		return;
	}

	AkOutputSettings newSettings;
	FillOutputSettingsFromControl(newSettings);

	AK::SoundEngine::ReplaceOutput(newSettings, 0/*Main Output*/);	
}

//Have 2 columns of controls instead of just one.
void DemoOptions::PositionControls(int in_First, int in_Last, int in_Col)
{	
	int iHeight = m_pParentMenu->GetHeight();
	int iLineHeight = GetLineHeight(DrawStyle_Text);

	// Figure out how far apart (vertically) the controls should be
	int iTopControl = TITLE_POS_Y + iLineHeight*2;  // Y-Position of the top control
	int iControlsDist = ((iHeight - iTopControl) - iLineHeight) / (int)m_Controls.size();
	if (iControlsDist > iLineHeight)
	{
		iControlsDist = iLineHeight;
	}

	if (in_Col > 0)
		iTopControl += iLineHeight;	//On many platforms the device name can be very long.  Let's leave space on the first line.

	// Set the positions of the coordinates
	int iWidth = m_pParentMenu->GetWidth();

	//Put all controls in 2 columns
	int iCumul = iTopControl;
	int iColumn = in_Col == 0 ? iWidth / 8 : iWidth /2;
	for (int i = in_First; i < in_Last; i++)
	{
		m_Controls[i]->SetPosition(iColumn, iCumul + iControlsDist);
		iCumul += iControlsDist;		
	}
}

//Overriden from base class to have an error line and allow controls to be drawn in "text" style.
void DemoOptions::Draw()
{
	demoOptionsUtil::UpdatePlatformSpecificControls(m_Controls);

	int iHeight = m_pParentMenu->GetHeight();
	int iWidth = m_pParentMenu->GetWidth();
	
	// Draw the page title
	DrawTextOnScreen(m_szPageTitle.c_str(), iWidth / 8, TITLE_POS_Y, DrawStyle_Title);

	// Tell the controls to draw themselves
	for (int i = 0; i < (int)m_Controls.size(); i++)
	{		
		if (i == m_iSelectedIndex)
		{
#if defined AK_ANDROID || defined AK_IOS
			m_Controls[i]->Draw(DrawStyle_Selected);
#else
			m_Controls[i]->Draw(DrawStyle_SelectedText);
#endif
		}
		else
		{
#if defined AK_ANDROID || defined AK_IOS
			m_Controls[i]->Draw(DrawStyle_Control);
#else
			m_Controls[i]->Draw(DrawStyle_Text);
#endif
		}
	}

	int iLineHeight = GetLineHeight(DrawStyle_Text);
	// Draw any init errors
	if (!m_InternalErrorMsg.empty())
	{
		DrawTextOnScreen(m_InternalErrorMsg.c_str(), 70, iHeight - 4* iLineHeight, DrawStyle_Error);
	}
	// Also draw other errors reported by wwise
	if (ErrorOccured())
	{
		int iBottomY = iHeight - 2 * GetLineHeight(DrawStyle_Text);
		int lines = 0;
		size_t errorLength = AkMin(INTEGRATIONDEMO_MAX_MESSAGE, m_szError.length());
		for (size_t i = 0; i < errorLength; i++)
			lines += m_szError.c_str()[i] == '\n';

		int height = GetLineHeight(DrawStyle_Error);
		DrawTextOnScreen(m_szError.c_str(), 20, iBottomY - lines * height, DrawStyle_Error);
	}
}
