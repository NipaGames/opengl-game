#pragma once

#include <core/ui/canvas.h>
#include <core/ui/component/buttoncomponent.h>
#include <game/components/animationcomponent.h>
#include "canvaslayout.h"

class MainMenu : public CanvasLayout {
private:
    Shape logoShape_;
    std::shared_ptr<Material> logoMaterial_;
    int mainButtonY_ = 250;
public:
    UI::ButtonComponent* AddMainButton(const std::string&);
    void Draw() override;
    void CreateHUDElements();
    void AssignToRenderer(Renderer&);
};