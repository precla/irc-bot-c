#include "events.h"

/*
 * Check what kind of message was received.
 * For example: private message, message in the active channel, ircd message, ...
 */
void interpret_message(irc *ircs, tokarr msg) {
    char *response = NULL;
    if (!strcmp(msg[1], "PRIVMSG")) {
        if (!strcmp(msg[2], irc_mynick(ircs))) {
            private_message(ircs, msg);
        } else if (lsi_ut_ischan(ircs, msg[2])) {
            response = channel_message(msg);
            reply(ircs, response);
        }
    } else if (!strcmp(msg[1], "NOTICE")) {
        /*
         * rfc2812 #3.3.2 Never send an automatic reply to a notice
         * hence no call to reply(), only do output the notice msg we received
         */
        fprintf(stdout, notice_message(msg), "%s");
    }
    free(response);
}

void reply(irc *ircs, char *response) {
    if (response != NULL && !irc_printf(ircs, response)) {
        fprintf(stderr, irc_lasterror(ircs), "%s");
        exit(EXIT_FAILURE);
    }
}

char *channel_message(tokarr msg) {
    clock_t startTime = clock();
    char *responseMsg;

    if ((responseMsg = calloc(MAXLENGTH, sizeof(char))) == NULL){
        fprintf(stderr, "Error in malloc() for responseMsg. Download more ram?\n");
        return NULL;
    }

    fprintf(stdout, "Checking if the following parameter triggers any function: %s\n" , msg[3]);

    if (!strncmp(msg[3], "!sysinfo", 9)) {
        fprintf(stdout, "match for !sysinfo... getting data ready *beep boop*\n");
        struct sysinfo info;

        if (!sysinfo(&info)) {
            int ret = snprintf(responseMsg, MAXLENGTH,
                                "Uptime: %ld h %ld m %ld s, RAM: %lu / %lu MB (used/total)",
                                info.uptime / 3600,                             // uptime hrs
                                (info.uptime % 3600) / 60,                      // uptime min
                                info.uptime % 60,                               // uptime sec
                                (info.totalram - info.freeram) / (1024*1024),   // get in MB
                                info.totalram / (1024*1024));                   // get in MB

            if (ret < 0 || ret > MAXLENGTH){
                fprintf(stderr, "Error in snprintf for sysinfo: %d\n", ret);
            }
        } else {
            fprintf(stderr, "Error in sysinfo() call, errno: %d\n", errno);
        }
    } else if (msg[3]) {
        if (check_message_for_url(msg[3])) {
            return NULL;
        }

        /*
         * specialDomain - for special domains with extra features.
         * See enum special_domains for the list of those domains and
         * the corresponding index.
         * It holds the index of the matched domain.
         * if it stays '-1' it means no match for any of those
         * notice: no need to check for "https://", reason:
         * without that, it wouldn't have matched above in the
         * call to check_message_for_url.
         * if we have a match of a special domain, skip the
         * rest of the checks, goto should be faster than
         * comparing with if(specialdomain != -1 &&..)
         */
        short specialDomain = -1;

        // check if yt link, except channel links
        if (search_special_domains(msg[3], "(www.)?youtu(.)?be(.com)?[^[:space:]]+") == 0){
            specialDomain = 0;
        } else if (search_special_domains(msg[3], "(www.)?imdb.com[[:punct:]]title[^[:space:]]+") == 0){
            specialDomain = 1;
        }

        responseMsg = grab_url_data(msg[3], specialDomain);
    }
    if (responseMsg) {
        fprintf(stderr, "\nTime to execute succesfully: %f seconds.\n", (clock() - startTime)/(double)CLOCKS_PER_SEC );
    }
    return responseMsg;
}

/*
 * nickserv auth
 */
char *notice_message(tokarr msg) {
    char reply[MAXLENGTH * 2];

    // TODO: strcasecmp / strcmpi / stricmp?
    if (strcmp(msg[2], "nickserv")) {
        return NULL;
    }

    if (strstr(msg[2], "This nick is not registered") == msg[2]) {
        // sprintf(buf, "REGISTER %s NOMAIL", ucfg.nickservPassword);
        strncpy(reply, "nickserv", MAXLENGTH * 2);
    } else if (strstr(msg[2], "This nickname is registered and protected") == msg[2]) {
        // sprintf(buf, "IDENTIFY %s", ucfg.nickservPassword);
        strncpy(reply, "NICKSERV IDENTIFY", MAXLENGTH * 2);
        strncat(reply " %s", ucfg.nickservPassword);
    } else if (strstr(msg[2], "Password accepted - you are now recognized") == msg[2]) {
        fprintf(stdout, "Nickserv authentication succeed.");
    }

    return NULL;
}

void private_message(irc *ircs, tokarr msg) {
    char *response = calloc(MAXLENGTH, sizeof(char));
    char *nickToReply = calloc(MAXLENGTH, sizeof(char));

    if (!(response || nickToReply)) {
        fprintf(stdout, "error in calloc within private_message\n");
        return;
    }

    lsi_ut_ident2nick(nickToReply, MAXLENGTH - 1, msg[0]);

    strcpy(response, "PRIVMSG ");
    strncat(response, nickToReply, MAXLENGTH - strlen(nickToReply) - 13);

    if (!strcmp(msg[3], "!h")) {
        /*
         * Find the starting position of the nick and move by
         * nick+1 chars, so it points to the end of that string.
         * helpText - use one space as the first char of string
         */
        char *replyTextStartPos = strstr(response, nickToReply) + strlen(nickToReply);
        char *helpText[HELPTEXTLINES] = {" !sysinfo\n",
                                        " the end.\n"};

        for (unsigned int i = 0; i < HELPTEXTLINES; ++i) {
            memcpy(replyTextStartPos, helpText[i], strlen(helpText[i]));
            reply(ircs, response);
        }
    } else {
        strncat(response, " !h for help", 13);
        reply(ircs, response);
    }

    free(response);
    free(nickToReply);
    return;
}
