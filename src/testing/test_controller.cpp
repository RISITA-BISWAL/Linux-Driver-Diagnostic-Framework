#include "test_controller.h"
#include "logger.h"

#include <iostream>

bool TestController::runTest(TestCase& test)
{
    std::cout << "Running: " << test.getName() << "\n";

    Logger::info("Starting test: " + test.getName());

    bool result = test.run();

    if (result)
    {
        std::cout << "RESULT: PASS\n";
        Logger::info("Test passed: " + test.getName());
    }
    else
    {
        std::cout << "RESULT: FAIL\n";
        Logger::error("Test failed: " + test.getName());
    }

    return result;
}
