#pragma once

#include <core/ui/canvas.h>
#include <core/ui/component/textcomponent.h>
#include <game/components/animationcomponent.h>

#pragma once

#include <core/ui/canvas.h>
#include "canvaslayout.h"

class MainMenu : public CanvasLayout {
private:
    Shape logoShape_;
    std::shared_ptr<Material> logoMaterial_;
    int mainButtonY_ = 250;
public:
    UI::TextComponent* AddMainButton(const std::string&);
    void Draw() override;
    void CreateHUDElements();
    void AssignToRenderer(Renderer&);
    void Update() { }
};