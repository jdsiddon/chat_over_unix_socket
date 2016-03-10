void error(const char *msg, int value) {
  perror(msg);
  exit(value);
}
