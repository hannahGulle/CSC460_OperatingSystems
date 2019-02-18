// Hannah Gulle
// Luigi's List
// Due 02/18/2019
// Changes Shared Memory Favorite Food for Hannah Gulle

#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>

struct bbStruct {
        int id;
        char name[20];
        int favNum;
        char favFood[30];
};

int main( int argc, char** argv ) {

        int shmid;
        struct bbStruct *shmem;
        FILE *fopen(), *fp;

        // Verify Favorite Food is Input from Command Line
        if( argc == 1 ) {
                printf( "Single String Input Required: Favorite Food\n" );
                return 0;
        }

        // Retrieve favorite food from first command line argument
        char* faveFood = argv[1];

        // Open the Bulletin Board shared memory ID text file to read
        if( (fp = fopen("/tmp/allen/csc460/bb/BBID.txt", "r")) == NULL ) {
                printf( "Error Opening File\n" );
                return 0;
        }

        // Retrieve the shared memory ID from the BBID.txt
        fscanf( fp, "%i", &shmid );
        // Close the BBID.txt file
        fclose( fp );

        // Retrieve the class Bulletin board Structs at the shmem
        shmem = ( struct bbStruct* ) shmat( shmid, NULL, SHM_RND );
        // Set to Hannah Gulle's Bulletin Board Struct
        shmem = shmem + 6;

        // Truncate Foods larger than 30 characters
        if( strlen( faveFood ) > 30 ) {
                strncpy( shmem->favFood, faveFood, 30 );
        }
        else {
                strcpy( shmem->favFood, faveFood );
        }

        return 0;
}