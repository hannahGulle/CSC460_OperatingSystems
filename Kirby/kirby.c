// Hannah Gulle
// CSC 460 Kirby Project
// Due 03/01/2019

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <math.h>

int main( int argc, char **argv ) {

        int i,j, semID, procNum;

        // Intialize 5 semaphores
        semID = semget( IPC_PRIVATE, 5, 0777 );
        if( semID == - 1 ) {
                printf( "Error Getting Semaphores...\n" );
                return( -1 );
        }
        // All semaphores are parents (no children)
        for( i = 0; i < 5; i++ ) {
                semctl( semID, i, SETVAL, 1 );
        }

        // Fork 5 processes
        procNum = 0;
        for( i = 0; i < 4; i++ ) {
                if( fork() != 0 ) break;

                // Increment process number [0,4]
                procNum++;
        }

        while( 1 ) {

                // Print "Thinking" to the process' column
                for( i = 0; i < procNum; i++ ) {
                        printf( "\t" );
                }
                printf( " %d Thinking...\n", procNum );

                // Eat up CPU
                for( i = 0; i < 1000; i++ ) {
                        for( j = 0; j < 1000; j++ ) {}
                }

                // Print "Hungry" to the process' column
                for( i = 0; i < procNum; i++ ) {
                        printf( "\t" );
                }
                printf( " %d Hungry...\n", procNum );

                // Grab Left Chopstick
                // Where processes are circularly linked
                p( (procNum+1)%5 , semID );
                // Grab Right Chopstick
                p( procNum , semID );

                // Print "Eating" To the process' column
                for( i = 0; i < procNum; i++ ) {
                        printf( "\t" );
                }
                printf( " %d Eating...\n", procNum );

                // Eat Up CPU
                for( i = 0; i < 1000; i++ ) {
                        for( j = 0; j < 1000; j++ ) {}
                }

                // Put Down Left Chopstick
                // Where processes are circularly linked
                v( (procNum+1)%5 , semID );
                // Put Down Right Chopstick
                v( procNum , semID );

        }
        return 0;
}

// P Function for Semaphores
p( int s, int sem_id ) {
        struct sembuf sops;

        sops.sem_num = s;
        sops.sem_op = -1;
        sops.sem_flg = 0;
        if((semop(sem_id, &sops, 1)) == -1) {
                printf("%s", "'P' error\n");
        }
}

// V Function for Semaphores
v( int s, int sem_id ) {
         struct sembuf sops;

         sops.sem_num = s;
         sops.sem_op = 1;
         sops.sem_flg = 0;
         if((semop(sem_id, &sops, 1)) == -1) {
                printf("%s","'V' error\n");
         }
}