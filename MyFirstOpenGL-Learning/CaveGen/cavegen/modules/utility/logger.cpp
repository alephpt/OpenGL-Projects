#include "logger.h"

#include <cstdarg>
#include <cstdio>
#include <iostream>

void Logger::SetLevel(DebugLevel level)
    {
        Logger::level = level;
    }

void Logger::Error(const char *message, ...)
    {
        if (Logger::level < DebugLevel::Error) { return; }

        va_list args;
        va_start(args, message);
        printf("[ERROR] - ");
        vprintf(message, args);
        va_end(args);
    }

void Logger::Warning(const char *message, ...)
    {
        if (Logger::level < DebugLevel::Warning) { return; }

        va_list args;
        va_start(args, message);
        printf("[WARNING] - ");
        vprintf(message, args);
        va_end(args);
    }

void Logger::Info(const char *message, ...)
    {
        if (Logger::level < DebugLevel::Info) { return; }

        va_list args;
        va_start(args, message);
        printf("[INFO] - ");
        vprintf(message, args);
        va_end(args);
    }

void Logger::Debug(const char *message, ...)
    {
        if (Logger::level < DebugLevel::Debug) { return; }

        va_list args;
        va_start(args, message);
        printf("[DEBUG] - ");
        vprintf(message, args);
        va_end(args);
    }

void Logger::Verbose(const char *message, ...)
    {
        if (Logger::level < DebugLevel::Verbose) { return; }

        va_list args;
        va_start(args, message);
        printf("[VERBOSE] - ");
        vprintf(message, args);
        va_end(args);
    }