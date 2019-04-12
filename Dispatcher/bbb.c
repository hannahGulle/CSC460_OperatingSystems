// Hannah Gulle
// CSC 460 Dispatcher Project
// Due 04/12/2019

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

int LEFT( int i );
int RIGHT( int i );
void P( int s, int sem_id );
void V( int s, int sem_id );

// Process information Struct
typedef struct request {
        double remainingTime;
        int PID;
        int semID;
        time_t startTime;
        time_t endTime;
} request_t;

#define FULL 0
#define EMPTY 1
#define MUTEX 2
#define START 3

int main( int argc, char **argv ) {

        int timeSlice;
        int fullID, emptyID, mutexID, startID;
        int dieID;
        int i, clockID, bufferID, procNum;
        int *clockMem, *timeToDie, semVal;
        int numSems = 3; // clock, cpu, dispatcher
        request_t (*bufferPtr)[5];
        FILE *fopen(), *fp;

        if( argc == 1 ) {
                timeSlice = 1;
                printf( "Time Slice = 1\n" );
        }
        else {
                timeSlice = atoi( argv[1] );
                printf( "Time Slice = %d\n", timeSlice );
        }

        // Allocate the clock shared memory
        clockID = shmget( IPC_PRIVATE, sizeof( int ), 0777 );
        if( clockID == -1 ) {
                printf( "Error Getting Clock Shared Memory...\n" );
                return( -1 );
        }
        clockMem = shmat( clockID, NULL, SHM_RND );

        // Allocate Time To Die Shared Memory
        dieID = shmget( IPC_PRIVATE, sizeof( int ), 0777 );
        if( dieID == -1 ) {
                printf( "Error Getting Time To Die Shared Memory...\n" );
                return( -1 );
        }
        timeToDie = shmat( dieID, NULL, SHM_RND );


        // Allocate the shared memory for the bounded buffer for user requests
        bufferID = shmget( IPC_PRIVATE, 5*sizeof( request_t ), 0777 );
        if( bufferID == -1 ) {
                printf( "Error Getting Buffer Shared Memory...\n" );
                return( -1 );
        }
        bufferPtr = shmat( bufferID, NULL, SHM_RND );

        // Write Shared Memory and Semaphore IDs to the file
        if( (fp = fopen( "syncfile", "w" )) == NULL ) {
                printf( "Error Opening Sync File for Writing...\n" );
                return( -1 );
        }

        // Allocate the semaphores
        fullID = semget( IPC_PRIVATE, 1, 0777 );
        emptyID = semget( IPC_PRIVATE, 1, 0777 );
        mutexID = semget( IPC_PRIVATE, 1, 0777 );
        startID = semget( IPC_PRIVATE, 1, 0777 );

        if( (fullID == -1) || (emptyID == -1) || (mutexID == -1) || (startID == -1) ) {
                printf( "Error Getting Semaphores...\n" );
                return( -1 );
        }

        // Initialize the semaphores
        semctl( fullID, FULL, SETVAL, 0 );
        semctl( emptyID, EMPTY, SETVAL, 4 );
        semctl( mutexID, MUTEX, SETVAL, 1 );
        semctl( startID, START, SETVAL, 1 );

        // Write Shared Memory to the File
        fprintf( fp, "%d\n", clockID );
        fprintf( fp, "%d\n", dieID );
        fprintf( fp, "%d\n", bufferID );
        // Write Semaphores to the file
        fprintf( fp, "%d\n", fullID );
        fprintf( fp, "%d\n", emptyID );
        fprintf( fp, "%d\n", mutexID );
        fprintf( fp, "%d\n", startID );
        fclose( fp );

        printf( "Let the Simulation Begin!\n" );

        procNum = 0;
        for( i = 0; i < numSems; i++ ) {
                if( fork() != 0 ) break;

                procNum++;
        }

        // Initialize the clock at zero
        *clockMem = 0;
        // Initialize time to die to False
        *timeToDie = 0;

        return 0;

        int procFinished = 0;
        if( procNum == 0 ) { // Clock Process
                while( 1 ) {
                        printf( "%d", *clockMem );
                        sleep( 1 );

                        // Increment the clock counter
                        (*clockMem)++;

                        if( *timeToDie == 1 ) break;
                        // TODO: V all processes in the bounded buffer
                        // to wake them up, wait for them to all finish
                        // then clean. no additional processes added
                }
        }
        else if( procNum == 1 ) { // CPU Scheduler
                while( 1 ){
                        // P on mutex to write to the buffer spot
                        semVal = semctl( fullID, FULL, GETVAL);
                        if( semVal > 0 ) {
                                P( MUTEX, mutexID );

                                // Retrieve the time remaining in the first spot in the bounded buffer
                                // Run for either the remaining time or the time slice (whichever is less)
                                // Display the job
                                // Sleep for the least amount of time
                                if( bufferPtr[0]->remainingTime < timeSlice ) {
                                        printf( "Job %d running for %d\n", bufferPtr[0]->PID, bufferPtr[0]->remainingTime );
                                        sleep( bufferPtr[0]->remainingTime );
                                        bufferPtr[0]->remainingTime = 0;
                                }
                                else{
                                        printf( "Job %d running for %d\n", bufferPtr[0]->PID, timeSlice );
                                        sleep( timeSlice );
                                        bufferPtr[0]->remainingTime -= timeSlice;
                                }

                                // If the process finished, display finish info
                                if( bufferPtr[0]->remainingTime == 0 ) {
                                        printf( "Job %d Finished after %d time\n", bufferPtr[0]->PID, (bufferPtr[0]->startTime - bufferPtr[0]->endTime));
                                        procFinished = 1;

                                        // Remember, we P'd on the start semaphore when we started the process in the user.c program
                                        // We V on it when the job is finished
                                        V( START, startID );
                                }
                                V( MUTEX, mutexID );

                                if( procFinished == 1 ) {
                                        P( FULL, fullID );
                                        P( MUTEX, mutexID );

                                        // Delete the process from the bounded buffer and move up the remaining processes
                                        for( i = 0; i < 4; i ++ ) {
                                                bufferPtr[i]->remainingTime = bufferPtr[i+1]->remainingTime;
                                                bufferPtr[i]->PID = bufferPtr[i+1]->PID;
                                                bufferPtr[i]->semID = bufferPtr[i+1]->semID;
                                                bufferPtr[i]->startTime = bufferPtr[i+1]->startTime;
                                                bufferPtr[i]->endTime = bufferPtr[i+1]->endTime;
                                        }
                                        bufferPtr[4]->remainingTime = 0;
                                        bufferPtr[4]->PID = 0;
                                        bufferPtr[4]->semID = 0;

                                        V( MUTEX, mutexID );
                                        V( EMPTY, emptyID );

                                        procFinished = 0;
                                }
                        }
                }
        }
        else { // Dispatcher
                while( 1 ){
                        P( EMPTY, emptyID );
                        P( MUTEX, mutexID );

                        // TODO: Add a new job from the queue into the last space in the buffer

                        V( MUTEX, mutexID );
                        V( FULL, emptyID );
                }
        }

        return 0;
}

int LEFT( int i ) {
        return ((i+4)%5);
}

int RIGHT( int i ) {
        return ((i+1)%5);
}

// P Function for Semaphores
void P( int s, int sem_id ) {
        struct sembuf sops;

        sops.sem_num = s;
        sops.sem_op = -1; // Subtract 1 from the semval
        sops.sem_flg = 0;
        if((semop(sem_id, &sops, 1)) == -1) {
                printf("%s", "'P' error\n");
        }
}

// V Function for Semaphores
void V( int s, int sem_id ) {
         struct sembuf sops;

         sops.sem_num = s;
         sops.sem_op = 1; // Add 1 to the semval
         sops.sem_flg = 0;
         if((semop(sem_id, &sops, 1)) == -1) {
                printf("%s","'V' error\n");
         }
}