#include "logger.h"

#include <fstream>
#include <iostream>

namespace
{
    const char* LOG_FILE = "reports/driver_test.log";
}

void Logger::info(const std::string& message)
{
    std::ofstream log(LOG_FILE, std::ios::app);

    if (log)
    {
        log << "[INFO] " << message << '\n';
    }

    std::cout << "[INFO] " << message << '\n';
}

void Logger::error(const std::string& message)
{
    std::ofstream log(LOG_FILE, std::ios::app);

    if (log)
    {
        log << "[ERROR] " << message << '\n';
    }

    std::cerr << "[ERROR] " << message << '\n';
}
