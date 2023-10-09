#include "game/game.h"

#include <core/stagemanager.h>
#include <core/input.h>
#include <core/io/files/materials.h>
#include <core/io/paths.h>
#include <core/io/resourcemanager.h>
#include <core/graphics/model.h>
#include <core/graphics/cubemap.h>
#include <core/graphics/component/meshrenderer.h>
#include <core/terrain/plane.h>
#include <core/ui/component/textcomponent.h>
#include <core/physics/component/rigidbody.h>

#include "components/playercontroller.h"
#include "components/rotatecube.h"
#include "components/debugoverlay.h"

#define LOG_FN_(fn) spdlog::debug("[{} called]", fn)
#define LOG_FN() LOG_FN_(__FUNCTION__)

Lights::PointLight* playerLight = nullptr;

bool MonkeyGame::InitWindow() {
    LOG_FN();
    window_ = GameWindow("apina peli !!!!!!", 1280, 720, false);
    if(!window_.Create(renderer_)) {
        return false;
    }
    return true;
}

void MonkeyGame::PreLoad() {
    LOG_FN();
}

void MonkeyGame::Start() {
    LOG_FN();
    Stage::AddStageFromFile(Paths::Path(Paths::STAGES_DIR, "test.json"));
    Stage::AddStageFromFile(Paths::Path(Paths::STAGES_DIR, "cave.json"));
    
    Entity& player = entityManager_.CreateEntity("Player");
    player.AddComponent<PlayerController>();
    playerLight = player.AddComponent<Lights::PointLight>();
    playerLight->intensity = .5f;
    playerLight->range = 5.0f;

    renderer_.skybox = Meshes::CreateMeshInstance(Meshes::CUBE_WITHOUT_TEXCOORDS);
    renderer_.skybox->material = std::make_shared<Material>(Shaders::ShaderID::SKYBOX);
    const char* faces[6] = {
        "graycloud_rt.jpg",
        "graycloud_lf.jpg",
        "graycloud_up.jpg",
        "graycloud_dn.jpg",
        "graycloud_ft.jpg",
        "graycloud_bk.jpg"
    };
    renderer_.skyboxTexture = Cubemap::LoadTextureFromFaces("cloud-skybox", faces, true);

    float range = 4.0f;
    int monkeyCount = 6;
    Model& monkeyModel = resources.modelManager["chimp.fbx"];
    for (auto mesh : monkeyModel.meshes) {
        mesh->material = std::make_shared<Material>(Shaders::ShaderID::LIT);
        glm::vec3 color = glm::vec3((double) rand() / (RAND_MAX), (double) rand() / (RAND_MAX), (double) rand() / (RAND_MAX));
        mesh->material->SetShaderUniform<glm::vec3>("color", color);
        mesh->material->SetShaderUniform<glm::vec3>("ambientColor", glm::vec3(.1f));
        mesh->material->SetShaderUniform<int>("specularHighlight", 32);
        mesh->material->SetShaderUniform<float>("specularStrength", 1.5f);
    }
    for (int i = 0; i < monkeyCount; i++) {
        Entity& monkey = entityManager_.CreateEntity();
        monkey.AddComponent<MeshRenderer>()->meshes = monkeyModel.meshes;
        monkey.AddComponent<RotateCube>();
        
        double rad = ((2 * M_PI) / monkeyCount) * i;
        monkey.transform->position = glm::vec3(cos(rad) * range, 0.0, sin(rad) * range);
        monkey.transform->size = glm::vec3(1.0f, 1.0f, .5f);
    }
    Entity& mogus = entityManager_.CreateEntity();
    auto mogusRenderer = mogus.AddComponent<MeshRenderer>();
    mogusRenderer->meshes = resources.modelManager["OBJ_MOGUS"].meshes;
    mogusRenderer->isStatic = true;
    mogus.transform->position = glm::vec3(0.0f, 0.25f, 0.0f);
    mogus.transform->size = glm::vec3(.5f);

    auto plane = std::make_shared<Plane>(glm::ivec2(25, 25));
    plane->heightVariation = 1.0f;
    plane->textureSize = glm::vec2(2);
    plane->GenerateVertices();
    plane->GenerateVAO();
    plane->material = renderer_.GetMaterial("MAT_GRASS");
    Entity& terrain = entityManager_.CreateEntity();
    auto terrainRenderer = terrain.AddComponent<MeshRenderer>();
    terrainRenderer->meshes.push_back(plane);
    terrainRenderer->isStatic = true;
    terrain.transform->size = glm::vec3(50, .25f, 50);
    terrain.transform->position.y = -.5f;
    auto terrainRb = terrain.AddComponent<Physics::RigidBody>();
    terrainRb->mass = 0.0f;
    terrainRb->collider = plane->CreateBtCollider();
    
    if (resources.fontManager.HasLoaded("firacode-regular.ttf")) {
        auto debugOverlay = entityManager_.CreateEntity().AddComponent<DebugOverlay>();
        debugOverlay->fontId = "firacode-regular.ttf";

        // super text rendering benchmark 9000
        /*UI::Canvas& canvas = GetRenderer().CreateCanvas("test");
        for (int i = 0; i < 100; i++) {
            Entity& testText = entityManager_.CreateEntity();
            auto textComponent = testText.AddComponent<UI::TextComponent>(&canvas);
            textComponent->AddToCanvas();
            textComponent->font = fontId;
            testText.transform->position.x = (float) ((i / 10) % 1280 * 128);
            testText.transform->position.y = (float) (720 - 72 - (i % 10) * 72);
            testText.transform->size.x = 1.0f;
            textComponent->SetText(std::to_string(i));
        }*/
    }
    Stage::LoadStage("teststage");
}

void MonkeyGame::Update() {
    playerLight->ApplyForAllShaders();
    if (Input::IsKeyPressedDown(GLFW_KEY_N))
        GetRenderer().highlightNormals = !GetRenderer().highlightNormals;

    if (Input::IsKeyPressedDown(GLFW_KEY_H))
        GetRenderer().showHitboxes = !GetRenderer().showHitboxes;

    if (Input::IsKeyPressedDown(GLFW_KEY_L)) {
        Stage::UnloadStage("cave");
        Stage::LoadStage("cave");
    }

    if (Input::IsKeyPressedDown(GLFW_KEY_U)) {
        Stage::UnloadAllStages();
    }
}