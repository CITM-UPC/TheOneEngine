#include "Panel.h"

Panel::Panel(PanelType type) : type(type) {}

Panel::~Panel() {}

void Panel::SetEnabled(bool enabled) { this->enabled = enabled; }

bool Panel::isEnabled() { return enabled; }

PanelType Panel::GetType() { return type; }