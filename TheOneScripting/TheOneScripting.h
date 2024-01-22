#pragma once

#ifdef THEONESCRIPTING_EXPORTS
#define THEONESCRIPTING_API __declspec(dllexport)
#else
#define THEONESCRIPTING_API __declspec(dllimport)
#endif

#define THEONE_API extern "C" THEONESCRIPTING_API

THEONE_API void OnCreate();

THEONE_API void OnPreUpdate();

THEONE_API void OnUpdate(double dt);

THEONE_API void OnFixedUpdate(double dt);

THEONE_API void OnPostUpdate();

THEONE_API void OnDestroy();
