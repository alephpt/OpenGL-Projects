#include "logger.h"

#include <cstdarg>
#include <cstdio>
#include <iostream>

DebugLevel Logger::level = DebugLevel::Info;

void Logger::Error(const char *message, ...)
    {
        if (level < DebugLevel::Error) { return; }

        va_list args;
        va_start(args, message);
        printf("[ERROR] - ");
        vprintf(message, args);
        va_end(args);
    }

void Logger::Warning(const char *message, ...)
    {
        if (level < DebugLevel::Warning) { return; }

        va_list args;
        va_start(args, message);
        printf("[WARNING] - ");
        vprintf(message, args);
        va_end(args);
    }

void Logger::Info(const char *message, ...)
    {
        if (level < DebugLevel::Info) { return; }

        va_list args;
        va_start(args, message);
        printf("[INFO] - ");
        vprintf(message, args);
        va_end(args);
    }

void Logger::Debug(const char *message, ...)
    {
        if (level < DebugLevel::Debug) { return; }

        va_list args;
        va_start(args, message);
        printf("[DEBUG] - ");
        vprintf(message, args);
        va_end(args);
    }

void Logger::Verbose(const char *message, ...)
    {
        if (level < DebugLevel::Verbose) { return; }

        va_list args;
        va_start(args, message);
        printf("[VERBOSE] - ");
        vprintf(message, args);
        va_end(args);
    }