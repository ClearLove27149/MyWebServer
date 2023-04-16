//
// Created by 26685 on 2023/3/26.
//
#pragma once
#ifndef MYWEBSERVER_ASYNCLOGGING_H
#define MYWEBSERVER_ASYNCLOGGING_H

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <memory>
#include <stdio.h>

#include "asio/detail/noncopyable.hpp"
#include "LogStream.h"
#include "LogFile.h"
#include "Semaphore.h"

class AsyncLogging : public asio::noncopyable {
public:

    AsyncLogging(std::string basename, int flushInterval = 2) \
    : m_basename(basename), m_flushInterval(flushInterval),
    m_running(false),m_currentBuffer(new Buffer),m_nextBuffer(new Buffer)
    {
        assert(basename.length() > 1);
        m_currentBuffer->bzero();
        m_nextBuffer->bzero();
        m_buffers.reserve(16);
    }

    ~AsyncLogging() {
        if (m_running) stop();
    }
    void append(const char* logline, int len) {
        std::lock_guard<std::mutex> locker(m_mutex);
        if (m_currentBuffer->avail() > len) {
            m_currentBuffer->append(logline, len);
        } else {
            m_buffers.push_back(m_currentBuffer);
            m_currentBuffer.reset();
            if (m_nextBuffer) {
                m_currentBuffer = std::move(m_nextBuffer);
            } else {
                m_currentBuffer.reset(new Buffer);
            }
            m_currentBuffer->append(logline, len);
            m_cond.notify_one();
        }
    }
    void start() {
        m_running = true;
        m_thread = std::thread(std::bind(&AsyncLogging::threadFunc, this), "Logging");
        m_semaphore.Wait();
    }
    void stop() {
        m_running = false;
        m_cond.notify_one();
        m_thread.join();
    }


private:
    void threadFunc();
    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
    typedef std::shared_ptr<Buffer> BufferPtr;
    int m_flushInterval;
    //std::chrono::duration<int> m_flushInterval;
    std::atomic_bool m_running;
    std::string m_basename;
    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    BufferPtr m_currentBuffer;
    BufferPtr m_nextBuffer;
    BufferVector m_buffers;
    Semaphore m_semaphore;
    // ?
};

void AsyncLogging::threadFunc() {
    assert(m_running == true);
    m_semaphore.Signal(); // 确保线程进入了线程函数
    //std::cout << "threadFunc" << std::endl;
    LogFile output(m_basename);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while (m_running) {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());
        {
            std::lock_guard<std::mutex> locker(m_mutex);
            if (m_buffers.empty()) {
                std::unique_lock<std::mutex> uniqueLock(*new std::mutex());
                m_cond.wait_for(uniqueLock, std::chrono::seconds(m_flushInterval));
            }
            m_buffers.push_back(m_currentBuffer);
            m_currentBuffer.reset();

            m_currentBuffer = std::move(newBuffer1);
            buffersToWrite.swap(m_buffers);
            if (!m_nextBuffer) {
                m_nextBuffer = std::move(newBuffer2);
            }
        }
        assert(!buffersToWrite.empty());
        if (buffersToWrite.size() > 25) {
            buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
        }
        //
        for (size_t i = 0; i<buffersToWrite.size(); ++i) {
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
        }
        if (buffersToWrite.size() > 2) {
            buffersToWrite.resize(2);
        }
        if (!newBuffer1) {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }
        if (!newBuffer2) {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }
        buffersToWrite.clear();
        output.flush();
    }
    //std::cout << "threadFunc" << std::endl;
    output.flush();
}
#endif //MYWEBSERVER_ASYNCLOGGING_H
