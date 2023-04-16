#include <iostream>
#include <functional>
#include <unordered_map>
#include <any>
#include "router.hpp"
int main() {
    std::cout << "Hello, World!" << std::endl;
    //std::unordered_map<std::string, std::function<int(int, int)>> func_map;
    std::unordered_map<std::string, std::any> func_map;
    std::any func1 = std::function<int(int,int)>([] (int a, int b) -> int {
        return a+b;
    });
//    std::any func2 = [] (int x) -> void {
//        std::cout << x <<std::endl;
//    };
//    auto ff = std::any_cast<std::function<int(int,int)>>(func1);
//    std::cout << ff(1,2) <<std::endl;
    func_map.insert(std::make_pair("add", func1));
    func_map.insert(std::make_pair("print", std::function<void(int)>([] (int x) -> void {
        std::cout << x <<std::endl;
    })));

    auto f1 = std::any_cast<std::function<int(int, int)> &>(func_map.at("add"));
    std::cout << f1(1,2) << std::endl;

    auto f2 = std::any_cast<std::function<void(int)> &>(func_map.at("print"));
    f2(4);

    // test router

    Router router;

    router.Register("print", std::function<void(int)>([] (int x) -> void {
        std::cout << x <<std::endl;
    }));
    router.Register("add", std::function<int(int, int)>([] (int x, int y) -> int {
        return x+y;
    }));

    router.Call<std::function<void(int)>, void>("print", 234);
    std::cout << router.Call<std::function<int(int, int)>, int>("add", 234, 123) << std::endl;
    return 0;
}
