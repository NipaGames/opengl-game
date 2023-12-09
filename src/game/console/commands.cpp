#include "console.h"
#include <game/event/eventparser.h>
#include <game/game.h>

#include <magic_enum/magic_enum.hpp>
#include <spdlog/spdlog.h>

#ifdef _WIN32
#undef APIENTRY
#include <windows.h>
#include <shellapi.h>
#endif

namespace Commands {
void RegisterCommands(Console& console) {
    console.RegisterCommand("quit", [](const std::string&) {
        spdlog::info("byee!! :3");
        GAME->Quit();
    });
    console.RegisterCommand("event", [](const std::string& args) {
        EVENT_PARSER.SetKeyword("this", MonkeyGame::GetGame()->GetPlayer().id);
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
        ShellExecuteA(0, "open", "https://github.com/NipaGames/opengl-game", 0, 0, SW_SHOW);
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
    console.RegisterCommand("load", MonkeyGame::LoadStage);
    console.RegisterCommand("unload", MonkeyGame::UnloadStage);
    console.RegisterCommand("spawn", [](const std::string&) {
        MonkeyGame::SpawnPlayer();
    });
    console.RegisterCommand("die", [](const std::string&) {
        MonkeyGame::KillPlayer();
    });
}
}