#ifndef __CANVAS_H__
#define __CANVAS_H__
#pragma once

#include "Component.h"
#include "ItemUI.h"

class Canvas : public Component
{

public:

    Canvas(std::shared_ptr<GameObject> containerGO);
    Canvas(std::shared_ptr<GameObject> containerGO, Canvas* ref);
    virtual ~Canvas();

    template <typename TUI>
    unsigned int AddItemUI()
    {
        std::unique_ptr<ItemUI> newItemUI = std::make_unique<TUI>(containerGO.lock());
        unsigned int tempID = newItemUI->GetID();
        uiElements.push_back(std::move(newItemUI));

        return tempID;
    }

    template <typename TUI>
    unsigned int AddItemUI(std::string path)
    {
        std::unique_ptr<ItemUI> newItemUI = std::make_unique<TUI>(containerGO.lock(), path);
        unsigned int tempID = newItemUI->GetID();
        uiElements.push_back(std::move(newItemUI));

        return tempID;
    }

    template <typename TUI>
    unsigned int AddCopiedItemUI(TUI* ref)
    {
        std::unique_ptr<ItemUI> newItemUI = std::make_unique<TUI>(containerGO.lock(), ref);
        unsigned int tempID = newItemUI->GetID();
        uiElements.push_back(std::move(newItemUI));

        return tempID;
    }

    template <typename TUI>
    TUI* GetItemUI(unsigned int id)
    {
        for (const auto& component : uiElements)
        {
            if (dynamic_cast<TUI*>(component.get()))
            {
                if (component.get()->GetID() == id)
                {
                    return static_cast<TUI*>(component.get());
                }
            }
        }
        return nullptr;
    }

    bool RemoveItemUI(unsigned int id);

    void DrawComponent(Camera* camera);

    void SetTo2DRenderSettings(Camera* camera, const bool& setTo);

    // @Get / Set --------------------------------
    Rect2D GetRect() const;

    void SetSize(float width, float height);

public:

    json SaveComponent();
    void LoadComponent(const json& canvasJSON);

    std::vector<ItemUI*> GetUiElements();

    bool debugDraw = true;

private:
    Rect2D rect = { 0,0,2,2 };
    std::vector<std::unique_ptr<ItemUI>> uiElements;
};

#endif //  !__CANVAS_H__