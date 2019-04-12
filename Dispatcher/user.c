// Hannah Gulle
// CSC 460 Dispatcher Project
// Due 04/12/2019

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>

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

        int numJobs;
        int startID, bufferID;
        int fullID, emptyID, mutexID;
        int dieID, clockID, *timeToDie, *clock;
        FILE *fopen(), *fp;
        request_t *buffer, *queue;
        request_t newJob = { 0 };
        int currTime;

        if( argc == 1 ) {
                printf( "Number of Jobs Not Given\n" );
                return( -1 );
        }
        else {
                numJobs = atoi( argv[1] );
                if( numJobs < 0 || numJobs > 50 ) {
                        printf( "Invalid Number of Jobs\n" );
                        return( -1 );
                }
        }

        if( numJobs == 0 ) *timeToDie = 1;

        // TODO: Create shared memory for the N sized job buffer
        // TODO: Write the queue shared memeory ID to the syncfile


        // Open the syncfile to read
        // Attach to the clockID, dieID, bufferID
        // Attach to the fullID, emptyID, mutexID
        if((fp = fopen( "syncfile", "r" )) == NULL ) {
                printf( "Error Opening Sync File\n" );
                return( -1 );
        }
        fscanf( fp, "%d", &clockID );
        fscanf( fp, "%d", &dieID );
        fscanf( fp, "%d", &bufferID );
        fscanf( fp, "%d", &fullID );
        fscanf( fp, "%d", &emptyID );
        fscanf( fp, "%d", &mutexID );
        fscanf( fp, "%d", &startID );
        fclose( fp );

        clock = (int*) shmat( clockID, NULL, SHM_RND );
        timeToDie = (int*) shmat( dieID, NULL, SHM_RND );
        buffer = (request_t*) shmat( bufferID, NULL, SHM_RND );

        int i, thinkTime, cpuTime;
        double totalTurnaround = 0;
        srand( getpid() );

        // TODO: Figure out if each job (of N) is forked or on one process
        while( i < numJobs ) {
                thinkTime = rand()%8 + 2;
                printf( "\t%d is thinking for %d seconds.\n", getpid(), thinkTime );
                sleep( thinkTime );

                // Check if it is time to die
                if( *timeToDie == 1 ) break;

                cpuTime = rand()%4 + 1;
                // set the job remaining time to cpuTime
                printf( "\t%d requests %d\n", getpid(), cpuTime );
                newJob.remainingTime = cpuTime;

                // Get the current bbb clock time from shared memory
                currTime = *clock;

                // Attach to the full and empty semaphores
                P( FULL, fullID );
                        // Fill the new job struct with initial info
                        newJob.PID = getpid();
                        newJob.semID = startID;
                        time( &(newJob.startTime) );

                        // TODO: Add the new job struct to the queue
                        // remember, the queue is size N and the buffer is size 5

                        // P on the start semaphore for the job
                        P( START, startID );
                        // TODO: bbb should V on the start semaphore for the job when it is done


                V( EMPTY, emptyID );


                if( *timeToDie == 1 ) break;

                // Retrieve current shared clock time
                time( &currTime );
                printf( "\t%d finished in %d\n", getpid(), (newJob.startTime - currTime) );
                totalTurnaround += (newJob.startTime - currTime );

                // Increment job count (out of N)
                i++;
        }

        printf( "\t%d logging off with avg TT= %d\n", getpid(), (totalTurnaround/numJobs));

        // TODO: Detach from all the shared memories and semaphores

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
        sops.sem_op = -1;
        sops.sem_flg = 0;
        if((semop(sem_id, &sops, 1)) == -1) {
                printf("%s", "'P' error\n");
        }
}

// V Function for Semaphores
void V( int s, int sem_id ) {
         struct sembuf sops;

         sops.sem_num = s;
         sops.sem_op = 1;
         sops.sem_flg = 0;
         if((semop(sem_id, &sops, 1)) == -1) {
                printf("%s","'V' error\n");
         }
}