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

// MenuPages.h
/// \file 
/// Include this file to have access to all Menu Pages.

#pragma once

#include "BaseMenuPage.h"
#include "DemoRTPCCarEngine.h"
#include "DemoDialogueMenu.h"
#include "DemoDynamicDialogue.h"
#include "DemoLocalization.h"
#include "DemoMarkers.h"
#include "DemoMusicMIDIMenu.h"
#include "DemoExternalSources.h"
#include "DemoFootstepsManyVariables.h"
#include "DemoPositioningMenu.h"
#include "DemoSpatialAudioMenu.h"
#include "DemoBankLoadingMenu.h"

#if defined AK_PLATFORM_SUPPORTS_MOTION
#include "DemoMotion.h"
#endif

#if defined INTEGRATIONDEMO_MICROPHONE
#include "DemoMicrophone.h"
#endif 

#include "DemoOptions.h"
