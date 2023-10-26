#pragma once

#include <core/entity/component.h>
#include <core/entity/entity.h>
#include <core/ui/component/textcomponent.h>

#include <opengl.h>
#include <fmt/format.h>

using namespace UI;

struct DebugTextElement {
    std::string var;
    std::string format;
    std::string str = "";
};

class DebugOverlay : public Component<DebugOverlay> {
private:
    double lastTime_ = 0;
    int frames_;
    std::vector<DebugTextElement> texts_;
    TextComponent* textComponent_;
public:
    std::string canvasId = "debugOverlay";
    std::string fontId;
    void Start();
    void FixedUpdate();
    void Update();
    template<typename... Args>
    void SetValue(const std::string& var, Args... args) {
        for (auto& t : texts_) {
            if (t.var != var)
                continue;
            t.str = fmt::format(t.format, args...);
        }
    }
    void AppendElement(const std::string&, const std::string& vars);
    void AppendNewline();
    void UpdateText();

};