#ifndef VTEST_IOCTL_H
#define VTEST_IOCTL_H

#include <sys/ioctl.h>

#define VTEST_IOCTL_MAGIC 'v'

#define VTEST_IOCTL_GET_SIZE \
    _IOR(VTEST_IOCTL_MAGIC, 1, unsigned int)

#endif
