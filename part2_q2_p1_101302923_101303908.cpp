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
	if (pid > 0) {
		std::cout << "Parent: " << getpid() << ", Child: " << pid << std::endl;
	}
	if (pid < 0) {
		std::cout << "Fork failed" << std::endl;
		return EXIT_FAILURE;
	}
	while (1) {
		if (pid == 0) {
			execlp("./bin/part2_q2_p2_101302923_101303908", "", NULL);
		}
		else {
			std::cout << "Cycle number: " << counter;
			if (counter % 3 == 0) {
				std::cout << " - " << counter << " is a multiple of 3";
			}
			std::cout <<std::endl;
			counter++;
			sleep(1);
		}
	}
	return EXIT_SUCCESS;

}
