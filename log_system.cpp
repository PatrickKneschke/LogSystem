
#include "log_system.h"

#include <assert.h>
#include <ctime>
#include <filesystem>
#include <fstream>


LogSystem* LogSystem::sInstance = nullptr;

const std::string LogSystem::sLogFileDir    = "Log/";
const size_t LogSystem::sBytesToBuffer      = 0;
const size_t LogSystem::sMaxMessageChars    = 1023;


LogSystem::~LogSystem() {

    if(sInstance != nullptr)
    {
        delete sInstance;
        sInstance = nullptr;
    }
}


void LogSystem::StartUp() {

    if(sInstance == nullptr)
    {
        sInstance = new LogSystem();
    }

    auto now = std::time(nullptr);
    char buffer[sizeof("YYYY-MM-DD_HH:MM:SS")];    
    sInstance->mLogFileName = std::string(buffer, buffer + std::strftime(buffer, sizeof(buffer),"%F_%T", std::gmtime(&now))) + ".log";

    std::filesystem::create_directory(sLogFileDir);
}


void LogSystem::DebugPrint(const std::string &file, const int line, const std::string &message, ...) {

    assert(sInstance != nullptr);

    static char buffer[sMaxMessageChars];

    std::ostringstream stream;
    stream << file << " [" << line << "] : " << message;

    va_list messageArgs;
    va_start(messageArgs, &message);

    int messageLength = vsnprintf(buffer, sMaxMessageChars, stream.str().c_str(), messageArgs);
    buffer[messageLength] = '\0';

    WriteBufferToFile(mLogFileName, mLogFileStream, buffer);
}


void LogSystem::WriteBufferToFile(const std::string &fileName, std::ostringstream &stream, const char *buffer) {

    stream << buffer;
    stream.seekp(0, std::ios::end);
    if( static_cast<int>(stream.tellp()) >= sBytesToBuffer )
    {
        stream.flush();

        std::ofstream file(sLogFileDir + fileName, std::ios_base::app);
        file << stream.str() << '\n';
        file.close();

        stream.str("");
        stream.clear();
        stream.seekp(0);
    }
}