#pragma once

#include <libircclient/libircclient.h>

typedef struct {
    char *botNick, *server, *channel, *nickservPassword;
    unsigned short port;
    char sslActivated[3];
} user_config;
