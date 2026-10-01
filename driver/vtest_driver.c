#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/mutex.h>

#include "vtest_ioctl.h"

#define DEVICE_NAME "vtest0"
#define BUFFER_SIZE 256

static char device_buffer[BUFFER_SIZE];
static size_t data_size;
static DEFINE_MUTEX(device_mutex);

static int vtest_open(struct inode *inode, struct file *file)
{
    pr_info("vtest_driver: device opened\n");
    return 0;
}

static ssize_t vtest_read(struct file *file,
                          char __user *buffer,
                          size_t count,
                          loff_t *offset)
{
    size_t available;
    size_t bytes_to_copy;

    if (mutex_lock_interruptible(&device_mutex))
        return -ERESTARTSYS;

    if (*offset >= data_size) {
        mutex_unlock(&device_mutex);
        return 0;
    }

    available = data_size - *offset;
    bytes_to_copy = min(count, available);

    if (copy_to_user(buffer, device_buffer + *offset, bytes_to_copy)) {
        mutex_unlock(&device_mutex);
        return -EFAULT;
    }

    *offset += bytes_to_copy;

    pr_info("vtest_driver: read %zu bytes\n", bytes_to_copy);

    mutex_unlock(&device_mutex);

    return bytes_to_copy;
}

static ssize_t vtest_write(struct file *file,
                           const char __user *buffer,
                           size_t count,
                           loff_t *offset)
{
    size_t bytes_to_copy;

    if (mutex_lock_interruptible(&device_mutex))
        return -ERESTARTSYS;

    bytes_to_copy = min(count, (size_t)(BUFFER_SIZE - 1));

    if (copy_from_user(device_buffer, buffer, bytes_to_copy)) {
        mutex_unlock(&device_mutex);
        return -EFAULT;
    }

    device_buffer[bytes_to_copy] = '\0';
    data_size = bytes_to_copy;

    pr_info("vtest_driver: wrote %zu bytes\n", bytes_to_copy);

    mutex_unlock(&device_mutex);

    return bytes_to_copy;
}

static long vtest_ioctl(struct file *file,
                        unsigned int cmd,
                        unsigned long arg)
{
    unsigned int size;

    switch (cmd)
    {
        case VTEST_IOCTL_GET_SIZE:
            if (mutex_lock_interruptible(&device_mutex))
                return -ERESTARTSYS;

            size = data_size;

            if (copy_to_user((unsigned int __user *)arg,
                             &size,
                             sizeof(size)))
            {
                mutex_unlock(&device_mutex);
                return -EFAULT;
            }

            pr_info("vtest_driver: ioctl returned data size %u\n", size);
            mutex_unlock(&device_mutex);
            return 0;

        default:
            return -EINVAL;
    }
}

static int vtest_release(struct inode *inode, struct file *file)
{
    pr_info("vtest_driver: device closed\n");
    return 0;
}

static const struct file_operations vtest_fops = {
    .owner = THIS_MODULE,
    .open = vtest_open,
    .read = vtest_read,
    .write = vtest_write,
    .unlocked_ioctl = vtest_ioctl,
    .release = vtest_release,
};

static struct miscdevice vtest_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &vtest_fops,
};

static int __init vtest_init(void)
{
    int ret;

    ret = misc_register(&vtest_device);

    if (ret) {
        pr_err("vtest_driver: failed to register device\n");
        return ret;
    }

    pr_info("vtest_driver: registered /dev/%s\n", DEVICE_NAME);

    return 0;
}

static void __exit vtest_exit(void)
{
    misc_deregister(&vtest_device);
    pr_info("vtest_driver: device unregistered\n");
}

module_init(vtest_init);
module_exit(vtest_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Linux Driver Diagnostic Framework");
MODULE_DESCRIPTION("Virtual Linux device driver for testing and diagnostics");
MODULE_VERSION("1.0");
