//
// Created by 26685 on 2023/3/24.
//
#include "aop_template.h"

struct AA {
    void Before(double i) {
        std::cout << "Before from AA " << i << std::endl;
    }
    void After(double i) {
        std::cout << "After from AA " << i << std::endl;
    }
};

void HT(double i) {
    std::cout << "HT function "<<i <<std::endl;
}

void test_aop() {
    Invoke<AA>(&HT, 1);
}

int main() {
    test_aop();
    static_assert(std::is_same<decltype(42, 3.14), double>::value, "Will not fire");

    return 0;
}