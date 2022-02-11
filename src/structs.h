#pragma once

typedef struct {
    char            *botNick,
                    *server,
                    *channel,
                    *nickservPassword;
    unsigned short  port;
    char            sslActivated;
} user_config;
