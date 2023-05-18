#pragma once

#include "core/entity/component.h"
#include "core/entity/entity.h"
#include "core/ui/textcomponent.h"

#include <opengl.h>
#include <fmt/format.h>

using namespace UI;

struct DebugTextElement {
    std::string id;
    std::string format;
    std::vector<std::string> var;
    std::string str = "";
    std::string prevStr = str;
    TextComponent* textComponent = nullptr;
};

class DebugTextContainer {
public:
    float spacing = 30;
    std::string canvasId;
    Text::FontID fontId;
    glm::vec2 pos = glm::vec2(0);
    std::vector<DebugTextElement> texts;

    DebugTextContainer() { }
    DebugTextContainer(const std::string& c, Text::FontID f) : canvasId(c), fontId(f) { }
    template<typename... Args>
    void SetValue(const std::string& id, Args... args) {
        auto& textElement = operator[](id);
        textElement.str = fmt::format(textElement.format, args...);
        if (textElement.str != textElement.prevStr) {
            textElement.textComponent->SetText(textElement.str);
            textElement.prevStr = textElement.str;
        }
    }
    const std::string& GetText(const std::string&);
    DebugTextElement& operator[](const std::string&);
    void AppendElement(const std::string&, const std::string&, bool);
};

class DebugOverlay : public Component<DebugOverlay> {
private:
    DebugTextContainer textContainer_;
    double lastTime_ = 0;
    int frames_;
public:
    std::string canvasId = "debugOverlay";
    Text::FontID fontId;
    void Start();
    void Update();
};