#include <AK/SoundEngine/Common/IAkPlugin.h>
#include <AK/Plugin/AkAudioInputSourceFactory.h>
DEFINE_PLUGIN_REGISTER_HOOK

#ifdef _DEBUG
AkAssertHook g_pAssertHook = NULL;
#endif