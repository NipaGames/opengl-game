#pragma once

#include <core/ui/canvas.h>
#include <core/ui/component/buttoncomponent.h>
#include <game/components/animationcomponent.h>
#include "canvaslayout.h"

class MainMenu : public CanvasLayout {
private:
    Shape logoShape_;
    std::shared_ptr<Material> logoMaterial_;
    int mainButtonY_ = 50;
public:
    UI::ButtonComponent* AddMainButton(const std::string&, const std::string&, int);
    UI::ButtonComponent* AddMainButton(const std::string&, const std::string& = "");
    struct MainMenuButton {
        std::string name;
        std::function<void()> onClick;
    };
    void CreateButtonSubmenu(const std::string&, const std::vector<MainMenuButton>&);
    void Draw() override;
    void CreateHUDElements();
    void AssignToRenderer(Renderer&);
};