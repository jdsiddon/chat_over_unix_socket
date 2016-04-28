#include <message.h>

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
