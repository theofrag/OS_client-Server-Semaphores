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
#include "general.h"
// client-server

int main(int args, char** argv){

    int K = atoi(argv[1]);  // number of clients
    int N = atoi(argv[2]);  // poses fores tha zitisei o kathe client grammi

    key_t key1,key2,key3;
    int sem_id1,sem_id2,sem_id3;


    // key for sem1
    if ((key1 = ftok("test.c", 'A')) == -1) {
		perror("ftok");
		exit(1);
	}
    // key for sem2
    if ((key2 = ftok("test.c", 'B')) == -1) {
		perror("ftok");
		exit(1);
	}

    // key for sem3
    if ((key3 = ftok("test.c", 'C')) == -1) {
		perror("ftok");
		exit(1);
	}

    // Create  sem1 and set its value to 1
    sem_id1 = semCreate(key1);
    semSetVal(sem_id1,1);

    // Create the sem2,set value to 0
    sem_id2 = semCreate(key2);
    semSetVal(sem_id2,0);

    // Create the sem3, set value to 0
    sem_id3 = semCreate(key3);
    semSetVal(sem_id3,0);


    // create shared memory
    int shmid= shareMemCreate((key_t)1234 ,sizeof(struct shared_use_st), 0666 | IPC_CREAT);

    // attach shared memory
    void *shared_memory = (void *)0;
    shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        //TODO na balo clear ipcs
        exit(EXIT_FAILURE);
    }
    printf("Shared memory segment with id %d attached at %p \n", shmid, shared_memory);

    // Access shared memory
    //shared_stuff now points in shared memory
    struct shared_use_st*  shared_stuff;
    shared_stuff= (struct shared_use_st* )shared_memory;

    
    // create clients
    int pid;
    for (int i = 0; i <K; ++i) {
        
        if ((pid = fork()) < 0) {
            perror("Could not fork, please create clients manually");
        }
        else if(pid==0){
            fprintf(stdout,"Just forked\n");
            break;
        }        
    }

    // Client segment
    
    if(pid==0){
        float meanTime=0,sttime1=0, sttime2=0;  //sttime1 and sttime2 are used to calculate overall procces time
        int numberOfLines= countNUmberOfLines("test.txt");
        for(int i=0;i<N;i++){   //kane to parakato gia N dosolipsies
            srand(time(NULL) ^ (getpid()<<16)); //arxikopoiisi genitrias tyxaion arithmon
            int line= rand()%numberOfLines;

            fprintf(stdout,"Sem1 before in by client %d is: %d\n",getpid(),semctl(sem_id1,0,GETVAL));
            fprintf(stdout,"Preparing to enter by client %d\n",getpid());

            if(semDown(sem_id1)==-1){   //error handling in semaphore down operation
                fprintf(stdout,"Unexpected error in sem1 before down operation\n");
                semctl(sem_id2, 0, IPC_RMID); //gia na katalabei o server oti kati egine kai na termatisei 
                exit(EXIT_FAILURE);
            }
            fprintf(stdout,"Entered by client %d\n",getpid());  //todo isos to sbiso auto den xreiazetai

            // Write in shared memory which line you want
            shared_stuff->line_number=line;
            sttime1 = ((double) clock())/CLOCKS_PER_SEC;
            fprintf(stdout," \x1B[32m request line: %d by client %d \x1B[0m \n",line, getpid());
            
            if(semUp(sem_id2)==-1){ //error handling in semaphore Up operation
                fprintf(stdout,"Unexpected error in sem2 before up operation\n");
                semctl(sem_id2, 0, IPC_RMID); //gia na katalabei o server oti kati egine kai na termatisei 
                exit(EXIT_FAILURE);
            }

            if(semDown(sem_id3)==-1){   //error handling in semaphore down operation
                fprintf(stdout,"Unexpected error in sem3 before down operation\n");
                semctl(sem_id2, 0, IPC_RMID); //gia na katalabei o server oti kati egine kai na termatisei 
                exit(EXIT_FAILURE);
            }
            
            // read x from memory and print it
            printf("\x1b[33m %s \x1b[0m",shared_stuff->some_text);
            sttime2 = ((double) clock())/CLOCKS_PER_SEC;
            meanTime+= sttime2-sttime1;


            fprintf(stdout,"Preparing to leave client %d\n",getpid());
            fprintf(stdout,"Sem1 before up is: %d\n",semctl(sem_id1,0,GETVAL));
            // semUp(sem_id1);  //up sem1 //!edo moy ebgale error ,giati? //!bale error handling
            if(semUp(sem_id1)==-1){
                fprintf(stdout,"Unexpected error in sem1 before up operation\n");
                semctl(sem_id2, 0, IPC_RMID); //gia na katalabei o server oti kati egine kai na termatisei 
                exit(EXIT_FAILURE);
            }

            fprintf(stdout, "Left client %d\n",getpid());
        }
            fprintf(stdout,"\x1b[31m Overall time for process %d is %f \x1b[0m \n ",getpid(),meanTime/N);
            printf("Child (%d) calling exit(0)\n", getpid());
            exit(0); 
    }

    //Server
    else{
       
       for(int i=0;i<K*N;i++){
            printf("preparing to enter SERVER part\n");

            if(semDown(sem_id2)==-1){
                fprintf(stdout,"Unexpected error in sem2 before down operation\n");
                clearIPCS(sem_id1,sem_id2,sem_id3,shmid/*,shmem2*/);
                exit(EXIT_FAILURE);
            }
            printf("entered toSERVER\n");

            char* toCopy=readLineFromFile(shared_stuff->line_number,"test.txt");
            strcpy(shared_stuff->some_text,toCopy);
            free(toCopy);   //gia na min exo memory leaks
            fprintf(stdout,"\x1B[32m response line: %d \x1B[0m \n",shared_stuff->line_number);

            if(semUp(sem_id3)==-1){
                fprintf(stdout,"Unexpected error in sem3 before up operation\n");
                clearIPCS(sem_id1,sem_id2,sem_id3,shmid/*,shmem2*/);
                exit(EXIT_FAILURE);
            }
            printf("exited from SERVER part\n"); 
        }

        int status;
        for(int i=0;i<K;i++){   //mazepse ta paidia diergasies
            wait(&status);
        }
        //! apo edo kai kato einai douleia tou server
        if (shmdt(shared_memory) == -1) {
            fprintf(stderr, "shmdt failed\n");
            clearIPCS(sem_id1,sem_id2,sem_id3,shmid);
            exit(EXIT_FAILURE);
        }
        clearIPCS(sem_id1,sem_id2,sem_id3,shmid);
    
        
        exit(EXIT_SUCCESS);
    }



    
}