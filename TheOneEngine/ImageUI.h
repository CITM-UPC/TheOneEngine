#ifndef __IMAGEUI_H__
#define __IMAGEUI_H__
#pragma once

#include "ItemUI.h"
#include "Texture.h"

class ImageUI : public ItemUI
{
public:
	ImageUI(std::shared_ptr<GameObject> containerGO, Rect2D rect = { 0,0,1,1 });
	ImageUI(std::shared_ptr<GameObject> containerGO, const std::string& path, Rect2D rect = { 0,0,1,1 });
	~ImageUI();

	void Draw2D();

	json SaveUIElement() override;
	void LoadUIElement(const json& UIElementJSON) override;

private:
	std::string imagePath;
	std::unique_ptr<Texture> image;
};

#endif // !__IMAGEUI_H__