#include "events.h"

/*
 * Check what kind of message was received.
 * For example: private message, message in the active channel, ircd message, ...
 */
void interpret_message(irc *ircs, tokarr msg) {
    char *response = NULL;
    if (!strcmp(msg[1], "PRIVMSG")) {

        // TODO: PRIVMSG and chan/user to reply is repeating too much, simplify!

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
        fprintf(stdout, "%s", notice_message(msg));
    }
    free(response);
}

void reply(irc *ircs, char *response) {
    if (response != NULL && !irc_printf(ircs, response)) {
        fprintf(stderr, "%s", irc_lasterror(ircs));
        exit(EXIT_FAILURE);
    }
}

char *channel_message(tokarr msg) {
    clock_t startTime = clock();
    char *response;

    if ((response = calloc(MAXLENGTH * 2, sizeof(char))) == NULL){
        fprintf(stderr, "Error in malloc() for responseMsg. Download more ram?\n");
        return NULL;
    }

    strcpy(response, "PRIVMSG ");
    strncat(response, msg[2], MAXLENGTH);
    strncat(response, " ", 1);

    fprintf(stdout, "Checking if the following parameter triggers any function: %s\n" , msg[3]);

    if (!strncmp(msg[3], "!sysinfo", 9)) {
        struct sysinfo info;
        char sysInfoMsg[MAXLENGTH];

        fprintf(stdout, "match for !sysinfo... getting data ready *beep boop*\n");

        if (!sysinfo(&info)) {
            int ret = snprintf(sysInfoMsg, MAXLENGTH,
                                " Uptime: %ld d %ld h %ld m %ld s, RAM: %lu / %lu MB (used/total)",
                                (info.uptime / 3600) / 24,                      // uptime days
                                info.uptime / 3600,                             // uptime hrs
                                (info.uptime % 3600) / 60,                      // uptime min
                                info.uptime % 60,                               // uptime sec
                                (info.totalram - info.freeram) / (1024*1024),   // used in MB
                                info.totalram / (1024*1024));                   // total in MB

            if (ret < 0 || ret > MAXLENGTH){
                fprintf(stderr, "Error in snprintf for sysinfo: %d\n", ret);
            }
        } else {
            fprintf(stderr, "Error in sysinfo() call, errno: %d\n", errno);
        }
        strncat(response, sysInfoMsg, MAXLENGTH);
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

        strncat(response, (grab_url_data(msg[3], specialDomain)), MAXLENGTH);
    }
    if (response) {
        fprintf(stderr, "\nTime to execute succesfully: %f seconds.\n", (clock() - startTime)/(double)CLOCKS_PER_SEC );
    }
    return response;
}

/*
 * nickserv auth
 */
char *notice_message(tokarr msg) {
    if (strcasecmp(msg[2], "nickserv")) {
        return "";
    }

    char *response;

    if ((response = calloc(MAXLENGTH * 2, sizeof(char))) == NULL){
        fprintf(stderr, "Error in malloc() for notice_message. Download more ram?\n");
        return "";
    }

    if (!strcasecmp(msg[3], "This nick is not registered")) {
        // sprintf(buf, "REGISTER %s NOMAIL", ucfg.nickservPassword);
        strncpy(response, "nickserv", MAXLENGTH * 2);
    } else if (!strcasecmp(msg[3], "This nickname is registered and protected")) {
        // sprintf(buf, "IDENTIFY %s", ucfg.nickservPassword);
        strncpy(response, "NICKSERV IDENTIFY", MAXLENGTH * 2);
        // strncat(response, response, MAXLENGTH * 2); /* ucfg.nickservPassword */
    } else if (!strcasecmp(msg[3], "Password accepted - you are now recognized")) {
        fprintf(stdout, "Nickserv authentication succeed.");
    }

    return response;
}

void private_message(irc *ircs, tokarr msg) {
    char response[MAXLENGTH];
    char nickToReply[MAXLENGTH];

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
        char helpText[HELPTEXTLINES][MAXLENGTH] = { " !sysinfo\n",
                                                    " the end.\n" };

        for (unsigned int i = 0; i < HELPTEXTLINES; ++i) {
            memcpy(replyTextStartPos, helpText[i], strlen(helpText[i]));
            reply(ircs, response);
        }
    } else {
        strncat(response, " !h for help", 13);
        reply(ircs, response);
    }
    return;
}
