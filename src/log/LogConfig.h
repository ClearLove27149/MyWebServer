//
// Created by 26685 on 2023/4/1.
//

#ifndef MYWEBSERVER_LOGCONFIG_H
#define MYWEBSERVER_LOGCONFIG_H


enum LoggerLevel {
    LOG_ALL = 0,
    LOG_DEBUG,
    LOG_TRACE,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
};

LoggerLevel g_logLevel = LOG_TRACE;
#endif //MYWEBSERVER_LOGCONFIG_H
