//
// Created by 26685 on 2023/4/1.
//
#include "InvokeTimer.h"

int main() {
    asio::io_service ioService;
    // 执行3次
    auto timer = InvokeTimer::CreateTimer(ioService, std::chrono::milliseconds(100), std::size_t(3), [] {
        std::cout << "do someting period duration 100ms.." << std::endl;
    });
    // 周期执行
    auto timer1 = InvokeTimer::CreateTimer(ioService, std::chrono::milliseconds(5000), true, [] {
        std::cout << "do someting period duration 5000ms.." << std::endl;
    });
    timer->Start();
    timer1->Start();
    ioService.run();

    return 0;
}