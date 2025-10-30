/*
* Code for Part 2.
* Will fork the process and perform the corresponding functionality.
*/

#include <cstddef>
#include <cstdlib>
#include <sys/types.h>
#include <iostream>
#include <unistd.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

int createSharedVariable(int **var, size_t size);

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
	int *counter = nullptr;
	int *multiple = nullptr;
	int counter_id = createSharedVariable(&counter, sizeof(int));
	if (counter_id == EXIT_FAILURE) {
		return EXIT_FAILURE;
	}
	int multiple_id = createSharedVariable(&multiple, sizeof(int));
	if (multiple_id == EXIT_FAILURE) {
		return EXIT_FAILURE;
	}
	*counter = 0;
	*multiple = 3;

	int sem = createSem(1);
	if (sem == -1) {
		return EXIT_FAILURE;
	}

	pid_t pid = fork();
	if (pid < 0) {
		std::cout << "Fork failed" << std::endl;
		return EXIT_FAILURE;
	}
	if (pid > 0) {
		std::cout << "Parent: " << getpid() << ", Child: " << pid << std::endl;
	}
	if (pid == 0) {
		while (*counter <= 100); //Wait until count hits 100
		while (*counter <= 500) {
			if (waitSem(sem, 0) == -1) {
				std::cout << "[Child] Wait Semaphore failed" << std::endl;
				return EXIT_FAILURE;
			}
			if (*counter % *multiple == 0) {
				std::cout << "[Child] Multiple: "<< *multiple << ", Count: " << *counter << std::endl;
			}
			*counter = *counter + 1;
			sleep(1);
			if (signalSem(sem) == -1) {
				std::cout << "[Child] Signal Semaphore failed" << std::endl;
				return EXIT_FAILURE;
			}
		}
	}
	else {
		while (*counter <= 500) {
			if (waitSem(sem, 0) == -1) {
				std::cout << "[Parent] Wait Semaphore failed" << std::endl;
				return EXIT_FAILURE;
			}
			if (*counter % *multiple == 0) {
				std::cout << "[Parent] Multiple: "<< *multiple << ", Count: " << *counter << std::endl;
			}
			*counter = *counter + 1;
			sleep(1);
			if (signalSem(sem) == -1) {
				std::cout << "[Parent] Signal Semaphore failed" << std::endl;
				return EXIT_FAILURE;
			}
		}
	}

	int detach_status;
	detach_status = shmdt(static_cast<void*>(counter));
	if (detach_status == -1) {
		std::cout << "Detach Memory failed" << std::endl;
		return EXIT_FAILURE;
	}
	shmdt(static_cast<void*>(multiple));
	if (detach_status == -1) {
		std::cout << "Detach Memory failed" << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int createSharedVariable(int **var, size_t size) {
	int var_id;
	void *var_addr;
	var_id = shmget(IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | 0666);
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

int createSem(int init) {
	int id;
	semun data;

	id = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666);
	if (id == -1) {
		std::cout << "Get Semaphore failed" << std::endl;
		return id;
	}
	data.val = init;
	if (semctl(id, 0, SETVAL, arg) == -1) {
		std::cout << "Init Semaphore failed" << std::endl;
		removeSem(id);
		return -1;
	}
	return id;
}

void removeSem(int id) {
	semctl(id, 0, IPC_RMID, 0);
}

int signalSem(int id) {
	sembuf Op;
	Op.sem_num = 0;
	Op.sem_op = 1;
	Op.sem_flg = 0;
	return semop(id, &Op, 1);
}

int waitSem(int id, int flag) {
	sembuf Op;
	Op.sem_num = 0;
	Op.sem_op = -1;
	Op.sem_flg = flag;
	return semop(id, &Op, 1);
}
