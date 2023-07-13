#include <linux/pci.h>
#include <linux/module.h>
#include <linux/init.h>
#include "my_driverIOCTL/IOCTL.h"

MODULE_LICENSE("GPL");

#define VENDOR_ID 0x8086
#define DEVICE_ID 0x100f

static struct pci_device_id pci_table[] = 
{
    {VENDOR_ID, DEVICE_ID, PCI_ANY_ID, PCI_ANY_ID, 0, 0, NULL},
    {0,}
};

MODULE_DEVICE_TABLE(pci, pci_table);

static int dev_probe(struct pci_dev *dev, const struct pci_device_id *id);
static void dev_remove(struct pci_dev *dev);

static int major;
static int mac_offset = 0; //Example
#define MAC_SIZE 6
static int mac[MAC_SIZE];
static u8 __iomem *devmem = NULL;

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
    int i = 0;

    switch(cmd)
	{
		case IOC_GETMAC:
            while(i < 6)
            {
                mac[i] = (unsigned int)ioread8(&devmem[mac_offset+i]);
                pr_info("[%d] = %02x", i, (mac[mac_offset+i]));
                i++;
            }
            copy_to_user(arg, &mac, sizeof(mac));
            break;

		default:
			return -ENOTTY;
	}

	return 0;
}

static struct file_operations fops =
{
	.open = my_open,
	.release = my_release,
	.unlocked_ioctl = my_ioctl
};

static struct pci_driver device_pci_driver = 
{
    .name = "Device_Name",
    .id_table = pci_table,
    .probe = dev_probe,
    .remove = dev_remove
};

int init_module(void)
{
	printk(KERN_INFO"Initializing module! \n");
	return pci_register_driver(&device_pci_driver);
}
void cleanup_module(void)
{
	printk(KERN_INFO"Cleanup!\n");	
	pci_unregister_driver(&device_pci_driver);
}

static int dev_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
    major = register_chrdev(0, "MyPCI", &fops);
    unsigned long memio_start, memio_len;
    // uint8_t data;
    memio_start = pci_resource_start(dev, 0);
    memio_len = pci_resource_len(dev, 0);
    devmem = ioremap(memio_start, memio_len);

    if(!devmem)
        return -EIO;
    
    // unsigned long offset = 0;
    // while(offset < memio_len)
    // {
    //     data = ioread8(&devmem[offset]);
    //     if(data = 0x00 && ioread8(&devmem[offset+1]) == 0x0c)
    //         pr_info("data: %02x, %02x, %02x, %02x, %02x, %02x\n", 
    //         (unsigned int)data,
    //         (unsigned int)ioread8(&devmem[offset+1]),
    //         (unsigned int)ioread8(&devmem[offset+2]),
    //         (unsigned int)ioread8(&devmem[offset+3]),
    //         (unsigned int)ioread8(&devmem[offset+4]),
    //         (unsigned int)ioread8(&devmem[offset+5]));
    //     offset++;
    // }
    pr_info("Load driver PCI %d\n", major);
    return 0;
}

static void dev_remove(struct pci_dev *dev)
{
    if(devmem)
        iounmap(devmem);
    unregister_chrdev(major, "MyPCI");
}
