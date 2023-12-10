#include "debugoverlay.h"
#include <core/game.h>
#include <core/input.h>
#include <core/stage.h>

#include <algorithm>
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

void DebugOverlay::Start() {
    Canvas& c = GAME->GetRenderer().CreateCanvas(canvasId);
    // c.isVisible = false;
    c.bgMaterial = std::make_shared<Material>();
    c.bgMaterial->SetShaderUniform("color", glm::vec4(.5f));
    c.offset = { 0, 720 };
    textComponent_ = parent->AddComponent<TextComponent>(&c);
    textComponent_->font = fontId;
    textComponent_->lineSpacing = 15;
    textComponent_->AddToCanvas();
    textComponent_->Start();

    AppendElement("build v{}.{}", "version");
    #ifdef VERSION_SPECIFIED
    SetValue("version", VERSION_MAJ, VERSION_MIN);
    #endif
    AppendNewline();

    AppendElement("pos: [ {:.2f}, {:.2f}, {:.2f} ]", "pos");
    AppendElement("entities: {}", "entities");
    AppendElement("rendered entities: {}", "entitiesOnFrustum");
    AppendElement("stages: [ {} ]", "stages");
    AppendNewline();

    AppendElement("fps: {}", "fps");
    lastTime_ = glfwGetTime();
    frames_ = 0;
    SetValue("fps", frames_);
    #ifdef LOG_SYSTEM_RESOURCES
    AppendElement("RAM usage: {:.2f} MB", "ram");
    //AppendElement("CPU %: {0:f}{0:%}", "res");
    #endif
    AppendNewline();

    AppendElement("normals shown: {}", "normalsShown");
    AppendElement("hitboxes shown: {}", "hitboxesShown");
    AppendElement("aabbs shown: {}", "aabbsShown");
}

void DebugOverlay::Update() {
    frames_++;
    if (glfwGetTime() - lastTime_ >= 1.0) {
        SetValue("fps", frames_);
        frames_ = 0;
        lastTime_ += 1.0;
    }
}

void DebugOverlay::FixedUpdate() {
    Canvas& c = GAME->GetRenderer().GetCanvas(canvasId);
    if (Input::IsKeyPressedDown(GLFW_KEY_F3)) {
        c.isVisible = !c.isVisible;
    }
    if (!c.isVisible)
        return;
    glm::vec3 camPos = GAME->GetRenderer().GetCamera().pos;
    SetValue("pos", camPos.x, camPos.y, camPos.z);
    SetValue("entities", GAME->GetEntityManager().CountEntities());
    SetValue("entitiesOnFrustum", GAME->GetRenderer().CountEntitiesOnFrustum());
    const std::vector<std::string>& stages = GAME->resources.stageManager.GetLoadedStages();
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
    SetValue("stages", stagesStr);

    SetValue("normalsShown", GAME->GetRenderer().highlightNormals);
    SetValue("hitboxesShown", GAME->GetRenderer().showHitboxes);
    SetValue("aabbsShown", GAME->GetRenderer().showAabbs);

    #ifdef LOG_SYSTEM_RESOURCES_WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    SetValue("ram", (float) pmc.WorkingSetSize / 0x100000);
    #endif
    UpdateText();
    glm::ivec2 size = textComponent_->GetTextSize();
    c.bgSize = glm::vec2(size.x + parent->transform->position.x * 2, size.y - parent->transform->position.y);
}

void DebugOverlay::AppendElement(const std::string& fmt, const std::string& var) {
    texts_.push_back({ var, fmt });
}

void DebugOverlay::AppendNewline() {
    texts_.push_back({ "", "" });
}

void DebugOverlay::UpdateText() {
    std::stringstream ss;
    for (int i = 0; i < texts_.size(); i++) {
        ss << texts_.at(i).str;
        if (i < texts_.size() - 1)
            ss << "\n";
    }
    textComponent_->SetText(ss.str());
}