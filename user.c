#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>
#include<errno.h>



#define DRIVER_NAME "Dev Character Device Driver"
#define DRIVER_PATH "/dev/process_list"
#define BUFFER_LENGTH 256


void exit_handler(int signal) {
	printf("You have presses Ctrl-C\n");
	exit(0);
}


int main(int argc, char const *argv[]) {

	int ret;
	FILE, file;
	char receive[BUFFER_LENGTH];
	printf("Starting device test code example...\n");
	printf("This is a simple program to interact with %s.\n", DRIVER_NAME);

	file = open(DRIVER_PATH, O_RDONLY);
	if (file < 0) {
    perror("Failed to open path %s, of %s", DRIVER_PATH, DRIVER_NAME);
    return errno;
	}
	else {
		printf("Reading from the %s\n", DRIVER_PATH);

		signal(SIGINT, exit_handler);
		while (1) {
			ret = read(file, receive, BUFFER_LENGTH);
   		if (ret < 0){
      	perror("Failed to read the message from the %s\n", DRIVER_PATH);
      	return errno;
   		}
			else {
				printf(">>>: %s\n", receive);
			}
		}
	}

	return 0;
}
