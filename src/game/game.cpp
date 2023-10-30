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
#include "components/gate.h"
#include "components/interactable.h"
#include "event.h"
#include "eventparser.h"

#ifdef _WIN32
#undef APIENTRY
#include <windows.h>
#include <shellapi.h>
#endif

#include <magic_enum/magic_enum.hpp>

#define LOG_FN_(fn) spdlog::debug("[{} called]", fn)
#define LOG_FN() LOG_FN_(__FUNCTION__)

Lights::PointLight* playerLight = nullptr;
const std::string playerId = "Player";

MonkeyGame* MonkeyGame::GetGame() {
    return static_cast<MonkeyGame*>(GAME.get());
}

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

void WhatIs(std::string str) {
    spdlog::info(str);
}

void SpawnPlayer() {
    GAME->GetEntityManager().GetEntity(playerId).GetComponent<PlayerController>()->Spawn();
}

void RegisterCommands(Console& console) {
    console.RegisterCommand("quit", [](const std::string&) {
        spdlog::info("byee!! :3");
        // yeah could clean up or something before quitting
        exit(0);
    });
    console.RegisterCommand("event", [](const std::string& args) {
        EVENT_PARSER.SetKeyword("this", playerId);
        EventReturnStatus err = EVENT_PARSER.ParseCommand(args);
        if (err != EventReturnStatus::OK) {
            spdlog::warn("failed: {}", magic_enum::enum_name(err));
        }
    });
    console.RegisterCommand("github", [](const std::string&) {
        #ifdef _WIN32
        ShellExecuteW(0, 0, L"https://github.com/NipaGames/opengl-game", 0, 0 , SW_SHOW);
        #endif
    });
    console.RegisterCommand("hi", [](const std::string&) {
        spdlog::info("haiii!! :3");
    });
    // the cis male command
    console.RegisterCommand("goodgirl", [](const std::string&) {
        spdlog::info("you're such a good girl <3");
    });
    console.RegisterCommand("list", [&](const std::string& strArgs) {
        std::vector<std::string> args = Console::SplitArgs(strArgs);
        bool listCommands = std::find(args.begin(), args.end(), "commands") != args.end();
        bool listEvents = std::find(args.begin(), args.end(), "events") != args.end();
        bool listEntityComponents = std::find(args.begin(), args.end(), "entities+c") != args.end();
        bool listEntities = listEntityComponents || std::find(args.begin(), args.end(), "entities") != args.end();
        if (args.empty()) {
            listCommands = true;
            listEvents = true;
            listEntities = true;
        }
        bool needsHeaders = args.size() != 1;
        if (listCommands) {
            if (needsHeaders)
                spdlog::info("COMMANDS");
            for (const std::string& c : console.ListCommands()) {
                spdlog::info("- {}", c);
            }
        }
        if (listEvents) {
            if (needsHeaders)
                spdlog::info("EVENTS");
            for (const std::string& c : EVENT_MANAGER.ListEvents()) {
                spdlog::info("- {}", c);
            }
        }
        if (listEntities) {
            if (needsHeaders)
                spdlog::info("ENTITIES");
            const auto& entities = GAME->GetEntityManager().GetEntities();
            size_t entityCount = entities.size();
            if (args.empty())
                entityCount = 5;
            size_t i = 0;
            for (const auto& e : entities) {
                spdlog::info("- {} {}", e->GetHash(), e->id);
                if (listEntityComponents) {
                    for (const std::string& c : e->ListComponentNames()) {
                        spdlog::info("    {}", c);
                    }
                }
                if (++i >= entityCount)
                    break;
            }
            if (entityCount < entities.size())
                spdlog::info("...");
        }
    });
    console.RegisterCommand("load", [](const std::string& stage) {
        if (!Stage::LoadStage(stage))
            spdlog::warn("Stage '{}' not found!", stage);
    });
    console.RegisterCommand("unload", [](const std::string& stage) {
        if (stage == "all") {
            Stage::UnloadAllStages();
            return;
        }
        if (!Stage::UnloadStage(stage))
            spdlog::warn("Stage '{}' not loaded!", stage);
    });
    console.RegisterCommand("spawn", [](const std::string& stage) {
        SpawnPlayer();
    });
}

