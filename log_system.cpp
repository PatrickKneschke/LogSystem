
#include "log_system.h"

#include <ctime>
#include <filesystem>
#include <fstream>

#include <unordered_map>


/*    LOG SYSTEM    */

LogSystem* LogSystem::sInstance = nullptr;

std::string LogSystem::sLogFileDir    = "Log/";
size_t LogSystem::sBytesToBuffer      = 2048;
size_t LogSystem::sMaxMessageChars    = 256;
Verbosity LogSystem::sVerbosityLevel  = Verbosity::All;


void LogSystem::StartUp() {

    if(sInstance == nullptr)
    {
        sInstance = new LogSystem();
    }

    // process config file 
    std::ifstream config("log.config");
    std::unordered_map<std::string, std::string> configVar;
    std::string line;
    while (getline(config, line))
    {
        auto space = line.find(' ');
        auto name = line.substr(0, space);
        auto value = line.substr(space+1);

        configVar[name] = value;
    }
    sLogFileDir      = configVar["LOG_FILE_DIR"];
    sBytesToBuffer   = stoi(configVar["BYTES_TO_BUFFER"]);
    sMaxMessageChars = stoi(configVar["MAX_MESSAGE_CHARS"]);
    sVerbosityLevel  = (Verbosity)stoi(configVar["VERBOSITY_LEVEL"]);

    // create log file directory if not yet created
    std::filesystem::create_directory(sLogFileDir);

    // time stamped log file name for this session
    auto now = std::time(nullptr);
    char buffer[sizeof("YYYY-MM-DD_HH:MM:SS")];    
    sInstance->mLogFileName = std::string(buffer, buffer + std::strftime(buffer, sizeof(buffer),"%F_%T", std::gmtime(&now))) + ".log";
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


void LogSystem::DebugPrint(const std::string &file, const int line, const Verbosity verbosity, const char *message, va_list messageArgs) {

    assert(sInstance != nullptr);


    std::ostringstream stream;
    stream << file << " [" << line << "] , " << ToString(verbosity) << "\t : " << message << '\n';

    char buffer[sMaxMessageChars];
    int messageLength = vsnprintf(buffer, sMaxMessageChars, stream.str().c_str(), messageArgs);
    buffer[messageLength] = '\0';

    WriteBufferToFile(sInstance->mLogFileName, sInstance->mLogFileStream, buffer);

    // also print message to default output if according to verbosity mask
    if (verbosity <= sVerbosityLevel)
    {
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


/*    LOG    */


void Log::operator() (const std::string &message) {

    va_list noArgs;
    LogSystem::DebugPrint(mFile, mLine, mVerbosity, message.c_str(), noArgs);
}


void Log::operator() (const char *message, ...) {

    va_list messageArgs;
    va_start(messageArgs, message);

    LogSystem::DebugPrint(mFile, mLine, mVerbosity, message, messageArgs);
}