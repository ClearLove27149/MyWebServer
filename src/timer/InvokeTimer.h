//
// Created by 26685 on 2023/4/1.
//
#pragma once
#ifndef MYWEBSERVER_INVOKETIMER_H
#define MYWEBSERVER_INVOKETIMER_H

#include <functional>
#include <memory>
#include <iostream>

#include "asio/detail/noncopyable.hpp"
#include "asio.hpp"

class InvokeTimer;

typedef std::shared_ptr<InvokeTimer> InvokeTimerPtr;
typedef std::weak_ptr<InvokeTimer> InvokeTimerWptr;

class InvokeTimer : public std::enable_shared_from_this<InvokeTimer> {
public:
    ~InvokeTimer() { }
    typedef std::function<void()> Function;
    static InvokeTimerPtr CreateTimer(asio::io_service& ioService, const asio::steady_timer::duration& duration,
                                      bool period, Function& f) {
        InvokeTimerPtr it(new InvokeTimer(ioService, duration, f, period));
        it->self_ = it;
        return it;
    }
    static InvokeTimerPtr CreateTimer(asio::io_service& ioService, const asio::steady_timer::duration& duration,
                                      bool period, Function&& f) {
        InvokeTimerPtr it(new InvokeTimer(ioService, duration, std::move(f), period));
        it->self_ = it;
        return it;
    }
    static InvokeTimerPtr CreateTimer(asio::io_service& ioService, const asio::steady_timer::duration& duration,
                                      std::size_t count, Function& f) {
        InvokeTimerPtr it(new InvokeTimer(ioService, duration, f, count));
        it->self_ = it;
        return it;
    }
    static InvokeTimerPtr CreateTimer(asio::io_service& ioService, const asio::steady_timer::duration& duration,
                                      std::size_t count, Function&& f) {
        InvokeTimerPtr it(new InvokeTimer(ioService, duration, std::move(f), count));
        it->self_ = it;
        return it;
    }
    void Start() {
        start_ = std::chrono::system_clock::now();
        timer_.async_wait(std::bind(&InvokeTimer::OnTrigger, this, std::placeholders::_1));
    }
    void Cancel() {
        periodic_ = false;
        timer_.cancel();
    }
    inline std::size_t getTickCount() const {
        return trigger_count_;
    }
    inline int64_t getElapsedMillionsecond() const {
        auto end = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end-start_).count();
    }
    inline int64_t getElapsedSecond() const {
        auto end = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(end-start_).count();
    }
    inline bool isTriggered() const {
        return isTriggered_;
    }
private:
    explicit InvokeTimer(asio::io_service& ioService, const asio::steady_timer::duration& duration,
                         Function& f, bool period) : timer_(ioService, duration),
                         duration_(duration), periodic_(period),callback_(f), count_(0),
                         trigger_count_(0), isTriggered_(false) { }

    explicit InvokeTimer(asio::io_service& ioService, const asio::steady_timer::duration& duration,
                         Function&& f, bool period) : timer_(ioService, duration),
                         duration_(duration), periodic_(period),callback_(std::move(f)), count_(0),
                         trigger_count_(0), isTriggered_(false) { }

    explicit InvokeTimer(asio::io_service& ioService, const asio::steady_timer::duration& duration,
                         Function& f, std::size_t count) : timer_(ioService, duration),
                         duration_(duration), periodic_(true),callback_(f), count_(count),
                         trigger_count_(0), isTriggered_(false) { }

    explicit InvokeTimer(asio::io_service& ioService, const asio::steady_timer::duration& duration,
                         Function&& f, std::size_t count) : timer_(ioService, duration),
                         duration_(duration), periodic_(true),callback_(std::move(f)), count_(count),
                         trigger_count_(0), isTriggered_(false) { }

    void OnTrigger(const asio::error_code& ec) {
        if (ec.value() != 0) {
            self_.reset();
            return;
        }
        isTriggered_ = true;
        trigger_count_++;
        try {
            if (callback_) {
                callback_();
            }
        } catch (const std::error_code& e) {
            // todo : use Logger
            std::cout<<"error code: "<<e.message()<<std::endl;
        }
        if (periodic_) {
            if (count_ == 0 || trigger_count_ < count_) {
                timer_.expires_at(timer_.expiry() + duration_);
                Start();
            } else {
                self_.reset();
            }
        } else {
            self_.reset();
        }
    }
                         asio::steady_timer timer_;
    asio::steady_timer::duration  duration_;
    bool periodic_; // 是否周期执行
    std::shared_ptr<InvokeTimer> self_;
    Function callback_;
    std::size_t count_;
    std::size_t trigger_count_;
    std::chrono::system_clock::time_point  start_;
    bool isTriggered_;
};

#endif //MYWEBSERVER_INVOKETIMER_H
