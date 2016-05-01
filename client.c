/**************************************************
** File: server.cpp
** Author: Justin Siddon
** Description: This file provides the server interface to a socket communication system.
**************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
// The libraries above are the same for server.
#include <netdb.h>          // This is different, it defines the structure hostent.

// Custom libraries.
#include "error.c"
#include "message.c"

void sendMess(int, char*, char*);
int receiveMess(int, char*);


/**************************************************
** Function: Main
** Description: Client style socket configuration, based off example at: http://www.linuxhowtos.org/C_C++/socket.htm
**   and http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
** Parameters: command line argument[1] hostname, arg[2] port number to connect.
** Returns: 1
**************************************************/
int main(int argc, char *argv[]) {
  int sockfd;             // Socket file descriptor.
  int newsockfd;          // Socket to use for new connection to server on port assigned by server.
  int portno;             // Master port number, port communication begins on.

  int n;                  // Integer to record how many characters were read/write to server.

  struct sockaddr_in serv_addr;   // Address of server.
  struct hostent *server;         // Pointer to host struct type.

  char buffer[1000];              // Buffer to read data into.
  char message[1000];             // Message to send to server.
  char username[11];


  // Check correct command line arguments provided.
  if(argc < 3) {
    error("usage: [hostname] [port]\n", 1);            // Provide hostname and port.
  }

  portno = atoi(argv[2]);  // Convert passed port number to integer.

  // Prompt for user handle.
  printf("Enter username: ");
  fgets(username, 10, stdin);
  // Clean newline from username.
  username[strcspn(username, "\n")] = 0;        // Change newline character to 0.


  // Set up socket.
  sockfd = socket(AF_INET, SOCK_STREAM, 0);     // Set up socket, unix domain, stream type, TCP connection.
  if(sockfd < 0)           // If socket = -1, wasn't successful.
    error("ERROR opening socket", 1);

  server = gethostbyname(argv[1]);      // Gets information about server, in this case its localhost.
  if(server == NULL) {
    fprintf(stderr, "ERROR no such host\n");
    exit(0);
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));      // Set serv_addr struct to 0's.

  // Set up server address.
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);  // Copy address returned by gethostname to serv_addr struct.
  serv_addr.sin_port = htons(portno);         // Set up port number.

  // Connect to server!
  if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR connecting", 1);

  // Connection successful, read new port number server sent.
  bzero(buffer, 1000);
  n = read(sockfd, buffer, 999);
  printf("new port: %s\n", buffer);
  close(sockfd);      // Close old connection.

  // Create new socket for communication.
  newsockfd = socket(AF_INET, SOCK_STREAM, 0);     // Set up socket, unix domain, stream type, TCP connection.

  // Start new connection on new port.
  portno = atoi(buffer);         // Convert new port number to integer.
  serv_addr.sin_port = htons(portno);     // Set server port on serv_addr struct.

  // Reconnect to server on new port.
  if(connect(newsockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR connecting on new port", 1);


  int quitProg = 0;
  int contConn = 1;       // Continue connection be default.

  while(1) {
    bzero(buffer, 1000);                                // Clean input buffer.
    prompt(username, buffer);                           // Get user message.

    quitProg = checkMessage(buffer);
    if(quitProg == 1) {
      break;

    } else {

      sendMess(newsockfd, buffer, username);

      bzero(buffer, 1000);

      contConn = receiveMess(newsockfd, buffer);
      if(contConn == 0) {       // Connection with server dropped.
        break;
      }

    }
  }

  close(newsockfd);
  return 0;
}


/**************************************************
** Function: receiveMess
** Description: Receives a message from connected server.
** Parameters: int socket - socket identifier, char buff - buffer to store message from server.
** Returns: 1 if server is still active, 0 if server is no longer active.
**************************************************/
int receiveMess(int socket, char* buff) {
  // response
  int n = recv(socket, buff, 1000, 0);
  if(n < 0) {
    error("ERROR: error reading from server", 1);

  } else if(n == 0) {
    printf("Server Connection Closed.\n");
    return 0;

  }

  printf("%s\n", buff);   // Print message from server.
  return 1;

}

/**************************************************
** Function: sendMess
** Description: Packages a message then sends a message to connected server.
** Parameters:
**  int socket - socket identifier,
**  char buff - buffer to store message from client
**  char usr - client user name.
** Returns: Nothing.
**************************************************/
void sendMess(int socket, char* buff, char* usr) {
  packageMess(usr, buff);

  int n = send(socket, buff, 1000, 0);            // Send message
  if(n < 0) {
    error("ERROR: sending to server", 1);
  }

}
