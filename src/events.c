#include "events.h"

/*
 * Check what kind of message was received.
 * For example: private message, message in the active channel, ircd message, ...
 */
void interpret_message(irc *ircs, tokarr msg) {
    if (!strcmp(msg[1], "PRIVMSG")) {
        if (!strcmp(msg[2], irc_mynick(ircs))) {
            private_message(ircs, msg);
        } else if (lsi_ut_ischan(ircs, msg[2])) {
            channel_message(msg);
        }
    }
}

void reply (irc *ircs, char *response) {
    if (!irc_printf(ircs, response)) {
        fprintf(stderr, irc_lasterror(ircs), "%s");
        exit(EXIT_FAILURE);
    }
}

char *channel_message(tokarr msg) {
//     clock_t startTime = clock();
//
//     fprintf(stdout, "Checking if the following parameter triggers any function: %s\n" , params[1]);
//
//     if (!strncmp(params[1], "!sysinfo", 9)) {
//         fprintf(stdout, "match for !sysinfo... getting data ready");
//         struct sysinfo info;
//
//         if (!sysinfo(&info)) {
//             char *infoData;
//
//             if ((infoData = (char*)malloc(sizeof(char) * 128)) == NULL){
//                 fprintf(stderr, "Error in malloc() for !sysinfo call.\n");
//                 return;
//             }
//
//             int ret = snprintf(infoData, 128,
//                                 "Uptime: %ld h %ld m %ld s, RAM: %lu / %lu MB (used/total)",
//                                 info.uptime / 3600,                             /* uptime hrs */
//                                 (info.uptime % 3600) / 60,                      /* uptime min */
//                                 info.uptime % 60,                               /* uptime sec */
//                                 (info.totalram - info.freeram) / (1024*1024),   /* get in MB */
//                                 info.totalram / (1024*1024));                   /* get in MB */
//
//             if (ret >= 0 && ret < 128){
//                 irc_cmd_msg(session, params[0], infoData);
//             } else {
//                 fprintf(stderr, "Error in snprintf() call, error: %d\n", ret);
//             }
//
//             free(infoData);
//         } else {
//             fprintf(stderr, "Error in sysinfo() call, errno: %d\n", errno);
//         }
//         fprintf(stderr, "\nTime to execute succesfully: %f seconds.\n", (clock() - startTime)/(double)CLOCKS_PER_SEC );
//         return;
//     }
//
//     if (params[1]) {
//
//         if (check_message_for_url(params[1])) {
//             return;
//         }
//
//         /* specialDomain - for special domains with extra features.
//         * See enum special_domains for the list of those domains and
//         * the corresponding index.
//         * It holds the index of the matched domain.
//         * if it stays '-1' it means no match for any of those
//         * notice: no need to check for "https://", reason:
//         * without that, it wouldn't have matched above in the
//         * call to check_message_for_url.
//         * if we have a match of a special domain, skip the
//         * rest of the checks, goto should be faster than
//         * comparing with if(specialdomain != -1 &&..)
//         */
//         short specialDomain = -1;
//
//         /* check if yt link, except channel links */
//         if (search_special_domains(params[1],
//                     "(www.)?youtu(.)?be(.com)?[^[:space:]]+") == 0){
//             specialDomain = 0;
//         } else if (search_special_domains(params[1],
//                     "(www.)?imdb.com[[:punct:]]title[^[:space:]]+") == 0){
//             specialDomain = 1;
//         }
//
//         char *messageToIrc = grab_url_data(params[1], specialDomain);
//
//         irc_cmd_msg(session, params[0], messageToIrc);
//
//         fprintf(stderr, "\nTime to execute succesfully: %f seconds.\n", (clock() - startTime)/(double)CLOCKS_PER_SEC );
//
//         return;
//     }
//
//     fprintf(stdout, "No trigger for this message.\n");
    return NULL;
}

/*
 * nickserv auth
 */
char *notice_message(tokarr msg) {
/*
    char buf[256];

    if (!origin || strcasecmp(origin, "nickserv")) {
        return;
    }

    if (strstr(params[1], "This nick is not registered") == params[1]) {
        sprintf(buf, "REGISTER %s NOMAIL", ucfg.nickservPassword);
        irc_cmd_msg(session, "nickserv", buf);
    } else if (strstr(params[1], "This nickname is registered and protected") == params[1]) {
        sprintf(buf, "IDENTIFY %s", ucfg.nickservPassword);
        irc_cmd_msg(session, "nickserv", buf);
    } else if (strstr(params[1], "Password accepted - you are now recognized") == params[1]) {
        fprintf(stdout, "Nickserv authentication succeed.");
    }
*/
    return NULL;
}

void private_message(irc *ircs, tokarr msg) {
    char *response = calloc(MAXLENGTH, sizeof(char));
    char *nickToReply = calloc(MAXLENGTH, sizeof(char));

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
                                        " \n"};

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
