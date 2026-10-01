#include "boundary_test.h"
#include "driver_interface.h"

#include <iostream>
#include <string>

std::string BoundaryTest::getName() const
{
    return "Boundary and Buffer Limit Test";
}

bool BoundaryTest::run()
{
    DriverInterface driver;

    if (!driver.openDevice())
    {
        std::cerr << "Boundary test: open failed\n";
        return false;
    }

    const size_t MAX_CAPACITY = 255;
    const std::string oversizedData(300, 'B');

    if (!driver.writeData(oversizedData))
    {
        std::cerr << "Boundary test: write failed\n";
        driver.closeDevice();
        return false;
    }

    driver.closeDevice();

    if (!driver.openDevice())
    {
        std::cerr << "Boundary test: reopen failed\n";
        return false;
    }

    unsigned int reportedSize = 0;
    if (!driver.getDataSize(reportedSize))
    {
        std::cerr << "Boundary test: ioctl get size failed\n";
        driver.closeDevice();
        return false;
    }

    if (reportedSize != MAX_CAPACITY)
    {
        std::cerr << "Boundary test: buffer capacity limit failed (expected "
                  << MAX_CAPACITY << ", got " << reportedSize << ")\n";
        driver.closeDevice();
        return false;
    }

    std::string readBack = driver.readData();
    driver.closeDevice();

    if (readBack.size() != MAX_CAPACITY)
    {
        std::cerr << "Boundary test: read data size mismatch (expected "
                  << MAX_CAPACITY << ", got " << readBack.size() << ")\n";
        return false;
    }

    if (readBack != oversizedData.substr(0, MAX_CAPACITY))
    {
        std::cerr << "Boundary test: data corruption detected\n";
        return false;
    }

    std::cout << "Buffer limit verification: PASS\n";
    std::cout << "IOCTL boundary size verification: PASS\n";

    return true;
}
