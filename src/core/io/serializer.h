#pragma once

#include <fstream>
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
    class SerializationArgs;
    
    template<typename T>
    using SerializerFunction = std::function<bool(SerializationArgs&, T)>;
    typedef SerializerFunction<const nlohmann::json&> JSONSerializerFunction;

    class IValueSerializer {
    public:
        virtual bool CompareToComponentType(std::shared_ptr<IComponentDataValue>) const = 0;
        virtual bool HasType(const type_info*) const = 0;
    };

    template<typename... T>
    class ValueSerializer : public IValueSerializer {
    public:
        bool CompareToComponentType(std::shared_ptr<IComponentDataValue> d) const override {
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
        virtual bool CompareToComponentType(std::shared_ptr<IComponentDataValue> v) const { return ValueSerializer<T...>::CompareToComponentType(v); }
        virtual bool HasType(const type_info* t) const { return ValueSerializer<T...>::HasType(t); }
    };
    
    inline std::vector<std::shared_ptr<IJSONValueSerializer>> JSON_COMPONENT_VAL_SERIALIZERS;
    // must return something
    template<typename S, typename I, typename F,  typename... T>
    void* AddSerializer(std::vector<std::shared_ptr<I>>& vec, const F& f) {
        auto count = std::count_if(vec.begin(), vec.end(), [&](const auto& s) {
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
            auto v = std::make_shared<S>();
            v->fn = f;
            vec.push_back(v);
        }
        return nullptr;
    }

    template<typename... T>
    void* AddJSONSerializer(const JSONSerializerFunction& f) {
        return AddSerializer<JSONValueSerializer<T...>, IJSONValueSerializer, JSONSerializerFunction, T...>(JSON_COMPONENT_VAL_SERIALIZERS, f);
    }

    bool SetJSONComponentValue(IComponent*, const std::string&, const nlohmann::json&, std::string = "");
    template<typename T>
    bool SetJSONPointerValue(T* ptr, const nlohmann::json& jsonVal) {
        auto it = std::find_if(Serializer::JSON_COMPONENT_VAL_SERIALIZERS.begin(), Serializer::JSON_COMPONENT_VAL_SERIALIZERS.end(), [&](const auto& s) {
            return s->HasType(&typeid(T));
        });
        if (it == Serializer::JSON_COMPONENT_VAL_SERIALIZERS.end())
            return false;
        const auto& serializer = *it;
        SerializationArgs args(SerializerType::ANY_POINTER);
        args.ptr = ptr;
        return (serializer->fn)(args, jsonVal);
    }


    class IFileSerializer {
    protected:
        std::string path_;
        virtual void ParseContents(std::ifstream&) = 0;
    public:
        virtual ~IFileSerializer() = default;
        virtual void SerializeFile();
        virtual void SerializeFile(const std::string&);
        IFileSerializer(const std::string& p) : path_(p) { }
        IFileSerializer() = default;
    };

    template<typename T>
    class SerializerItemInterface {
    typedef std::unordered_map<std::string, T> ItemsContainer;
    protected:
        ItemsContainer items_;
    public:
        const ItemsContainer& GetItems() const {
            return items_;
        }
        const T& GetItem(const std::string& item) const {
            return items_.at(item);
        }
        // copy all items into an external container
        virtual void Register(ItemsContainer& container) {
            for (auto& item : items_) {
                container.insert(item);
            }
        }
    };

    class JSONFileSerializer : public IFileSerializer {
    using IFileSerializer::IFileSerializer;
    protected:
        nlohmann::json jsonData_;
        virtual void ParseJSON() = 0;
        virtual void ParseContents(std::ifstream&) override;
    public:
        virtual ~JSONFileSerializer() = default;
    };

    enum class SerializerType {
        COMPONENT_DATA,
        ANY_POINTER
    };
    class SerializationArgs {
    public:
        SerializerType type;

        // metadata
        std::string entityId;

        // SerializerType::COMPONENT_DATA
        ComponentData* ctData = nullptr;
        std::string ctK;

        // SerializerType::ANY_POINTER
        void* ptr;

        SerializationArgs(const SerializerType& t) : type(t) { }

        template<typename T>
        void Return(const T& val) {
            switch (type) {
                case SerializerType::COMPONENT_DATA:
                    ctData->Set(ctK, val);
                    break;
                case SerializerType::ANY_POINTER:
                    *static_cast<T*>(ptr) = val;
                    break;
            }
        }
    };
};
