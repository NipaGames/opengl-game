#pragma once

#include <any>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <spdlog/spdlog.h>

enum class ComponentDataValueType {
    SINGLE,
    VECTOR
};

class ComponentData;
class IComponentDataValue {
public:
    std::string name;
    const type_info* type;
    ComponentDataValueType componentType;
    virtual void CloneValuesTo(const std::shared_ptr<IComponentDataValue>&) = 0;
    // Only present in vector types
    virtual void CopyValuesFromComponentDataArray(const ComponentData&) = 0;
};

typedef std::unordered_map<std::string, std::shared_ptr<IComponentDataValue>> VariableMap;
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
    const std::shared_ptr<IComponentDataValue> GetComponentDataValue(const std::string& key) {
        if (!vars.count(key))
            return nullptr;
        return vars.at(key);
    }
};

template<typename T>
class ComponentDataValue : public IComponentDataValue {
public:
    T* ptr = nullptr;
    T val;
    ComponentDataValue() {
        this->type = &typeid(T);
        this->componentType = ComponentDataValueType::SINGLE;
    }
    ComponentDataValue(const std::string& name, T* ptr) : ComponentDataValue() {
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
        std::shared_ptr<ComponentDataValue<T>> data = std::static_pointer_cast<ComponentDataValue<T>>(c);
        data->name = name;
        data->val = val;
        if (data->ptr != nullptr) {
            *data->ptr = val;
        }
    }
    virtual void CopyValuesFromComponentDataArray(const ComponentData&) { }
};

template<typename T>
class ComponentDataValueVector : public ComponentDataValue<std::vector<T>> {
public:
    ComponentDataValueVector() {
        this->type = &typeid(T);
        this->componentType = ComponentDataValueType::VECTOR;
    }
    ComponentDataValueVector(const std::string& name, std::vector<T>* ptr) : ComponentDataValueVector() {
        this->name = name;
        this->ptr = ptr;
        if (ptr != nullptr)
            this->val = *this->ptr;
    }
    virtual void CopyValuesFromComponentDataArray(const ComponentData& data) override {
        if (this->ptr == nullptr)
            return;
        this->ptr->reserve(data.vars.size());
        for (auto v : data.vars) {
            if (v.second->type != &typeid(T))
                continue;
            auto dataValue = std::dynamic_pointer_cast<ComponentDataValue<T>>(v.second);
            int i = std::stoi(v.first);
            this->ptr->insert(this->ptr->begin() + i, dataValue->val);
            this->val = *this->ptr;
        }
    }
    static std::shared_ptr<ComponentDataValueVector<T>> Create(const std::string& name, std::vector<T>* ptr, VariableMap& map) {
        std::shared_ptr<ComponentDataValueVector<T>> c = std::make_shared<ComponentDataValueVector<T>>(name, ptr);
        map[name] = c;
        return c;
    }
};

#define DEFINE_COMPONENT_DATA_VALUE(T, name, val) \
T name = val; \
std::shared_ptr<ComponentDataValue<T>> _valPtr_##name = ComponentDataValue<T>::Create(#name, &name, this->data.vars)

#define DEFINE_COMPONENT_DATA_VALUE_DEFAULT(T, name) \
T name; \
std::shared_ptr<ComponentDataValue<T>> _valPtr_##name = ComponentDataValue<T>::Create(#name, &name, this->data.vars)

#define DEFINE_COMPONENT_DATA_VALUE_VECTOR(T, name) \
std::vector<T> name; \
std::shared_ptr<ComponentDataValue<std::vector<T>>> _valPtr_##name = ComponentDataValueVector<T>::Create(#name, &name, this->data.vars)

struct ComponentType;
class Entity;
class IComponent {
friend class Entity;
private:
    bool hasStarted_ = false;
    bool hasHadFirstUpdate_ = false;
    static inline std::vector<ComponentType> COMPONENT_TYPES_;
public:
    ComponentData data;
    Entity* parent;
    size_t typeHash;
    virtual IComponent* Clone() const = 0;
    virtual ~IComponent() = default;
    virtual void IStart() = 0;
    virtual void IFirstUpdate() = 0;
    virtual void IUpdate() = 0;
    virtual void IFixedUpdate() = 0;
    template<typename C>
    static IComponent* CreateInstance(const ComponentData& data) {
        IComponent* c = new C();
        for (const auto&[k, v] : data.vars) {
            v->CloneValuesTo(c->data.vars[k]);
        }
        return c;
    }
    template<typename C>
    static bool RegisterComponent() {
        const type_info* type = &typeid(C);
        std::string name = std::string(type->name());
        size_t i = name.find(" ");
        if (i != std::string::npos)
            name = name.substr(i + 1);

        COMPONENT_TYPES_.push_back({ name, type, CreateInstance<C> });
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
    virtual ~Component() = default;
    virtual IComponent* Clone() const override {
        return new Derived(dynamic_cast<const Derived&>(*this));
    }
    void IStart() override { dynamic_cast<Derived*>(this)->Start(); }
    void IUpdate() override { dynamic_cast<Derived*>(this)->Update(); }
    void IFirstUpdate() override { dynamic_cast<Derived*>(this)->FirstUpdate(); }
    void IFixedUpdate() override { dynamic_cast<Derived*>(this)->FixedUpdate(); }

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
    virtual void FixedUpdate() { }
    virtual void FirstUpdate() { }
};