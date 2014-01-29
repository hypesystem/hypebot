#include "ircparser.h"

//TODO: Should no longer use \r for EOS - \0 suffices

/* Reads from the pointer ptr until the first whitespace, replaces whitespace with \0, sets ptr to beginning of next word, returns original ptr. */
char *first_word(char **ptr) {
	char *word_start = *ptr;

	while(**ptr != ' ') {
		if(**ptr == '\r') {
			*ptr = NULL;
			return word_start;
		}
		(*ptr)++;
	}
	**ptr = '\0';
	(*ptr)++;

	return word_start;
}

/* Reads the rest of the string as arguments. Returns a pointer to array of argument strings, sets ptrs value to null and sets argc to the number of arguments. */
char **read_args(char **ptr, int *argc) {
	char *word = *ptr;
	char **words = malloc(sizeof(ptr));
	int wordcount;

	for(wordcount = 0; wordcount < 15; wordcount++) {
		//If : is found, the rest is one "word"
		if(**ptr == ':') {
			words[wordcount] = *ptr + 1;
			while(**ptr != '\r') (*ptr)++;
			**ptr = '\0';
			*argc = wordcount + 1;
			*ptr = NULL;
			return words;
		}

		while(**ptr != ' ') {
			if(**ptr == '\r') {
				words[wordcount] = word;
				*argc = wordcount + 1;
				*ptr = NULL;
				return words;
			}
			(*ptr)++;
		}
		**ptr = '\0';
		words[wordcount] = word;
		(*ptr)++;
	}

	*ptr = NULL;
	*argc = wordcount + 1;
	return words;
}

/* Returns 0 if any character in the strings first n characters is NOT a digit. Otherwise returns 1. */
int strndigit(char *str, int n) {
	int i;
	for(i = 0; i < n; i++)
		if(isdigit(str[i])) return 0;
	return 1;
}

//TODO: What happens if ptr becomes NULL? Hand the case, throw an error, save a racist.

/* Parses a string into the provided IrcMessage struct. Returns 1 on success, 0 on failure. */
int irc_parse_string(char *msgstr, int len, IrcMessage *msg) {
	//Copy msgstr into the full field
	char *fullcpy = malloc(sizeof(char) * strlen(msgstr));
	msg->full = strcpy(fullcpy, msgstr);

	//Set up all other fields
	msg->prefix = NULL;
	msg->command = IRC_NONE;
	msg->argc = 0;
	msg->argv = NULL;

	//Make parser pointer
	char *ptr = msgstr;

	//If first char is : we have a prefix
	if(msgstr[0] == ':') {
		ptr++;
		msg->prefix = first_word(&ptr);
	}

	if(ptr == NULL) return 1;
	
	//First word after optional prefix is command
	char *commandstr = first_word(&ptr);

	//Convert command string to command const
	int cmdlen = strlen(commandstr);
	if(cmdlen <= 3 && strndigit(commandstr,cmdlen)) {
		msg->command = IRC_NUMERIC;
	} else if(!strncmp(commandstr,"PING",4)) {
		msg->command = IRC_PING;
	} else if(!strncmp(commandstr,"NOTICE",6) || !strncmp(commandstr,"PRIVMSG",7)) {
		msg->command = IRC_MESSAGE;
	} else if(!strncmp(commandstr,"ERROR",5)) {
		msg->command = IRC_ERROR;
	} else if(!strncmp(commandstr,"JOIN",4)) {
		msg->command = IRC_JOIN;
	} else {
		msg->command = IRC_OTHER;
	}

	if(ptr == NULL) return 1;

	//Read additional arguments
	msg->argv = read_args(&ptr, &(msg->argc));

	return 1;
}
