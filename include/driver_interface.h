#ifndef DRIVER_INTERFACE_H
#define DRIVER_INTERFACE_H

#include <string>

class DriverInterface {
public:
    DriverInterface();
    ~DriverInterface();

    bool openDevice();
    bool writeData(const std::string& data);
    std::string readData();
    bool getDataSize(unsigned int& size);
    void closeDevice();
    int getFd() const;

private:
    int fd;
};

#endif
