#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
// The libraries above are the same for server.
#include <netdb.h>





/**************************************************
** Function: sendText
** Description: This method sends text to a socket.
** Parameters: char text - text to send.
**  int newsockfd, socket to send text to.
** Returns: nothing
**************************************************/
// void prompt(char *username, char *buffer) {
//   printf("%s> ", username);
//   fgets(buffer, 999, stdin);         // Get message.
//   return;
// }

// void packageMess(char *username, char *buffer) {
//   char pkgMess[1000];
//
//   strcat(pkgMess, username);           // Copy username into the buffer to send.
//   strcat(pkgMess, "> ");
//   strcat(pkgMess, buffer);
//   bzero(buffer, 1000);
//   strcpy(buffer, pkgMess);
//   return;
// }

/**************************************************
** Function: checkMessage
** Description: Checks message to see if user wrote quit string.
**  Provides interface to exapand additional options on interface.
** Parameters: char buffer - text user typed into console.
** Returns: 1 if user wants to quit, 0 if they don't want to quit.
**************************************************/
// int checkMessage(char *buffer) {
//   char quitMess[] = "\\quit";
//   char *option;                         // Pointer to message.
//
//   option = strtok(buffer, "\n");        // Strip newline off to check if it matches '\quit'.
//
//   if(strcmp(option, quitMess) == 0) {   // User wants to quit.
//     printf("Closing connection\n");
//     return 1;
//   }
//   return 0;
// }


//
int sendall(int socket, char *buffer, int *length) {
  int n = 0;                // Number of bytes sent.
  int totalSent = 0;        // Total number of bytes sent.
  int left = *length;       // Total number of bytes left to send

  while(totalSent < *length) {
    n = send(socket, buffer+totalSent, left, 0);        // Write to socket, buffer at position left off, number of bytes left to send.
    if(n < 0) {
      error("ERROR sending!", 1);
    }

    totalSent+=n;
    left-=n;
  }

  *length = totalSent;

  return n > 0 ? 0 : -1;      // If n < 0 then we had an error send back -1, otherwise 0.
}
