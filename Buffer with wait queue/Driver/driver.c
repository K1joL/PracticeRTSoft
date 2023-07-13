#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/wait.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/ioctl.h>
#include "my_driverIOCTL.h"

#define BUFFER_SIZE 25

MODULE_LICENSE("GPL");

static int MAJOR = 0;
static char *DEVICE_NAME = "kij";

//Wait Queue
static wait_queue_head_t wq;

static struct task_struct *inc_counter_ts;
static int COUNTER_thread = 0;
static int inc_timeout = 10;
//Флаг показывающий, прочитаны ли данные 0 - да, 1 - нет
static int flag_inc_count_read = 1;
static int inc_counter(void *data)
{
	while(true)
	{
		msleep(inc_timeout);
		COUNTER_thread++;
		if(!flag_inc_count_read)
			flag_inc_count_read = 1;
		//Пробуждаем процесс после изменения данных
		wake_up_interruptible(&wq);

		if(kthread_should_stop())
			break;
	}
	return 0;
}

static int my_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO"File opened!\n");
	try_module_get(THIS_MODULE);
	return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO"File closed!\n");
	module_put(THIS_MODULE);
	return 0;
}

static long my_ioctl(struct file* f, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case IOC_READC:
			//Усыпляем процесс если данные не изменены
			wait_event_interruptible(wq, flag_inc_count_read);
			copy_to_user(arg, &COUNTER_thread, sizeof(int));
			flag_inc_count_read = 0;
			break;

		case IOC_CLEARC:
			COUNTER_thread = 0;
			break;

		case IOC_SET_TIMEOUT:
			copy_from_user(&inc_timeout, arg, sizeof(int));
			break;

		default:
			return -ENOTTY;
	}

	return 0;
}


struct file_operations fops =
{
	.open = my_open,
	.release = my_release,
	.unlocked_ioctl = my_ioctl
};

int init_module(void)
{
	init_waitqueue_head(&wq);
	inc_counter_ts = kthread_run(inc_counter, NULL, "inc_counter_thread");
	printk(KERN_INFO"Initializing module! \n");
	MAJOR = register_chrdev(0, DEVICE_NAME, &fops);
	if(MAJOR < 0)
	{
		printk(KERN_ALERT"Registering char device failed with %d\n", MAJOR);
		return MAJOR;
	}
	printk(KERN_ALERT"Registered char device with major number %d\n", MAJOR);
	printk(KERN_ALERT"The driver create a dev file with\n");
	printk(KERN_ALERT"'sudo mknod /dev/%s c %d 0.'\n", DEVICE_NAME, MAJOR);
	return 0;
}
void cleanup_module(void)
{
	printk(KERN_INFO"Cleanup!\n");	
	unregister_chrdev(MAJOR, DEVICE_NAME);
	kthread_stop(inc_counter_ts);
}



