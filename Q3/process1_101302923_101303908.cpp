/*
* Code for Part 2.
* Will fork the process and perform the corresponding functionality.
*/
#include <cstddef>
#include <cstdlib>
#include <sys/types.h>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <wait.h>

int main(){
	pid_t pid = fork();
	int counter = 0;
	if (pid < 0) {
		std::cout << "Fork failed" << std::endl;
		return EXIT_FAILURE;
	}
	if (pid > 0) {
		std::cout << "Parent: " << getpid() << ", Child: " << pid << std::endl;
	}
	if (pid == 0) {
		execlp("./bin/process2_101302923_101303908", "", NULL);
	}
	else {
		int exit_status;
		wait(&exit_status);
		if (exit_status < 0) {
			std::cout << "Wait failed" << std::endl;
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;

}
