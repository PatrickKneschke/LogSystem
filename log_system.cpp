
#include "log_system.h"

#include <assert.h>
#include <ctime>
#include <filesystem>
#include <fstream>


/*    LOG SYSTEM    */

LogSystem* LogSystem::sInstance = nullptr;

const std::string LogSystem::sLogFileDir    = "Log/";
const size_t LogSystem::sBytesToBuffer      = 4096;
const size_t LogSystem::sMaxMessageChars    = 1023;

 const std::bitset<static_cast<int>(Verbosity::All) - 1> LogSystem::sVerbosityMask = static_cast<int>(Verbosity::Error) + static_cast<int>(Verbosity::Warning);


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


void LogSystem::ShutDown() {

    if(sInstance == nullptr) 
    {
        return;
    }

    sInstance->mLogFileStream.flush();

    std::ofstream file(sLogFileDir + sInstance->mLogFileName, std::ios_base::app);
    file << sInstance->mLogFileStream.str() << '\n';
    file.close();

    sInstance->mLogFileStream.str("");
    sInstance->mLogFileStream.clear();
    sInstance->mLogFileStream.seekp(0);
}


void LogSystem::DebugPrint(const std::string &file, const int line, const Verbosity verbosity, const std::string &message, va_list messageArgs) {

    assert(sInstance != nullptr);

    static char buffer[sMaxMessageChars];

    std::ostringstream stream;
    stream << file << " [" << line << "] , " << ToString(verbosity) << "\t : " << message << '\n';

    int messageLength = vsnprintf(buffer, sMaxMessageChars, stream.str().c_str(), messageArgs);
    buffer[messageLength] = '\0';

    WriteBufferToFile(sInstance->mLogFileName, sInstance->mLogFileStream, buffer);

    // also print message to default output if according to verbosity mask
    if(CheckVerbosity(verbosity)) {

        std::cout << buffer <<'\n';
    }
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


 bool LogSystem::CheckVerbosity(Verbosity verbosity) {
    
    return sVerbosityMask.test(static_cast<int>(verbosity) - 1);
 }


/*    LOG    */


void Log::operator() (const std::string &message) {

    va_list noArgs;
    LogSystem::DebugPrint(mFile, mLine, mVerbosity, message, noArgs);
}


void Log::operator() (const char *message, ...) {

    va_list messageArgs;
    va_start(messageArgs, message);

    LogSystem::DebugPrint(mFile, mLine, mVerbosity, message, messageArgs);
}