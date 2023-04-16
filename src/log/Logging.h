// Created by 26685 on 2023/3/26.
//
#pragma once
#ifndef MYWEBSERVER_LOGGING_H
#define MYWEBSERVER_LOGGING_H

#include <stdio.h>
#include <string.h>
#include <string>
#include <thread>
#include <mutex>
#include <iomanip>
#include <assert.h>
#include <time.h>
#include <sys/time.h>

#include "LogConfig.h"
#include "LogStream.h"
#include "AsyncLogging.h"

//class AsyncLogging;

void output(const char* msg, int len);
void async_logger_init();

class Logger {
public:
    Logger(const char* fileName, int line, LoggerLevel level) : m_impl(fileName, line, level) { }
    ~Logger() {
        //std::cout << "Logger delete" << std::endl;
        m_impl.im_stream << " -- " << m_impl.im_basename << ':' << m_impl.im_line << '\n';
        const LogStream::Buffer& buf(stream().buffer());
        output(buf.data(), buf.length());
    }
    LogStream &stream() { return m_impl.im_stream; }
    static void setLogFileName(std::string fileName) { m_logFileName = fileName;}
    static std::string getLogFileName()  { return m_logFileName; }

private:
    class Impl {
    public:
        LogStream im_stream;
        int im_line;
        std::string im_basename;
        Impl(const char* fileName, int line, LoggerLevel level) : im_stream(), im_line(line), im_basename(fileName) {
            im_stream.setLevel(level);
            formatLevel(level);
            formatTime();
        }
        void formatLevel(LoggerLevel level) {
            switch (level) {
                case LOG_INFO:
                    im_stream << "[INFO]    ";
                    break;
                case LOG_ERROR:
                    im_stream << "[ERROR]   ";
                    break;
                case LOG_WARNING:
                    im_stream << "[WARNING] ";
                    break;
                case LOG_ALL:
                    im_stream << "[ALL]     ";
                    break;
                case LOG_TRACE:
                    im_stream << "[TRACE]   ";
                    break;
                case LOG_DEBUG:
                    im_stream << "[DEBUG]   ";
                    break;
            }

        }
        void formatTime() { // c++ 重写？
            auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::stringstream ss;
            ss << std::put_time(std::localtime(&t), "%Y-%m-%d %H.%M.%S");
            std::string str = ss.str();
            im_stream << str << " -- ";
            //cout << std::put_time(std::localtime(&t), "%Y-%m-%d %H.%M.%S") << endl;

//            struct timeval tv;
//            time_t time;
//            char str_t[26] = {0};
//            gettimeofday(&tv, NULL);
//            time = tv.tv_sec;
//            struct tm* p_time = localtime(&time);
//            strftime(str_t, 26, "%Y-%m-%d %H:%M:%s\n", p_time);
//            im_stream << str_t;
//            std::cout << str_t << std::endl;
        }

    };

private:
    static std::string m_logFileName;
    Impl m_impl;
};


// common
static AsyncLogging* g_AsyncLogger;

static std::once_flag onceFlag;
void async_logger_init() {
    g_AsyncLogger = new AsyncLogging(Logger::getLogFileName());
    g_AsyncLogger->start();
}
void output(const char* msg, int len) {
    std::call_once(onceFlag, async_logger_init);
    g_AsyncLogger->append(msg, len);
}

#define LOG Logger(__FILE__, __LINE__, LOG_ALL).stream()

#define ERROR Logger(__FILE__, __LINE__, LOG_ERROR).stream()
#define WARNING Logger(__FILE__, __LINE__, LOG_WARNING).stream()
#define INFO Logger(__FILE__, __LINE__, LOG_INFO).stream()
#define DEBUG Logger(__FILE__, __LINE__, LOG_DEBUG).stream()
#define TRACE Logger(__FILE__, __LINE__, LOG_TRACE).stream()
#endif //MYWEBSERVER_LOGGING_H
