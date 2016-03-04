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
  int newsockfd;          // Socket to use for new connection to server on port assigned by server.
  int portno;             // Master port number, port communication begins on.

  int n;                  // Integer to record how many characters were read/write to server.

  struct sockaddr_in serv_addr;   // Address of server.
  struct hostent *server;         // Pointer to host struct type.

  char buffer[256];       // Buffer to read data into.
  char message[1000];     // Message to send to server.

  // Connection Set Up
  if(argc < 3) {
    fprintf(stderr, "usage %s hostname port\n", argv[0]);       // Print error if user didn't pass address and port when starting.
    exit(0);
  }

  portno = atoi(argv[2]);  // Convert passed port number to integer.

  sockfd = socket(AF_INET, SOCK_STREAM, 0);     // Set up socket, unix domain, stream type, TCP connection.
  if(sockfd < 0)           // If socket = -1, wasn't successful.
    error("ERROR opening socket");

  server = gethostbyname("localhost");      // Gets information about server, in this case its localhost.
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
  close(sockfd);      // Close old connection.

  // Create new socket for communication.
  newsockfd = socket(AF_INET, SOCK_STREAM, 0);     // Set up socket, unix domain, stream type, TCP connection.

  // Start new connection on new port.
  portno = atoi(buffer);         // Convert new port number to integer.
  serv_addr.sin_port = htons(portno);     // Set server port on serv_addr struct.

  // Reconnect to server on new port.
  if(connect(newsockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR connecting on new port");


  // Connection successful, communicate.
  bzero(buffer, 256);

  // Open plain text file.
  FILE *plaintext;
  plaintext = fopen(argv[1], "r");

  // First send total file length to the server so it knows how many characters to wait for.
  // fseek(plaintext, 0L, SEEK_END);
  char *noNewLine;
  int totalSize; //= ftell(plaintext);                   // Get total length.
  while(fgets(buffer, 255, plaintext) != NULL) {
    noNewLine = strtok(buffer, "\n");
    totalSize = totalSize + strlen(noNewLine);
  }

  totalSize = htonl(totalSize);   // COnvert to netbyte order.

  fseek(plaintext, 0L, SEEK_SET);                     // Reset location pointer in file to beginning.

  // Send total file size to server.
  n = write(newsockfd, (char*)&totalSize, sizeof(totalSize));     // Send total lenght to server.
  if(n < 0) error("ERROR sending total message size");


  int completeRec = 0;
  // Send plaintext file to server.
  int msgLen = 0;
  bzero(buffer, 256);
  while(fgets(buffer, 255, plaintext) != NULL) {      // Read lines from plaintext file into buffer.

    noNewLine = strtok(buffer, "\n");                 // Strip off newline character.

    strcpy(message, noNewLine);                   // starting at index 1. index 0 is a checksum value.
    msgLen = (int)strlen(message);                 // Get string length of the buffer so server knows how much text to expect.
    int tmp = htonl(msgLen);// convert to network byte order.
    printf("tmp: %d\n", ntohl(tmp));        // Debugging message to send.
    //printf("Message len: %d", (int)strlen(message));
    n = write(newsockfd, (char*)&tmp, sizeof(tmp));   // Write message to server.
    if(n < 0) error("ERROR sending length");

    n = write(newsockfd, message, msgLen);
    if(n < 0) error("ERROR sedngin message");
    bzero(message, 1000);                             // Reset message to 0.
    bzero(buffer, 256);
  }

  if(n < 0)
    error("ERROR writing to socket!");


  return 0;

}



void error(const char *msg) {
  perror(msg);
  exit(0);
}
