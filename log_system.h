#pragma once


#include <cstdarg>
#include <iostream>
#include <sstream>
#include <string>


#define LOG Log(__FILE__, __LINE__)


class LogSystem {

    friend class Log;

public:

    static LogSystem* Get() { return sInstance; }

    static void StartUp();
    static void ShutDown();
    
    ~LogSystem() = default;
    

private:

    LogSystem() = default;

    static void DebugPrint(const std::string &file, const int line, const std::string &message, va_list messageArgs);
    static void WriteBufferToFile(const std::string &file, std::ostringstream &stream, const char *buffer);

    // config variables
    static const std::string sLogFileDir;
    static const size_t sBytesToBuffer;
    static const size_t sMaxMessageChars;

    static LogSystem* sInstance;
    
    std::string mLogFileName;
    std::ostringstream mLogFileStream;
};


class Log {

public:

    Log(std::string file_, int line_) : file {file_}, line {line_} {}
    ~Log() = default;

    void operator() (const std::string &message);
    void operator() (const char *message, ...);


private:

    std::string file;
    int line;
};