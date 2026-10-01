#include "functional_test.h"
#include "driver_interface.h"

#include <iostream>

std::string FunctionalTest::getName() const
{
    return "Functional Driver Test";
}

bool FunctionalTest::run()
{
    DriverInterface driver;

    if (!driver.openDevice())
    {
        std::cerr << "Functional test: open failed\n";
        return false;
    }

    const std::string testData = "Functional Test";

    if (!driver.writeData(testData))
    {
        std::cerr << "Functional test: write failed\n";
        driver.closeDevice();
        return false;
    }

    driver.closeDevice();

    if (!driver.openDevice())
    {
        std::cerr << "Functional test: reopen failed\n";
        return false;
    }

    std::string readData = driver.readData();

    if (readData != testData)
    {
        std::cerr << "Functional test: read verification failed\n";
        driver.closeDevice();
        return false;
    }

    unsigned int size = 0;

    if (!driver.getDataSize(size))
    {
        std::cerr << "Functional test: ioctl failed\n";
        driver.closeDevice();
        return false;
    }

    driver.closeDevice();

    if (size != testData.size())
    {
        std::cerr << "Functional test: unexpected data size\n";
        return false;
    }

    std::cout << "Read verification: PASS\n";
    std::cout << "IOCTL size verification: PASS\n";

    return true;
}
