#pragma once

#define DEBUG

#define Assert(statement, ...) do { if (!(statement)) LogImplementation(LogLevel::ASSERTION, #statement, __FILE__, __LINE__, __VA_ARGS__); } while(false)

#define Info(...)    LogImplementation(LogLevel::INFO,    #__VA_ARGS__, __FILE__, __LINE__, __VA_ARGS__)
#define Error(...)   LogImplementation(LogLevel::ERROR,   #__VA_ARGS__, __FILE__, __LINE__, __VA_ARGS__)
#define Warning(...) LogImplementation(LogLevel::WARNING, #__VA_ARGS__, __FILE__, __LINE__, __VA_ARGS__)

#ifdef DEBUG
#define DEBUG_BLOCK(x) { x }
#else
#define DEBUG_BLOCK(x)
#endif


#define TIME_SCOPE(name)  const auto name##__LINE__ = Timer(#name)


#include <unordered_map>
#include <GLFW/glfw3.h>


enum class LogLevel
{
    INFO, WARNING, ERROR, ASSERTION
};

void LogImplementation(LogLevel level, const char* statement, const char* file, unsigned line, const char* message = "", ...);


class Timer
{
public:
    static std::unordered_map<const char*, double> times;

    const char*  name;
    const double start;

    explicit Timer(const char* name) : name(name), start(glfwGetTime()) {}
    ~Timer();
    static void PrintTimes();
};

