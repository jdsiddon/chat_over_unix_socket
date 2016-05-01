/**************************************************
** File: error.c
** Author: Justin Siddon
** Description: This file provides a standard error interface for server and clients
**   to print and return errors, based off error function: http://www.linuxhowtos.org/C_C++/socket.htm.
**************************************************/


/**************************************************
** Function: error
** Description: Prints error to stdout and exits the program.
** Parameters:
**  char msg - Error message
**  int value - Error value, usually 1.
** Returns: Nothing.
**************************************************/
void error(const char *msg, int value) {
  perror(msg);
  exit(value);
}
