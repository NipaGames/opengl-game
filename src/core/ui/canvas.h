#pragma once

#include <opengl.h>
#include <map>
#include <core/graphics/shader.h>
#include <core/graphics/shape.h>

namespace UI {
    class UIComponent;
    class Canvas {
    private:
        std::multimap<int, UI::UIComponent*> components_;
        Shader bgShader_;
        Shape bgShape_;
    public:
        bool isVisible = true;
        glm::ivec2 offset = glm::ivec2(0, 0);
        glm::vec4 bgColor = glm::vec4(0.0f);
        glm::vec2 bgSize = glm::vec2(1280.0f, 720.0f);

        Canvas();
        virtual ~Canvas();
        void Draw();
        void AddUIComponent(UI::UIComponent*, int = 0);
        void RemoveUIComponent(const UI::UIComponent*);
        void UpdateWindowSize();
        glm::mat4 GetProjectionMatrix() const;
    };
};