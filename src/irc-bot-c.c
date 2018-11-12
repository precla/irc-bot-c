#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>

/* libcurl - curl.haxx.se/libcurl/ */
#include <curl/curl.h>

/* libircclient - ulduzsoft.com/libircclient/ */
#include <libircclient/libircclient.h>
#include <libircclient/libirc_rfcnumeric.h>

#include "events.h"
#include "responses.h"
#include "structs.h"

#define MAXLENGTH 128

/* global variable with settings */
user_config ucfg;

int main(int argc, char **argv) {
    irc_callbacks_t callbacks;
    irc_session_t   *s;

    if (argc != 2) {
        fprintf(stdout, "Usage: %s server.cfg\n", argv[0]);
        exit(1);
    }

    /* Initialize the callbacks */
    memset(&callbacks, 0, sizeof(callbacks));

    /* Set up the callbacks we will use */
    callbacks.event_connect = event_connect;
    callbacks.event_join = event_join;
    callbacks.event_numeric = event_numeric;
    callbacks.event_channel = event_channel;
    callbacks.event_notice = event_notice;
    callbacks.event_privmsg = event_privmsg;

    /* open cfg file */
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", argv[1]);
        exit(1);
    }

    fprintf(stdout, "Config file %s loaded.\n", argv[1]);

    char *checkCfgParameter = (char *)calloc(MAXLENGTH, sizeof(char));
    ucfg.botNick = (char *)calloc(MAXLENGTH, sizeof(char));
    ucfg.server = (char *)calloc(MAXLENGTH, sizeof(char));
    ucfg.channel = (char *)calloc(MAXLENGTH, sizeof(char));
    ucfg.nickservPassword = (char *)calloc(MAXLENGTH, sizeof(char));

    while (!feof(f)) {
        fscanf(f, "%96s", checkCfgParameter);

        if (!strcmp(checkCfgParameter, "bot_nick")) {
            fscanf(f, " %96s", ucfg.botNick);

        } else if (!strcmp(checkCfgParameter, "server")) {
            fscanf(f, " %96s", ucfg.server);

        } else if (!strcmp(checkCfgParameter, "port")) {
            fscanf(f, " %hu", &ucfg.port);

        } else if (!strcmp(checkCfgParameter, "channel")) {
            fscanf(f, " %96s", ucfg.channel);

        } else if (!strcmp(checkCfgParameter, "ssl")) {
            fscanf(f, " %96s", ucfg.sslActivated);

        } else if (!strcmp(checkCfgParameter, "nickserv_auth")) {
            fscanf(f, " %96s", ucfg.nickservPassword);
        }
    }

    fprintf(stdout, "bot nick: %s\nssl: %s\nserver: %s\nport: %d\nchannel(s): %s\nnickserv auth: ***\n",
                    ucfg.botNick, ucfg.sslActivated, ucfg.server, ucfg.port, ucfg.channel);
    
    /* create the IRC session; 0 means error */
    s = irc_create_session(&callbacks);

    if (!s) {
        fprintf(stderr, "Could not create IRC session\n");
        fclose(f);
        free(checkCfgParameter);
        exit(1);
    }

    fprintf(stdout, "IRC session created\n");

    irc_set_ctx(s, &ucfg);
    irc_option_set(s, LIBIRC_OPTION_STRIPNICKS);

    if (ucfg.sslActivated[0] == 'y') {
        /* If SSL connection required, add '#' at the beginning of the server
         * Create new string with # at beginning, append server to it.
         * copy the new string into the old ucfg.server string.
         */
        char *sslServer = (char *)malloc(strlen(ucfg.server) + 2);
        strcpy(sslServer, "#");
        strcat(sslServer, ucfg.server);
        /* todo: fix ssl initialization fail
        ucfg.server = (char *)malloc(strlen(sslServer) + 1);
        strcpy(ucfg.server, sslServer);
        */
        free(sslServer);

        /* To handle the "SSL certificate verify failed" 
         * do not verify the ssl cert if 'v' is not set
         */
        if (ucfg.sslActivated[1] != 'v') {
            irc_option_set(s, LIBIRC_OPTION_SSL_NO_VERIFY);
        }
    }

    /* Initiate the IRC server connection */
    if (irc_connect(s, ucfg.server, ucfg.port, 0, ucfg.botNick, 0, 0)) {
        fprintf(stderr, "Could not connect: %s\n", irc_strerror(irc_errno(s)));
        fclose(f);
        free(checkCfgParameter);
        exit(1);
    }

    fprintf(stdout, "IRC server connection successfully created.\n");

    /* and run into forever loop, generating events */
    if (irc_run(s)) {
        fprintf(stderr, "Could not connect or I/O error: %s\n", irc_strerror(irc_errno(s)));
        fclose(f);
        free(checkCfgParameter);
        exit(1);
    }

    free(checkCfgParameter);
    fclose(f);
    exit(0);
}
