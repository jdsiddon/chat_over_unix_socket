#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
// The libraries above are the same for server.
#include <netdb.h>          // This is different, it defines the structure hostent.

#include "error.c"
#include "transmission.c"


void getMessage(char *username, char *buffer) {
  printf("%s> ", username);
  fgets(buffer, 999, stdin);          // Get message.
  return;
}

/**
  argv[1] = hostname
  argv[2] = portno
*/
int main(int argc, char *argv[]) {
  int sockfd;             // Socket file descriptor.
  int newsockfd;          // Socket to use for new connection to server on port assigned by server.
  int portno;             // Master port number, port communication begins on.

  int n;                  // Integer to record how many characters were read/write to server.

  struct sockaddr_in serv_addr;   // Address of server.
  struct hostent *server;         // Pointer to host struct type.

  char buffer[1000];       // Buffer to read data into.
  char message[1000];     // Message to send to server.
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


  // Connection successful, communicate.
  bzero(buffer, 1000);

  getMessage(username, buffer);
  printf("message: %s\n", buffer);


  n = write(newsockfd, buffer, 1000);

  n = read(newsockfd, buffer, 1000);

  if(n < 0) {
    error("ERROR: error reading from server", 1);
  }
  printf("Received: %s\n", buffer);

  close(newsockfd);





  return 0;

}