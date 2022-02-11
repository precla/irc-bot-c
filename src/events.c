#include "events.h"

extern user_config ucfg;

void interpret_message(tokarr msg) {

}

/*
void event_join(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count) {
    if (!origin) {
        return;
    }
}

void event_connect(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count) {
    user_config * ctx = (user_config *)irc_get_ctx(session);
    irc_cmd_join(session, ctx->channel, 0);
}

void event_numeric(irc_session_t * session, unsigned int event, const char * origin, const char ** params, unsigned int count) {
    if (event > 400) {
        fprintf(stderr, "ERROR %u: %s: %s %s %s %s\n",
            event,
            origin ? origin : "unknown",
            params[0],
            count > 1 ? params[1] : "",
            count > 2 ? params[2] : "",
            count > 3 ? params[3] : "");
    }
}

void event_channel(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count) {
    if (!origin) {
        return;
    }

    clock_t startTime = clock();

    fprintf(stdout, "Checking if the following parameter triggers any function: %s\n" , params[1]);

    if (!strncmp(params[1], "!sysinfo", 9)) {
        fprintf(stdout, "match for !sysinfo... getting data ready");
        struct sysinfo info;

        if (!sysinfo(&info)) {
            char *infoData;

            if ((infoData = (char*)malloc(sizeof(char) * 128)) == NULL){
                fprintf(stderr, "Error in malloc() for !sysinfo call.\n");
                return;
            }

            int ret = snprintf(infoData, 128,
                                "Uptime: %ld h %ld m %ld s, RAM: %lu / %lu MB (used/total)",
                                info.uptime / 3600,                             /* uptime hrs * /
                                (info.uptime % 3600) / 60,                      /* uptime min * /
                                info.uptime % 60,                               /* uptime sec * /
                                (info.totalram - info.freeram) / (1024*1024),   /* get in MB * /
                                info.totalram / (1024*1024));                   /* get in MB * /

            if (ret >= 0 && ret < 128){
                irc_cmd_msg(session, params[0], infoData);
            } else {
                fprintf(stderr, "Error in snprintf() call, error: %d\n", ret);
            }

            free(infoData);
        } else {
            fprintf(stderr, "Error in sysinfo() call, errno: %d\n", errno);
        }
        fprintf(stderr, "\nTime to execute succesfully: %f seconds.\n", (clock() - startTime)/(double)CLOCKS_PER_SEC );
        return;
    }

    if (params[1]) {

        if (check_message_for_url(params[1])) {
            return;
        }

        /* specialDomain - for special domains with extra features.
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
        * /
        short specialDomain = -1;

        /* check if yt link, except channel links * /
        if (search_special_domains(params[1],
                    "(www.)?youtu(.)?be(.com)?[^[:space:]]+") == 0){
            specialDomain = 0;
        } else if (search_special_domains(params[1],
                    "(www.)?imdb.com[[:punct:]]title[^[:space:]]+") == 0){
            specialDomain = 1;
        }

        char *messageToIrc = grab_url_data(params[1], specialDomain);

        irc_cmd_msg(session, params[0], messageToIrc);

        fprintf(stderr, "\nTime to execute succesfully: %f seconds.\n", (clock() - startTime)/(double)CLOCKS_PER_SEC );

        return;
    }

    fprintf(stdout, "No trigger for this message.\n");
    return;
}

/*
    nickserv auth taken from: https://www.ulduzsoft.com/libircclient/index.html#how-to-register-auth-with-nickserv
    no need to reinvent the wheel by myself..
* /
void event_notice(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count) {
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
}

void event_privmsg(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count) {
    irc_cmd_msg(session, origin, "Hi, I'm a bot.");
}

*/
