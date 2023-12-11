#pragma once

#include <opengl.h>

#include "../canvas.h"
#include <core/entity/component.h>
#include <core/eventhandler.h>

namespace UI {
    struct UITransform {
        glm::vec2 pos;
        float size;
    };
    struct Rect {
        float left, right, top, bottom;
    };
    enum class UITransformFrom {
        ENTITY_TRANSFORM_2D,
        UI_TRANSFORM
    };
    class UIComponent : public Component<UIComponent> {
    friend class Canvas;
    protected:
        Canvas* canvas_ = nullptr;
        int priority_ = 0;
        bool isAddedToCanvas_ = false;
    public:
        UITransform transform { glm::vec2(0.0f), 1.0f };
        UITransformFrom transformFrom = UITransformFrom::ENTITY_TRANSFORM_2D;
        EventHandler<std::string> eventHandler;
        bool isActive = true;
        bool isVisible = true;
        UIComponent(Canvas*, int = 0);
        UIComponent() = default;
        virtual ~UIComponent();
        UITransform GetTransform();
        virtual void SetTransform(const UITransform&);
        virtual void AddToCanvas();
        virtual void AddToCanvas(Canvas*);
        virtual void Render(const glm::mat4&) { }
        virtual void UpdateWindowSize() { }
    };
};