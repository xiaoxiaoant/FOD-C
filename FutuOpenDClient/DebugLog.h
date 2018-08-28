#pragma once

#include<string>
#include<stdio.h>
#include<stdarg.h>
#include<memory.h>

enum LogErrorLevel
{
    // /跟踪信息,默认Mask输出级别,默认全部输出
    LOG_TRACE   = 1,
    // /调试信息
    LOG_DEBUG   = 2,
    // /普通信息
    LOG_INFO    = 3,
    // /错误信息
    LOG_ERROR   = 4,
    // /告警类型的错误
    LOG_ALERT   = 5,
    // /致命错误
    LOG_FATAL   = 6,
};


#define LOGT(...)
#define LOGD(FMT, ...) (write_log(__FILE__, __LINE__, __FUNCTION__, FMT, ##__VA_ARGS__))
#define LOGE(...) LOGD(__VA_ARGS__)
#define LOGW(...) LOGD(__VA_ARGS__)

//void write_log(char*prefix, char*fmt, ...);
//
void write_log(const char file[], int line, const char func[], const  char* fmt, ...);

std::string hex_to_str(const char *hex, int len);
