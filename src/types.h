#pragma once

/* libsrsirc - https://github.com/fstd/libsrsirc */
#include <libsrsirc/irc.h>
#include <libsrsirc/irc_ext.h>
#include <libsrsirc/util.h>

#define MAXLENGTH 255
#define HELPTEXTLINES 2
#define STRLENGTH(x) STRLENGTHVAL(x)
#define STRLENGTHVAL(x) #x

typedef struct {
    char            *botNick,
                    *botUname,
                    *botFname,
                    *server,
                    *channel,
                    *serverPassword,
                    *nickservPassword;
    unsigned short  port;
    char            sslActivated;
} user_config;
