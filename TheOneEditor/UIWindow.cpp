#include "UIWindow.h"

UIWindow::UIWindow(UIWindowType type) : type(type) {}

UIWindow::~UIWindow() {}

void UIWindow::SetEnabled(bool enabled) { this->enabled = enabled; }

bool UIWindow::isEnabled() { return enabled; }

UIWindowType UIWindow::GetType() { return type; }