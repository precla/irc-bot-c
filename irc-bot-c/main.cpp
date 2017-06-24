#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <cstddef>

// libcurl - curl.haxx.se/libcurl/
#include <curl\curl.h>

// libircclient - ulduzsoft.com/libircclient/
#include <libircclient\libircclient.h>
#include <libircclient\libirc_rfcnumeric.h>

// oniguruma - github.com/kkos/oniguruma
#include <oniguruma\oniguruma.h>

#include "events.h"
#include "responses.h"
#include "structs.h"

int main(int argc, char **argv) {
    irc_callbacks_t	callbacks;
    irc_ctx_t ctx;
    irc_session_t * s;
    unsigned short port = 6667;

    if (argc != 4) {
        printf("Usage: %s <server> <nick> <channel>\n", argv[0]);
        return 1;
    }

    // Initialize the callbacks
    memset(&callbacks, 0, sizeof(callbacks));

    // Set up the callbacks we will use
    callbacks.event_connect = event_connect;
    callbacks.event_join = event_join;
    callbacks.event_numeric = event_numeric;
    callbacks.event_channel = event_channel;

    ctx.channel = argv[3];
    ctx.nick = argv[2];

    // And create the IRC session; 0 means error
    s = irc_create_session(&callbacks);

    if (!s) {
        printf("Could not create IRC session\n");
        return 1;
    }

    irc_set_ctx(s, &ctx);
    irc_option_set(s, LIBIRC_OPTION_STRIPNICKS);

    // If the port number is specified in the server string, use the port 0 so it gets parsed
    if (strchr(argv[1], ':') != 0) {
        port = 0;
    }

    // To handle the "SSL certificate verify failed" from command line we allow passing ## in front 
    // of the server name, and in this case tell libircclient not to verify the cert
    if (argv[1][0] == '#' && argv[1][1] == '#') {
        // Skip the first character as libircclient needs only one # for SSL support, i.e. #irc.freenode.net
        argv[1]++;

        irc_option_set(s, LIBIRC_OPTION_SSL_NO_VERIFY);
    }

    // Initiate the IRC server connection
    if (irc_connect(s, argv[1], port, 0, argv[2], 0, 0)) {
        printf("Could not connect: %s\n", irc_strerror(irc_errno(s)));
        return 1;
    }

    // and run into forever loop, generating events
    if (irc_run(s)) {
        printf("Could not connect or I/O error: %s\n", irc_strerror(irc_errno(s)));
        return 1;
    }

    return 1;
}
