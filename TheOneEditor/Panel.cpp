#include "Panel.h"

Panel::Panel(PanelType type) : type(type) {}

Panel::~Panel() {}

bool Panel::GetState() { return enabled; }

void Panel::SetState(bool enabled) { this->enabled = enabled; }

void Panel::SwitchState()
{  
	enabled = enabled == true ? false : true;
}

PanelType Panel::GetType() { return type; }