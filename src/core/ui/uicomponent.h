#pragma once

#include <opengl.h>
#include "core/entity/component.h"

namespace UI {
class UIComponent : public Component<UIComponent> {
friend class Canvas;
protected:
    Canvas* canvas_;
public:
    virtual ~UIComponent() { }
    virtual void Render(const glm::mat4&) { }
    virtual void UpdateWindowSize() { }
};
};