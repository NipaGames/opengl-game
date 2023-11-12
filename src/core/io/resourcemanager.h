#pragma once

#include <fstream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <map>
#include <filesystem>
#include <optional>
#include <variant>
#include <spdlog/spdlog.h>

#include "paths.h"
#include "files/materials.h"
#include "files/objects.h"
#include <core/graphics/shader.h>
#include <core/graphics/texture.h>
#include <core/graphics/model.h>
#include <core/ui/text.h>

// forward declarations
namespace CFG {
    class ICFGField;
    template<typename>
    class CFGField;
    typedef CFGField<std::vector<ICFGField*>> CFGObject;
};

namespace Resources {
    using AdditionalImportData = std::vector<std::variant<std::string, float, int>>;
    struct Import {
        std::string path;
        std::string id;
        AdditionalImportData additionalData;
    };
    struct ShaderImport {
        std::string id;
        std::string vertexPath;
        std::string fragmentPath;
        std::string geometryPath;
    };
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
        AdditionalImportData additionalData_;
    protected:
        std::map<std::string, T, ItemComp> items_;
        std::fs::path path_;
        virtual std::optional<T> LoadResource(const std::fs::path&) = 0;
        void SetItemID(const std::string& id) { itemID_ = id; }
        void SetAdditionalData(const AdditionalImportData& data) { additionalData_ = data; }
        const std::string& GetItemID() { return itemID_; }
        const AdditionalImportData& GetAdditionalData() { return additionalData_; }
    public:
        ResourceManager(const std::fs::path& p, const std::string& t = "resource") : path_(p), typeStr_(t) { }
        virtual void LoadAll() {
            for (const auto& f : std::fs::directory_iterator(path_))
                Load(f.path());
        }
        virtual void LoadAll(const std::vector<Import>& imports) {
            for (const auto& f : imports) {
                if (f.id.empty())
                    SetItemID(std::fs::proximate(f.path, path_).generic_string());
                else
                    SetItemID(f.id);
                SetAdditionalData(f.additionalData);
                Load(path_ / f.path);
            }
        }
        virtual void Load(const std::fs::path& p) {
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
        void Set(const std::string& id, const T& val) {
            items_[id] = val;
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
        void LoadShader(const std::string&, const std::string&, const std::string&, const std::string& = "");
        void LoadStandardShader(Shaders::ShaderID, const std::string&, Shaders::ShaderType);
        void LoadStandardShader(Shaders::ShaderID, const std::string&, const std::string&, const std::string& = "");
    public:
        ShaderManager() : ResourceManager<GLuint>(Paths::SHADER_DIR) { }
        virtual void LoadAll(const std::vector<ShaderImport>&);
        GLuint& Get(Shaders::ShaderID);
        GLuint& Get(const std::string& s) { return ResourceManager::Get(s); }
    };

    #define BASE_FONT_SIZE 48
    class FontManager : public ResourceManager<UI::Text::Font> {
    protected:
        std::optional<UI::Text::Font> LoadResource(const std::fs::path&) override;
        glm::ivec2 fontSize_ = { 0, BASE_FONT_SIZE };
    public:
        FontManager() : ResourceManager<UI::Text::Font>(Paths::FONTS_DIR, "font") { }
        void SetFontSize(const glm::ivec2&);
        void SetFontSize(int);
    };

    class ModelManager : public ResourceManager<Model> {
    protected:
        std::optional<Model> LoadResource(const std::fs::path&) override;
    public:
        ModelManager() : ResourceManager<Model>(Paths::MODELS_DIR, "model") { }
    };

    struct VideoSettings {
        float gamma;
        bool useVsync;
        bool fullscreen;
        glm::ivec2 resolution;
        glm::ivec2 fullscreenResolution;
    };
};

class ResourceManager {
public:
    const CFG::CFGObject* imports;
    Serializer::MaterialSerializer materialsFile;
    Serializer::ObjectSerializer objectsFile;

    Resources::VideoSettings videoSettings;

    Resources::TextureManager textureManager;
    Resources::ShaderManager shaderManager;
    Resources::FontManager fontManager;
    Resources::ModelManager modelManager;

    void RestoreDefaultVideoSettings();
    void ParseVideoSettings(CFG::CFGObject*);
    void LoadAll();
};