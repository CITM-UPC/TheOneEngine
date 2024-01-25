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

// IntegrationDemo.h
/// \file 
/// Declares a wrapper class for the integration demo application.

#pragma once

#include <AK/SoundEngine/Common/AkTypes.h>

#include <AK/SoundEngine/Common/AkMemoryMgr.h>		// Memory Manager
#include <AK/SoundEngine/Common/AkModule.h>			// Default memory and stream managers
#include <AK/SoundEngine/Common/IAkStreamMgr.h>		// Streaming Manager
#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine
#include <AK/MusicEngine/Common/AkMusicEngine.h>	// Music Engine
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>	// AkStreamMgrModule
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>	// Spatial Audio module
#include <AK/SoundEngine/Common/AkCallback.h>    // Callback
#include "AkJobWorkerMgr.h"

#ifndef AK_OPTIMIZED
#include <AK/Comm/AkCommunication.h>	// Communication between Wwise and the game (excluded in release build)
#endif

#include "Drawing.h"
#include "UniversalInput.h"

#include "AkFilePackageLowLevelIODeferred.h"
#include "IAkInputMgr.h"

/////////////////////////
//  GLOBAL CONSTANTS
/////////////////////////

static const AkUInt16 DESIRED_FPS   = 60;
static const AkReal32 MS_PER_FRAME  = ( 1000 / (AkReal32)DESIRED_FPS );

static const AkGameObjectID LISTENER_ID = 10000;

// Android devices show poor performance with more than 2 worker threads
#if defined(AK_SUPPORT_THREADS)
#ifdef AK_ANDROID
static const AkUInt32 AK_MAX_WORKER_THREADS = 2;
#else
static const AkUInt32 AK_MAX_WORKER_THREADS = 8;
#endif
#else
static const AkUInt32 AK_MAX_WORKER_THREADS = 0;
#endif


// Please use foward declaration to keep to reduce include size
class Menu;

#ifndef AK_OPTIMIZED
#include <AK/SoundEngine/Common/AkXMLErrorMessageTranslator.h>
#endif //AK_OPTIMIZED

//////////////////////////////
//  IntegrationDemo Class
//////////////////////////////

/// Wrapper class for the Integration Demo application. Implements a Singleton design pattern.
class IntegrationDemo
{
public:

	/// Returns a reference to the instance of the Integration Demo application.
	/// \return Reference to the instance of the IntegrationDemo object.
	static IntegrationDemo& Instance();
	

	/// Accessor to the low-level IO manager.
	/// \return Reference to the Low-level IO manager used by the demo application.
	CAkFilePackageLowLevelIODeferred& IOManager();	

	/// Initializes the Integration Demo application.
	/// \return True if the application's modules initialized properly and false otherwise.
	bool Init(
		void* in_pParam,                        ///< - Extra parameter that might be necessary to initialize the platform specific components of the application
		IAkInputMgr* in_pInputMgr,              ///< - Input handler
		AkOSChar* in_szErrorBuffer,             ///< - Buffer where error details will be written (if the function returns false)
		unsigned int in_unErrorBufferCharCount, ///< - Number of characters available in in_szErrorBuffer, including terminating NULL character
		AkUInt32 in_windowWidht = 640,
		AkUInt32 in_windowHeight = 480
			  );

	/// Gets the settings last used to re-initialize the Sound Engine.  This is used by the DemoOption page, to reinitialize the sound engine with various settings.
	AkMemSettings& GetMemSettings() { return m_memSettings; }	
	AkStreamMgrSettings& GetStreamMgrSettings() { return m_stmSettings; }
	AkDeviceSettings& GetDeviceSettings() {	return m_deviceSettings; }
	AkInitSettings& GetInitSettings() { return m_initSettings; }
	AkPlatformInitSettings& GetPlatformInitSettings() { return m_platformInitSettings; }
	AkMusicSettings& GetMusicSettings() { return m_musicInit; }
	AK::JobWorkerMgr::InitSettings& GetJobWorkerSettings() { return m_jobWorkerSettings; }
#ifndef AK_OPTIMIZED
	AkCommSettings& GetCommSettings() { return m_commSettings; }
#endif

	/// Initializes the Wwise sound engine only, not the IntegrationDemo app.  Also used to change critical settings in DemoOption page.
	bool InitWwise(
		AkOSChar* in_szErrorBuffer,				///< - Buffer where error details will be written (if the function returns false)
		unsigned int in_unErrorBufferCharCount	///< - Number of characters available in in_szErrorBuffer, including terminating NULL character
	);

	/// Terminates the Wwise sound engine only, not the IntegrationDemo app
	void TermWwise();

