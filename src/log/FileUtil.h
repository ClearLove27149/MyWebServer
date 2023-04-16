//
// Created by 26685 on 2023/3/26.
//
#pragma once
#ifndef MYWEBSERVER_FILEUTIL_H
#define MYWEBSERVER_FILEUTIL_H

#include <string>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "asio/detail/noncopyable.hpp"

class AppendFile : public asio::noncopyable {
public:
    explicit AppendFile(std::string fileName) : _fp(fopen(fileName.c_str(), "a+")){
        //setbuffer(_fp, _buf, sizeof _buf);
        setvbuf(_fp, _buf, _IONBF,sizeof _buf);
    }
    ~AppendFile() {
        fclose(_fp);
    }
    // 向文件写
    void append(const char* logline, const size_t len) {
        size_t n = this->write(logline, len);
        size_t remain = len - n;
        while (remain > 0) {
            size_t x = this->write(logline + n, remain);
            if (x == 0) {
                int err = ferror(_fp);
                if (err) {
                    fprintf(stderr, "AppendFile: :append failed\n");
                    break;
                }
            }
            n += x;
            remain = len - n;
        }
    }
    void flush() {
        fflush(_fp); // 线程安全的
    }

private:
    size_t write(const char* logline, size_t len) {
#if defined(_WIN32) || defined(_WIN64)
        return fwrite(logline, 1, len, _fp);
#else
        return fwrite_unlocked(logline, 1, len, _fp);
#endif
    }
    FILE* _fp;
    char _buf[64*1024];
};
#endif //MYWEBSERVER_FILEUTIL_H
