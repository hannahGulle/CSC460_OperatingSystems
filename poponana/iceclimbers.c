// Hannah Gulle
// CSC 460 - Operating Systems
// 02/03/2019
// Popo and Nana Assignment
// Due 02/06/2019

// To Compile: cc iceclimbers.c
// To Run: a.out #
// Single required input: value within [1,100]

#include <stdio.h>
#include <string.h>

#define true (1)
#define false !(true)

int main( int argc, char** argv ) {


        // Exits program if a single command line argument is not given
        if ( argc != 2 ) {
                printf("Invalid Inputs; a number [1,100] is required\n");
                return 0;
        }

        // Exits program if the command line argument is not within bounds
        int N = atoi( argv[1] );
        if (( N < 1 ) || ( N > 100 )) {
                printf("Invalid Input; a number [1,100] is required\n");
                return 0;
        }

        // Initialize file pointer and pointer to file open function
        FILE *filePtr, *fopen();

        // Open File to Write
        if ( ( filePtr = fopen( "synchronize", "w" )) == NULL ) {
                printf( "Error Writing to File\n" );
                return 0;
        }

        // Write '1' to synchronize file
        fprintf( filePtr, "%i", 1 );

        // Close synchronize file
        fclose( filePtr );

        int i, utensil, curr, next;
        int isPopo = true;

        // Holds the current and next process numbers
        curr = 1;
        next = 1;

        // Holds the process number read from the synchronize file
        int read;

        // Print 2N processes 5 times = 10N-1
        for ( i = 0; i < (5*2*N-1); i++ ) {

                // Fork the process
                utensil = fork();

                // Break if parent
                if ( utensil != 0 ) break;

                // Alternating Popo/Nana
                isPopo = !(isPopo);

                curr = next;
                if ( !isPopo ) next++;

                // Process number [1,N] for both Popo and Nana
                if ( next > N ) {
                        next = 1;
                }
        }

        read = N;
        while ( curr != read ) {
                if ( (filePtr = fopen( "synchronize", "r" )) == NULL ) {
                        printf( "Error Reading file" );
                        return 0;
                }
                fscanf( filePtr, "%i", &read );
                fclose( filePtr );
        }

        if ( isPopo ) {
                printf( "Popo%i\n", curr );
        }
        else {
                printf( "Nana%i\n", curr );
        }

        if (( filePtr = fopen( "synchronize", "w" )) == NULL ) {
                printf( "Error Writing File" );
                return 0;
        }

        fprintf( filePtr, "%i", next );
        fclose( filePtr );

        sleep(1);
        return 0;
}