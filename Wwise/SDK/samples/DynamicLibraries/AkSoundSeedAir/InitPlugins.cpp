#include <AK/SoundEngine/Common/IAkPlugin.h>
#include <AK/Plugin/AkSoundSeedWindSourceFactory.h>
#include <AK/Plugin/AkSoundSeedWooshSourceFactory.h>
DEFINE_PLUGIN_REGISTER_HOOK

#ifdef _DEBUG
AkAssertHook g_pAssertHook = NULL;
#endif