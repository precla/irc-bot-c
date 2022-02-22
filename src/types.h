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
    char            *nick,
                    *uname,
                    *fname,
                    *email,
                    *server,
                    *channel,
                    *serverPwd,
                    *nickservPwd;
    unsigned short  port;
    char            sslActive;
} user_config;
