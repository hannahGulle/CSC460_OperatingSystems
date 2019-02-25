// Hannah Gulle
// CSC 460
// Wario Project - 16 Processes Access a Bank Account
// Due Mon 02/25/19

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <math.h>

int main( int argc, char** argv ) {

        FILE *fopen(), *fp;
        int i,j,k, a, semID, numSems, shmBalanceID, amount;
        int* balance;

        // Initialize Semaphores and Shared Memory
        if( argc == 1 ) { // No command line arguments given
                printf("Initializing Memory...\n");

                numSems = 1;

                if( (fp = fopen( "syncFile", "w" )) == NULL ) {
                        printf( "Error Opening Sync File\n" );
                        return 0;
                }

                // Get semaphore
                semID = semget( IPC_PRIVATE, numSems, 0777 );
                if( semID == -1 ) {
                        printf("Error Requesting Semaphore Memory\n");
                        return 0;
                }

                // Initialize Semaphore
                semctl( semID, 0, SETVAL, 1 );

                // Initialize Shared Memory
                shmBalanceID = shmget( IPC_PRIVATE, 4, 0777 );
                if( shmBalanceID == -1 ) {
                        printf("Error Requesting Shared Memory\n");
                        return( -1 );
                }
                // Read shared memory value at the location
                balance = ( int* ) shmat( shmBalanceID, NULL, SHM_RND );
                *balance = 1000; // Set initial balance to 1000

                // Sync File contains sem ID and shm ram location
                fprintf( fp, "%d\n", shmBalanceID );
                fprintf( fp, "%d\n", semID );
                fclose( fp );
        }
        else if( argc > 1 ) { // One Command Line Argument Given

                int input = atoi( argv[1] );
                int myPID = getpid();
                int myID = 0;

                if( strcmp( "die", argv[1] ) == 0 ) { // Command Line Argument to Terminate program

                        if( (fp = fopen( "syncFile", "r" )) == NULL ) {
                                printf( "Error Opening Sync File: Program Not Initialized\n" );
                                return 0;
                        }

                        // Read shared memory and semaphore IDs from file
                        fscanf( fp, "%d", &shmBalanceID );
                        fscanf( fp, "%d", &semID );
                        fclose( fp );

                        // Retrieve balance at shared memory
                        balance = ( int* ) shmat( shmBalanceID, NULL, SHM_RND );
                        printf( "Account Balance: %d\n", *balance );

                        // Delete Shared Memory
                        if( (semctl( semID, 0, IPC_RMID, 0 )) == -1 ) {
                                printf( "Error Removing Semaphore\n" );
                        }

                        // Delete Shared Memory
                        if( (shmctl( shmBalanceID, IPC_RMID, NULL )) == -1 ) {
                                printf( "Error Removing Shared Memory\n" );
                        }

                        printf( "Terminating Program...\n" );
                } // End Die
                else if( strcmp( "balance", argv[1] ) == 0 ) { // Command Line Arg to print balance

                        if( (fp = fopen( "syncFile", "r" )) == NULL ) {
                                printf( "Error Opening Sync File: Program Not Initialized\n" );
                                return 0;
                        }

                        // Read the shared memory ID from the file
                        fscanf( fp, "%d", &shmBalanceID );
                        fclose( fp );

                        // Retrieve balance from shared memory
                        balance = ( int* ) shmat( shmBalanceID, NULL, SHM_RND );
                        printf( "Account Balance: %d\n", *balance );
                }
                else if( input > 0 && input < 101 ) { // Simulation Input in [1,100]

                        printf( "Running Simulation...\n" );

                        if( (fp = fopen( "syncFile", "r" )) == NULL ) {
                                printf( "Error Opening Sync File: Program Not Initialized\n" );
                                return 0;
                        }
                        // Read the shared memory and semaphore IDs from the file
                        fscanf( fp, "%d", &shmBalanceID );
                        fscanf( fp, "%d", &semID );
                        fclose( fp );

                        // Retrieve the balance from the shared memory
                        balance = ( int* ) shmat( shmBalanceID, NULL, SHM_RND );

                        for( i = 0; i < 16; i++ ) {

                                // Set process amount
                                amount = ( int ) pow( 2, i );

                                a = fork();
                                // First fork makes a linear tree of processes (parents)
                                if( a == 0 ) {
                                        // Second fork makes the actual child process
                                        if( fork() == 0 ) {
                                                // Child amount is subtracted from balance
                                                amount = 0-amount;
                                        }
                                        break;
                                }
                                myID++; // Increment process ID
                        }

                        // Run Processes until the parent ID is the initial program ID
                        if( getpid() != myPID ) {
                                // Run the simulation for the number of times given at the command line
                                for( i = 0; i < input; i++ ) {
                                        if( amount < 0 ) { // Child Process
                                                for( j = 0; j < input; j++ ) {
                                                        p(0,semID);
                                                        // -(amount) to print positive amount
                                                        printf("\t\t%d - %d = %d\n", *balance, -(amount), (*balance + amount) );
                                                        *balance += amount;
                                                        v( 0, semID );
                                                }
                                        }
                                        else { // Parent Process
                                                for( k = 0; k < input; k++ ) {
                                                        p(0,semID);
                                                        printf("%d + %d = %d\n", *balance, amount, (*balance + amount) );
                                                        *balance += amount;
                                                        v( 0, semID );
                                                }
                                        }
                                }
                        }
                }
                else { // Invalid Argumnent Type
                        printf( "Invalid Input\n" );
                        return 0;
                }

        }
        else { // Invalid Command Line Number Input
                printf( "Invalid Number of Command Line Arguments\n" );
        }
        return 0;
}

// P Function for Binary Semaphore
p( int s, int sem_id ) {
        struct sembuf sops;

        sops.sem_num = s;
        sops.sem_op = -1;
        sops.sem_flg = 0;
        if((semop(sem_id, &sops, 1)) == -1) {
                printf("%s", "'P' error\n");
        }
}

// V Function for Binary Semaphore
v( int s, int sem_id ) {
        struct sembuf sops;

        sops.sem_num = s;
        sops.sem_op = 1;
        sops.sem_flg = 0;
        if((semop(sem_id, &sops, 1)) == -1) {
                printf("%s","'V' error\n");
        }
}