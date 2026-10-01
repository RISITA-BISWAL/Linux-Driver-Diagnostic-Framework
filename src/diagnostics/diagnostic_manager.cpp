#include "diagnostic_manager.h"

#include <iostream>
#include <fcntl.h>
#include <unistd.h>

bool DiagnosticManager::checkDevice()
{
    int fd = open("/dev/vtest0", O_RDWR);

    if (fd < 0)
        return false;

    close(fd);
    return true;
}

void DiagnosticManager::printStatus()
{
    if (checkDevice())
        std::cout << "Diagnostic: /dev/vtest0 is available\n";
    else
        std::cout << "Diagnostic: /dev/vtest0 is unavailable\n";
}
