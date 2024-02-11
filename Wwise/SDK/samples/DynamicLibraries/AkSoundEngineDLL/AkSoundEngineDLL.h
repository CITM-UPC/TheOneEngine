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

#ifndef AK_SOUND_ENGINE_DLL_H_
#define AK_SOUND_ENGINE_DLL_H_

#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/Comm/AkCommunication.h>

namespace AK
{
	namespace SOUNDENGINE_DLL
	{
		AKSOUNDENGINE_API AKRESULT Init(
			AkMemSettings *     in_pMemSettings,
			AkStreamMgrSettings *  in_pStmSettings,
			AkDeviceSettings *  in_pDefaultDeviceSettings,
			AkInitSettings *    in_pSettings,
			AkPlatformInitSettings * in_pPlatformSettings,
			AkMusicSettings *	in_pMusicSettings,
			AkCommSettings *	in_pCommSettings = nullptr
			);
		AKSOUNDENGINE_API void     Term();

		AKSOUNDENGINE_API void     Tick();

		// File system interface.
		AKSOUNDENGINE_API AKRESULT SetBasePath(
			const AkOSChar*   in_pszBasePath
			);
	}
}

#endif //AK_SOUND_ENGINE_DLL_H_
