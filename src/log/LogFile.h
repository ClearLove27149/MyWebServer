//
// Created by 26685 on 2023/3/26.
//
#pragma once
#ifndef MYWEBSERVER_LOGFILE_H
#define MYWEBSERVER_LOGFILE_H

#include <memory>
#include <string>
#include <mutex>
#include <stdio.h>

#include "FileUtil.h"
#include "asio/detail/noncopyable.hpp"

//
class LogFile : public asio::noncopyable {
public:
    LogFile(const std::string basename, int flushEveryN = 1024) : _basename(basename), _flushEveryN(flushEveryN),
    _count(0) {
        // 每次append flushEveryN次， flush一次，写文件
        _file.reset(new AppendFile(basename));
    }
    ~LogFile() { }
    void append(const char* logline, int len) {
        std::lock_guard<std::mutex> lock(_mtx);
        append_unlocked(logline, len);
    }
    void flush() {
        std::lock_guard<std::mutex> lock(_mtx);
        _file->flush();
    }
    bool rollFile();

private:
    void append_unlocked(const char* logline, int len) {
        _file->append(logline, len);
        // test
        _file->flush();
        ++_count;
        if (_count >= _flushEveryN) {
            _count = 0;
            _file->flush();
        }
    }
    const std::string _basename;
    const int _flushEveryN;
    int _count;
    std::mutex _mtx;
    std::unique_ptr<AppendFile> _file;
};
#endif //MYWEBSERVER_LOGFILE_H
