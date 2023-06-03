#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <spdlog/spdlog.h>

#include "paths.h"
#include <core/graphics/shader.h>
#include <core/graphics/texture.h>

class Resources {
public:
    template <typename T, typename K = std::string>
    class ResourceManager {
    protected:
        std::unordered_map<K, T> items_;
        std::fs::path path_;
        virtual T LoadResource(const std::fs::path&) = 0;
        virtual K CreateID() { throw "not implemented"; }
    public:
        ResourceManager(const std::string& p) : path_(p) { }
        ResourceManager(const std::fs::path& p) : path_(p) { }
        virtual void LoadAll() {
            for (const auto& f : std::fs::directory_iterator(path_))
                Load(f.path());
        }
        virtual void Load(const std::fs::path& p) {
            K id;
            if constexpr(std::is_same_v<K, std::string>)
                id = std::fs::proximate(p, path_).generic_string();
            else
                id = CreateID();
            spdlog::info("Loading resource '" + std::fs::proximate(p, path_.parent_path()).generic_string() + "'");
            items_[id] = LoadResource(std::fs::absolute(p));
        }
        virtual T& Get(const K& item) {
            return items_.at(item);
        }
        T& operator [](const K& item) {
            if (items_.count(item) == 0) {
                T t;
                items_[item] = t;
            }
            return Get(item);
        }
        const std::unordered_map<K, T>& GetAll() {
            return items_;
        }
    };

    class TextureManager : public ResourceManager<Texture::TextureID> {
    protected:
        Texture::TextureID LoadResource(const std::fs::path&) override;
    public:
        TextureManager() : ResourceManager<Texture::TextureID>(Paths::TEXTURES_DIR) { }
    };

    class ShaderManager : public ResourceManager<GLuint> {
    protected:
        GLuint LoadResource(const std::fs::path&) override;
        void LoadShader(GLuint, const std::string&, Shaders::ShaderType);
        void LoadStandardShader(Shaders::ShaderID, const std::string&, Shaders::ShaderType);
    public:
        ShaderManager() : ResourceManager<GLuint>(Paths::SHADER_DIR) { }
        void LoadAll() override;
        GLuint& Get(Shaders::ShaderID);
    };

    TextureManager textureManager;
    ShaderManager shaderManager;

    void LoadAll();
};