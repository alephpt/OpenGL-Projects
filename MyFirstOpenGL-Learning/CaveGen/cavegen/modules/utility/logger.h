#pragma once

// A Basic Logger type that can be called Logger::Debug("Message %s", "Hello World");
// This is a simple logger that can be used to log messages to the console.

enum DebugLevel{
    Verbose,
    Debug,
    Info,
    Warning,
    Error
};

class Logger{
    public:
        static void Verbose(const char* message, ...);
        static void Debug(const char* message, ...);
        static void Info(const char* message, ...);
        static void Warning(const char* message, ...);
        static void Error(const char* message, ...);
        static void SetLevel(DebugLevel);

    private:
        static DebugLevel level;  
};