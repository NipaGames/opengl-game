#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <core/entity/component.h>

// this is just messed up, sorry
// may god have mercy on anyone who has chosen to read the following code
// -----------------------------
/*
    Well, I'll write a small documentation for all this serialization crap

    So, for example, somehow we need to turn the following JSON:
        pos: [ 0.0, 2.0, 1.0 ]
    into:
        glm::vec3(0.0f, 2.0f, 1.0f)

    given that we know the type (glm::vec3, my magnicifient "reflection" interface will do that),
    we need to call the serializer of the corresponding type.

    Every serializable type will need to have a ValueSerializer,
    few of the default types are defined at 'serializetypes.h'.

    In this case we want to make a instance of JSONValueSerializer with a type of glm::vec3.
    JSONValueSerializer will have 'fn' function pointer which will get called when serializing a variable.
    The JSON will be passed as a parameter for 'fn'.

    I've provided a bunch of macros defined in 'serializetypes.h' to make defining the serializers for each
    type as painless as possible.


    The inheritance makes this kind of messy, but it will hopefully come in handy if I ever want to support
    more file formats (XML, custom binary formats, etc.).
*/
namespace Serializer {
    typedef std::function<bool(ComponentData&, const std::string&, const nlohmann::json&)> JSONSerializerFunction;

    class IValueSerializer {
    public:
        virtual bool CompareType(std::shared_ptr<IComponentDataValue>) const = 0;
        virtual bool HasType(const type_info*) const = 0;
    };

    template<typename... T>
    class ValueSerializer : public IValueSerializer {
    public:
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
    
    class IJSONValueSerializer : public IValueSerializer {
    public:
        JSONSerializerFunction fn;
    };

    template<typename... T>
    class JSONValueSerializer : public IJSONValueSerializer, public ValueSerializer<T...> {
    public:
        virtual bool CompareType(std::shared_ptr<IComponentDataValue> v) const { return ValueSerializer<T...>::CompareType(v); }
        virtual bool HasType(const type_info* t) const { return ValueSerializer<T...>::HasType(t); }
    };
    
    inline std::vector<std::shared_ptr<IJSONValueSerializer>> JSON_COMPONENT_VAL_SERIALIZERS;
    // must return something
    template<typename... T>
    void* AddJSONSerializer(const JSONSerializerFunction& f) {
        auto count = std::count_if(JSON_COMPONENT_VAL_SERIALIZERS.begin(), JSON_COMPONENT_VAL_SERIALIZERS.end(), [&](const auto& s) {
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
            auto v = std::make_shared<JSONValueSerializer<T...>>();
            v->fn = f;
            JSON_COMPONENT_VAL_SERIALIZERS.push_back(v);
        }
        return nullptr;
    }

    class IFileSerializer {
    public:
        virtual void Serialize(const std::string&) = 0;
    };

    class JSONFileSerializer : public IFileSerializer {

    };
};
