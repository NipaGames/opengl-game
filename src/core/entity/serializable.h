#pragma once

#include <string>
#include "component.h"

template<typename T, const char* Name = nullptr, bool Vector = false, typename VecT = int>
class Serializable {
private:
    T val_;
    std::string name_;
    IComponent* parent_;
public:
    virtual void Init() {
        if constexpr (Name != nullptr)
            name_ = Name;
        if (parent_ != nullptr) {
            if constexpr (Vector)
                ComponentDataValueVector<VecT>::Create(name_, static_cast<std::vector<VecT>*>(&val_), parent_->data.vars);
            else
                ComponentDataValue<T>::Create(name_, &val_, parent_->data.vars);
        }
    }
    Serializable(IComponent* p = nullptr) : parent_(p), val_() { Init(); }
    Serializable(const T& v, IComponent* p = nullptr) : parent_(p), val_(v) { Init(); }
    template<typename... Args>
    Serializable(Args... args, IComponent* p = nullptr) : parent_(p), val_(args...) { Init(); }
    operator T&() {
        return val_;
    }
    T& Val() {
        return val_;
    } 
};

// define serializable component data value
#define SDEF(T, name) static constexpr char _valname_##name[] = #name; Serializable<T, _valname_##name>(name)
// define serializable variable with default value
#define SDEFD(T, name) SDEF(T, name) = this
// define serializable component data value vector
#define SDEFV(T, name) static constexpr char _valname_##name[] = #name; Serializable<std::vector<T>, _valname_##name, true, T>(name)
// define serializable vector with default value
#define SDEFVD(T, name) SDEFV(T, name) = this
// don't have to type 'this' every time
#define SVAL(...) { __VA_ARGS__, this }