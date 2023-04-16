//
// Created by 26685 on 2023/3/11.
//
#include "thread_pool.hpp"

using namespace thread_pool;
void TestThreadPool() {
    ThreadPool pool(2);
    //pool.Start(2);

    std::thread thd1([&pool] {
        for (int i = 0; i < 10; i++) {
            auto thdid = std::this_thread::get_id();
            pool.AddTask([thdid] {
                std::cout << "同步线程1的线程id: " << thdid << std::endl;
            });
        }
    });
    std::thread thd2([&pool] {
        for (int i = 0; i < 10; i++) {
            auto thdid = std::this_thread::get_id();
            pool.AddTask([thdid] {
                std::cout << "同步线程2的线程id: " << thdid << std::endl;
            });
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(2));
    getchar();
    pool.Stop();
    thd1.join();
    thd2.join();
}

int main() {

    TestThreadPool();
    return 0;
}