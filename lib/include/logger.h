#pragma once
#include "lockqueue.h"
#include <iostream>
#include <time.h>
#include <string>
#include <thread>

enum LogLevel {
    INFO,   //信息
    ERROR,  //错误
};

class Logger {
public:
    //设置日志级别
    void SetLogLevel(LogLevel level);

    //写日志
    void Log(std::string msg);


    //单例
    static Logger& GetInstance();

private:
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;

    int m_loglevel;                     //记录日志级别
    LockQueue<std::string> m_lckQue;    //日志缓冲队列
};

//宏
#define LOG_INFO(logmsgformat, ...) \
    do { \
        Logger& logger = Logger::GetInstance(); \
        logger.SetLogLevel(INFO); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__);    \
        logger.Log(c); \
    } \
    while (0);

#define LOG_ERROR(logmsgformat, ...) \
    do { \
        Logger& logger = Logger::GetInstance(); \
        logger.SetLogLevel(ERROR); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__);    \
        logger.Log(c); \
    } \
    while (0);
