

#include <cstddef>
#include <cstdlib>
#include <sys/types.h>
#include <iostream>
#include <unistd.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

int signalSem(int id);

int waitSem(int id, int flag);

int main () {
	int counter_id = shmget(34, sizeof(int), 0666);
	int multiple_id = shmget(35, sizeof(int), 0666);
	volatile int *counter = (int*)shmat(counter_id, NULL, 0);
	volatile int *multiple = (int*)shmat(multiple_id, NULL, 0);
	key_t key = ftok(".", 11);
	int sem = semget(key, 1, 0666);
	if (sem == -1 || counter == (void*)-1 || multiple == (void*)-1) {
	    std::cerr << "[Child] Setup failed" << std::endl;
	    return EXIT_FAILURE;
	}

	bool run = true;
	do {
		sleep(1);
		if (waitSem(sem, SEM_UNDO) == -1) {
			std::cout << "[Child] Wait Semaphore failed" << std::endl;
			return EXIT_FAILURE;
		}
		if (*counter > 500) run = false;
		if (run && *counter > 100) {
			if (*counter % *multiple == 0) {
				std::cout << "[Child] Multiple: "<< *multiple << ", Count: " << *counter << std::endl;
			}
			*counter = *counter + 1;
		}
		if (signalSem(sem) == -1) {
			std::cout << "[Child] Signal Semaphore failed" << std::endl;
			return EXIT_FAILURE;
		}
	} while (run);
	return EXIT_SUCCESS;
}

/*
* Signal that shared resources are no longer in use
*/
int signalSem(int id) {
	sembuf Op;
	Op.sem_num = 0;
	Op.sem_op = 1;
	Op.sem_flg = 0;
	return semop(id, &Op, 1);
}

/*
* Wait for shared resources to become available
*/
int waitSem(int id, int flag) {
	sembuf Op;
	Op.sem_num = 0;
	Op.sem_op = -1;
	Op.sem_flg = flag;
	return semop(id, &Op, 1);
}
