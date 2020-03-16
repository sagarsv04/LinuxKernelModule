# CS-550 Section 01 Spring 2020 Assignment 02
Character Device Driver to List Processes

# Goals
The goal of this assignment is to write a Kernel Character Device Driver for a "device" which lists all processes, along with information about those processes. This assignment should provide experience with developing Kernel Modules, and learning about character device drivers, as well as how the kernel manages processes.


# Description
Implement a kernel module in dev_ps.c that creates a /dev/process_list character device. The character device should support the read() operation. When the read() system call is invoked on your character device from a user space process, your kernel module should return to the following information for all currently running processes:
The process ID
The parent process ID
The CPU on which the process is running
The current state of the process
For example, the output might look as follows:

  PID=1 PPID=0 CPU=4 STATE=TASK_RUNNING
  PID=2 PPID=0 CPU=2 STATE=TASK_INTERRUPTIBLE
  PID=10 PPID=2 CPU=0 STATE=TASK_UNINTERRUPTIBLE
  PID=16434 PPID=16424 CPU=10 STATE=TASK_DEAD
  PID=14820 PPID=16424 CPU=8 STATE=TASK_WAKEKILL,TASK_UNINTERRUPTIBLE
  ...and so forth

# Hints:
You may want to use the for_each_process macro provided in the kernel source to loop through the list of processes.
Inside the kernel, processes are managed using the task_struct structure.
The state field in this structure is a bit-wise ORed value whose components are defined in sched.h defines.
The function task_cpu() can help you extract the currently assigned CPU for a process
You may choose to parse and expand process values in the kernel module, or you may choose to write raw process data from your kernel module, and expand that into printable information in the user space module that invokes the "device". Either method is acceptable.
Beware that bugs in kernel code may either crash your kernel immediately or may have no immediate visible effect, but may have a delayed effect that is disastrous. Therefore, you cannot assume that the thing you did most recently is necessarily the cause of a crash.
Provide a user-space C program called user.c that opens your character device and outputs the list of processes retrieved from your character device. For example, your user program might look like:

char (POINTER)buffer;

// allocate memory for character buffers HERE before you use them

fd = open("/dev/process_list", O_RDONLY);
// check for errors HERE

while(!some termination condition)
{
    bytes_read = read(fd, buffer, buffer_length);
    // check for errors HERE. Exit loop if all processes have been retrieved.
    // print the output you have read so far.
}

close(fd);


# References
Here are some references which might be useful for this assignment:

Example Kernel modules hello.c and hellon.c
The Kernel Modules lecture notes
The Linux Kernel Module Programming Guide - an online text.
bootlin - Online Kernel Source Cross Reference web site
Linux Device Drivers - an online textbook about Linux Device Drivers. Chapters 1 and 2 provide a good introduction to the kernel, though with a bias towards device-driver development.
Google "Linux Kernel" to find lots more references.

# Grading Guidelines

Tar and gzip a directory that contains the following:

Your kernel Module source code, dev_ps.c
Your user program source code, user.c
Makefile - the makefile you used to build the kernel module and your user program
A README file that describes any special features or shortcomings of your code.
We will grade your results as follows:

10 points if your intermediate solution shows how you will complete the assignment on time.
40 points if your kernel module builds and runs correctly.
30 points if your user space program correctly invokes the device driver and produces correct output for a single file descriptor.
10 points if your user space program demonstrates open of multiple simultaneous file descriptors using your device driver.
10 points if you code handles all error conditions, is clean, modular, well commented, and produces clean readable output.
Note that as long as your code runs, you may make your own implementation decisions about anything not explicitly described in the instructions and grading criteria above. Use the README file to document specific decisions you make.
