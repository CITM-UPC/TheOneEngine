#ifndef __CANVAS_H__
#define __CANVAS_H__
#pragma once

#include "Component.h"
#include "ItemUI.h"

class Canvas : public Component
{

public:

    Canvas(std::shared_ptr<GameObject> containerGO);
    virtual ~Canvas();

    template <typename TUI>
    bool AddItemUI()
    {
        std::unique_ptr<ItemUI> newItemUI = std::make_unique<TUI>(containerGO.lock());
        uiElements.push_back(std::move(newItemUI));

        return true;
    }

    void DrawComponent();

    void SetTo2DRenderSettings(const bool& setTo);

    // @Get / Set --------------------------------
    Rect2D GetRect() const;

    void SetSize(float width, float height);

public:

    json SaveComponent();
    void LoadComponent(const json& transformJSON);

    bool debugDraw = true;

private:
    Rect2D rect = { 0,0,50,50 };
    std::vector<std::unique_ptr<ItemUI>> uiElements;


};

#endif //  !__CANVAS_H__