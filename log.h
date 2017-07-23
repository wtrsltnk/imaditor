#ifndef LOG_H
#define LOG_H

class Log
{
    static Log* _instance;
    Log();
public:
    static Log& Current();
    virtual ~Log();

    void Assert(const char* message);
    void Debug(const char* message);
    void Error(const char* message);
    void Info(const char* message);
    void Verbose(const char* message);
    void Warn(const char* message);
};

#endif // LOG_H
