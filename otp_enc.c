#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
// The libraries above are the same for server.
#include <netdb.h>          // This is different, it defines the structure hostent.


void error(const char *msg);

int main(int argc, char *argv[]) {
  int sockfd;             // Socket file descriptor.
  int portno;             // Master port number, port communication begins on.

  int n;                  // Integer to record how many characters were read/write to server.

  struct sockaddr_in serv_addr;   // Address of server.
  struct hostent *server;         // Pointer to host struct type.

  char buffer[256];       // Buffer to read data into.

  // Connection Set Up
  if(argc < 3) {
    fprintf(stderr, "usage %s hostname port\n", argv[0]);       // Print error if user didn't pass address and port when starting.
    exit(0);
  }

  portno = atoi(argv[2]);  // Convert passed port number to integer.

  sockfd = socket(AF_INET, SOCK_STREAM, 0);     // Set up socket, unix domain, stream type, TCP connection.
  if(sockfd < 0)           // If socket = -1, wasn't successful.
    error("ERROR opening socket");

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
    error("ERROR connecting");

  // Connection successful, read port number server sent.
  bzero(buffer, 256);
  n = read(sockfd, buffer, 255);
  printf("%s", buffer);
  close(sockfd);
  return 0;

}



void error(const char *msg) {
  perror(msg);
  exit(0);
}
