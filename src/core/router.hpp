//
// Created by 26685 on 2023/3/12.
//

#ifndef MYWEBSERVER_ROUTER_H
#define MYWEBSERVER_ROUTER_H

#include <functional>
#include <any>
#include <unordered_map>
#include <iostream>
/****
 * 利用any和闭包擦除参数类型，用作rpc服务的容器
 * 重写Router
 */
class Router {
public:
    Router() { };

    //template<typename F>
    bool Register(std::string func_name, std::any&& f) {
        // trnasfer to std::function
        //std::any func = std::function<F>(std::move(f));
        func_map.insert(std::make_pair(func_name, f));
    }

    template<typename F, typename Ret, typename ...Args>
    Ret Call(std::string func_name, Args&& ...args) {
        auto it = func_map.find(func_name);
        if (it != func_map.end()) {
            std::cout << "Call function: "<<it->first <<std::endl;
            auto func = std::any_cast<F>(it->second);
            auto call_func = std::bind(func, args...);
            // 这里根据function_traits得到返回值，或者在模板中指定

            return call_func(args...);
        }
    }

private:
    std::unordered_map<std::string, std::any> func_map;
};

#endif //MYWEBSERVER_ROUTER_H
