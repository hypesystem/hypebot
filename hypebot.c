#include "ircparser.h"

char *nick;
char *channels[4] = { "#halloftheking", "#itu", "#devirc", "#gaming" };
int num_channels = 4;
int pending_join = 1;

void on_any(IrcMessage *msg);
void on_connect(IrcMessage *msg);
void on_ping(IrcMessage *msg);
void on_join(IrcMessage *msg);
void on_message(IrcMessage *msg);
void on_other(IrcMessage *msg);

char *get_nick(char *prefix) {
	//Find separator.
	char *sep;
	if((sep = strchr(prefix,'!')) != NULL) {
		int nick_len = (int) (sep - prefix);
		char *nick = malloc(nick_len * sizeof(char));
		return strncpy(nick, prefix, nick_len);
	}
	//No separator found, prefix is nick.
	return prefix;
}

int main(int argc, char **argv) {
	char *host, *port;

	nick = "hypebot";
	host = "mrbech.net";
	port = "6667";

	irc_connect(nick, host, port);

	send_raw("USER %s 0 0 :%s\r\n", nick, nick);
    send_raw("NICK %s\r\n", nick);

	IrcMessage *msg = malloc(sizeof(IrcMessage));

	while(next_message(msg)) {
		//Print msg 
		printf(">> %s", msg->full);

		//Handle msg
		switch(msg->command) {
			case IRC_CONNECT:
				on_connect(msg);
				break;
			case IRC_PING:
				on_ping(msg);
				break;
			case IRC_JOIN:
				on_join(msg);
				break;
			case IRC_MESSAGE:
				on_message(msg);
				break;
			case IRC_OTHER:
				on_other(msg);
				break;
		}
		on_any(msg);
	}
	//something went wrong - we broke the loop! :(
	printf("Error: %s\n", msg->full);
}

/* Called when any message is received from server */
void on_any(IrcMessage *msg) {
	//TODO: Hmm?
}

/* Called when the bot connects to a server */
void on_connect(IrcMessage *msg) {
	//TODO: How can we tell?!
}

/* Called when the bot is pinged */
void on_ping(IrcMessage *msg) {
	//Pong back
	msg->full[1] = 'O';
	send_raw(msg->full);
}

/* Called when a user joins a channel the bot is monitoring */
void on_join(IrcMessage *msg) {
	char *usrnick = get_nick(msg->prefix);
	op(msg->argv[0], usrnick);

	//If join of self is registered.
	if(!strncmp(msg->prefix,nick,strlen(nick))) {
		printf("on join argv[0]: %s\n",msg->argv[0]);
		//Set topics
		if(!strncmp(msg->argv[0],"#gaming",7)) {
			topic(msg->argv[0],"VIDEYA GARMEZ");
		} else if(!strncmp(msg->argv[0],"#devirc",7)) {
			topic(msg->argv[0],"Improve IRC, fuck bitches.");
		} else if(!strncmp(msg->argv[0],"#halloftheking",14)) {
			topic(msg->argv[0],"hypesystem is king of the hill");
		}
	}
}

/* Called when a message is seend by the bot */
void on_message(IrcMessage *msg) {
	//Print message
    printf("Prefix: %s\nCommand: %d\nArgvs\n", msg->prefix, msg->command);
	int i;
	for(i = 0; i < msg->argc; i++)
		printf("%d: %s\n", i, msg->argv[i]);

	//TODO: Support for votekick/voteban operations; spam report?
}

/* Unknown message type received */
void on_other(IrcMessage *msg) {
	if(pending_join) {
		int i;
		for(i = 0; i < num_channels; i++) {
			join(channels[i]);
		}
		pending_join = 0;
	}
}

void on_numeric(IrcMessage *msg) {
	puts("^-- NUMERIC");
}