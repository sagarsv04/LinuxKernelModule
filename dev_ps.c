/*
 *  dev_ps.c
 *  Contains implementation of kernel module to list process info
 *
 *  Author :
 *  Sagar Vishwakarma (svishwa2@binghamton.edu)
 *  State University of New York, Binghamton
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/fs.h>
#include <linux/slab.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sagar Vishwakarma");
MODULE_DESCRIPTION("A simple Linux Character Device Driver");
MODULE_VERSION("1.0");


#define DRIVER_NAME "dev_ps"
#define STATE_STR_LEN 64
#define PROCESS_STR_LEN 128


typedef struct process_node {
	int process_id;
	char process_info[PROCESS_STR_LEN];
} process_node;

typedef struct process_list {
	process_node *node;
	struct process_list *next;
} process_list;

static int major_number;
static int number_of_processes;
static process_list *p_list;

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


static void get_task_state_name(char *state_string, long state) {

	switch (state) {
		case TASK_RUNNING:
			strcpy(state_string, "TASK_RUNNING");
			break;
		case TASK_INTERRUPTIBLE:
			strcpy(state_string, "TASK_INTERRUPTIBLE");
			break;
		case TASK_UNINTERRUPTIBLE:
			strcpy(state_string, "TASK_UNINTERRUPTIBLE");
			break;
		case __TASK_STOPPED:
			strcpy(state_string, "__TASK_STOPPED");
			break;
		case __TASK_TRACED:
			strcpy(state_string, "__TASK_TRACED");
			break;
		case TASK_PARKED:
			strcpy(state_string, "TASK_PARKED");
			break;
		case TASK_DEAD:
			strcpy(state_string, "TASK_DEAD");
			break;
		case TASK_WAKEKILL:
			strcpy(state_string, "TASK_WAKEKILL");
			break;
		case TASK_WAKING:
			strcpy(state_string, "TASK_WAKING");
			break;
		case TASK_NOLOAD:
			strcpy(state_string, "TASK_NOLOAD");
			break;
		case TASK_NEW:
			strcpy(state_string, "TASK_NEW");
			break;
		case TASK_STATE_MAX:
			strcpy(state_string, "TASK_STATE_MAX");
			break;
		case TASK_KILLABLE:
			strcpy(state_string, "TASK_WAKEKILL, TASK_UNINTERRUPTIBLE");
			break;
		case TASK_STOPPED:
			strcpy(state_string, "TASK_WAKEKILL, __TASK_STOPPED");
			break;
		case TASK_TRACED:
			strcpy(state_string, "TASK_WAKEKILL, __TASK_TRACED");
			break;
		case TASK_IDLE:
			strcpy(state_string, "TASK_UNINTERRUPTIBLE, TASK_NOLOAD");
			break;
		case TASK_NORMAL:
			strcpy(state_string, "TASK_INTERRUPTIBLE, TASK_UNINTERRUPTIBLE");
			break;
		default:
			strcpy(state_string, "UNKNOWN_STATE");
			break;
	}
}

static int dev_open(struct inode *pinode, struct file *pfile) {

	printk(KERN_INFO "DEV Module: Inside %s function of Dev Character Device Driver\n", __FUNCTION__);

	printk(KERN_INFO "DEV Module: Hey %s function of Dev Character Device Driver\n", __FUNCTION__);

	// at this point add that process into the list

	return 0;
}

static ssize_t dev_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) {

	int errors = 0;
	char *message = "Hey what are you trying to read ...\n";
	int message_len = strlen(message);
	pid_t pid = current->pid;
	printk(KERN_INFO "DEV Module: Inside %s function of Dev Character Device Driver\n", __FUNCTION__);

	errors = copy_to_user(buffer, message, message_len);

	printk(KERN_INFO "DEV Module: Exit %s function of Dev Character Device Driver PID %d\n", __FUNCTION__, pid);

	return errors == 0 ? message_len : -EFAULT;
}

static ssize_t dev_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) {

	printk(KERN_ALERT "DEV Module: Inside %s function of Dev Character Device Driver\n", __FUNCTION__);

	return length;
}

static int dev_close(struct inode *pinode, struct file *pfile) {

	printk(KERN_INFO "DEV Module: Inside %s function of Dev Character Device Driver\n", __FUNCTION__);

	// at this point remove that process into the list

	return 0;
}


static void add_process_to_list(process_list * p_list, struct task_struct *task, char *state_string) {
	p_list->next = NULL;
	p_list->node = NULL;
}



// called when module is installed

static int __init dev_module_init(void) {

	// scan through the task and create a link list
	// Kmalloc new memory if required
	struct task_struct *task = current;
	char state_string[64];

	p_list = (process_list*)kmalloc(sizeof(process_list), GFP_KERNEL);
	if (!p_list) {
		printk(KERN_ALERT "DEV Module: Failed to initialize memory for Process List\n");
		return -EFAULT;
	}

	printk(KERN_INFO "DEV Module: Initializing the Dev Character Device Driver\n");

	major_number = register_chrdev(0, DRIVER_NAME, &dev_file_op); // Registering with Kernel a Character Device Driver
	if (major_number < 0) {
		printk(KERN_ALERT "DEV Module: Failed to register a major number\n");
		kfree(p_list);
		return major_number;
	}
	else {
		printk(KERN_INFO "DEV Module: Dev Character Device Driver Registered with major number %d\n", major_number);

		if (task) {
			for_each_process(task) {
				number_of_processes += number_of_processes;
				get_task_state_name(state_string, task->state);
				// make link list of process
				printk(KERN_INFO "PID = %8d  PPID = %8d  CPU = %4d  STATE = %s\n", task->pid, task->parent->pid, task->cpu, state_string);
			}
		}
	}
	return 0;
}

// called when module is removed
static void __exit dev_module_exit(void) {

	// free the Kmalloc memory
	if (p_list) {
		kfree(p_list);
	}

	printk(KERN_ALERT "DEV Module: Removing the Dev Character Device Driver\n");
	unregister_chrdev(major_number, DRIVER_NAME);
	printk(KERN_INFO "DEV Module: Goodbye From Dev Character Device Driver!\n");
}

module_init(dev_module_init);
module_exit(dev_module_exit);
