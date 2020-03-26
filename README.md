# LinuxKernelModule

A simple Character Device Driver to List Processes.


Author :
============
Sagar Vishwakarma (svishwa2@binghamton.edu)

State University of New York, Binghamton


File-Info
============

1)	Makefile         - Compile the program
2)	dev_ps.c         - Kernel Character Device Driver
3)	user.c           - User Space C program


How to compile and run
============
-	Open a terminal in project directory      : make (to build both kernel and user module)
- Load the kernel module use                : sudo insmod dev_ps.ko
- Unload the kernel module use              : sudo rmmod dev_ps
- Above two commands should print messages in kern.log (Use: tail -f /var/log/kern.log)


- To register a new device/node which will point to our dev_ps
- cd /dev/ (dev contains nodes to all the devices on the system)
- sudo mknod process_list c major_number 0
- To remove node : sudo rm process_list


Note
============
