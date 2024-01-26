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

#include "Page.h"
#include "Platform.h"
#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/AkCallback.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <string>

/// Options page to configure settings on the sound engine
class DemoOptions : public Page
{
public:
	struct DeviceId {
		DeviceId()
			: sharesetId(0)
			, deviceId(0)
		{}

		DeviceId(AkUInt32 in_sharesetId, AkUInt32 in_deviceId)
			: sharesetId(in_sharesetId)
			, deviceId(in_deviceId)
		{}

		AkUInt32 sharesetId;
		AkUInt32 deviceId;		
	};

	/// DemoOptions class constructor
	DemoOptions(Menu& in_ParentMenu);

	/// Initializes the page.
	/// \return True if successful and False otherwise.
	virtual bool Init();

	/// Releases resources used by the page.
	virtual void Release();

	/// Override of the Page::Draw() method.
	virtual void Draw();
	void InitControls();

private:

	void UpdateSpeakerConfigForShareset();
	void InitSpeakerPanning();
	void InitSpeakerConfig();
	void InitAudioSettings();
	void InitMemorySettings();
	void InitJobWorkerSettings();


	void outputDeviceOption_Changed(void* in_pSender, ControlEvent* in_pControlEvent);
	void spatialAudioOption_Changed(void* in_pSender, ControlEvent* in_pControlEvent);
	void speakerPosOption_Changed(void* in_pSender, ControlEvent* in_pControlEvent);
	void speakerConfigOption_Changed(void* in_pSender, ControlEvent* in_pControlEvent);
	void jobSettings_Changed(void* in_pSender, ControlEvent* in_pControlEvent);

	void InitSettings_Changed(void* in_pSender, ControlEvent* in_pControlEvent);	

	void PopulateOutputDeviceOptions();
	void FillOutputSettingsFromControl(AkOutputSettings& out_settings);
	void updateOutputDevice();

	static void LocalErrorCallback(
		AK::Monitor::ErrorCode in_eErrorCode,	///< Error code number value
		const AkOSChar* in_pszError,	///< Message or error string to be displayed
		AK::Monitor::ErrorLevel in_eErrorLevel,	///< Specifies whether it should be displayed as a message or an error
		AkPlayingID in_playingID,   ///< Related Playing ID if applicable, AK_INVALID_PLAYING_ID otherwise
		AkGameObjectID in_gameObjID); ///< Related Game Object ID if applicable, AK_INVALID_GAME_OBJECT otherwise


	Control* FindControl(const char* in_label);
	void PositionControls(int in_First, int in_Last, int in_Col);

	AkUInt32 m_activeDeviceIdx;
	std::vector<DeviceId> m_deviceIds;

	AkPanningRule m_activePanningRule;
	AkChannelConfig m_activeChannelConfig;
	bool m_spatialAudioRequested;
	bool m_spatialAudioDeviceAvailable;

	ToggleControl* m_speakerConfigToggle;	
	static DemoOptions* s_pThisDemoPage;
	string m_InternalErrorMsg;
};

