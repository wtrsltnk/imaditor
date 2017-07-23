#include "log.h"

Log* Log::_instance = nullptr;

Log::Log()
{ }

Log& Log::Current()
{
    if (Log::_instance == nullptr) Log::_instance = new Log();

    return *Log::_instance;
}

Log::~Log()
{ }

#ifdef BUILD_ANDROID

#include <android/log.h>
#define APPNAME "PassPerfect"

void Log::Assert(const char* message)
{
    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, message, 1);
}

void Log::Debug(const char* message)
{
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, message, 1);
}

void Log::Error(const char* message)
{
    __android_log_print(ANDROID_LOG_ERROR, APPNAME, message, 1);
}

void Log::Info(const char* message)
{
    __android_log_print(ANDROID_LOG_INFO, APPNAME, message, 1);
}

void Log::Verbose(const char* message)
{
    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, message, 1);
}

void Log::Warn(const char* message)
{
    __android_log_print(ANDROID_LOG_WARN, APPNAME, message, 1);
}

#else
#include <iostream>

void Log::Assert(const char* message)
{
    std::cout << "ASSERT: " << message << std::endl;
}

void Log::Debug(const char* message)
{
    std::cout << "DEBUG: " << message << std::endl;
}

void Log::Error(const char* message)
{
    std::cerr << "ERROR: " << message << std::endl;
}

void Log::Info(const char* message)
{
    std::cout << "INFO: " << message << std::endl;
}

void Log::Verbose(const char* message)
{
    std::cout << "VERBOSE: " << message << std::endl;
}

void Log::Warn(const char* message)
{
    std::cout << "WARN: " << message << std::endl;
}

#endif // BUILD_ANDROID
