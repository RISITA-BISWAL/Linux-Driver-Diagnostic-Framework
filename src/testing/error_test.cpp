#include "error_test.h"
#include "driver_interface.h"

#include <iostream>
#include <sys/ioctl.h>

bool ErrorTest::run()
{
    DriverInterface driver;

    if (!driver.openDevice())
    {
        std::cerr << "Error test: open failed\n";
        return false;
    }

    int result = ioctl(-1, 999, nullptr);

    driver.closeDevice();

    if (result == -1)
    {
        std::cout << "Invalid ioctl test: PASS\n";
        return true;
    }

    std::cerr << "Invalid ioctl test: FAIL\n";
    return false;
}

std::string ErrorTest::getName() const
{
    return "Error Handling Test";
}
