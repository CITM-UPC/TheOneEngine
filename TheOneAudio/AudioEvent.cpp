#include "AudioEvent.h"
#include "AudioCore.h"
AudioEvent::AudioEvent()
{
	playing_id = 0L;
	event_call_back = &AudioCore::EventCallBack;
}

bool AudioEvent::IsEventPlaying()
{
	return playing_id != 0L;
}