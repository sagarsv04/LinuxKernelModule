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

#define DRIVER_DEBUG 0

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

static void free_process_list(void);
static void get_process_info(char *, loff_t *);
static void get_task_state_name(char *, long);
static void create_process_list(struct task_struct *);
static void add_to_process_list(struct task_struct *);


static struct file_operations dev_file_op = {
	.owner		= THIS_MODULE,
	.open			= dev_open,
	.read			= dev_read,
	.write		= dev_write,
	.release	= dev_close,
};


static void free_process_list() {

	/* To free the global head of the link list */

	if (p_list) {
		if (DRIVER_DEBUG) {
			printk(KERN_INFO "DEV Module: Releasing memory for Process List\n");
		}
		kfree(p_list);
	}
}


static void get_task_state_name(char *state_string, long state) {

	/* Switch Case to find string representation of sate code */

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
			if (DRIVER_DEBUG) {
				printk(KERN_ALERT "DEV Module: Failed to find state string for %ld\n", state);
			}
			strcpy(state_string, "UNKNOWN_STATE");
			break;
	}
}


static void get_process_info(char *message, loff_t *offset) {

	/* Pass process Info based on how many lines already read */

	int skip_node = (int)(*offset);
	process_list *p = p_list;
	if (p==NULL) {
		if (DRIVER_DEBUG) {
			printk(KERN_ALERT "DEV Module: Empty Process List\n");
		}
		strcpy(message, "EXIT_CODE\n");
	}
	else {
		int i;
		for (i = 0; (i < skip_node)&&(i < number_of_processes); i++) {
			p = p->next;
		}
		if (p==NULL) {
			if (DRIVER_DEBUG) {
				printk(KERN_ALERT "DEV Module: Empty Process Node\n");
			}
			strcpy(message, "EXIT_CODE\n");
		}
		else {
			strcpy(message, p->node->process_info);
			*offset += 1;
		}
	}
}

static void add_to_process_list(struct task_struct *task) {

	/* Add new process to linked list */

	char state_string[64];
	process_list *temp = NULL;
	process_list *p = NULL;

	get_task_state_name(state_string, task->state);
	temp = (process_list*)kmalloc(sizeof(process_list), GFP_KERNEL);
	if (!temp) {
		printk(KERN_ALERT "DEV Module: Failed to initialize memory for Process List\n");
		return;
	}
	temp->node = (process_node*)kmalloc(sizeof(process_node), GFP_KERNEL);
	if (!temp->node) {
		printk(KERN_ALERT "DEV Module: Failed to initialize memory for Process Node\n");
		return;
	}
	temp->next = NULL;
	temp->node->process_id = task->pid;

	sprintf(temp->node->process_info, "PID = %8d  PPID = %8d  CPU = %4d  STATE = %s\n", task->pid, task->parent->pid, task->cpu, state_string);

	if (p_list==NULL) {
		p_list = temp;
	}
	else {
		p = p_list;
		while (p->next!=NULL) {
			p = p->next;
		}
		p->next = temp;
	}
	number_of_processes += 1;
}


static void create_process_list(struct task_struct *task) {

	/* Create a linked list of all the current process using task */

	char state_string[64];
	process_list *temp = NULL;
	process_list *p = NULL;

	for_each_process(task) {
		get_task_state_name(state_string, task->state);
		temp = (process_list*)kmalloc(sizeof(process_list), GFP_KERNEL);
		if (!temp) {
			printk(KERN_ALERT "DEV Module: Failed to initialize memory for Process List\n");
			return;
		}
		temp->node = (process_node*)kmalloc(sizeof(process_node), GFP_KERNEL);
		if (!temp->node) {
			printk(KERN_ALERT "DEV Module: Failed to initialize memory for Process Node\n");
			return;
		}
		temp->next = NULL;
		temp->node->process_id = task->pid;

		sprintf(temp->node->process_info, "PID = %8d  PPID = %8d  CPU = %4d  STATE = %s\n", task->pid, task->parent->pid, task->cpu, state_string);

		if (p_list==NULL) {
			p_list = temp;
		}
		else {
			p = p_list;
			while (p->next!=NULL) {
				p = p->next;
			}
			p->next = temp;
		}
		number_of_processes += 1;
	}
}


static int dev_open(struct inode *pinode, struct file *pfile) {

	/* file_operations open implementation, adding the calling program to the link list */

	pid_t pid;
	struct task_struct *task = current;
	add_to_process_list(task); // at this point add the current invoking process into the list
	pid = task->pid;
	printk(KERN_INFO "DEV Module: Process %d has called %s function of Dev Character Device Driver\n", pid, __FUNCTION__);
	return 0;
}

static ssize_t dev_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) {

	/* file_operations read implementation, copy process info to user space */

	int errors = 0;
	char message[PROCESS_STR_LEN];
	int message_len = 0;
	pid_t pid;

	pid = current->pid;
	get_process_info(message, offset);
	message_len = strlen(message);
	errors = copy_to_user(buffer, message, message_len);
	if (DRIVER_DEBUG) {
		printk(KERN_INFO "DEV Module: Process %d has called %s function with Offset %lld\n", pid, __FUNCTION__, *offset);
	}
	return errors == 0 ? message_len : -EFAULT;
}

static ssize_t dev_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) {

	/* file_operations write implementation, does nothing for now */

	pid_t pid;
	struct task_struct *task = current;
	pid = task->pid;
	if (DRIVER_DEBUG) {
		printk(KERN_INFO "DEV Module: Process %d has called %s function of Dev Character Device Driver\n", pid, __FUNCTION__);
	}
	return length;
}

static int dev_close(struct inode *pinode, struct file *pfile) {

	/* file_operations close implementation, we can remove the current process from the list */

	pid_t pid;
	struct task_struct *task = current;
	pid = task->pid;
	printk(KERN_INFO "DEV Module: Process %d has called %s function of Dev Character Device Driver\n", pid, __FUNCTION__);
	return 0;
}

static int __init dev_module_init(void) { // called when module is installed

	/* kernel init module implementation, register a char driver, build the process list, called when module is installed */

	struct task_struct *task = current;

	major_number = register_chrdev(0, DRIVER_NAME, &dev_file_op);
	if (major_number < 0) {
		printk(KERN_ALERT "DEV Module: Failed to register a major number\n");
		return major_number;
	}
	else {
		printk(KERN_INFO "DEV Module: Dev Character Device Driver Registered with major number %d\n", major_number);
		if (task) {
			create_process_list(task);
			if (DRIVER_DEBUG) {
				printk(KERN_INFO "DEV Module: Number of Process Added to List %d\n", number_of_processes);
			}
		}
	}
	return 0;
}

static void __exit dev_module_exit(void) { // called when module is removed

	/* kernel exit module implementation, un-register the char driver, free the process list, called when module is removed */

	free_process_list();
	unregister_chrdev(major_number, DRIVER_NAME);
	printk(KERN_ALERT "DEV Module: Removed the Dev Character Device Driver\n");
}

module_init(dev_module_init);
module_exit(dev_module_exit);
