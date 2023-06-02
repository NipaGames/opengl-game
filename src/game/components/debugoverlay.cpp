#include "debugoverlay.h"
#include <core/game.h>
#include <core/input.h>
#include <core/io/stage.h>

#include <string>

#define LOG_SYSTEM_RESOURCES
#ifdef LOG_SYSTEM_RESOURCES
#ifdef _WIN32
// apparently glfw messes with APIENTRY, will need to change this if things suddenly stop working
#undef APIENTRY
#include <windows.h>
#include <psapi.h>
#define LOG_SYSTEM_RESOURCES_WIN32
#endif
#endif

void DebugTextContainer::AppendElement(const std::string& fmt, const std::string& var) {
    DebugTextElement e;
    e.var = var;
    e.format = fmt;
    Entity& textEntity = game->GetEntityManager().CreateEntity();
    e.textComponent = textEntity.AddComponent<UI::TextComponent>(&game->GetRenderer().GetCanvas(canvasId));
    e.textComponent->font = fontId;
    e.textComponent->AddToCanvas();
    textEntity.transform->position.x = pos.x;
    textEntity.transform->position.y = pos.y - spacing / 2 - lines++ * spacing;
    textEntity.transform->size.z = .5f;
    texts.push_back(e);
}

void DebugTextContainer::AppendNewline() {
    lines++;
}

int DebugTextContainer::GetWidth() {
    return std::max_element(texts.begin(), texts.end(), [] (const DebugTextElement& lhs, const DebugTextElement& rhs) {
        return lhs.textComponent->GetTextSize().x < rhs.textComponent->GetTextSize().x;
    })->textComponent->GetTextSize().x;
}

void DebugOverlay::Start() {
    Canvas& c = game->GetRenderer().CreateCanvas(canvasId);
    c.isVisible = false;
    c.bgColor = glm::vec4(.5f);
    c.offset = { 0, 720 };
    textContainer_ = DebugTextContainer(canvasId, fontId);
    textContainer_.pos = { 10, -10 };

    textContainer_.AppendElement("build v{}.{}", "version");
    #ifdef VERSION_SPECIFIED
    textContainer_.SetValue("version", VERSION_MAJ, VERSION_MIN);
    #endif
    textContainer_.AppendNewline();

    textContainer_.AppendElement("pos: [ {:.2f}, {:.2f}, {:.2f} ]", "pos");
    textContainer_.AppendElement("entities: {}", "entities");
    textContainer_.AppendElement("stages: [ {} ]", "stages");
    textContainer_.AppendNewline();

    textContainer_.AppendElement("fps: {}", "fps");
    lastTime_ = glfwGetTime();
    frames_ = 0;
    textContainer_.SetValue("fps", frames_);
    #ifdef LOG_SYSTEM_RESOURCES
    textContainer_.AppendElement("RAM usage: {:.2f} MB", "ram");
    //textContainer_.AppendElement("CPU %: {0:f}{0:%}", "res");
    #endif
    textContainer_.AppendNewline();

    textContainer_.AppendElement("normals shown: {}", "normalsShown");
    textContainer_.AppendElement("hitboxes shown: {}", "hitboxesShown");
}

void DebugOverlay::Update() {
    frames_++;
    if (glfwGetTime() - lastTime_ >= 1.0) {
        textContainer_.SetValue("fps", frames_);
        frames_ = 0;
        lastTime_ += 1.0;
    }
}

void DebugOverlay::FixedUpdate() {
    Canvas& c = game->GetRenderer().GetCanvas(canvasId);
    if (Input::IsKeyPressedDown(GLFW_KEY_F3)) {
        c.isVisible = !c.isVisible;
    }
    if (!c.isVisible)
        return;
    glm::vec3 camPos = game->GetRenderer().GetCamera().pos;
    textContainer_.SetValue("pos", camPos.x, camPos.y, camPos.z);
    textContainer_.SetValue("entities", game->GetEntityManager().CountEntities());
    const std::vector<std::string>& stages = Stage::GetLoadedStages();
    std::string stagesStr;
    if (stages.size() > 0) {
        stagesStr = stages.at(0);
        for (int i = 1; i < stages.size(); i++) {
            stagesStr += ", " + stages.at(i);
        }
    }
    else {
        stagesStr = "empty";
    }
    textContainer_.SetValue("stages", stagesStr);

    textContainer_.SetValue("normalsShown", game->GetRenderer().highlightNormals);
    textContainer_.SetValue("hitboxesShown", game->GetRenderer().showHitboxes);

    #ifdef LOG_SYSTEM_RESOURCES_WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    textContainer_.SetValue("ram", (float) pmc.WorkingSetSize / 0x100000);
    #endif
    c.bgSize = glm::vec2(textContainer_.GetWidth() + textContainer_.pos.x * 2, textContainer_.lines * textContainer_.spacing + textContainer_.pos.x);
}