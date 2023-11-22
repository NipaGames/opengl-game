#include "game/game.h"

#include <core/stage.h>
#include <core/input.h>
#include <core/io/files/materials.h>
#include <core/io/paths.h>
#include <core/io/resourcemanager.h>
#include <core/io/files/cfg.h>
#include <core/graphics/model.h>
#include <core/graphics/cubemap.h>
#include <core/graphics/component/meshrenderer.h>
#include <core/graphics/component/light.h>
#include <core/terrain/plane.h>
#include <core/ui/component/textcomponent.h>
#include <core/physics/component/rigidbody.h>

#include "components/player.h"
#include "components/rotatecube.h"
#include "components/debugoverlay.h"
#include "components/gate.h"
#include "components/animationcomponent.h"
#include "components/interactable.h"
#include "components/huditem.h"
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

void WhatIs(std::string str) {
    spdlog::info(str);
}

void LoadStage(std::string stage) {
    if (!GAME->resources.stageManager.LoadStage(stage))
        spdlog::warn("Stage '{}' not found!", stage);
}

void UnloadStage(std::string stage) {
    if (stage == "all") {
        GAME->resources.stageManager.UnloadAllStages();
        return;
    }
    if (!GAME->resources.stageManager.UnloadStage(stage))
        spdlog::warn("Stage '{}' not loaded!", stage);
}

void ShowAreaMessage() {
    MonkeyGame* game = MonkeyGame::GetGame();
    const std::vector<std::string>& stages = GAME->resources.stageManager.GetLoadedStages();
    if (!stages.empty()) {
        const nlohmann::json& dataJson = GAME->resources.stageManager.Get(stages.at(0)).data;
        if (dataJson["location"].is_string())
            game->hud.ShowAreaMessage(dataJson["location"]);
    }
}

void SpawnPlayer() {
    MonkeyGame::GetGame()->GetPlayer().GetComponent<PlayerController>()->Spawn();
    ShowAreaMessage();
}

void KillPlayer() {
    MonkeyGame::GetGame()->GetPlayer().GetComponent<Player>()->SetHealth(0);
}

