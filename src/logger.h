#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <string_view>
#include <sstream>

void SetupLogger(bool shouldLog);
bool ShouldLog();

template <typename ...Args>
void Log(Args&& ...args) {
    if(!ShouldLog())
        return;
    std::ostringstream stream;
    (stream << ... << std::forward<Args>(args)) << std::endl;
    std::cout << stream.str();
}


#endif // LOGGER_H