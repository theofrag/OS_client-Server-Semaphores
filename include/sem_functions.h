#include <sys/ipc.h>
#define TEXT_SZ 200


// struct for shared data
struct shared_use_st {
	int line_number;
	char some_text[TEXT_SZ];
};


//function to increase the value of the semaphore with id semid
int semUp(int );


//function to decrease the value of the semaphore with id semid
int semDown(int);

//set the initial value of semaphore with id=semid to value=value
int semSetVal(int,int);

// create a sempaphre
int semCreate(key_t);

// create shared memory
int shareMemCreate(key_t key,int size,int flags);

// clear IPC semaphores and shared memory, created by user
void clearIPCS(int semid1,int semid2,int semid3,int shmem1/*,int shmem2*/);