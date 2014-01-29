#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdarg.h>

#define IRC_OTHER -2
#define IRC_NONE -1
#define IRC_JOIN 0
#define IRC_PING 1
#define IRC_CONNECT 2
#define IRC_MESSAGE 3
#define IRC_NUMERIC 4
#define IRC_ERROR 666

typedef struct ircmessage {
  char *prefix;
  int command;
  int argc;
  char **argv;
  char *full;
} IrcMessage;
