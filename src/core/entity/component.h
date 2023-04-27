#pragma once

#include <any>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <spdlog/spdlog.h>

typedef std::unordered_map<std::string, std::any> VariableMap;

template<typename T>
class ComponentDataValue {
public:
    std::string name;
    T* ptr = nullptr;
    ComponentDataValue() { }
    ComponentDataValue(const std::string& name, T* ptr, VariableMap& map) {
        this->name = name;
        this->ptr = ptr;
        map[name] = *this;
    }
};

class ComponentData {
public:
    VariableMap vars;
    template<typename T>
    const T& Get(const std::string& key) {
        if (!vars.count(key))
            ComponentDataValue<T>(key, nullptr, vars);
        return *std::any_cast<const ComponentDataValue<T>&>(vars.at(key)).ptr;
    }
    template<typename T>
    void Set(const std::string& key, const T& val) {
        *std::any_cast<ComponentDataValue<T>&>(vars[key]).ptr = val;
    }
};

#define DEFINE_COMPONENT_DATA_VALUE(T, name, val) \
T name = val; \
ComponentDataValue<T> _valPtr_##name = ComponentDataValue<T>(#name, &name, this->data.vars)


class IComponent {
friend class Entity;
private:
    static inline std::unordered_map<const type_info*, IComponent*(*)(const ComponentData&)> COMPONENT_INITIALIZERS_;
public:
    ComponentData data;
    Entity* parent;
    virtual IComponent* Clone() const = 0;
    virtual ~IComponent() = default;
    virtual void IStart() = 0;
    virtual void IUpdate() = 0;
    static IComponent* CreateInstance(const ComponentData&) { return nullptr; }
    template<typename C>
    static bool RegisterComponent() {
        const type_info* type = &typeid(C);
        COMPONENT_INITIALIZERS_[type] = *C::CreateInstance;
        std::string name = std::string(type->name());
        size_t i = name.find(" ");
        if (i != std::string::npos)
            name = name.substr(i + 1);
        std::cout << "<Registered component " << name << ">" << std::endl;
        return true;
    }
};
// all components that aren't straightly derived from Component will need this
#define REGISTER_COMPONENT(C) inline bool _##C_isRegistered = IComponent::RegisterComponent<C>()

template<class Derived>
class Component : public IComponent {
protected:
    inline static bool _isRegistered = IComponent::RegisterComponent<Derived>();
public:
    virtual ~Component() { }
    virtual IComponent* Clone() const override {
        return new Derived(static_cast<Derived const&>(*this));
    }
    void IStart() override { static_cast<Derived*>(this)->Start(); }
    void IUpdate() override { static_cast<Derived*>(this)->Update(); }

    template<typename T>
    const T& GetValue(const std::string& key) {
        return data.Get<T>(key);
    }
    template<typename T>
    void SetValue(const std::string& key, T val) {
        data.Set(key, val);
        UpdateValues();
    }

    virtual void Start() { }
    virtual void Update() { }
    virtual void UpdateValues() { }
    static IComponent* CreateInstance(const ComponentData& data) { 
        IComponent* c = new Derived();
        c->data = data;
        static_cast<Component*>(c)->UpdateValues();
        return c;
    }
};