#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
// The libraries above are the same for server.
#include <netdb.h>


void sendFile(char *file, int newsockfd) {
  // Open plain text file.
  FILE *plaintext;
  plaintext = fopen(file, "r");
  char buffer[256];
  char message[1000];
  int n = 0;

  // First send total file length to the server so it knows how many characters to wait for.
  // fseek(plaintext, 0L, SEEK_END);
  char *noNewLine;
  int totalSize = 0; //= ftell(plaintext);                   // Get total length.
  while(fgets(buffer, 255, plaintext) != NULL) {
    noNewLine = strtok(buffer, "\n");
    totalSize = totalSize + strlen(noNewLine);
  }

  totalSize = htonl(totalSize);   // Convert to netbyte order.
  //printf("total size sending a: %d\n", ntohl(totalSize));

  fseek(plaintext, 0L, SEEK_SET);                     // Reset location pointer in file to beginning.

  // Send total file size to server.
  n = write(newsockfd, (char*)&totalSize, sizeof(totalSize));     // Send total lenght to server.
  if(n < 0) error("ERROR sending total message size", 1);


  int completeRec = 0;
  // Send plaintext file to server.
  int msgLen = 0;
  bzero(buffer, 256);
  while(fgets(buffer, 255, plaintext) != NULL) {      // Read lines from plaintext file into buffer.

    noNewLine = strtok(buffer, "\n");                 // Strip off newline character.

    strcpy(message, noNewLine);                       // Copy the text into message.
    msgLen = (int)strlen(message);                    // Get string length of the buffer so server knows how much text to expect.
    int tmp = htonl(msgLen);                          // convert to network byte order.

    n = write(newsockfd, (char*)&tmp, sizeof(tmp));   // Write message length to server.
    if(n < 0) error("ERROR sending length", 1);

    n = write(newsockfd, message, msgLen);            // Write message to server.
    if(n < 0) error("ERROR sedngin message", 1);

    // Reset buffers to 0.
    bzero(message, 1000);
    bzero(buffer, 256);
  }

}

void sendText(char *text, int newsockfd) {
  char buffer[256];
  char message[1000];
  int n = 0;
  int totalSize = strlen(text);                         // Get total text length.

  totalSize = htonl(totalSize);   // Convert to netbyte order.


  // Send total text size to server.
  n = write(newsockfd, (char*)&totalSize, sizeof(totalSize));     // Send total lenght to server.
  if(n < 0) error("ERROR sending total message size", 1);


  int completeRec = 0;
  // Send text.
  int msgLen = 0;
  bzero(buffer, 256);
  //while(fgets(buffer, 255, plaintext) != NULL) {      // Read lines from plaintext file into buffer.

    //noNewLine = strtok(buffer, "\n");                 // Strip off newline character.

    strcpy(message, text);                       // Copy the text into message.
    msgLen = (int)strlen(message);                    // Get string length of the buffer so server knows how much text to expect.
    int tmp = htonl(msgLen);                          // convert to network byte order.

    n = write(newsockfd, (char*)&tmp, sizeof(tmp));   // Write message length to server.
    if(n < 0) error("ERROR sending length", 1);

    n = write(newsockfd, message, msgLen);            // Write message to server.
    if(n < 0) error("ERROR sedngin message", 1);

    // Reset buffers to 0.
    bzero(message, 1000);
    bzero(buffer, 256);
  //}

}



char* receiveMessage(int newchildsockfd) {
  // First message read is to figure out the total message length that is coming in. (all of the streams).
  int totalMessLen;       // Hold total message length.
  int n;
  char buffer[1000];


  n = read(newchildsockfd, (char*)&totalMessLen, sizeof(totalMessLen));     // Read total file length.
  totalMessLen = ntohl(totalMessLen);                                       // Convert to int from netbyte order.

  // printf("Total Message Length: %d\n", totalMessLen);
  if(n < 0) error("ERROR Reading total message length from client", 1);

  char *entireMessage = (char*) malloc(totalMessLen);                   // Create space to store entire message. (+1 to end with newline).

  int bufLen = 0;
  int currPos = 0;

  // Get all text from client.
  while(totalMessLen > 0) {

    n = read(newchildsockfd, (char*)&bufLen, sizeof(bufLen));       // First lets get the total message length that is going to be sent next.
    if(n < 0) error("ERROR reading length", 1);

    bufLen = ntohl(bufLen);                     // Convert sent bufLen to normal int. (type cast happens automatically)

    // Read actual message.
    n = read(newchildsockfd, buffer, bufLen);
    if(n < 0) error("ERROR reading message", 1);

    strcpy(&entireMessage[currPos], buffer);
    currPos = currPos + bufLen;
    totalMessLen = totalMessLen - bufLen;
    bzero(buffer, 1000);                        // Reset buffer to 0.
  }

  return entireMessage;
}
