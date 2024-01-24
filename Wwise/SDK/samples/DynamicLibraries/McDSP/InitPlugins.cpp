#include <AK/SoundEngine/Common/IAkPlugin.h>
#include <AK/Plugin/McDSPFutzBoxFXFactory.h>
#include <AK/Plugin/McDSPLimiterFXFactory.h>
DEFINE_PLUGIN_REGISTER_HOOK

#ifdef _DEBUG
AkAssertHook g_pAssertHook = NULL;
#endif