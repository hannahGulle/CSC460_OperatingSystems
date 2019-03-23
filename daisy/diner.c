// Hannah Gulle
// CSC 460 Daisy Project
// Due 03/22/2019

// Program should print "SUCCESS" before termination

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

void THINK( );
void EAT( );
void takeFork( int i, char* state, int utenID, int* clockMem );
void dropFork( int i, char* state, int utenID );
void test( int i, char* state, int utenID );
void p( int s, int sem_id );
void v( int s, int sem_id );

// Define the possible diner states
#define hungry          "HUNGRY "
#define eating          "EATING "
#define thinking        "THINKIN"
#define done            "DONE   "

int main( int argc, char **argv ) {

        // Iterating integers and shared/sem IDs
        int i, clockID, philID, utenID, procNum;

        char* state;            // Shared Memory char* to hold diner's state
        int* clockMem;          // Shared Memory int* to count seconds since start
        int numSems = 6;        // 5 philosophers and 1 clock

        // Assumed timer value of 100

        // Allocate the clock shared memory
        clockID = shmget( IPC_PRIVATE, sizeof( int ), 0777 );
        if( clockID == -1 ) {
                printf( "Error Getting Clock Shared Memory...\n" );
                return( -1 );
        }
        clockMem = shmat( clockID, NULL, SHM_RND );

        // Allocate the philosopher's state shared memory
        // 5 philosophers with a character state of at most 8 characters
        philID = shmget( IPC_PRIVATE, 5*sizeof( char )*8, 0777 );
        if( philID == -1 ) {
                printf( "Error Getting Philosophers' Shared Memory...\n" );
                return( -1 );
        }
        state = shmat( philID, NULL, SHM_RND );

        // Initialize all Philosopher's state to THINKING
        for( i = 0; i < 5; i++ ) {
                strncpy( (state + (8*i)), thinking, 8 );
        }

        // Allocate the utensil semaphore (forks!)
        utenID = semget( IPC_PRIVATE, numSems, 0777 );
        if( utenID == - 1 ) {
                printf( "Error Getting Utensil Semaphores...\n" );
                return( -1 );
        }

        // Initialize all utensil semaphores
        for( i = 0; i < 5; i++ ) {
                semctl( utenID, i, SETVAL, 0 );
        }

        // Initialize mutex semaphore
        semctl( utenID, 5, SETVAL, 1 );

        // Fork 5 processes for each philosopher
        // and 1 process for the clock
        procNum = 0;
        for( i = 0; i < 5; i++ ) {
                if( fork() != 0 ) break;

                // Increment process number [0,5]
                procNum++;
        }

        // seed the random timer for thinking and eating delays
        srand( getpid() );

        // Initialize the clock at zero
        *clockMem = 0;

        // If the process is not the clock
        if( procNum != 5 ) {
                // and the clock has not surpassed the timer
                while( *clockMem < 100 ) {

                        // Delay for thinking time
                        THINK( );

                        // Pick up fork
                        takeFork( procNum, state, utenID, clockMem );

                        // Break if the clock has surpassed the timer
                        if( *clockMem > 100 ) break;

                        // Delay for eating time
                        EAT( );

                        // Break if the clock has surpassed the timer
                        if( *clockMem > 100 ) break;

                        // Put down fork
                        dropFork( procNum, state, utenID );
                }
                // After the clock has surpassed the timer, set the process' state to done
                strncpy( (state + (8*procNum)), done, 8 );
        }
        // If the process is the clock
        else {
                // Count time in seconds indefinitely
                while( 1 ) {
                        // Print the current time and state of all 5 processes in order [0,4]
                        printf( "%d:\t", *clockMem );
                        printf( "%.*s\t%.*s\t%.*s\t%.*s\t%.*s\n", 8, state, 8, state + 8, 8, state + 16, 8, state + 24, 8, state + 32 );
                        sleep( 1 );     // Sleep 1 second

                        // Increment the clock counter
                        (*clockMem)++;

                        // If the clock has surpassed the timer and all diners are in the done state then break the loop
                        if( (*clockMem >= 100) &&
                                ( strcmp( state, done ) == 0 ) &&
                                ( strcmp( state + 8, done) == 0 ) &&
                                ( strcmp( state + 16, done ) == 0 ) &&
                                ( strcmp( state + 24, done ) == 0 ) &&
                                ( strcmp( state + 32, done ) == 0 )
                        ) {
                                (*clockMem)++;
                                break;
                        }
                        // If the clcok has surpassed the timer and not all diners are in the done state then set the
                        // current process' state to done and break the loop
                        if( *clockMem >= 100 && (strcmp( state + 8*i, done ) != 0) ) {
                                (*clockMem)++;
                                strncpy( (state + (8*procNum)), done, 8 );
                                break;
                        }
                }
        }
        // If all else fails and the state STILL hasn't been set to done for the current process, then do so.
        strncpy( (state + (8*procNum)), done, 8 );

        // Increment the clock and print
        (*clockMem)++;
        printf( "%d:\t", *clockMem );
        printf( "%.*s\t%.*s\t%.*s\t%.*s\t%.*s\n", 8, state, 8, state + 8, 8, state + 16, 8, state + 24, 8, state + 32 );

        // If all prcesses are done, then removed shared memory and semaphores
        if( (strcmp( state, done ) == 0 ) &&
            (strcmp( state+8, done ) == 0 ) &&
            (strcmp( state+16, done ) == 0 ) &&
            (strcmp( state+24, done ) == 0 ) &&
            (strcmp( state+32, done ) == 0 ) ) {


                if( ( shmctl( philID, IPC_RMID, NULL ) ) == - 1 )
                        printf( "Error Removing Shared Memory\n" );
                else
                        printf( "Shared Memory Removed Successfully!\n" );

                if( ( shmctl( clockID, IPC_RMID, NULL ) ) == -1 )
                        printf( "Error Removing Shared Memory\n" );
                else
                        printf( "Shared Clock Removed Successfully!\n" );

                if( ( semctl( utenID, 0, IPC_RMID, 0 ) ) == - 1 )
                        printf( "Error Removing Semaphores\n" );
                else
                        printf( "Semaphores Removed Successfully!\n" );

                printf( "SUCCESS\n" );
        }

        return 0;
}

