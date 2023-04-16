//
// Created by 26685 on 2023/3/11.
//

#ifndef MYWEBSERVER_THREAD_POOL_HPP
#define MYWEBSERVER_THREAD_POOL_HPP

#include <list>
#include <iostream>
#include <functional>
#include <thread>
#include <memory>
#include <atomic>
#include "sync_queue.hpp"

namespace thread_pool {
    const int MaxTaskCount = 100;
    class ThreadPool {
    public:
        using Task = std::function<void()>;
        ThreadPool(int numThreads = std::thread::hardware_concurrency()) : m_task_queue(MaxTaskCount) {
            Start(numThreads);
        }
        ~ThreadPool() {
            Stop();
        }
        void Stop() {
            // 多线程下只调用一次StopThreadGroup
            std::call_once(m_call_stop, [this] { StopThreadGroup(); });
        }
        void AddTask(const Task& task) {
            m_task_queue.Put(task);
        }
        void AddTask(Task&& task) {
            m_task_queue.Put(std::forward<Task>(task));
        }

        void Start(int numThreads) {
            m_running= true;
            for (int i = 0; i < numThreads; i++) {
                m_thread_group.push_back(std::make_shared<std::thread>(&ThreadPool::RunThread, this));
            }
        }

    private:
        std::list<std::shared_ptr<std::thread>> m_thread_group;
        SyncQueue<Task> m_task_queue;
        std::atomic_bool m_running;
        std::once_flag m_call_stop;

        void RunThread() {
            while (m_running) {
                // take task list
                std::list<Task> tasks;
                m_task_queue.Take(tasks);
                for (auto& task : tasks) {
                    if (!m_running) {
                        return;
                    }
                    task();
                }
            }
        }
        void StopThreadGroup() {
            m_task_queue.Stop();
            m_running = false;

            for (auto running_thread  : m_thread_group) {
                if (running_thread->joinable()) {
                    running_thread->join();
                }
            }
            m_thread_group.clear();
        }
    };
}

#endif //MYWEBSERVER_THREAD_POOL_HPP
