#include <AK/SoundEngine/Common/IAkPlugin.h>
#include <AK/Plugin/iZHybridReverbFXFactory.h>
#include <AK/Plugin/iZTrashBoxModelerFXFactory.h>
#include <AK/Plugin/iZTrashDelayFXFactory.h>
#include <AK/Plugin/iZTrashDistortionFXFactory.h>
#include <AK/Plugin/iZTrashDynamicsFXFactory.h>
#include <AK/Plugin/iZTrashFiltersFXFactory.h>
#include <AK/Plugin/iZTrashMultibandDistortionFXFactory.h>
DEFINE_PLUGIN_REGISTER_HOOK

#ifdef _DEBUG
AkAssertHook g_pAssertHook = NULL;
#endif