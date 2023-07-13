#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/wait.h>

#define BUFFER_SIZE 25

MODULE_LICENSE("GPL");

static int MAJOR = 0;
static char *DEVICE_NAME = "kij";

//Buffer of driver to exchange information
static char BUFFER[BUFFER_SIZE];
static int WRITE_COUNTER = 1;
static int READ_COUNTER = 0;

//Wait Queue
static wait_queue_head_t wq_write;
static wait_queue_head_t wq_read;
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
static ssize_t my_read(struct file *flip, char *buffer, size_t length, loff_t *offset)
{
	if((READ_COUNTER+1 != WRITE_COUNTER) //buffer empty
		&&(flip->f_flags & O_NONBLOCK))
		return -EAGAIN;
	if(wait_event_interruptible(wq_read, (READ_COUNTER+1 != WRITE_COUNTER)))
		return -ERESTARTSYS;
	
	int bytes_read = 0;
	while(bytes_read < length && (READ_COUNTER+1 != WRITE_COUNTER) )
	{
		put_user(BUFFER[++READ_COUNTER], buffer++);
		printk(KERN_INFO"read: readc %d, writec %d, buf[read] %c\n", READ_COUNTER, WRITE_COUNTER, BUFFER[READ_COUNTER]);
		bytes_read++;
		if(READ_COUNTER == BUFFER_SIZE-1)
			READ_COUNTER = -1;
	}
	wake_up_interruptible(&wq_write);
	
	printk(KERN_INFO"read: length %d, bytes %d\n", length, bytes_read);
	return bytes_read;
}

static ssize_t my_write(struct file* flip, const char *buffer, size_t length, loff_t *offset)
{
	if((READ_COUNTER != WRITE_COUNTER) //buffer if full
		&&(flip->f_flags & O_NONBLOCK))
		return -EAGAIN;
	if(wait_event_interruptible(wq_write, (READ_COUNTER != WRITE_COUNTER)))
		return -ERESTARTSYS;
	
	int bytes_written = 0;
	while( bytes_written < length && (WRITE_COUNTER != READ_COUNTER) )
	{
		printk(KERN_INFO"write1: readc %d, writec %d\n", READ_COUNTER, WRITE_COUNTER);
		get_user(BUFFER[WRITE_COUNTER++], buffer++);
		bytes_written++;
		if(WRITE_COUNTER == BUFFER_SIZE)
			WRITE_COUNTER = 0;
		printk(KERN_INFO"write2: readc %d, writec %d\n", READ_COUNTER, WRITE_COUNTER);
	}
	wake_up_interruptible(&wq_read);
	
	return bytes_written;
}

struct file_operations fops =
{
	.read = my_read,
	.write = my_write,
	.open = my_open,
	.release = my_release
};

int init_module(void)
{
	init_waitqueue_head(&wq_write);
	init_waitqueue_head(&wq_read);
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
}



