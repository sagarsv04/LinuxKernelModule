#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sagar Vishwakarma");
MODULE_DESCRIPTION("A simple Linux Character Device Driver");
MODULE_VERSION("1.0");


#define DRIVER_NAME "Dev Character Device Driver"

static int major_number;



static struct file_operations dev_file_op = {
	.owner		= THIS_MODULE,
	.open			= dev_open,
	.read			= dev_read,
	.write		= dev_write,
	.release	= dev_close,
};


int dev_open(struct inode *pinode, struct file *pfile) {
	printk(KERN_ALERT "DEV Module: Inside %s function of Dev Character Device Driver\n", __FUNCTION__);

	return 0;
}


int dev_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) {
	printk(KERN_ALERT "DEV Module: Inside %s function of Dev Character Device Driver\n", __FUNCTION__);

	return 0;
}


int dev_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) {
	printk(KERN_ALERT "DEV Module: Inside %s function of Dev Character Device Driver\n", __FUNCTION__);

	return length;
}


int dev_close(struct inode *pinode, struct file *pfile) {
	printk(KERN_ALERT "DEV Module: Inside %s function of Dev Character Device Driver\n", __FUNCTION__);

	return 0;
}

// called when module is installed
int dev_module_init(void) {

	printk(KERN_ALERT "DEV Module: Initializing the Dev Character Device Driver\n");

	// Registering with Kernel a Character Device Driver
	major_number = register_chrdev(0, DRIVER_NAME, &dev_file_op);
	if (major_number < 0) {
		printk(KERN_ALERT "DEV Module: Failed to register a major number\n");
		return major_number;
	}
	else {
		printk(KERN_INFO "DEV Module: Dev Character Device Driver Registered with major number %d\n", major_number);
	}

	return 0;
}


// called when module is removed
void dev_module_exit(void) {

	printk(KERN_ALERT "DEV Module: Removing the Dev Character Device Driver\n");
	unregister_chrdev(major_number, DRIVER_NAME);
	printk(KERN_INFO "DEV Module: Goodbye From Dev Character Device Driver!\n");

}

module_init(dev_module_init);
module_exit(dev_module_exit);
