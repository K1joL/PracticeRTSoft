#ifndef MY_DRIVER_IOCTL_H
#define MY_DRIVER_IOCTL_H

#include <linux/ioctl.h>

#define MAGIC_NUM 0xF32
#define IOC_GETMAC _IOR(MAGIC_NUM, 0, int*)

#endif // !MY_DRIVER_IOCTL_H