// Function to sleep for a random amount within the interval [5, 15]
// while in the thinking state
void THINK( ) {

        int randNum = rand()%10 + 5;
        sleep( randNum );
}

// Function to sleep for a random amount within the interval [1, 3]
// while in the eating state
void EAT( ) {

        int randNum = rand()%2 + 1;
        sleep( randNum );
}

// Take the fork initiates the hungry state and the possibility of eating
void takeFork( int i, char* state, int utenID, int* clockMem ) {

        p( 5, utenID );
        strncpy( (state + (8*i)), hungry, 8 );

        // If the clock has surpassed the timer exit the function
        if( *clockMem > 100 ) {
                v( 5, utenID );
                return;
        }
        test( i, state, utenID );
        v( 5, utenID );
        p( i, utenID );
}

// Drop the fork initiates the thinking state after eating
void dropFork( int i, char* state, int utenID ) {

        p( 5, utenID );
        strncpy( (state + (8*i)), thinking, 8 );
        test( ((i+4)%5), state, utenID );
        test( ((i+1)%5), state, utenID );
        v( 5, utenID );

}

// Function tests to see if the given philosopher (i)
// can eat
void test( int i, char* state, int utenID ) {

        if(   ( strcmp( (state + (8*i)), hungry ) == 0 )           &&
              ( strcmp( (state + (8*( (i+4)%5 ))), eating ) != 0 ) &&
              ( strcmp( (state + (8*( (i+1)%5 ))), eating ) != 0 ) ) {

                strncpy( (state + (8*i)), eating, 8 );
                v( i, utenID );
        }
}


// P Function for Semaphores
void p( int s, int sem_id ) {
        struct sembuf sops;

        sops.sem_num = s;
        sops.sem_op = -1;
        sops.sem_flg = 0;
        if((semop(sem_id, &sops, 1)) == -1) {
                printf("%s", "'P' error\n");
        }
}

// V Function for Semaphores
void v( int s, int sem_id ) {
         struct sembuf sops;

         sops.sem_num = s;
         sops.sem_op = 1;
         sops.sem_flg = 0;
         if((semop(sem_id, &sops, 1)) == -1) {
                printf("%s","'V' error\n");
         }
}