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
    std::string prevStr = str;
    TextComponent* textComponent = nullptr;
};

class DebugTextContainer {
public:
    float spacing = 30;
    std::string canvasId;
    std::string fontId;
    glm::vec2 pos = glm::vec2(0);
    std::vector<DebugTextElement> texts;
    int lines = 0;

    DebugTextContainer() { }
    DebugTextContainer(const std::string& c, const std::string& f) : canvasId(c), fontId(f) { }
    template<typename... Args>
    void SetValue(const std::string& var, Args... args) {
        for (auto& textElement : texts) {
            if (textElement.var != var)
                continue;
            textElement.str = fmt::format(textElement.format, args...);
            if (textElement.str != textElement.prevStr) {
                textElement.textComponent->SetText(textElement.str);
                textElement.prevStr = textElement.str;
            }
        }
    }
    void AppendElement(const std::string&, const std::string& vars);
    void AppendNewline();
    int GetWidth();
};

class DebugOverlay : public Component<DebugOverlay> {
private:
    DebugTextContainer textContainer_;
    double lastTime_ = 0;
    int frames_;
public:
    std::string canvasId = "debugOverlay";
    std::string fontId;
    void Start();
    void FixedUpdate();
    void Update();
};