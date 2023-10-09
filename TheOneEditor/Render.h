#pragma once

#ifndef __RENDER_H__
#define __RENDER_H__

#include "Module.h"


class Render : public Module
{
public:

	Render();

	// Destructor
	virtual ~Render();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();


public:


};

#endif // __RENDER_H__