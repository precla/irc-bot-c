#include <ctype.h>
#include <errno.h>
#include <regex.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <time.h>

#include "messages.h"

/*
 * Check what kind of message was received.
 * For example: private message, message in the active channel, ircd message, ...
 */
void interpret_message(irc *ircs, tokarr msg, user_config *ucfg) {
    if (!strcmp(msg[1], "PRIVMSG")) {

        // TODO: PRIVMSG and chan/user to reply is repeating too much, simplify!

        if (!strcmp(msg[2], irc_mynick(ircs))) {
            private_message(ircs, msg);
        } else if (lsi_ut_ischan(ircs, msg[2])) {
            reply(ircs, channel_message(msg));
        }
    } else if (!strcmp(msg[1], "NOTICE")) {
        /*
         * rfc2812 #3.3.2 Never send an automatic reply to a notice
         * hence no call to reply(), only do output the notice msg we received
         */
        reply(ircs, notice_message(msg, ucfg));
    }
}

void reply(irc *ircs, char *response) {
    if (response != NULL && !irc_printf(ircs, response)) {
        fprintf(stderr, "%s", irc_lasterror(ircs));
        exit(EXIT_FAILURE);
    }
    free(response);
}

char *channel_message(tokarr msg) {
    clock_t startTime = clock();
    char *response;

    if ((response = calloc(MAXLENGTH * 2, sizeof(char))) == NULL){
        fprintf(stderr, "Error in calloc() for responseMsg.\n");
        return NULL;
    }

    strcpy(response, "PRIVMSG ");
    strncat(response, msg[2], MAXLENGTH);
    strncat(response, " ", 2);

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
            strncat(response, sysInfoMsg, MAXLENGTH);
        } else {
            fprintf(stderr, "Error in sysinfo() call, errno: %d\n", errno);
        }
    } else if (msg[3]) {
        if (check_message_for_url(msg[3])) {
            free(response);
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
 * NickServ auth is within notice
 */
char *notice_message(tokarr msg, user_config *ucfg) {
    if (strcasecmp(msg[2], ucfg->nick)) {
        fprintf(stdout, "Received notice from %s, content: %s\n", msg[2], msg[3]);
        return NULL;
    }

    fprintf(stdout, "Received notice: %s\n", msg[3]);

    char *response = calloc(MAXLENGTH * 4, sizeof(char));
    if (response == NULL) {
        return NULL;
    }

    if (!strncasecmp(msg[3], "This nick is not registered", 27) ||
        !strncasecmp(msg[3], "Your nickname is not registered", 31)) {
        strcpy(response, "PRIVMSG NickServ REGISTER ");
        strncat(response, ucfg->nickservPwd, MAXLENGTH);
        strcat(response, " ");
        strncat(response, ucfg->email, MAXLENGTH);
    } else if (!strncasecmp(msg[3], "This nickname is registered and protected", 41)) {
        strcpy(response, "PRIVMSG NickServ IDENTIFY ");
        strncat(response, ucfg->nickservPwd, MAXLENGTH);
    } else if (!strncasecmp(msg[3], "Password accepted - you are now recognized", 42)) {
        fprintf(stdout, "Nickserv authentication succeed.");
    }

    if (strlen(response) > 18) {
        fprintf(stdout, "Sending notice message: %s\n", response);
        return response;
    }

    free(response);
    return NULL;
}

void private_message(irc *ircs, tokarr msg) {
    char *response = calloc(MAXLENGTH * 4, sizeof(char));
    if (response == NULL) {
        return;
    }

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
