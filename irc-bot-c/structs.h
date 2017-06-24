#pragma once

#include <libircclient\libircclient.h>

typedef struct {
    char *channel;
    char *nick;
} irc_ctx_t;

typedef struct {
    char *ptr;
    size_t len;
} string_c;