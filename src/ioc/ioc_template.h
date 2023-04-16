//
// Created by 26685 on 2023/3/25.
//
#pragma once
#ifndef MYWEBSERVER_IOC_TEMPLATE_H
#define MYWEBSERVER_IOC_TEMPLATE_H

#include <memory>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <any>

#include "asio/detail/noncopyable.hpp"

using namespace std;

class IocContainer : public asio::noncopyable{
public:
    IocContainer() { };
    ~IocContainer() { };

    template<class T, typename Depend, typename... Args>
    typename std::enable_if<!std::is_base_of<T, Depend>::value>::type RegisterType(const string& strKey) {
        // 闭包擦除了参数类型
        std::function<T* (Args...)> func = [] (Args... args) {return new T(new Depend(args...)); };
        RegisterType(strKey, func);
    }
    template<class T, typename Depend, typename... Args>
    typename std::enable_if<std::is_base_of<T, Depend>::value>::type RegisterType(const string& strKey) {
        // 闭包擦除了参数类型
        std::function<T* (Args...)> func = [] (Args... args) {return new Depend(args...); };
        RegisterType(strKey, func);
    }
    template<class T, typename... Args>
    void RegisterSimple(const string& strKey) {
        std::function<T* (Args...)> func = [](Args... args) { return new T(args...); };
        RegisterType(strKey, func);
    }

    template<class T, typename... Args>
    T* Resolve(const string& strKey, Args... args) {
        if (m_creatorMap.find(strKey) == m_creatorMap.end()) {
            return nullptr;
        }
        any resolver = m_creatorMap[strKey];
        std::function<T* (Args...)> function = any_cast<std::function<T* (Args...)>>(resolver);

        return function(args...); //
    }

    template<class T, typename... Args>
    std::shared_ptr<T> ResolveShared(const string& strKey, Args... args) {
        T* func = Resolve<T>(strKey, args...);
        return std::shared_ptr<T>(func);
    }
private:
    void RegisterType(const string& strKey, any constructor) {
        if (m_creatorMap.find(strKey) != m_creatorMap.end()) {
            throw std::invalid_argument("this key is exist");
        }
        // 通过any擦除了不同类型的构造器
        m_creatorMap.emplace(strKey, constructor);
    }
    unordered_map<std::string, any> m_creatorMap;
};

#endif //MYWEBSERVER_IOC_TEMPLATE_H
