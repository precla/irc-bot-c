#pragma once

#include <ctype.h>
#include <errno.h>
#include <regex.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <time.h>

/* libcurl - curl.haxx.se/libcurl/ */
#include <curl/curl.h>

/* libsrsirc - https://github.com/fstd/libsrsirc */
#include <libsrsirc/irc.h>
#include <libsrsirc/irc_ext.h>
#include <libsrsirc/util.h>

#include "events.h"
#include "responses.h"

#define MAXLENGTH 255
#define HELPTEXTLINES 2
#define STRLENGTH(x) STRLENGTHVAL(x)
#define STRLENGTHVAL(x) #x

typedef struct {
    char            *botNick,
                    *server,
                    *channel,
                    *nickservPassword;
    unsigned short  port;
    char            sslActivated;
} user_config;

void cleanupcfg(user_config);