void AddPlayerStatus(std::string type, float t) {
    Player* player = MonkeyGame::GetGame()->GetPlayer().GetComponent<Player>();
    auto enumCast = magic_enum::enum_cast<StatusEffectType>(type);
    if (!enumCast.has_value()) {
        spdlog::warn("Invalid status effect '{}'!", type);
        return;
    }
    switch (enumCast.value()) {
        case StatusEffectType::POISON:
            player->AddStatusType<PoisonEffect>(t);
            break;
        case StatusEffectType::RADIATION:
            player->AddStatusType<RadiationEffect>(t);
            break;
    }
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
    console.RegisterCommand("build", [](const std::string&) {
        bool debug = false;
        #ifdef DEBUG_BUILD
        debug = true;
        #endif
        #ifdef VERSION_SPECIFIED
        spdlog::info("v{}.{} [{}]", VERSION_MAJ, VERSION_MIN, debug ? "Debug" : "Release");
        // yeah this is actually the timestamp of this file, would have to recompile this specifically every time
        spdlog::info("built {}", __DATE__);
        #else
        spdlog::warn("can't get version info");
        #endif
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
    console.RegisterCommand("load", LoadStage);
    console.RegisterCommand("unload", UnloadStage);
    console.RegisterCommand("spawn", [](const std::string&) {
        SpawnPlayer();
    });
    console.RegisterCommand("die", [](const std::string&) {
        KillPlayer();
    });
}

void MonkeyGame::PreLoad() {
    LOG_FN();

    REGISTER_EVENT(WhatIs);
    REGISTER_EVENT(LoadStage);
    REGISTER_EVENT(UnloadStage);
    REGISTER_EVENT(ShowAreaMessage);
    EVENT_MANAGER.RegisterEvent("Spawn", SpawnPlayer);
    EVENT_MANAGER.RegisterEvent("PlayerStatus", AddPlayerStatus);
    EVENT_MANAGER.RegisterEvent("PlayAnimation", AnimationComponent::PlayAnimation);

    RegisterCommands(console);
}

void MonkeyGame::Start() {
    LOG_FN();

    Resources::LoadConfig(Config::CONTROLS_PATH, controlsConfig);
    Resources::LoadConfig(Config::GENERAL_PATH, generalConfig);

    postProcessing = PostProcessing();
    if (generalConfig.blurEdges) {
        postProcessing.kernel.offset = 1.0f / 1000.0f;
        postProcessing.kernel.vignette.isActive = true;
        postProcessing.kernel.vignette.size = 0.5f;
        postProcessing.ApplyKernel(Convolution::GaussianBlur<7>());
    }
    postProcessing.vignette.isActive = true;

    renderer_.ApplyPostProcessing(postProcessing);
    
    Entity& player = entityManager_.CreateEntity(playerId);
    player.AddComponent<PlayerController>();
    player.AddComponent<Player>();
    /*playerLight = player.AddComponent<Lights::PointLight>();
    playerLight->intensity = .5f;
    playerLight->range = 5.0f;*/

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
    
    renderer_.GetCamera().yaw = 90.0f;
    renderer_.GetCamera().pitch = 0.0f;

    Entity& spawn = entityManager_.CreateEntity();
    auto spawnRenderer = spawn.AddComponent<MeshRenderer>();
    spawnRenderer->meshes = resources.modelManager["OBJ_SPAWN"].meshes;
    spawnRenderer->isStatic = true;
    spawn.transform->position = glm::vec3(0.0f, 157.0f, -20.0f);
    spawn.transform->size = glm::vec3(.5f);
    auto spawnInteractable = spawn.AddComponent<Interactable>();
    Event spawnEvent;
    spawnEvent.AddCommand("Spawn()");
    spawnInteractable->event = spawnEvent;
    spawnInteractable->trigger = TriggerType::IN_PROXIMITY;

    Entity& hudItem = entityManager_.CreateEntity();
    auto hudItemRenderer = hudItem.AddComponent<HUDItemRenderer>();
    hudItemRenderer->meshes = resources.modelManager["OBJ_SWORD"].meshes;
    hudItemRenderer->renderAfterPostProcessing = true;
    hudItem.transform->size = glm::vec3(.1f);
    hudItem.transform->position = glm::vec3(.75f, -.5f, 0.0f);
    hudItem.AddComponent<ItemInHand>();

    Entity& board = entityManager_.CreateEntity();
    auto boardRenderer = board.AddComponent<MeshRenderer>();
    boardRenderer->meshes = resources.modelManager["OBJ_BOARD"].meshes;
    boardRenderer->isStatic = true;
    board.transform->position = glm::vec3(15.0f, 154.6f, 0.0f);
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
        monkey.transform->position = glm::vec3(cos(rad) * range, 155.0, sin(rad) * range);
        monkey.transform->size = glm::vec3(1.0f, 1.0f, .5f);
    }
    Entity& mogus = entityManager_.CreateEntity();
    mogus.id = "amongus";
    auto mogusRenderer = mogus.AddComponent<MeshRenderer>();
    mogusRenderer->meshes = resources.modelManager["OBJ_MOGUS"].meshes;
    mogusRenderer->isStatic = true;
    mogus.transform->position = glm::vec3(0.0f, 155.25f, 0.0f);
    mogus.transform->size = glm::vec3(.5f);
    Physics::RigidBody* mogusRigidbody = mogus.AddComponent<Physics::RigidBody>();
    mogusRigidbody->colliderFrom = Physics::ColliderConstructor::AABB;
    mogusRigidbody->mass = 0.0f;
    mogus.AddComponent<LivingEntity>();

    Entity& lightEntity = entityManager_.CreateEntity();
    lightEntity.transform->position = glm::vec3(0.0f, 157.5f, 0.0f);
    lightEntity.AddComponent<Lights::PointLight>()->intensity = .5f;

    auto plane = std::make_shared<Plane>(glm::ivec2(25, 25));
    plane->heightVariation = 1.0f;
    plane->GenerateVertices();
    plane->GenerateVAO();
    plane->material = renderer_.GetMaterial("MAT_GRASS");
    Entity& terrain = entityManager_.CreateEntity();
    auto terrainRenderer = terrain.AddComponent<MeshRenderer>();
    terrainRenderer->meshes.push_back(plane);
    terrainRenderer->isStatic = true;
    terrain.transform->size = glm::vec3(50, .25f, 50);
    terrain.transform->position.y = 154.5f;
    auto terrainRb = terrain.AddComponent<Physics::RigidBody>();
    terrainRb->mass = 0.0f;
    terrainRb->collider = plane->CreateBtCollider();
    
    if (resources.fontManager.HasLoaded("FONT_FIRACODE")) {
        auto debugOverlay = entityManager_.CreateEntity().AddComponent<DebugOverlay>();
        debugOverlay->parent->transform->size.z = .35f;
        debugOverlay->parent->transform->position = glm::vec3(10, -20, 0);
        debugOverlay->fontId = "FONT_FIRACODE";

        // super text rendering benchmark 9000
        /*
        UI::Canvas& canvas = GetRenderer().CreateCanvas("test");
        for (int i = 0; i < 100; i++) {
            Entity& testText = entityManager_.CreateEntity();
            auto textComponent = testText.AddComponent<UI::TextComponent>(&canvas);
            textComponent->AddToCanvas();
            textComponent->font = "FONT_FIRACODE";
            testText.transform->position.x = (float) ((i / 10) % 1280 * 128);
            testText.transform->position.y = (float) (720 - 72 - (i % 10) * 72);
            testText.transform->size.x = 1.0f;
            textComponent->SetText(std::to_string(i));
        }
        */
    }
    hud = HUD();
    hud.CreateHUDElements();

    GAME->resources.stageManager.LoadStage("teststage");
    SpawnPlayer();
}

Entity& MonkeyGame::GetPlayer() {
    return entityManager_.GetEntity(playerId);
}

bool MonkeyGame::TryHitEntity(const btVector3& from, const btVector3& to, std::function<void(LivingEntity*)> callback) {
    btCollisionWorld::ClosestRayResultCallback res(from, to);
    Physics::dynamicsWorld->rayTest(from, to, res);
    if (!res.hasHit())
        return false;
    for (const auto& entity : entityManager_.GetEntities()) {
        Physics::RigidBody* rb = entity->GetComponent<Physics::RigidBody>();
        LivingEntity* living = entity->GetComponent<LivingEntity>();
        if (rb == nullptr || living == nullptr)
            continue;
        if (rb->collider == res.m_collisionObject->getCollisionShape()) {
            callback(living);
            return true;
        }
    }
    return false;
}

void MonkeyGame::Update() {
    if (playerLight != nullptr)
        playerLight->ApplyForAllShaders();
    hud.Update();
    if (Input::IsKeyPressedDown(GLFW_KEY_F6))
        GetRenderer().highlightNormals = !GetRenderer().highlightNormals;

    if (Input::IsKeyPressedDown(GLFW_KEY_F7))
        GetRenderer().showHitboxes = !GetRenderer().showHitboxes;

    if (Input::IsKeyPressedDown(GLFW_KEY_F8))
        GetRenderer().showAabbs = !GetRenderer().showAabbs;

    if (Input::IsKeyPressedDown(GLFW_KEY_L)) {
        GAME->resources.stageManager.UnloadStage("passage");
        GAME->resources.stageManager.LoadStage("passage");
    }

    if (Input::IsKeyPressedDown(GLFW_KEY_U)) {
        GAME->resources.stageManager.UnloadAllStages();
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