
#include <cstddef>
#include <cstdlib>
#include <sys/types.h>
#include <iostream>
#include <unistd.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>


int main () {
	int counter_id = shmget(1, sizeof(int), 0666);
	int multiple_id = shmget(2, sizeof(int), 0666);
	int *counter = (int*)shmat(counter_id, NULL, 0);
	int *multiple = (int*)shmat(multiple_id, NULL, 0);

	while (*counter <= 100); //Wait until count hits 100
	while (*counter <= 500) {
		if (*counter % *multiple == 0) {
			std::cout << "[Child] Multiple: "<< *multiple << ", Count: " << *counter << std::endl;
		}
		*counter = *counter + 1;
		sleep(1);
	}
	return EXIT_SUCCESS;
}
