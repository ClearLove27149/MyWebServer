//
// Created by 26685 on 2023/3/11.
//

#ifndef MYWEBSERVER_SYNC_QUEUE_HPP
#define MYWEBSERVER_SYNC_QUEUE_HPP

#include <list>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace thread_pool {

    template<typename T>
    class SyncQueue {
    private:
        std::list<T> m_queue;
        std::mutex m_mutex;
        std::condition_variable m_notEmpty;
        std::condition_variable m_notFull;
        int m_maxSize;
        bool m_needStop;

        template<class F>
        void Add(F&& f) {
            std::unique_lock<std::mutex> locker(m_mutex);
            m_notFull.wait(locker, [this] { return m_needStop || NotFull(); });
            if (m_needStop) {
                return;
            }
            m_queue.push_back(std::forward<F>(f));
            m_notEmpty.notify_one();
        }
        bool NotEmpty() {
            bool empty = m_queue.empty();
            if (empty) {
                std::cout << "缓冲区满了，需要等待...，异步层的线程ID: " << std::this_thread::get_id() << std::endl;
            }
            return !empty;
        }
        bool NotFull() {
            bool full = m_queue.size() >= m_maxSize;
            if(full) {
                std::cout << "缓冲区满了，需要等待..." << std::endl;
            }
            return !full;
        }
    public:
        SyncQueue(int maxSize) : m_maxSize(maxSize), m_needStop(false) {
            //
        }
        void Put(const T& x) {
            Add(x);
        }
        void Put(T&& x) {
            Add(std::forward<T>(x));
        }
        void Take(std::list<T>& list) {
            std::unique_lock<std::mutex> locker(m_mutex);
            m_notEmpty.wait(locker, [this] { return m_needStop || NotEmpty(); });
            if (m_needStop) {
                return;
            }
            list = std::move(m_queue);
            m_notFull.notify_one();
        }
        void Take(T& x) {
            std::unique_lock<std::mutex> locker(m_mutex);
            m_notEmpty.wait(locker, [this] {return m_needStop || NotEmpty(); });
            if (m_needStop) {
                return;
            }
            x = m_queue.front();
            m_queue.pop_front();
            m_notFull.notify_one();
        }
        void Stop() {
            {
                std::lock_guard<std::mutex> locker(m_mutex);
                m_needStop = true;
            }
            m_notEmpty.notify_all();
            m_notFull.notify_all();
        }
        bool Empty() {
            std::lock_guard<std::mutex> locker(m_mutex);
            return m_queue.empty();
        }
        bool Full() {
            std::lock_guard<std::mutex> locker(m_mutex);
            return m_queue.size() == m_maxSize;
        }
        size_t Size() {
            std::lock_guard<std::mutex> locker(m_mutex);
            return m_queue.size();
        }
    };

}

#endif //MYWEBSERVER_SYNC_QUEUE_HPP
