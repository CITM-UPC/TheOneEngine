#pragma once


class Component
{
public:
	
	Component(/*GameObject* attachedGO*/) : active(false) {}
	~Component(){}

	virtual bool IsActive() { return active; }
	
	void Enable() {
		if (!active)
		{
			active = true;
		}
	}
	
	void Disable() {
		if (active)
		{
			active = false;
		}
	}

private:

	bool active;
};