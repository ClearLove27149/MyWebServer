//
// Created by 26685 on 2023/3/26.
//
#pragma once
#ifndef MYWEBSERVER_LOGSTREAM_H
#define MYWEBSERVER_LOGSTREAM_H

#include <assert.h>
#include <string>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <algorithm>

#include "LogConfig.h"
#include "asio/detail/noncopyable.hpp"

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

const char digitsHex[] = "0123456789ABCDEF";//十六进制时使用

// common
template<typename T>
size_t convert(char buf[], T value) {
    T i = value;
    char* p = buf;
    do {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0) {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);
    return p - buf;
}
template<typename T>
size_t convertHex(char buf[], T v) {
    uintptr_t i = v;
    char* p = buf;
    do {
        int lsd = static_cast<int>(i % 16);
        i /= 16;
        *p++ = digitsHex[lsd];
    } while (i != 0);

    *p = '\0';
    std::reverse(buf, p);
}


const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*1000;

// common buffer
template<int SIZE>
class FixedBuffer : public asio::noncopyable {
public:
    FixedBuffer() : m_cur(m_data) { };
    ~FixedBuffer() { };
    void append(const char* buffer, size_t len) {
        if (avail() > static_cast<int>(len)) {
            memcpy(m_cur, buffer, static_cast<int>(len));
            m_cur  += len;
        }
    }
    const char* data() const {
        return m_data;
    }
    int length() const {
        return static_cast<int>(m_cur - m_data);
    }
    char* current() const { return m_cur; };
    int avail() const { return static_cast<int>(end() - m_cur); };
    void add(size_t len) { m_cur += len; };
    void bzero() { memset(m_data, 0, sizeof m_data); };
    void reset() { m_cur = m_data; };
private:
    const char* end() const { return m_data + sizeof m_data; };
    char* m_cur;
    char m_data[SIZE];
};
// LogStream

class LogStream : public asio::noncopyable {
public:
    typedef FixedBuffer<kSmallBuffer> Buffer;
    LogStream() : m_level(LOG_ALL) { }
    LogStream(LoggerLevel level) : m_level(level) { }
    void setLevel(LoggerLevel level) { m_level = level; }

//    template<class T>
//    LogStream& operator<< (T t) {
//        if (m_level < g_logLevel) {
//            return *this;
//        }
//        else {
//            return operator<<(dynamic_cast<T>(t));
//        }
//    }
//private:
    LogStream& operator<< (bool v) {
        if (m_level < g_logLevel) {
            return *this;
        }
        m_buffer.append(v ? "1" : "0", 1);
        return *this;
    }
    LogStream& operator<< (char v) {
        if (m_level < g_logLevel) {
            return *this;
        }
        m_buffer.append(&v, 1);
        return *this;
    }
    LogStream& operator<< (const char* v) {
        if (m_level < g_logLevel) {
            return *this;
        }
        if (v) {
            m_buffer.append(v, strlen(v));
            return *this;
        }
        m_buffer.append("(null", 6);
        return *this;
    }
    LogStream& operator<< (const unsigned char* v) {
        return operator<<(reinterpret_cast<const char*>(v));
    }
    LogStream& operator<< (const std::string& v) {
        if (m_level < g_logLevel) {
            return *this;
        }
        m_buffer.append(v.c_str(), v.length());
        return *this;
    }
    LogStream& operator<< (short v) {
        *this << static_cast<int>(v);
        return *this;
    }
    LogStream& operator<< (unsigned short v) {
        *this << static_cast<unsigned int>(v);
        return *this;
    }
    LogStream& operator<< (int v) {
        if (m_level < g_logLevel) {
            return *this;
        }
        formatInteger(v);
        return *this;
    }
    LogStream& operator<< (unsigned int v) {
        if (m_level < g_logLevel) {
            return *this;
        }
        formatInteger(v);
        return *this;
    }
    LogStream& operator<< (long v) {
        if (m_level < g_logLevel) {
            return *this;
        }
        formatInteger(v);
        return *this;
    }
    LogStream& operator<< (unsigned long v) {
        if (m_level < g_logLevel) {
            return *this;
        }
        formatInteger(v);
        return *this;
    }
    LogStream& operator<< (long long v) {
        if (m_level < g_logLevel) {
            return *this;
        }
        formatInteger(v);
        return *this;
    }
    LogStream& operator<< (unsigned long long v) {
        if (m_level < g_logLevel) {
            return *this;
        }
        formatInteger(v);
        return *this;
    }

    LogStream& operator<< (float v) {
        *this << static_cast<double>(v);
        return *this;
    }
    LogStream& operator<< (double v) {
        if (m_level < g_logLevel) {
            return *this;
        }
        if (m_buffer.avail() >= kMaxNumericSize) {
            int len = snprintf(m_buffer.current(), kMaxNumericSize, "%.12g", v);
            m_buffer.add(len);
        }
        return *this;
    }
    LogStream& operator<< (long double v) {
        if (m_level < g_logLevel) {
            return *this;
        }
        if (m_buffer.avail() >= kMaxNumericSize) {
            int len = snprintf(m_buffer.current(), kMaxNumericSize, "%.12g", v);
            m_buffer.add(len);
        }
        return *this;
    }

    // LogStream& operator<< (const void*);
public:
    void append(const char* data, int len) {
        m_buffer.append(data, len);
    }
    const Buffer& buffer() const {
        return m_buffer;
    }
    void resetBuffer() { m_buffer.reset(); };

private:
    void staticCheck();

    template<class T> void formatInteger(T v) {
        // m_buffer放不下kMax..个字符会被直接丢弃
        if (m_buffer.avail() >= kMaxNumericSize) {
            size_t len = convert(m_buffer.current(), v);
            m_buffer.add(len);
        }
    }
    Buffer m_buffer;
    static const int kMaxNumericSize = 32;
    LoggerLevel m_level;

};
#endif //MYWEBSERVER_LOGSTREAM_H
