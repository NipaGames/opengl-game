#pragma once

#include "core/entity/component.h"

class IUIComponent : public Component<IUIComponent> {
public:
    virtual ~IUIComponent() = default;
    virtual void IRender(const glm::mat4&) { }
};

template<class Derived>
class UIComponent : public IUIComponent {
public:
    virtual ~UIComponent() { }
    void IRender(const glm::mat4& projection) override { static_cast<Derived*>(this)->Render(projection); }
    virtual void Render(const glm::mat4&) { }
};