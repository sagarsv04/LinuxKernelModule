#include<stdlib.h>
#include<signal.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>
#include<errno.h>



#define DRIVER_NAME "Dev Character Device Driver"
#define DRIVER_PATH "/dev/process_list"

void exit_handler(int signal) {
	printf("You have presses Ctrl-C\n");
	exit(0);
}


int main(int argc, char const *argv[]) {

	ssize_t read;
	size_t len = 0;
	FILE *file;
	char *line = NULL;

	printf("This is a simple program to interact with %s\n", DRIVER_NAME);

	file = fopen(DRIVER_PATH, "r");
	if (file == NULL) {
		fprintf(stderr, "Failed to open path %s, of %s\n", DRIVER_PATH, DRIVER_NAME);
		return errno;
	}
	else {
		printf("Reading from the %s\n", DRIVER_PATH);

		signal(SIGINT, exit_handler);
		while (1) {
			read = getline(&line, &len, file);
			if (read < 0){
				fprintf(stderr, "Failed to read the message from the %s\n", DRIVER_PATH);
				return errno;
			}
			else {
				printf(">>>: %s\n", line);
			}
		}

		fclose(file);
		if (line) {
			free(line);
		}
	}
	return 0;
}
