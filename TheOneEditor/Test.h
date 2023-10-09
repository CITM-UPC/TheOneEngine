#pragma once

#ifndef __TEST_H__
#define __TEST_H__

#include "Module.h"


class Test : public Module
{
public:

	Test();

	// Destructor
	virtual ~Test();

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

#endif // __TEST_H__