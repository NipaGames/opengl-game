#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <nlohmann/json.hpp>
#include <opengl.h>

#include <core/entity/entity.h>

namespace Stage {
    struct Stage {
        std::string id;
        std::list<Entity> entities;
        std::unordered_set<size_t> instantiatedEntities;
    };

    // Returns a vector of all loaded stage IDs with the most recent being at index 0
    const std::vector<std::string>& GetLoadedStages();
    Stage ReadStageFromFile(const std::string&);
    void AddStage(const Stage&);
    bool LoadStage(const std::string&);
    bool UnloadStage(const std::string&);
    void UnloadAllStages();

    // this is just messed up, sorry
    // may god have mercy on anyone who has chosen to read the following code
    // -----------------------------
    typedef std::function<bool(ComponentData&, const std::string&, const nlohmann::json&)> SerializerFunction;
    class IValueSerializer {
    public:
        SerializerFunction fn;
        virtual bool CompareType(std::shared_ptr<IComponentDataValue>) const = 0;
        virtual bool HasType(const type_info*) const = 0;
    };
    template<typename... T>
    class ValueSerializer : public IValueSerializer {
        bool CompareType(std::shared_ptr<IComponentDataValue> d) const override {
            bool found = false;
            ([&] {
                // dynamic_pointer_cast won't do here so we'll need this workaround
                void* cast;
                switch (d->componentType) {
                    case ComponentDataValueType::SINGLE:
                        cast = dynamic_cast<ComponentDataValue<T>*>(&*d);
                        break;
                    case ComponentDataValueType::VECTOR:
                        cast = dynamic_cast<ComponentDataValue<std::vector<T>>*>(&*d);
                        break;
                }
                if (cast != nullptr) {
                    found = true;
                    return;
                }
            }(), ...);
            return found;
        }
        bool HasType(const type_info* t) const override {
            bool found = false;
            ([&] {
                if (t == &typeid(T)) {
                    found = true;
                    return;
                }
            }(), ...);
            return found;
        }
    };
    inline std::vector<std::shared_ptr<IValueSerializer>> _COMPONENT_VAL_SERIALIZERS;
    // must return something
    template<typename... T>
    void* AddSerializer(const SerializerFunction& f) {
        auto count = std::count_if(_COMPONENT_VAL_SERIALIZERS.begin(), _COMPONENT_VAL_SERIALIZERS.end(), [&](const auto& s) {
            bool found = false;
            ([&] {
                if (s->HasType(&typeid(T))) {
                    found = true;
                    return;
                }
            }(), ...);
            return found;
        });
        if (count == 0) {
            auto v = std::make_shared<ValueSerializer<T...>>();
            v->fn = f;
            _COMPONENT_VAL_SERIALIZERS.push_back(v);
        }
        return nullptr;
    }
}