#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>

#define DEV_MEM_SIZE 512
char device_buffer[DEV_MEM_SIZE];

/* this holds the device number */
dev_t device_number; 

// cdev vaiable 
struct cdev pcd_cdev;

loff_t pcd_lseek (struct file *filep, loff_t offset, int whence)
{
	pr_info("lseek \n");
	loff_t tmp;

	pr_info("Current file position: %lld\n", filep->f_pos);
	switch(whence) {
	case SEEK_SET:
		if (offset > DEV_MEM_SIZE || offset < 0)
			return -EINVAL;
		filep->f_pos = offset;
		break;
	case SEEK_CUR:
		tmp = filep->f_pos + offset;
		if (tmp > DEV_MEM_SIZE || tmp <0 )
			return -EINVAL;
		filep->f_pos = tmp;
		break;
	case SEEK_END:
		tmp = DEV_MEM_SIZE + offset;
		if (tmp > DEV_MEM_SIZE || tmp <0 )
			return -EINVAL;
		filep->f_pos = tmp;
		break;
	default:
		return -EINVAL;
	}
	pr_info("Updated file position: %lld\n", filep->f_pos);
	return filep->f_pos;
}

ssize_t pcd_read (struct file *filep, char __user *buff, size_t count, loff_t *f_pos)
{
	pr_info("Read requested for %zu \n", count);
	pr_info("Current file position: %lld\n", *f_pos);

	/* Adjust the count */
	if((*f_pos + count) > DEV_MEM_SIZE)
		count = DEV_MEM_SIZE - *f_pos;

	// copy to user 
	if(copy_to_user(buff, &device_buffer[*f_pos], count))
		return -EFAULT;

	// update the current file postion 
	*f_pos += count;

	pr_info("Number of bytes successfully read: %zu\n", count);
	pr_info("Updated file position: %lld\n", *f_pos);

	// Return number of byte which have been sucessfully read
	return count;
}

ssize_t pcd_write (struct file *filep, const char __user *buff, size_t count, loff_t *f_pos)
{
	pr_info("Wead requested for %zu \n", count);
	pr_info("Current file position: %lld\n", *f_pos);
	

	/* Adjust the count */
	if((*f_pos + count) > DEV_MEM_SIZE)
		count = DEV_MEM_SIZE - *f_pos;

	if(!count)
		return -ENOMEM;

	// copy from user 
	if(copy_from_user(&device_buffer[*f_pos], buff, count))
		return -EFAULT;

	// update the current file postion 
	*f_pos += count;

	pr_info("Number of bytes successfully write: %zu\n", count);
	pr_info("Updated file position: %lld\n", *f_pos);

	// Return number of byte which have been sucessfully read
	return count;
}

int pcd_open (struct inode *inode, struct file *filep)
{
	pr_info("open \n");
	return 0;
}

int pcd_release (struct inode *inode, struct file *filep)
{
	pr_info("close \n");
	return 0;
}

// File ops of the driver 
struct file_operations pcd_fops =
{
	.llseek = pcd_lseek,
	.read = pcd_read,
	.write = pcd_write,
	.open = pcd_open,
	.release = pcd_release,
	.owner = THIS_MODULE
};

struct class *class_pcd;

struct device *device_pcd;

static int __init pcd_driver_init(void)
{
	int ret;

	/* 1. Dynamicly allocate a device number */
	alloc_chrdev_region(&device_number,0,1,"pcd_devices");
	pr_info("%s: Device number <major>:<minor> = %d:%d \n", __func__, MAJOR(device_number), MINOR(device_number));

	/* 2. Initialize the cdev structure with fops */
	cdev_init(&pcd_cdev,&pcd_fops);
	 
	/* 3. Register a device (cdev structure) with VFS */
	pcd_cdev.owner = THIS_MODULE;
	ret = cdev_add(&pcd_cdev, device_number, 1);

	/* 4. Create device class under /sys/class */
	class_pcd = class_create(THIS_MODULE, "pcd_class");

	/* 5. Populate the sysfs with device infomation */
	device_pcd = device_create(class_pcd, NULL, device_number, NULL, "pcd");

	pr_info("Module init was SUCCESSFUL\n");
	return 0;
}

static void __exit pcd_driver_cleanup(void)
{
	device_destroy(class_pcd, device_number);
	class_destroy(class_pcd);
	cdev_del(&pcd_cdev);
	unregister_chrdev_region(device_number,1);
	pr_info("Module unloaded\n");
}

module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("minkha");
MODULE_DESCRIPTION("A pseduo charater driver");