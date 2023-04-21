#pragma once

#include <opengl.h>
#include "core/ui/canvas.h"
#include "core/entity/component.h"

namespace UI {
class UIComponent : public Component<UIComponent> {
friend class Canvas;
protected:
    Canvas* canvas_ = nullptr;
public:
    UIComponent(Canvas* c, int p = 0) {
        canvas_ = c;
        canvas_->AddUIComponent(this, p);
    }
    UIComponent() { }
    virtual ~UIComponent() {
        if (canvas_ != nullptr)
            canvas_->RemoveUIComponent(this);
    }
    virtual void Render(const glm::mat4&) { }
    virtual void UpdateWindowSize() { }
};
};