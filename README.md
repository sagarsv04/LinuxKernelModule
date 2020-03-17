# LinuxKernelModule

A simple Character Device Driver to List Processes.


Author :
============
Sagar Vishwakarma (svishwa2@binghamton.edu)

State University of New York, Binghamton



TODO
============
- Currently I have build a demo module to understand working of kernel module
- I will be using file_operations struct for all the file operations using the functions like dev_open, dev_read, dev_write, dev_close,
- I will be implementing functions which will get process ID, Parent Process ID, and state of processes and writing it into /dev/process_list
- I will create a user program which will read /dev/process_list and print all the info in user space.
- above tasks can be achived in a week


File-Info
============

1)	Makefile         - Compile the program


How to compile and run
============


Commands Supported
============


Note
============
