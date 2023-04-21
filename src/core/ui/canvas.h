#pragma once

#include <opengl.h>
#include <map>

namespace UI {
class UIComponent;
class Canvas {
private:
    glm::ivec2 offset_;
    std::multimap<int, UI::UIComponent*> components_;
public:
    Canvas();
    virtual ~Canvas();
    void Draw();
    void SetOffset(const glm::ivec2&);
    void AddUIComponent(UI::UIComponent*, int = 0);
    void RemoveUIComponent(UI::UIComponent*);
    void UpdateWindowSize();
    glm::mat4 GetProjectionMatrix();
};
};