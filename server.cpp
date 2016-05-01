/**************************************************
** File: server.cpp
** Author: Justin Siddon
** Description: This file provides the server interface to a socket communication system.
**************************************************/
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>       // Required for sockets to work.
#include <sys/wait.h>
#include <netinet/in.h>       // Required for internet domain access.

#include "error.c"
#include "message.c"


// Prototypes
int receiveMess(int, char*);
int sendMess(int, char*, char*);

/**************************************************
** Function: Main
** Description: Server style socket configurations based off example at: http://www.linuxhowtos.org/C_C++/socket.htm
** Parameters: command line argument port number to listen on.
** Returns: 1
**************************************************/
int main(int argc, char *argv[]) {
  int sockfd;               // Socket file descriptor.
  int newsockfd;            // Socket file descriptor for use once socket it bound to a port.
  int portno;               // Port we are accepting connections on.
  int pid;                  // PID of child processes.

  socklen_t clilen;         // Size of clients address, required by 'accept()'.
  char buffer[1000];         // Data buffer to read incoming messages into.
  char username[] = "localhost";

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

  int quit = 0;

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
      if(newchildsockfd < 0) {
        error("ERROR accepting connection from client", 1);
      }

      int messLen = 0;
      int contConn = 1;           // Be default we want to continue receiving message.


      while(1) {
        contConn = receiveMess(newchildsockfd, buffer);
        if(contConn == 0) {       // Client decided to close connection.
          break;
        }

        contConn = sendMess(newchildsockfd, buffer, username);
        if(quit == 1) {               // \quit was entered.
          close(newchildsockfd);
          close(newsockfd);
          kill(pid, SIGKILL);
          return 0;
        }

        // // Write back.
        // bzero(buffer, 1000);
        // printf("%s> ", username);
        // fgets(buffer, 1000, stdin);
        // quit = checkMessage(buffer);
        //
        // // \quit was entered.
        // if(quit == 1) {
        //   close(newchildsockfd);
        //   close(newsockfd);
        //   kill(pid, SIGKILL);
        //   return 0;
        // }
        //
        // packageMess(username, buffer);    // Package message to send.
        //
        // n = send(newchildsockfd, buffer, 1000, 0);
        // if(n < 0) {
        //   error("ERROR: error sending back to client.", 1);
        // }

      }

      exit(0);
    } else {                    // PARENT
      waitpid(0, &status, WNOHANG);
      close(newsockfd);
    }
  } /* End of while loop */

  close(sockfd);
  return 0;

}



int receiveMess(int socket, char* buff) {
  int n = recv(socket, buff, 1000, 0);       // Get message from client.
  if(n < 0) {
    error("ERROR: error reading from server", 1);

  } else if(n == 0) {
    printf("Client Connection Closed.\n");
    return 0;

  }
  printf("%s\n", buff);
  return 1;
}


int sendMess(int socket, char* buff, char* usr) {
  // Write back.
  bzero(buff, 1000);
  printf("%s> ", usr);
  fgets(buff, 1000, stdin);
  int quit = checkMessage(buff);

  // \quit was entered.
  if(quit == 1) {
    return 0;
  }

  packageMess(usr, buff);    // Package message to send.

  int n = send(socket, buff, 1000, 0);
  if(n < 0) {
    error("ERROR: error sending back to client.", 1);
  }

  return 1;
}
