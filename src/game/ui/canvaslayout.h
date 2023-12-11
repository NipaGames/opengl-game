#pragma once

#include <core/ui/canvas.h>
#include <core/ui/component/uicomponent.h>
#include <core/graphics/renderer.h>

class CanvasLayout : public UI::Canvas {
private:
    std::vector<UI::UIComponent*> ownedComponents_;
    std::unordered_map<std::string, std::vector<UI::UIComponent*>> pages_;
    bool skipNextUpdate_ = false;
public:
    virtual ~CanvasLayout();
    CanvasLayout() = default;
    CanvasLayout(const CanvasLayout&) = delete;
    CanvasLayout(CanvasLayout&&) = default;
    CanvasLayout& operator=(const CanvasLayout&) = delete;
    CanvasLayout& operator=(CanvasLayout&&);
    virtual void CreateHUDElements() = 0;
    virtual void AssignToRenderer(Renderer&) = 0;
    virtual void Start();
    virtual void Update();
    virtual void Destroy();
    void SetCurrentPage(const std::string&);
    void CloseCurrentPage();
    template <typename T>
    T* AddUIComponent(const std::string& page = "", int priority = 0) {
        T* component = new T(this, priority);
        UI::UIComponent* uiComponent = dynamic_cast<UI::UIComponent*>(component);
        uiComponent->transformFrom = UI::UITransformFrom::UI_TRANSFORM;
        ownedComponents_.push_back(uiComponent);
        if (!page.empty()) {
            pages_[page].push_back(uiComponent);
        }
        uiComponent->AddToCanvas();
        return component;
    }
};