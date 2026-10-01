#include "stress_test.h"
#include "driver_interface.h"

#include <iostream>

std::string StressTest::getName() const
{
    return "Stress Test";
}

bool StressTest::run()
{
    const int iterations = 100;

    for (int i = 0; i < iterations; ++i)
    {
        DriverInterface driver;

        if (!driver.openDevice())
        {
            std::cerr << "Stress test: open failed at iteration "
                      << i + 1 << "\n";
            return false;
        }

        std::string data = "Stress Test";

        if (!driver.writeData(data))
        {
            std::cerr << "Stress test: write failed at iteration "
                      << i + 1 << "\n";
            driver.closeDevice();
            return false;
        }

        driver.closeDevice();

        if (!driver.openDevice())
        {
            std::cerr << "Stress test: reopen failed at iteration "
                      << i + 1 << "\n";
            return false;
        }

        std::string result = driver.readData();

        if (result != data)
        {
            std::cerr << "Stress test: read failed at iteration "
                      << i + 1 << "\n";
            driver.closeDevice();
            return false;
        }

        driver.closeDevice();
    }

    std::cout << "Stress test iterations: " << iterations << "\n";
    std::cout << "Stress test: PASS\n";

    return true;
}