void MonkeyGame::Start() {
    LOG_FN();
    Stage::AddStageFromFile(Paths::Path(Paths::STAGES_DIR, "test.json"));
    Stage::AddStageFromFile(Paths::Path(Paths::STAGES_DIR, "cave.json"));

    REGISTER_EVENT(WhatIs);
    EVENT_MANAGER.RegisterEvent("Spawn", SpawnPlayer);
    EVENT_MANAGER.RegisterEvent("OpenGate", Gate::OpenGate);

    RegisterCommands(console);
    
    Entity& player = entityManager_.CreateEntity(playerId);
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

    Entity& spawn = entityManager_.CreateEntity();
    auto spawnRenderer = spawn.AddComponent<MeshRenderer>();
    spawnRenderer->meshes = resources.modelManager["OBJ_SPAWN"].meshes;
    spawnRenderer->isStatic = true;
    spawn.transform->position = glm::vec3(0.0f, 2.0f, -20.0f);
    spawn.transform->size = glm::vec3(.5f);
    auto spawnInteractable = spawn.AddComponent<Interactable>();
    Event spawnEvent;
    spawnEvent.AddCommand("Spawn()");
    spawnInteractable->event = spawnEvent;
    spawnInteractable->trigger = TriggerType::IN_PROXIMITY;

    Entity& board = entityManager_.CreateEntity();
    auto boardRenderer = board.AddComponent<MeshRenderer>();
    boardRenderer->meshes = resources.modelManager["OBJ_BOARD"].meshes;
    boardRenderer->isStatic = true;
    board.transform->position = glm::vec3(15.0f, -.4f, 0.0f);
    board.transform->rotation = glm::quat(glm::vec3(0.0f, M_PI, 0.0f));
    auto boardRigidbody = board.AddComponent<Physics::RigidBody>();
    boardRigidbody->colliderFrom = Physics::ColliderConstructor::AABB;
    boardRigidbody->mass = 0.0f;

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
        debugOverlay->parent->transform->size.z = .5f;
        debugOverlay->parent->transform->position = glm::vec3(10, -30, 0);
        debugOverlay->fontId = "firacode-regular.ttf";

        // super text rendering benchmark 9000
        /*
        UI::Canvas& canvas = GetRenderer().CreateCanvas("test");
        for (int i = 0; i < 100; i++) {
            Entity& testText = entityManager_.CreateEntity();
            auto textComponent = testText.AddComponent<UI::TextComponent>(&canvas);
            textComponent->AddToCanvas();
            textComponent->font = "firacode-regular.ttf";
            testText.transform->position.x = (float) ((i / 10) % 1280 * 128);
            testText.transform->position.y = (float) (720 - 72 - (i % 10) * 72);
            testText.transform->size.x = 1.0f;
            textComponent->SetText(std::to_string(i));
        }
        */
    }
    if (resources.fontManager.HasLoaded("Augusta.ttf")) {
        hud.fontId = "Augusta.ttf";
        hud.CreateHUDElements();
    }
    Stage::LoadStage("teststage");
}

void MonkeyGame::Update() {
    playerLight->ApplyForAllShaders();
    if (Input::IsKeyPressedDown(GLFW_KEY_F6))
        GetRenderer().highlightNormals = !GetRenderer().highlightNormals;

    if (Input::IsKeyPressedDown(GLFW_KEY_F7))
        GetRenderer().showHitboxes = !GetRenderer().showHitboxes;

    if (Input::IsKeyPressedDown(GLFW_KEY_F8))
        GetRenderer().showAabbs = !GetRenderer().showAabbs;

    if (Input::IsKeyPressedDown(GLFW_KEY_L)) {
        Stage::UnloadStage("cave");
        Stage::LoadStage("cave");
    }

    if (Input::IsKeyPressedDown(GLFW_KEY_U)) {
        Stage::UnloadAllStages();
    }

    // console
    if (Input::IsKeyPressedDown(GLFW_KEY_F1)) {
        Input::IS_MOUSE_LOCKED = false;
        Input::CURSOR_MODE_CHANGE_PENDING = true;
        Input::CONSOLE_FOCUS_PENDING = true;
        std::string line;
        std::cout << std::endl;
        spdlog::set_pattern("  %v");
        spdlog::info("--------- Opened console, 'exit' to return ----------");
        spdlog::info("[copyright nipagames information systems corporation]");
        std::cout << std::endl;
        bool exit = false;
        FreezeDeltaTime();
        while (!exit) {
            std::cout << "> ";
            std::getline(std::cin, line);
            switch (console.ExecuteCommandInput(line)) {
                case CommandReturnStatus::EXIT_CONSOLE:
                    exit = true;
                    break;
                case CommandReturnStatus::COMMAND_NOT_FOUND:
                    spdlog::warn("Command not found!");
                    break;
            }
        }
        spdlog::set_pattern(SPDLOG_PATTERN);
        std::cout << std::endl;
        Input::IS_MOUSE_LOCKED = true;
        Input::CURSOR_MODE_CHANGE_PENDING = true;
        Input::WINDOW_FOCUS_PENDING = true;
    }
}