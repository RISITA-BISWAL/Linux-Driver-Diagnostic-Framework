#include "error_test.h"
#include "driver_interface.h"

#include <cerrno>
#include <cstring>
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

    const unsigned long INVALID_IOCTL = 0xDEADBEEF;
    int result = ioctl(driver.getFd(), INVALID_IOCTL, nullptr);
    int savedErrno = errno;

    driver.closeDevice();

    if (result == -1 && savedErrno == EINVAL)
    {
        std::cout << "Invalid ioctl test: PASS\n";
        return true;
    }

    std::cerr << "Invalid ioctl test: FAIL (expected result=-1, errno=EINVAL, got result="
              << result << ", errno=" << savedErrno
              << " [" << std::strerror(savedErrno) << "])\n";
    return false;
}

std::string ErrorTest::getName() const
{
    return "Error Handling Test";
}
