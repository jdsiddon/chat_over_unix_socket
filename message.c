/**************************************************
** File: message.c
** Description: This group of functions is used to provide
**  consistent interface to server and clients.
**************************************************/


/**************************************************
** Function: checkMessage
** Description: Checks message to see if user wrote quit string.
**  Provides interface to exapand additional options on interface.
** Parameters: char buffer - text user typed into console.
** Returns: 1 if user wants to quit, 0 if they don't want to quit.
**************************************************/
int checkMessage(char *buffer) {
  char quitMess[] = "\\quit";
  char *option;                         // Pointer to message.

  option = strtok(buffer, "\n");        // Strip newline off to check if it matches '\quit'.

  if(strcmp(option, quitMess) == 0) {
    return 1;
  }
  return 0;
}

/**************************************************
** Function: packageMess
** Description: Creates message of format [username]> [message]
** Parameters:
**  char username - user message is from.
**  char buffer - text user typed into console.
** Returns: nothing.
**************************************************/
void packageMess(char *username, char *buffer) {
  char pkgMess[1000];

  bzero(pkgMess, 1000);                 // Initialize buffer to zero.
  strcat(pkgMess, username);           // Copy username into the buffer to send.
  strcat(pkgMess, "> ");
  strcat(pkgMess, buffer);             // Copy message to packaged message.
  bzero(buffer, 1000);
  strcpy(buffer, pkgMess);             // Copy package back to original buffer that will be sent.

  return;
}

/**************************************************
** Function: prompt
** Description: Prompts user for input into passed buffer.
** Parameters:
**  char username - user message is from.
**  char buffer - text user wants to send.
** Returns: nothing.
**************************************************/
void prompt(char *username, char *buffer) {
  printf("%s> ", username);
  fgets(buffer, 999, stdin);         // Get message.
  return;
}
