//
// Created by 26685 on 2023/3/26.
//
#pragma once
#ifndef MYWEBSERVER_SEMAPHORE_H
#define MYWEBSERVER_SEMAPHORE_H
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class Semaphore {
public:
    explicit Semaphore(int count = 0) : count_(count) {}

    void Signal() {
        std::unique_lock<std::mutex> lock(mutex_);
        ++count_;
        cv_.notify_one();
    }

    void Wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [=] { return count_ > 0; });
        --count_;
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int count_;
};
#endif //MYWEBSERVER_SEMAPHORE_H
