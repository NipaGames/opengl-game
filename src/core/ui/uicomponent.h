#pragma once

#include <opengl.h>
#include "core/ui/canvas.h"
#include "core/entity/component.h"

namespace UI {
class UIComponent : public Component<UIComponent> {
friend class Canvas;
protected:
    Canvas* canvas_ = nullptr;
    int priority_ = 0;
    bool isAddedToCanvas_ = false;
public:
    UIComponent(Canvas* c, int p = 0) {
        canvas_ = c;
        priority_ = p;
    }
    UIComponent() { }
    virtual ~UIComponent() {
        if (isAddedToCanvas_ && canvas_ != nullptr)
            canvas_->RemoveUIComponent(this);
    }
    virtual void AddToCanvas() {
        canvas_->AddUIComponent(this, priority_);
        isAddedToCanvas_ = true;
    }
    virtual void Render(const glm::mat4&) { }
    virtual void UpdateWindowSize() { }
};
};