	/// Call this method at the start of every game loop itteration to signal the 
	/// application to start a new frame.
	void StartFrame();

	/// Processes a single frame of the application.
	/// \return False when the user wants to quit the Integration Demo, otherwise True.
	bool Update();

	void OnPointerEvent( PointerEventType in_eType, int in_x, int in_y );

	/// Back button pressed.
	void OnBack();

	/// Renders a single frame of the application.
	void Render();

	/// Call this method at the end of every game loop itteration to signal the
	/// application to end the current frame and regulate framerate.
	void EndFrame();

	/// Uninitializes the application's components and releases any used resources.
	void Term();
	
	/// Get the application input mgr
	IAkInputMgr* GetInputMgr()  { return m_pInput; }

#if defined (OLD_INTDEMO_RENDER)
	/// Get the application window height
	AkUInt32 GetWindowWidth() { return ::GetWindowWidth(); }

	/// Get the application window width
	AkUInt32 GetWindowHeight() { return ::GetWindowHeight(); }
#else
	/// Get the application window height
	AkUInt32 GetLayoutWidth() { return ::GetLayoutWidth(); }

	/// Get the application window width
	AkUInt32 GetLayoutHeight() { return ::GetLayoutHeight(); }
#endif

	void Mute();
	void Unmute();
	void Pause();
	void Resume();
	
	inline void SetGamePaused(bool in_bPaused) { m_bGamePaused = in_bPaused; }
	inline bool IsGamePaused() { return m_bGamePaused; }

	// Helpers for configuring Audio settings	
	AkOutputDeviceID GetOutputDeviceId() const { return AK::SoundEngine::GetOutputID((AkUniqueID)0, 0); }
	
	static AkUInt32 GetDefaultAudioDeviceSharesetId();
	static const std::vector<const char*> GetSupportedAudioDeviceSharesets();
	static AkUInt32 GetSpatialAudioSharesetId();
	static AkUInt32 GetDefaultAudioDeviceId();

	static void ResourceMonitorDataCallback(const AkResourceMonitorDataSummary * in_pdataSummary);
	static AkResourceMonitorDataSummary ResourceDataSummary;
private:

	/// Hidden default constructor.
	IntegrationDemo();

	/// Hidden copy constructor.
	IntegrationDemo( IntegrationDemo const& ){};

	/// Hidden assignment operator.
	IntegrationDemo& operator=( IntegrationDemo const& );
	
	/// Hidden default desctructor.
	~IntegrationDemo();	

	/// Gets the default settings for the various sub components of the sound engine.  Modify these settings to suit your application better.
	/// The settings should be modified prior to the call AK::SoundEngine::Init to be effective.
	void GetDefaultSettings();

	/// Calculates the amount of time to sleep at the end of each frame.
	AkInt32 CalculateSleepTime();
	// Menu System Objects
	IAkInputMgr* m_pInput;	///< The application's input manager pointer.
	Menu*     m_pMenu;	///< The menu system pointer.

	//The callback function called when the AK::Monitor does a localoutput
	static void LocalErrorCallback(
		AK::Monitor::ErrorCode in_eErrorCode,	///< Error code number value
		const AkOSChar* in_pszError,	///< Message or error string to be displayed
		AK::Monitor::ErrorLevel in_eErrorLevel,	///< Specifies whether it should be displayed as a message or an error
		AkPlayingID in_playingID,   ///< Related Playing ID if applicable, AK_INVALID_PLAYING_ID otherwise
		AkGameObjectID in_gameObjID ///< Related Game Object ID if applicable, AK_INVALID_GAME_OBJECT otherwise
	);;

	/// We're using the default Low-Level I/O implementation that's part
	/// of the SDK's sample code, with the file package extension
	CAkFilePackageLowLevelIODeferred* m_pLowLevelIO;

	// Timing Variables
	AkInt64  m_PerfFreq;			///< The system's performance frequency.
	AkInt64  m_FrameStartPerf;		///< Performance counter value at the start of a frame.
	
	bool m_bGamePaused;	

	//Settings kept for reinitialization in DemoOptions page.
	AkMemSettings			m_memSettings;
	AkStreamMgrSettings		m_stmSettings;
	AkDeviceSettings		m_deviceSettings;
	AK::JobWorkerMgr::InitSettings m_jobWorkerSettings;
	AkInitSettings			m_initSettings;
	AkPlatformInitSettings	m_platformInitSettings;
	AkMusicSettings			m_musicInit;

#ifndef AK_OPTIMIZED
	AkCommSettings			m_commSettings;
	AkXMLErrorMessageTranslator m_xmlMsgTranslator;
#endif

#if defined(AK_ENABLE_FRAME_LIMITATION)
	int m_iFrameCounter;
#endif
};
