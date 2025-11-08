/*
* Code for Part 2.
* Will fork the process and perform the corresponding functionality.
*/

#include <cstddef>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <iostream>
#include <unistd.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

int createSharedVariable(int **var, size_t size, int key);

int createSem(int init);

void removeSem(int id);

int signalSem(int id);

int waitSem(int id, int flag);

union semun {
	int val;
	semid_ds *buf;
	ushort *array;
	seminfo *__buf;
}arg;

int main(){
	//Create and set shared variables
	int *counter = nullptr;
	int *multiple = nullptr;
	int counter_id = createSharedVariable(&counter, sizeof(int), 34);
	if (counter_id == EXIT_FAILURE) {
		return EXIT_FAILURE;
	}
	int multiple_id = createSharedVariable(&multiple, sizeof(int), 35);
	if (multiple_id == EXIT_FAILURE) {
		return EXIT_FAILURE;
	}
	*counter = 0;
	*multiple = 3;

	//Create semaphore
	int sem = createSem(1);
	if (sem == -1) {
		return EXIT_FAILURE;
	}
	
	//Counting logic
	pid_t pid = fork();
	if (pid < 0) {
		std::cout << "Fork failed" << std::endl;
		return EXIT_FAILURE;
	}
	if (pid > 0) {
		std::cout << "Parent: " << getpid() << ", Child: " << pid << std::endl;
	}
	if (pid == 0) {
		int error = execlp("bin/process2_101302923_101303908", "bin/process2_101302923_101303908", NULL);
	}
	else {
		bool run = true;
		do {
			if (waitSem(sem, SEM_UNDO) == -1) {
				std::cout << "[Parent] Wait Semaphore failed" << std::endl;
				return EXIT_FAILURE;
			}
			if (*counter > 500) run = false;
			if (run) {
				if (*counter % *multiple == 0) {
					std::cout << "[Parent] Multiple: "<< *multiple << ", Count: " << *counter << std::endl;
				}
				*counter = *counter + 1;
			}
			if (signalSem(sem) == -1) {
				std::cout << "[Parent] Signal Semaphore failed" << std::endl;
				return EXIT_FAILURE;
			}
			sleep(1);
		} while (run);
	}

	//detach shared variables
	int detach_status;
	detach_status = shmdt(static_cast<void*>(counter));
	if (detach_status == -1) {
		std::cout << "Detach Memory failed" << std::endl;
		return EXIT_FAILURE;
	}
	detach_status = shmdt(static_cast<void*>(multiple));
	if (detach_status == -1) {
		std::cout << "Detach Memory failed" << std::endl;
		return EXIT_FAILURE;
	}
	removeSem(sem);
	return EXIT_SUCCESS;
}

/*
 * Function for creating a shared integer
 */
int createSharedVariable(int **var, size_t size, int key) {
	int var_id;
	void *var_addr;
	var_id = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
	if (var_id == -1) {
		std::cout << "Get Shared Memory failed" << std::endl;
		return -1;
	}
	var_addr = shmat(var_id, nullptr, 0);
	if (var_addr == (void*)-1) {
		std::cout << "Attach Shared Memory failed" << std::endl;
		shmctl(var_id, IPC_RMID, nullptr);
		return -1;
	}
	*var = static_cast<int*>(var_addr);
	return var_id;
}

/*
* Function for creating and initializing a semaphore
*/
int createSem(int init) {
	int id;
	semun data;

	key_t key = ftok(".", 11);
	id = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
	if (id == -1) {
		std::cout << "Get Semaphore failed" << std::endl;
		return id;
	}
	data.val = init;
	if (semctl(id, 0, SETVAL, data) == -1) {
		std::cout << "Init Semaphore failed" << std::endl;
		removeSem(id);
		return -1;
	}
	return id;
}

/*
* Function for removing a semaphore
*/
void removeSem(int id) {
	semctl(id, 0, IPC_RMID, 0);
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
