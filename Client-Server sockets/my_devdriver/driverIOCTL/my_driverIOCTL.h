#ifndef MY_DRIVER_IOCTL_H
#define MY_DRIVER_IOCTL_H

#include <linux/ioctl.h>

#define MAGIC_NUM 0xF32
#define IOC_READC _IOR(MAGIC_NUM, 0, int*)
#define IOC_CLEARC _IO(MAGIC_NUM, 1)
#define IOC_SET_TIMEOUT _IOW(MAGIC_NUM, 2, int*)

#endif // !MY_DRIVER_IOCTL_H
