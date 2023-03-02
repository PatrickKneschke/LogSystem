#pragma once


#include <bitset>
#include <cstdarg>
#include <iostream>
#include <sstream>
#include <string>


enum class Verbosity {

    Off = 0,
    Error,
    Warning,
    Info,
    All
};

static const std::string ToString(Verbosity verbosity) {

    switch (verbosity) {

        case Verbosity::Off:
            return "Off";

        case Verbosity::Error:
            return "Error";

        case Verbosity::Warning:
            return "Warning";

        case Verbosity::Info:
            return "Info";

        case Verbosity::All:
            return "All";
            
        default:
            return "Unknown";
    }
}


#define LOG         Log(__FILE__, __LINE__)
#define LOG_ERROR   Log(__FILE__, __LINE__, Verbosity::Error)
#define LOG_WARNING Log(__FILE__, __LINE__, Verbosity::Warning)
#define LOG_INFO    Log(__FILE__, __LINE__, Verbosity::Info)


class LogSystem {

    friend class Log;

public:

    static void StartUp();
    static void ShutDown();
    
    ~LogSystem() = default;
    

private:

    LogSystem() = default;

    static void DebugPrint(const std::string &file, const int line, const Verbosity verbosity, const std::string &message, va_list messageArgs);
    static void WriteBufferToFile(const std::string &file, std::ostringstream &stream, const char *buffer);

    static bool CheckVerbosity(Verbosity verbosity);

    // config variables
    static const std::string sLogFileDir;
    static const size_t sBytesToBuffer;
    static const size_t sMaxMessageChars;
    static const std::bitset<static_cast<int>(Verbosity::All) - 1> sVerbosityMask;

    static LogSystem* sInstance;
    
    std::string mLogFileName;
    std::ostringstream mLogFileStream;
};


class Log {

public:

    Log(std::string file, int line, Verbosity verbosity = Verbosity::Info) : mFile {file}, mLine {line}, mVerbosity {verbosity} {}
    ~Log() = default;

    void operator() (const std::string &message);
    void operator() (const char *message, ...);


private:

    std::string mFile;
    int mLine;
    Verbosity mVerbosity; 
};