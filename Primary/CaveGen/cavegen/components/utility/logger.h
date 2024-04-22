#pragma once

// A Basic Logger type that can be called Logger::Debug("Message %s", "Hello World");
// This is a simple logger that can be used to log messages to the console.

enum class DebugLevel{
    Verbose = 5,
    Debug = 4,
    Info = 3,
    Warning = 2,
    Error = 1,
};

class Logger{
    public:
        static void Verbose(const char* message, ...);
        static void Debug(const char* message, ...);
        static void Info(const char* message, ...);
        static void Warning(const char* message, ...);
        static void Error(const char* message, ...);
        
        static DebugLevel level;
};