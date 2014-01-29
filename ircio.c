#include "ircparser.h"

#define IRCIO_BUFFER_SIZE 512

int conn;

int i, j, l, sl, o = -1, start, wordcount;
char buf[IRCIO_BUFFER_SIZE];

struct addrinfo hints, *res;

void irc_connect(char *nick, char *host, char *port);
int next_message(IrcMessage *msg);
void send_raw(char *fmt, ...);
void topic(char *channel, char *topic);
void mode(char *channel, char *mode);
void user_mode(char *channel, char *user, char *mode);
void op(char *channel, char *user);
void join(char *channel);

void irc_connect(char *nick, char *host, char *port) {
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(host, port, &hints, &res);
	conn = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	connect(conn, res->ai_addr, res->ai_addrlen);
}

//TODO: error handling by returning integers

/* Waits for the next message from the server, then parses it into the supplied IrcMessage. Returns 1 on success, 0 on failure. */
int next_message(IrcMessage *msg) {
	sl = read(conn, buf, IRCIO_BUFFER_SIZE);
	if(sl != -1) {
        for(i = 0; i < sl; i++) {
            if(buf[i] == '\n' && buf[i - 1] == '\r') {
                buf[i] = '\0';

				return irc_parse_string(buf,i,msg);
            }
        }
    }
	msg->command = IRC_ERROR;
	msg->full = "Reading from server failed.";

	return 0;
}

void send_raw(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(sbuf, 512, fmt, ap);
    va_end(ap);
    printf("<< %s", sbuf);
    write(conn, sbuf, strlen(sbuf));
}

void topic(char *channel, char *topic) {
    send_raw("TOPIC %s :%s\r\n",channel,topic);
}

void mode(char *channel, char *mode) {
	send_raw("MODE %s %s\r\n",channel);
}

void user_mode(char *channel, char *user, char *mode) {
	send_raw("MODE %s %s %s\r\n", channel, mode, user);
}

void op(char *channel, char *user) {
    user_mode(channel, user, "+o");
}

void join(char *channel) {
	send_raw("JOIN %s\r\n",channel);
}
