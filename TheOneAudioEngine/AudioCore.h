#ifndef __AUDIOCORE_H__
#define __AUDIOCORE_H__
#pragma once

class AudioCore
{
public:
	AudioCore();

	virtual ~AudioCore();

	bool Awake();

	bool PreUpdate();

	bool Update(double dt);

private:

};

#endif // !__AUDIOCORE_H__