#pragma once

#include<string>
#include<stdio.h>
#include<stdarg.h>
#include<memory.h>

#define DEBUGLOG(FMT, ...) (write_log(__FILE__, __LINE__, __FUNCTION__, FMT, ##__VA_ARGS__))
//void write_log(char*prefix, char*fmt, ...);
//
void write_log(const char file[], int line, const char func[], const  char* fmt, ...);

std::string hex_to_str(const char *hex, int len);
