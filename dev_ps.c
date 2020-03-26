#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sagar Vishwakarma");
MODULE_DESCRIPTION("A simple Linux Character Device Driver");
MODULE_VERSION("1.0");


#define DRIVER_NAME "dev_ps"

static int major_number;

static int dev_open(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static int dev_close(struct inode *, struct file *);


static struct file_operations dev_file_op = {
	.owner		= THIS_MODULE,
	.open			= dev_open,
	.read			= dev_read,
	.write		= dev_write,
	.release	= dev_close,
};


static int dev_open(struct inode *pinode, struct file *pfile) {

	printk(KERN_INFO "DEV Module: Inside %s function of Dev Character Device Driver\n", __FUNCTION__);

	return 0;
}

static ssize_t dev_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) {

	int errors = 0;
	char *message = "Hey what are you trying to read ...\n";
	int message_len = strlen(message);
	printk(KERN_INFO "DEV Module: Inside %s function of Dev Character Device Driver\n", __FUNCTION__);

	errors = copy_to_user(buffer, message, message_len);

	return errors == 0 ? message_len : -EFAULT;
}

static ssize_t dev_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) {

	printk(KERN_INFO "DEV Module: Inside %s function of Dev Character Device Driver\n", __FUNCTION__);

	return length;
}

static int dev_close(struct inode *pinode, struct file *pfile) {

	printk(KERN_INFO "DEV Module: Inside %s function of Dev Character Device Driver\n", __FUNCTION__);

	return 0;
}

// called when module is installed
static int __init dev_module_init(void) {

	printk(KERN_INFO "DEV Module: Initializing the Dev Character Device Driver\n");
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
static void __exit dev_module_exit(void) {

	printk(KERN_ALERT "DEV Module: Removing the Dev Character Device Driver\n");
	unregister_chrdev(major_number, DRIVER_NAME);
	printk(KERN_INFO "DEV Module: Goodbye From Dev Character Device Driver!\n");
}

module_init(dev_module_init);
module_exit(dev_module_exit);
