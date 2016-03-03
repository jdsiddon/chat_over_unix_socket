#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// keygen: This program creates a key file of specified length.
// The characters in the file generated will be any of the 27 allowed characters,
// generated using the standard UNIX randomization methods. Do not create spaces
// every five characters, as has been historically done. Note that you specifically
// do not have to do any fancy random number generation: we’re not looking for
// cryptographically secure random number generation!
//
// The syntax for keygen is as follows:
//
// keygen keylength
//
// Where keylength is the length of the key file in characters. keygen outputs to stdout.
// Here is an example run, which redirects stdout to a key file of 256 characters called “mykey”:
//
// $ keygen 256 > mykey

#define VALID_CHARS 27

void main(int argc, char *argv[]) {
  int i, randNum, keylength;
  char randChar;
  char *key;

  srand(time(NULL));          // Create random seed.

  if(argc < 2) {
    printf("ERROR format, keygen [keylength]\n");
    exit(0);
  }

  keylength = atoi(argv[1]);

  key = (char*) malloc (keylength+1);     // Allocate space for key.

  // Valid chars A=65 - Z=90 and (space) = 32
  for(i = 0; i < keylength; i++) {
    randNum = rand() % VALID_CHARS;
    randNum = 65 + randNum;

    if(randNum > 90) {
      randChar = 32;
    } else {
      randChar = randNum;
    }

    key[i] = randChar;
  }
  key[keylength] = '\n';        // Set last character to newline.

  printf("%s", key);

  // Deallocate key.
  free(key);
  key = 0;

}
