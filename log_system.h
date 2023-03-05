#pragma once


#include <assert.h>
#include <bitset>
#include <cstdarg>
#include <iostream>
#include <sstream>
#include <string>


enum class Verbosity : uint8_t{

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


/* @brief Singleton class that collects debug messages and writes them into a time stamped log file.
 *        Must be initialized with a call to StartUp. ShutDown should be called to flush any messages still buffered to the log file.
 *        The LOG_* macros provide the convenient user interface for submitting debug messages.
 *        Log system properties like log file directory and verbosity level can be configured by editing the log.config file.
 *
 * @class
 */
class LogSystem {

    friend class Log;

public:

    /* @brief Initializes the log system instance if not already initialized.
     *        Reads the config file and sets the appropriate config variables.
     *        Creates a time stamped log file name for the current session.
     *        Calling StartUp again will start a new session with a new log file.
     */
    static void StartUp();

    /* @brief Flushes the message buffer one final time if log system has been initialized before.
     */
    static void ShutDown();
    
    ~LogSystem() = default;
    

private:

    LogSystem() = default;

    /* @brief Called by LOG_* macros.
     *        Prepends the degug message with its origin file and line and the message verbosity before formatting it with the message arguments.
     *        All messages are written to the log file viw WriteBufferToFile and messages that pass a verbosity check are also printed to the default output.
     * 
     * @param file    The file from which LOG_* has been called.
     * @param line    The line from which LOG_* has been called.
     * @param verbosity    The message verbosity level.
     * @param message    The (formatted) degug message to output.
     * @param messageArgs    The list of arguments to format the debug message with.
     */
    static void DebugPrint(const std::string &file, const int line, const Verbosity verbosity, const char *message, va_list messageArgs);

    /* @brief Transfers the message buffer to a string stream.
     *        Writes the contents of the stream to the log file once more than sBytesToWrite have been collected.
     *
     * @param fileName    The name of the log file to write to.
     * @param stream    The string stream to transfer the buffered message to,
     * @param buffer    Pointer to a char buffer holding a formatted debug message.
     */
    static void WriteBufferToFile(const std::string &fileName, std::ostringstream &stream, const char *buffer);

    // config variables
    static std::string sLogFileDir;
    static size_t sBytesToBuffer;
    static size_t sMaxMessageChars;
    static Verbosity sVerbosityLevel;

    static LogSystem* sInstance;
    
    std::string mLogFileName;
    std::ostringstream mLogFileStream;
};


/* @brief Helper class used by the LOG_* macros to intrinsically pass __FILE__, __LINE__ and message verbosity.
 *
 * @class
 */
class Log {

public:

    Log(std::string file, int line, Verbosity verbosity = Verbosity::Info) : mFile {file}, mLine {line}, mVerbosity {verbosity} {}
    ~Log() = default;

    /* @brief Calls DebugPrint(), passing the debug message and an empty message argument list.
     * 
     * @param message    The debug message.
     */
    void operator() (const std::string &message);

    /* @brief Calls DebugPrint() passing the degun message
     *
     * @param message    The debug message to be formatted.
     * @param ...    The arguments used to format the debug message
     */
    void operator() (const char *message, ...);


private:

    std::string mFile;
    int mLine;
    Verbosity mVerbosity; 
};