#pragma once

#include <fstream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <map>
#include <filesystem>
#include <optional>
#include <spdlog/spdlog.h>

#include "paths.h"
#include <core/graphics/shader.h>
#include <core/graphics/texture.h>
#include <core/ui/text.h>

class Resources {
public:
    template <typename T>
    class ResourceManager {
    private:
        struct ItemComp {
            bool operator() (const std::string& l, const std::string& r) const {
                #ifdef strcasecmp
                    return strcasecmp(l.c_str(), r.c_str()) < 0;
                #else
                    return _stricmp(l.c_str(), r.c_str()) < 0;
                #endif
            }
        };
        std::string itemID_;
        std::string typeStr_;
    protected:
        std::map<std::string, T, ItemComp> items_;
        std::fs::path path_;
        virtual std::optional<T> LoadResource(const std::fs::path&) = 0;
        void SetItemID(const std::string& id) { itemID_ = id; }
    public:
        ResourceManager(const std::fs::path& p, const std::string& t = "resource") : path_(p), typeStr_(t) { }
        virtual void LoadAll() {
            for (const auto& f : std::fs::directory_iterator(path_))
                Load(f.path());
        }
        virtual void Load(const std::fs::path& p) {
            itemID_ = std::fs::proximate(p, path_).generic_string();
            std::string fileName = std::fs::proximate(p, path_.parent_path()).generic_string();
            spdlog::info("Loading {} '{}'", typeStr_, fileName);
            std::optional<T> resource = LoadResource(std::fs::absolute(p));
            spdlog::debug("  (id: {})", itemID_);
            if (resource.has_value())
                items_[itemID_] = resource.value();
            else
                spdlog::info("Failed loading {} '{}'", typeStr_, fileName);
        }
        virtual T& Get(const std::string& item) {
            return items_.at(item);
        }
        bool HasLoaded(const std::string& item) {
            return items_.count(item) > 0;
        }
        T& operator [](const std::string& item) {
            if (items_.count(item) == 0) {
                T t;
                items_[item] = t;
            }
            return Get(item);
        }
        const std::map<std::string, T, ItemComp>& GetAll() {
            return items_;
        }
    };

    class TextureManager : public ResourceManager<Texture::TextureID> {
    protected:
        std::optional<Texture::TextureID> LoadResource(const std::fs::path&) override;
    public:
        TextureManager() : ResourceManager<Texture::TextureID>(Paths::TEXTURES_DIR, "texture") { }
    };

    class ShaderManager : public ResourceManager<GLuint> {
    protected:
        std::optional<GLuint> LoadResource(const std::fs::path&) override;
        void LoadShader(GLuint, const std::string&, Shaders::ShaderType);
        void LoadStandardShader(Shaders::ShaderID, const std::string&, Shaders::ShaderType);
    public:
        ShaderManager() : ResourceManager<GLuint>(Paths::SHADER_DIR) { }
        void LoadAll() override;
        GLuint& Get(Shaders::ShaderID);
    };

    class FontManager : public ResourceManager<UI::Text::Font> {
    protected:
        std::optional<UI::Text::Font> LoadResource(const std::fs::path&) override;
        glm::ivec2 fontSize_ = { 0, 48 };
    public:
        FontManager() : ResourceManager<UI::Text::Font>(Paths::FONTS_DIR, "font") { }
        void SetFontSize(const glm::ivec2&);
        void SetFontSize(int);
    };

    TextureManager textureManager;
    ShaderManager shaderManager;
    FontManager fontManager;

    void LoadAll();
};