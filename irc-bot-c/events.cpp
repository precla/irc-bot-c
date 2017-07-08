#include "events.h"

extern user_config ucfg;

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
        printf("ERROR %u: %s: %s %s %s %s\n",
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

    if (params[1]) {

        char *returnValue = check_message_for_url(params[1]);

        if (!returnValue) {
            return;
        }

        irc_cmd_msg(session, params[0], returnValue);
    }
}

/*
    nickserv auth taken from: https://www.ulduzsoft.com/libircclient/index.html#how-to-register-auth-with-nickserv
    no need to reinvent the wheel by myself..
*/
void event_notice(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count) {
    char buf[256];

    if (!origin || strcmpi(origin, "nickserv")) {
        return;
    }

    if (strstr(params[1], "This nick is not registered") == params[1]) {
        sprintf(buf, "REGISTER %s NOMAIL", ucfg.nickservPassword);
        irc_cmd_msg(session, "nickserv", buf);
    } else if (strstr(params[1], "This nickname is registered and protected") == params[1]) {
        sprintf(buf, "IDENTIFY %s", ucfg.nickservPassword);
        irc_cmd_msg(session, "nickserv", buf);
    } else if (strstr(params[1], "Password accepted - you are now recognized") == params[1]) {
        printf("Nickserv authentication succeed.");
    }
}

void event_privmsg(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count) {
    irc_cmd_msg(session, origin, "Hi, I'm a bot.");
}
