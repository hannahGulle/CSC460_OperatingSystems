// Hannah Gulle
// Homework 4 - Ice Climbers
// Compile: cc iceClimbers.c
// Run: a.out #
// Requires command line integer input between [1,26]
// Outputs the parent and child PID from forking N times
// 5 times to the screen.
// May be somewhat slow in printing (be patient).
// 01.23.2019

#include<stdio.h>

int main( int argc, char **argv ) {

        // Verify an input has been received
        if (argc != 2) {
                printf("Input required: a number in [1,26]\n");
                return 0;
        }

        // Retrieve the input and verify it is within range
        int N = atoi( argv[1] );
        if ((N<1) || (N>26)) {
                printf("Input required: a number in [1,26]\n");
                return 0;
        }

        // Initialize the Alphabet character array
        // and outputLetters character array
        char alphabet[26] = "abcdefghijklmnopqrstuvwxyz";
        char outputLetters[N];

        // Make a character subarray of only the letters needed for output
        int i,j,k;
        for( i=0; i<N; i++ ){
                outputLetters[i] = alphabet[i];
        }

        // The letterIndex keeps track of which outputLetter character to print
        int letterIndex;

        // Begin forking the appropriate number of times
        if( fork() != 0 ) {//Parent
                // Repeat the parent 5 times like abc,abc,abc,abc,abc
                for(j=0;j<5;j++){
                        printf("%c: %d\n", outputLetters[0], getpid());

                        // Sleep N seconds to account for the N prints of the child PIDs
                        sleep(N);
                }
        }
        else {
                // LetterIndex starts at 1 since the parent has already been printed
                letterIndex=1;
                // Because the outputLetters need to be printed 5 times for each set of N
                // outputLetters, loop through 5N times
                for( i=0; i < (N)*5; i++ ){
                        // Reset the letterIndex when it equals 5 to 1 (accounting for parent
                        // PID previously printed); otherwise, print the child
                        if( letterIndex != (N)){
                                // Child processes are tabbed for visual distinction
                                printf("\t%c: %d\n", outputLetters[letterIndex], getpid());
                                letterIndex++;
                        }
                        else{
                                letterIndex=1;
                        }
                        // Sleep for one second to account for printing of the child
                        sleep(1);
                }
        }
        return 0;
}