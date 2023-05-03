#pragma once

#include <any>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <spdlog/spdlog.h>

class IComponentDataValue {
public:
    std::string name;
    const type_info* type;
    virtual void CloneValuesTo(const std::shared_ptr<IComponentDataValue>&) = 0;
};

typedef std::unordered_map<std::string, std::shared_ptr<IComponentDataValue>> VariableMap;

template<typename T>
class ComponentDataValue : public IComponentDataValue {
public:
    T* ptr = nullptr;
    T val;
    ComponentDataValue() {
        this->type = &typeid(T);
    }
    ComponentDataValue(const std::string& name, T* ptr) {
        this->type = &typeid(T);
        this->name = name;
        this->ptr = ptr;
        if (ptr != nullptr)
            val = *ptr;
    }
    static std::shared_ptr<ComponentDataValue<T>> Create(const std::string& name, T* ptr, VariableMap& map) {
        std::shared_ptr<ComponentDataValue<T>> c = std::make_shared<ComponentDataValue<T>>(name, ptr);
        map[name] = c;
        return c;
    }
    virtual void CloneValuesTo(const std::shared_ptr<IComponentDataValue>& c) override {
        std::shared_ptr<ComponentDataValue<T>>& data = std::static_pointer_cast<ComponentDataValue<T>>(c);
        data->name = name;
        data->val = val;
        if (data->ptr != nullptr) {
            *data->ptr = val;
        }
    }
};

class ComponentData {
public:
    VariableMap vars;
    template<typename T>
    const T& Get(const std::string& key) {
        if (!vars.count(key))
            ComponentDataValue<T>::Create(key, nullptr, vars);
        std::shared_ptr<ComponentDataValue<T>> data = std::static_pointer_cast<ComponentDataValue<T>>(vars.at(key));
        if (data->ptr == nullptr)
            return data->val;
        else
            return *data->ptr;
    }
    template<typename T>
    void Set(const std::string& key, const T& val) {
        if (!vars.count(key))
            ComponentDataValue<T>::Create(key, nullptr, vars);
        std::shared_ptr<ComponentDataValue<T>> data = std::static_pointer_cast<ComponentDataValue<T>>(vars.at(key));
        data->val = val;
        if (data->ptr != nullptr)
            *data->ptr = val;
    }
    const type_info* GetType(const std::string& key) {
        if (!vars.count(key))
            return nullptr;
        return vars.at(key)->type;
    }
};

#define DEFINE_COMPONENT_DATA_VALUE(T, name, val) \
T name = val; \
std::shared_ptr<ComponentDataValue<T>> _valPtr_##name = ComponentDataValue<T>::Create(#name, &name, this->data.vars)

#define DEFINE_COMPONENT_DATA_VALUE_DEFAULT(T, name) \
T name; \
std::shared_ptr<ComponentDataValue<T>> _valPtr_##name = ComponentDataValue<T>::Create(#name, &name, this->data.vars)

struct ComponentType;

class IComponent {
friend class Entity;
private:
    static inline std::vector<ComponentType> COMPONENT_TYPES_;
public:
    ComponentData data;
    Entity* parent;
    size_t typeHash;
    virtual IComponent* Clone() const = 0;
    virtual ~IComponent() = default;
    virtual void IStart() = 0;
    virtual void IUpdate() = 0;
    static IComponent* CreateInstance(const ComponentData&) { return nullptr; }
    template<typename C>
    static bool RegisterComponent() {
        const type_info* type = &typeid(C);
        std::string name = std::string(type->name());
        size_t i = name.find(" ");
        if (i != std::string::npos)
            name = name.substr(i + 1);

        COMPONENT_TYPES_.push_back({ name, type, C::CreateInstance });
        std::cout << "<Registered component " << name << ">" << std::endl;
        return true;
    }
};
// all components that aren't straightly derived from Component will need this
#define REGISTER_COMPONENT(C) inline bool _isRegistered_##C = IComponent::RegisterComponent<C>()

struct ComponentType {
    std::string name;
    const type_info* type;
    IComponent*(*initializer)(const ComponentData&);
};

template<class Derived>
class Component : public IComponent {
protected:
    inline static bool _isRegistered = IComponent::RegisterComponent<Derived>();
public:
    virtual ~Component() { }
    virtual IComponent* Clone() const override {
        return new Derived(static_cast<const Derived&>(*this));
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
    }

    virtual void Start() { }
    virtual void Update() { }
    static IComponent* CreateInstance(const ComponentData& data) {
        IComponent* c = new Derived();
        for (auto[k, v] : data.vars) {
            v->CloneValuesTo(c->data.vars[k]);
        }
        return c;
    }
};