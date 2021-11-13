#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h>

#include "common.h"

#define TEXT_SZ 200

//! as balo kai kana typedef
//! as allaxo onoma se kamia metabliti
//! ftiaxno makefile
//! fprintf(stdout,"") gia na ta bgazei kateutheian

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

// struct for shared data
//! to bgazo apo edo
struct shared_use_st {
	int line_number;
	char some_text[TEXT_SZ];
};


// reads a specific line from a file
// line_number represents which line to read
// filename is the name of the file
// TODO kane to ligo kalytero me ta paths
//! to bgazo apo edo
char* readLineFromFile(int line_number, char* fileName){  
    char buffer[100];
    FILE* file;
    char* toReturn;
    int buffer_length=100;
    
    file=fopen(fileName,"r");
    int i=0;  
    while(fgets(buffer,buffer_length,file)){
        if(i++==line_number){
            toReturn = malloc(sizeof(buffer));
            strcpy(toReturn,buffer);
            return toReturn;}
    
    }
}

// client-server
//! gia SERVER einai edo
int main(int args, char** argv){

    int LINES = atoi(argv[1]);

    key_t key1,key2,key3;
    int sem_id1,sem_id2,sem_id3;
    union semun arg1,arg2,arg3;

    //! kane ta me pinaka kalytera edo
    // for sem1
    if ((key1 = ftok("test.c", 'A')) == -1) {
		perror("ftok");
		exit(1);
	}
    // for sem2
    if ((key2 = ftok("test.c", 'B')) == -1) {
		perror("ftok");
		exit(1);
	}

    // for sem3
    if ((key3 = ftok("test.c", 'C')) == -1) {
		perror("ftok");
		exit(1);
	}

    //! kai edo me pinaka kai synartisi
    // Create the sem1, set value to 1
    sem_id1 = semget(key1, 1, IPC_CREAT | IPC_EXCL | 0600);
    if (sem_id1 < 0) {
        perror("Could not create sem");
        exit(3);
    }
    arg1.val=1;
    if (semctl(sem_id1, 0, SETVAL, arg1) < 0) {
        perror("Could not set value of semaphore");
        exit(4);
    }
    
    // Create the sem2,set value to 1
    sem_id2 = semget(key2, 1, IPC_CREAT | IPC_EXCL | 0600);
    if (sem_id2 < 0) {
        perror("Could not create sem");
        exit(3);
    }
    arg2.val=0;
    if (semctl(sem_id2, 0, SETVAL, arg2) < 0) {
        perror("Could not set value of semaphore");
        exit(4);
    }

    // Create the sem3, set value to 0
    sem_id3 = semget(key3, 1, IPC_CREAT | IPC_EXCL | 0600);
    if (sem_id3 < 0) {
        perror("Could not create sem");
        exit(3);
    }
    arg3.val=0;
    if (semctl(sem_id3, 0, SETVAL, arg3) < 0) {
        perror("Could not set value of semaphore");
        exit(4);
    }


    // Set shared memory

    // Initialize sembufs
    //! kai edo min einai etsi
    struct sembuf sb1,sb2,sb3;
    sb1.sem_num = 0;
    sb1.sem_op = -1;  /* set to allocate resource */
    sb1.sem_flg = 0;

    sb2.sem_num = 0;
    sb2.sem_op = 1;  /* set to allocate resource */
    sb2.sem_flg = 0;

    sb3.sem_num = 0;
    sb3.sem_op = -1;  /* set to allocate resource */
    sb3.sem_flg = 0;

    // Now create some clients
    //! prosoxi edo vazo exec
    int pid;
    for (int i = 0; i < 1; ++i) {
        
        if ((pid = fork()) < 0) {
            perror("Could not fork, please create clients manually");
        }
        fprintf(stdout,"Just forked\n");
        
    }

    
    

    // Client segment
    //! auto tha figi apo edo, tha paei sto client.c
    if(pid==0){

        // Create shared memory
        int shmid;
        shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
        if (shmid == -1) {
            fprintf(stderr, "shmget failed\n");
            exit(EXIT_FAILURE);
        }
        // attach shared memory
        void *shared_memory = (void *)0;
        shared_memory = shmat(shmid, (void *)0, 0);
        if (shared_memory == (void *)-1) {
            fprintf(stderr, "shmat failed\n");
            exit(EXIT_FAILURE);
        }
        printf("Shared memory segment with id %d attached at %p\n", shmid, shared_memory);

        // Access shared memory
        struct shared_use_st *shared_stuff;
        shared_stuff= (struct shared_use_st* )shared_memory;


        srand(time(NULL) ^ (getpid()<<16));
        semop(sem_id1,&sb1,1);  //down sem1
        // TODO fetch line x,diladi grapse to sto shared memory
        int line= rand()%9;
        shared_stuff->line_number=line;
        fprintf(stdout,"Fetch line: %d\n",line);
        //
        semop(sem_id2,&sb2,1);  //up sem2
        semop(sem_id3,&sb3,1);  //down sem3
        //  TODO read x from memory and print it
        printf("%s",shared_stuff->some_text);
        //
        semop(sem_id3,&sb3,1);  //down sem3

        sb1.sem_op=1;
        semop(sem_id1,&sb1,1);  //up sem1



        

    }

    //Server
    //! Ayto paei sto server.c, diladi menei edo
    else{
        // Create shared memory
        int shmid;
        shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
        if (shmid == -1) {
            fprintf(stderr, "shmget failed\n");
            exit(EXIT_FAILURE);
        }
        // attach shared memory
        void *shared_memory = (void *)0;
        shared_memory = shmat(shmid, (void *)0, 0);
        if (shared_memory == (void *)-1) {
            fprintf(stderr, "shmat failed\n");
            exit(EXIT_FAILURE);
        }
        printf("Shared memory segment with id %d attached at %p\n", shmid, shared_memory);

        // Access shared memory
        struct shared_use_st *shared_stuff;
        shared_stuff= (struct shared_use_st* )shared_memory;

        sb2.sem_op=-1;
        semop(sem_id2,&sb2,1);  //down sem2
        //TODO bres tin grammi,grapse tin sthn mnimi
        //! prosexe edo thelei free. H synartisi readLineFromFile epistrefei string me malloc
        strcpy(shared_stuff->some_text,readLineFromFile(shared_stuff->line_number,"test.txt"));
        fprintf(stdout,"Take line: %d\n",shared_stuff->line_number);

        
        //
        sb3.sem_op=1;
        semop(sem_id3,&sb3,1);  //up sem3




        //! apo edo kai kato einai douleia toy server
        if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
        }
    
        /* remove it semaphores: */
        if (semctl(sem_id1, 0, IPC_RMID) == -1) {
            perror("semctl");
            exit(1);
        }
        if (semctl(sem_id2, 0, IPC_RMID) == -1) {
            perror("semctl");
            exit(1);
        }
        if (semctl(sem_id3, 0, IPC_RMID) == -1) {
            perror("semctl");
            exit(1);
        }
        
        exit(EXIT_SUCCESS);
    }



    
}