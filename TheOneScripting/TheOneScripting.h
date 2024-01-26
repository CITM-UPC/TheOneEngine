#pragma once

#include "pch.h"

///*ENGINE ACCESSIBLE COMPONENTS*/
//#include "..\TheOneEngine\GameObject.h"
//#include "..\TheOneEngine\Component.h"
//#include "..\TheOneEngine\Transform.h"
//#include "..\TheOneEngine\Mesh.h"
//#include "..\TheOneEngine\Camera.h"
//#include "..\TheOneEngine\Texture.h"
//#include "..\TheOneEngine\Script.h"
//#include "..\TheOneEngine\CPPScript.h"
//
///*EDITOR ACCESSIBLE COMPONENTS*/
//#include "..\TheOneEditor\SceneManager.h"

#ifdef THEONESCRIPTING_EXPORTS
#define THEONESCRIPTING_API __declspec(dllexport)
#else
#define THEONESCRIPTING_API __declspec(dllimport)
#endif

#define THEONE_API extern "C" THEONESCRIPTING_API

////Called even if the script is disabled and the GameObject is still enabled? Is called just once since the script is loaded. Works as an Awake function.
//THEONE_API void OnMelt();
//
////Called every time the GameObject is created and the scene starts. Works as a Start function.
//THEONE_API void OnCast();
//
////Called each frame
//THEONE_API void OnPrePolishing();
//
////Called each frame with deltaTime variable for having a timestep performance
//THEONE_API void OnPolishing(double dt);
//
////Called each second (not frame)
//THEONE_API void OnFixedPolishing(double dt);
//
////PostUpdate function for a script, is called after the Update function
//THEONE_API void OnEngraving();
//
////Called when the GameObject is destroyed.
//THEONE_API void OnDestroy();
//
////If the GameoObject detects collision
//THEONE_API void OnClash();
