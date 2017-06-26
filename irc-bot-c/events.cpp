#include "events.h"

void event_join(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count) {
    if (!origin) {
        return;
    }
}

void event_connect(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count) {
    irc_ctx_t * ctx = (irc_ctx_t *)irc_get_ctx(session);
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

        char *ReturnValue = check_message_for_url(params[1]);

        if (!ReturnValue) {
            return;
        }

        irc_cmd_msg(session, params[0], ReturnValue);
    }
}
