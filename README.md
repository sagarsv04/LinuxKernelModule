# LinuxKernelModule

A simple Character Device Driver to List Processes.


Author :
============
Sagar Vishwakarma (svishwa2@binghamton.edu)

State University of New York, Binghamton


Files :
============

1)	Makefile         - Compile the program
2)	dev_ps.c         - Kernel Character Device Driver
3)	user.c           - User Space C program


Flags :
============

- DRIVER_DEBUG      - To print additional information happening in kernel program
- USER_SLEEP        - Wait in user space program between each read
- USER_DEBUG        - To print additional information happening in user program


Run :
============

- Open a terminal in project directory      : make (to build both kernel and user module)
- Load the kernel module use                : sudo insmod dev_ps.ko
- Unload the kernel module use              : sudo rmmod dev_ps
- Run user code                             : sudo ./user

- Above two commands should print messages in kern.log (Use: tail -f /var/log/kern.log)

- Older commit didn't register device automatically, in such case we can manually make a node as follows:

- To register a new device/node which will point to our dev_ps
- cd /dev/ (dev contains nodes to all the devices on the system)
- sudo mknod process_list c major_number 0
- To remove node : sudo rm process_list


Note
============

- I have implemented a simple Character Driver which lists processes
	eg: PID=1 PPID=0 CPU=4 STATE=TASK_RUNNING
- When the kernel module is install it creates a list of all the current running processes
- This list is saved globally in kernel space
- When a new process access the kernel node (ie: opens "process_list") it gets added to the kernel space process list
- When the new process performs a read operation, iteratively info of each process from the list is copied to user space
- No repetitive process info is passed to the same process
- Multiple user space program can be run simultaneously each accessing the node "process_list"
- Each process will get process info independently
- Any user space program stared later will also get the process info of the other user space programs started earlier

- "EXIT_CODE" string is copied to user space if all the process info is passed into user space
- This is to stop user space program from continuously keep reading from kernel space
