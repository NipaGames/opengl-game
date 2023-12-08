#pragma once

#include <opengl.h>
#include <map>
#include <memory>
#include <core/graphics/shader.h>
#include <core/graphics/shape.h>
#include <core/graphics/material.h>

namespace UI {
    class UIComponent;
    class Canvas {
    private:
        std::multimap<int, UI::UIComponent*> components_;
        Shader bgShader_;
        Shape bgShape_;
    public:
        bool isOwnedByRenderer = false;
        bool isVisible = true;
        glm::ivec2 offset = glm::ivec2(0, 0);
        std::shared_ptr<Material> bgMaterial = nullptr;
        glm::vec2 bgSize = glm::vec2(1280.0f, 720.0f);
        Canvas();
        Canvas(const Canvas&) = delete;
        Canvas(Canvas&&);
        Canvas& operator=(const Canvas&) = delete;
        Canvas& operator=(Canvas&&) = default;
        virtual ~Canvas();
        void GenerateBackgroundShape();
        void Draw();
        void AddUIComponent(UI::UIComponent*, int = 0);
        void RemoveUIComponent(const UI::UIComponent*);
        void UpdateWindowSize();
        glm::mat4 GetProjectionMatrix() const;
        Canvas* GetCanvas();
    };
};