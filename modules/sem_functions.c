#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/wait.h>

#include "common.h"
#include "sem_functions.h"


//function to increase the value of the semaphore with id semid
int semUp(int semid){
    struct sembuf sb;
    sb.sem_num=0;
    sb.sem_op=1; // semaphore up
    sb.sem_flg=0;

    if(semop(semid,&sb,1)== -1){
        perror("Semaphore up");
        return -1;
    }
    return 0; // Operation was succesfull
}

//function to decrease the value of the semaphore with id semid
int semDown(int semid){
    struct sembuf sb;
    sb.sem_num=0;
    sb.sem_op=-1; // semaphore down
    sb.sem_flg=0;

    if(semop(semid,&sb,1)== -1){
        perror("Semaphore Down");
        return -1;
    }
    return 0; // Operation was succesfull
}

//set the initial value of semaphore with id=semid to value=value
int semSetVal(int semid,int value){
    union semun arg;
    arg.val=value;
    if (semctl(semid, 0, SETVAL, arg) < 0) {
        perror("Could not set value of semaphore");
        exit(4);
    }

    return 0; // Operation was succesfull
}


// create a sempaphre
int semCreate(key_t key ){
    int semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0600);
    if (semid < 0) {
        perror("Could not create sem");
        exit(3);
    }
    return semid;
}


// create shared memory
int shareMemCreate(key_t key,int size,int flags){
    int shmid = shmget(key, size, flags);
    if (shmid == -1) {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

// clear IPC semaphores and shared memory, created by user
void clearIPCS(int semid1,int semid2,int semid3,int shmem1/*,int shmem2*/){
    
    
    /* remove  semaphores: */
    if (semctl(semid1, 0, IPC_RMID) == -1) {
        perror("semctl removing sem1");
    }
    if (semctl(semid2, 0, IPC_RMID) == -1) {
        perror("semctl removing sem2");
    }
    if (semctl(semid3, 0, IPC_RMID) == -1) {
        perror("semctl removing sem3");
    }
    /* Delete the shared memory segment */
    
    if (shmctl(shmem1,IPC_RMID,NULL) == -1) {
        perror("semctl removing shared memory");
    }
}