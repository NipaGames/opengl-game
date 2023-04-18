#pragma once

#include <opengl.h>
#include <map>

#include "core/ui/uicomponent.h"

namespace UI {
class Canvas {
private:
    glm::ivec2 offset_;
    std::multimap<int, UI::UIComponent*> components_;
public:
    Canvas();
    void Draw();
    void SetOffset(const glm::ivec2&);
    void AddUIComponent(UI::UIComponent* c, int p = 0);
    void UpdateWindowSize();
    glm::mat4 GetProjectionMatrix();
};
};