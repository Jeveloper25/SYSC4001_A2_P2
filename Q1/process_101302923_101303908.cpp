/*
* Code for Part 2.
* Will fork the process and perform the corresponding functionality.
*/
#include <cstddef>
#include <cstdlib>
#include <sys/types.h>
#include <iostream>
#include <unistd.h>

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
	while (1) {
			std::cout << counter << std::endl;
			counter++;
			sleep(1);
	}
	return EXIT_SUCCESS;

}
