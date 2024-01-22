#pragma once

#ifdef THEONESCRIPTING_EXPORTS
#define THEONESCRIPTING_API __declspec(dllexport)
#else
#define THEONESCRIPTING_API __declspec(dllimport)
#endif

#define THEONE_API extern "C" ALIEN_ENGINE_API

extern "C" THEONESCRIPTING_API void OnCreate();

extern "C" THEONESCRIPTING_API void OnPreUpdate();

extern "C" THEONESCRIPTING_API void OnUpdate(double dt);

extern "C" THEONESCRIPTING_API void OnFixedUpdate(double dt);

extern "C" THEONESCRIPTING_API void OnPostUpdate();

extern "C" THEONESCRIPTING_API void OnDestroy();
