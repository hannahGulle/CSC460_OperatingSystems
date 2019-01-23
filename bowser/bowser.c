// Hannah Gulle
// CSC 460
// Homework 3 - Bowser's Pitch Fork
// 01/23/2019

#include<stdio.h>

int main( int argc, char **argv ) {

        int N = atoi( argv[1] );
        if ( (N < 0) || (N > 5) ) {
                printf( "The Number of Generations must be between [0,5].\n" );
                return 0;
        }

        int treeID;
        int i,j;

        i = 0;
        while ( i < N ) {
                treeID = fork();
                for ( j=1; j < i ; j++ ) {

                        treeID = fork();
                        if ( treeID == 0 ) break;
                }
                if ( treeID == 0 ) break;
                i++;
        }


        printf( "%d\t%d\t%d\n", treeID, getpid(), getppid() );
        sleep( 1 );

        return 0;
}