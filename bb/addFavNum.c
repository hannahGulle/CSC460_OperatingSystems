// Hannah Gulle
// Luigi's List
// Due 02/18/2019
// Changes Shared Memory Favorite Num for Hannah Gulle

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

        int shmid, faveNum;
        struct bbStruct *shmem;
        FILE *fopen(), *fp;

        // Verify Favorite Num Input at Command Line
        if( argc == 1 ) {
                printf( "Single Integer Input Required: Favorite Number\n" );
                return 0;
        }

        // Set the Favorite Num
        faveNum = atoi( argv[1] );

        // Open BBID.txt for reading of the shared memory id
        if( (fp = fopen("/tmp/allen/csc460/bb/BBID.txt", "r")) == NULL ) {
                printf( "Error Opening File\n" );
                return 0;
        }

        // Retrieve the shared memory ID
        fscanf( fp, "%i", &shmid );
        // Close BBID.txt
        fclose( fp );

        // Retrieve the class Bulletin board Structs at the shmem
        shmem = ( struct bbStruct* ) shmat( shmid, NULL, SHM_RND );
        // Set to Hannah Gulle's Bulletin Board Struct
        shmem = shmem + 6;

        // Set Hannah Gulle's Favorite Number in the shared memory struct
        // to the number received at the command line
        shmem->favNum = faveNum;

        return 0;
}