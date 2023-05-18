#include "game/components/debugoverlay.h"
#include "game/game.h"
#include "core/input.h"
#include "core/stage/stage.h"

#include <string>

const std::string& DebugTextContainer::GetText(const std::string& id) {
    return operator[](id).str;
}

DebugTextElement& DebugTextContainer::operator[](const std::string& id) {
    return *std::find_if(texts.begin(), texts.end(), [&](const DebugTextElement& t) { return t.id == id; });
}

void DebugTextContainer::AppendElement(const std::string& id, const std::string& fmt, bool renderEveryFrame = false) {
    DebugTextElement e;
    e.id = id;
    e.format = fmt;
    Entity& textEntity = game->GetEntityManager().CreateEntity();
    e.textComponent = textEntity.AddComponent<UI::TextComponent>(&game->GetRenderer().GetCanvas(canvasId));
    e.textComponent->font = fontId;
    e.textComponent->AddToCanvas();
    if (renderEveryFrame)
        e.textComponent->renderingMethod = UI::TextRenderingMethod::RENDER_EVERY_FRAME;
    textEntity.transform->position.x = pos.x;
    textEntity.transform->position.y = pos.y - spacing / 2 - texts.size() * spacing;
    textEntity.transform->size.z = .5f;
    texts.push_back(e);
}

void DebugOverlay::Start() {
    Canvas& c = game->GetRenderer().CreateCanvas(canvasId);
    c.isVisible = false;
    textContainer_ = DebugTextContainer(canvasId, fontId);
    textContainer_.pos = { 10, 710 };

    textContainer_.AppendElement("version", "build v{}.{}");
    #ifdef VERSION_SPECIFIED
    textContainer_.SetValue("version", VERSION_MAJ, VERSION_MIN);
    #endif

    textContainer_.AppendElement("fps", "fps: {}");
    lastTime_ = glfwGetTime();
    frames_ = 0;
    textContainer_.SetValue("fps", frames_);

    textContainer_.AppendElement("pos", "pos: [ {:.2f}, {:.2f}, {:.2f} ]");
    textContainer_.AppendElement("entities", "entities: {}");
    textContainer_.AppendElement("stages", "stages: [ {} ]");
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
    Canvas& c = game->GetRenderer().CreateCanvas(canvasId);
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
}