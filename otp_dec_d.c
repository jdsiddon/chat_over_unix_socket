/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>       // Required for sockets to work.
#include <netinet/in.h>       // Required for internet domain access.

#include "error.c"
#include "transmission.c"

char* decrypt(char *cipher, char *key);     // Back out the encryption!

// Server style socket configurations based off example at: http://www.linuxhowtos.org/C_C++/socket.htm
int main(int argc, char *argv[]) {
  int sockfd;               // Socket file descriptor.
  int newsockfd;            // Socket file descriptor for use once socket it bound to a port.
  int portno;               // Port we are accepting connections on.
  int pid;                  // PID of child processes.

  socklen_t clilen;         // Size of clients address, required by 'accept()'.
  char buffer[1000];         // Data buffer to read incoming messages into.

  struct sockaddr_in serv_addr;         // Address of the server (here).
  struct sockaddr_in cli_addr;          // Address of the client (who connects).

  int n;                    // Integer to hold number of characters read or write returns.
  int status;               // Status variable to communicate child process status.

  int portCounter = 0;     // Integer to increment port number by as clients connect.

  // Begin socket setup.

  // Check user passed in a port to set socket up on.
  if(argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    fflush(stderr);
    exit(1);
  }

  // Create new socket.
  sockfd = socket(AF_INET, SOCK_STREAM, 0);     // Socket uses unix domain, stream type socket, with TCP protocol.
  if(sockfd < 0)                                // -1 means socket created errored out.
    error("ERROR opening socket", 1);

  bzero((char *) &serv_addr, sizeof(serv_addr));  // Set serv_addr struct to all 0's.

  portno = atoi(argv[1]);                       // Convert user entered port number to integer.

  // Set up server address struct.
  serv_addr.sin_family = AF_INET;               // Set to unix domain.
  serv_addr.sin_port = htons(portno);           // Set port number to network byte order.
  serv_addr.sin_addr.s_addr = INADDR_ANY;       // Set server address to it's own IP address. (INADDR_ANY returns current machines IP).

  // Bind socket to server address.
  if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR binding", 1);

  listen(sockfd, 5);            // Listen on socket for connections.

  while(1) {                    // Loop forever waiting for connections.
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);         // Accept connection, make new socket for connection.

    portCounter = portCounter + 1;  // Increment port increase variable, so they can communicate on new port #.

    if(newsockfd < 0)           // If newsockfd is less than 0, the accept call failed.
      error("ERROR on accept", 1);

    // Connection was successful so fork a new process.
    pid = fork();
    if(pid < 0)
      error("ERROR forking process", 1);

    if(pid == 0) {              // CHILD
      close(sockfd);            // Close the old socket.
      char childPort[50];        // Character array to hold port number child should talk on.
      int childsockfd;          // New socket.
      int newchildsockfd;       // New communication between child process and client. Sock bound to new port.

      childsockfd = socket(AF_INET, SOCK_STREAM, 0);     // Socket uses unix domain, stream type socket, with TCP protocol.

      sprintf(childPort, "%d", (portno + portCounter));   // Convert port integer to string and store it in childPort.

      // Set up new port to talk on, port is 5 characters.
      write(newsockfd, childPort, 5);

      // Copy old serv address to new one.
      struct sockaddr_in child_serv_addr;         // Address of the server (here).

      // Copy over master server address information to child.
      memcpy(&child_serv_addr, &serv_addr, sizeof(serv_addr));

      // Set port number of child.
      child_serv_addr.sin_port = htons((portno + portCounter));           // Set new port number.

      // Bind socket to child server address.
      if(bind(childsockfd, (struct sockaddr *) &child_serv_addr, sizeof(child_serv_addr)) < 0)
        error("ERROR binding", 1);

      listen(childsockfd, 5);            // Listen on socket for connections.

      newchildsockfd = accept(childsockfd, (struct sockaddr *) &cli_addr, &clilen);         // Accept connection, make new socket for connection.
      if(newchildsockfd < 0)
        error("ERROR accepting connection from client", 1);



      char* cipher = receiveMessage(newchildsockfd);
      // printf("Back in main: %s\n", cipher);

      char* key = receiveMessage(newchildsockfd);
      // printf("key: %s\n", key);


      // INSERT CALL TO decrypt FUNCTION HERE.
      char* plaintext = decrypt(cipher, key);

      // Send plaintext text back to client.
      sendText(plaintext, newchildsockfd);



      // Free memory
      free(plaintext);
      plaintext = 0;

      free(key);
      key = 0;

      free(cipher);
      cipher = 0;


      exit(0);
    } else {                    // PARENT
      waitpid(0, &status, WNOHANG);

      printf("Child exit: %d\n", WEXITSTATUS(status));
      fflush(stdout);
      close(newsockfd);
    }
  } /* End of while loop */

  close(sockfd);
  return 0;

}

int mod(int a, int b) {
  if(a == 32) { // a is space
    a = 26;
  } else {
    a -= 65;
  }


  if(b == 32) {
    b = 26;       // b is space
  } else {
    b -= 65;
  }

  int sum = a - b;
  if(sum < 0)
    return sum + 27;

  return sum % 27;
}

char* decrypt(char *cipher, char *key) {
  int i = 0;
  cipher[strlen(cipher)] = '\0';  // Take off newline.

  char *plaintext = (char*) malloc(strlen(cipher));
  int cipherKey = 0;

  for(i = 0; i < strlen(cipher); i++) {


    plaintext[i] = mod(cipher[i], key[i]);// Decrypt character, using 27 because we are including (space) char.
    // printf("int: %d\n", cipher[i]);
    plaintext[i] = plaintext[i] + 65;

    if(plaintext[i] == 91) {                   // When a character is outside the bounds of capital letters, it is a space char.
      plaintext[i] = 32;
    }

  }
  // plaintext[strlen(plaintext)] = '\n';  // set last to newline.

  return plaintext;
}
