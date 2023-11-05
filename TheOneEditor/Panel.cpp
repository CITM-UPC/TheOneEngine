#include "Panel.h"

Panel::Panel(PanelType type, std::string name) : type(type), name(name) {}

Panel::~Panel() {}

PanelType Panel::GetType() { return type; }

std::string Panel::GetName() { return name; }

bool Panel::GetState() { return enabled; }

void Panel::SetState(bool enabled) { this->enabled = enabled; }

void Panel::SwitchState()
{  
	enabled = enabled == true ? false : true;
}