//
// Created by 26685 on 2023/3/25.
//

#ifndef MYWEBSERVER_IOC_TEMPLATE1_H
#define MYWEBSERVER_IOC_TEMPLATE1_H

#include <memory>
#include <functional>
#include <unordered_map>
#include <iostream>

using namespace std;
template<typename T>
class IocContainer {
public:
    IocContainer() { };
    ~IocContainer() { };

    template<typename Drived>
    void RegisterType(string strKey) {
        std::function<T* ()> func = [] {return new Drived(); };
        RegisterType(strKey, func);
    }
    T* Resolve(string strKey) {
        if (m_creatorMap.find(strKey) == m_creatorMap.end()) {
            return nullptr;
        }
        std::function<T* ()> func = m_creatorMap[strKey];
        return func(); // call new drived();
    }
    std::shared_ptr<T> ResolveShared(string strKey) {
        T* func = Resolve(strKey);
        return std::shared_ptr<T>(func);
    }
private:
    void RegisterType(string strKey, std::function<T* ()> func) {
        if (m_creatorMap.find(strKey) != m_creatorMap.end()) {
            throw std::invalid_argument("this key is exist");
        }
        m_creatorMap.emplace(strKey, func);
    }
    unordered_map<std::string, std::function<T* ()>> m_creatorMap;
};


#endif //MYWEBSERVER_IOC_TEMPLATE1_H
