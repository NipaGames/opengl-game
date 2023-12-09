#pragma once

#include <core/ui/canvas.h>
#include <core/ui/component/uicomponent.h>
#include <core/graphics/renderer.h>

class CanvasLayout : public UI::Canvas {
private:
    std::vector<UI::UIComponent*> ownedComponents_;
public:
    virtual ~CanvasLayout();
    CanvasLayout() = default;
    CanvasLayout(const CanvasLayout&) = delete;
    CanvasLayout(CanvasLayout&&) = default;
    CanvasLayout& operator=(const CanvasLayout&) = delete;
    CanvasLayout& operator=(CanvasLayout&&) = default;
    virtual void CreateHUDElements() = 0;
    virtual void AssignToRenderer(Renderer&) = 0;
    virtual void Start();
    virtual void Update();
    virtual void Destroy();
    template <typename T>
    T* AddUIComponent() {
        T* component = new T();
        UI::UIComponent* uiComponent = dynamic_cast<UI::UIComponent*>(component);
        uiComponent->transformFrom = UI::UITransformFrom::UI_TRANSFORM;
        ownedComponents_.push_back(uiComponent);
        uiComponent->AddToCanvas(GetCanvas());
        return component;
    }
};