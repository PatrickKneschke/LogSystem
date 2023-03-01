#pragma once


#include <cstdarg>
#include <iostream>
#include <sstream>
#include <string>


class LogSystem {


public:

    static LogSystem* Get() { return sInstance; }
    static void StartUp();
    static void ShutDown();
    
    ~LogSystem() = default;
    
    void DebugPrint(const std::string &file, const int line, const std::string &message, ...);


private:

    LogSystem() = default;

    void WriteBufferToFile(const std::string &file, std::ostringstream &stream, const char *buffer);

    // config variables
    static const std::string sLogFileDir;
    static const size_t sBytesToBuffer;
    static const size_t sMaxMessageChars;

    static LogSystem* sInstance;
    
    std::string mLogFileName;
    std::ostringstream mLogFileStream;
};