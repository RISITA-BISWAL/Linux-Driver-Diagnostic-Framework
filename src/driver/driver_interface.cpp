#include "driver_interface.h"
#include "vtest_ioctl.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

DriverInterface::DriverInterface()
    : fd(-1)
{
}

DriverInterface::~DriverInterface()
{
    closeDevice();
}

bool DriverInterface::openDevice()
{
    fd = open("/dev/vtest0", O_RDWR);

    return fd >= 0;
}

bool DriverInterface::writeData(const std::string& data)
{
    if (fd < 0)
        return false;

    ssize_t bytesWritten = write(fd, data.c_str(), data.size());

    return bytesWritten == static_cast<ssize_t>(data.size());
}

std::string DriverInterface::readData()
{
    if (fd < 0)
        return "";

    char buffer[256] = {0};

    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);

    if (bytesRead <= 0)
        return "";

    return std::string(buffer, bytesRead);
}

bool DriverInterface::getDataSize(unsigned int& size)
{
    if (fd < 0)
        return false;

    if (ioctl(fd, VTEST_IOCTL_GET_SIZE, &size) < 0)
        return false;

    return true;
}

void DriverInterface::closeDevice()
{
    if (fd >= 0)
    {
        close(fd);
        fd = -1;
    }
}
