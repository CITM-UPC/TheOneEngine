#ifndef __BUILDER_MODULE_H__
#define __BUILDER_MODULE_H__
#pragma once

class BuilderApp;

class BuilderModule
{
public:
	// Private constructor to enforce singleton pattern
	BuilderModule() {}
	BuilderModule(BuilderApp* parent) : active(false), app(parent)
	{}

	void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake()
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(double dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	void Enable() {
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Disable() {
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

public:

	bool active;
	BuilderApp* app;
};

#endif // !__BUILDER_MODULE_H__