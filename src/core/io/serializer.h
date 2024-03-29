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

    template<typename F>
    class IValueSerializer {
    public:
        F fn;
        virtual bool CompareToComponentType(std::shared_ptr<IComponentDataValue>) const = 0;
        virtual bool HasType(const type_info*) const = 0;
    };

    template<typename F, typename... T>
    class ValueSerializer : public IValueSerializer<F> {
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
    
    // must return something
    template<typename F,  typename... T>
    void* AddSerializer(std::vector<std::shared_ptr<IValueSerializer<F>>>& vec, const F& f) {
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
            auto v = std::make_shared<ValueSerializer<F, T...>>();
            v->fn = f;
            vec.push_back(v);
        }
        return nullptr;
    }

    template<typename T, typename P, typename M>
    bool SetPointerValue(T* ptr, const P& param, const M& map) {
        auto it = std::find_if(map.begin(), map.end(), [&](const auto& s) {
            return s->HasType(&typeid(T));
        });
        if (it == map.end())
            return false;
        const auto& serializer = *it;
        SerializationArgs args(SerializerType::ANY_POINTER);
        args.ptr = ptr;
        return (serializer->fn)(args, param);
    }

    inline std::vector<std::shared_ptr<IValueSerializer<JSONSerializerFunction>>> JSON_SERIALIZERS;

    template<typename... T>
    void* AddJSONSerializer(const JSONSerializerFunction& f) {
        return AddSerializer<JSONSerializerFunction, T...>(JSON_SERIALIZERS, f);
    }

    template<typename T>
    void* AddJSONEnumSerializer() {
        return AddSerializer<JSONSerializerFunction, T>(JSON_SERIALIZERS, [](Serializer::SerializationArgs& args, const nlohmann::json& j) {
            if (!j.is_string())
                return false;
            auto e = magic_enum::enum_cast<T>((std::string) j);
            if (!e.has_value())
                return false;
            args.Return(e.value());
            return true;
        });
    }

    bool SetJSONComponentValue(IComponent*, const std::string&, const nlohmann::json&, const std::string& = "");
    template<typename T>
    bool SetJSONPointerValue(T* ptr, const nlohmann::json& jsonVal) {
        return SetPointerValue<T>(ptr, jsonVal, Serializer::JSON_SERIALIZERS);
    }

    enum class SerializationStatus {
        OK,
        FAILED
    };
    class IFileSerializer {
    protected:
        std::string path_;
        SerializationStatus status_;
        virtual bool ParseContents(std::ifstream&) = 0;
    public:
        virtual ~IFileSerializer() = default;
        virtual void SerializeFile();
        virtual void SerializeFile(const std::string&);
        IFileSerializer(const std::string& p) : path_(p) { }
        IFileSerializer() = default;
        SerializationStatus GetReturnStatus() { return status_; }
        bool Success() { return GetReturnStatus() == SerializationStatus::OK; }
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
        bool HasItem(const std::string& item) const {
            return items_.count(item) > 0;
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
        virtual bool ParseJSON() = 0;
        virtual bool ParseContents(std::ifstream&) override;
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
