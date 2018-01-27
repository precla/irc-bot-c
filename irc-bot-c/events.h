#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <cstddef>

/* libcurl - curl.haxx.se/libcurl/ */
#include <curl\curl.h>

/* libircclient - ulduzsoft.com/libircclient/ */
#include <libircclient\libircclient.h>
#include <libircclient\libirc_rfcnumeric.h>

/* oniguruma - github.com/kkos/oniguruma */
#include <oniguruma\oniguruma.h>

#include "responses.h"
#include "structs.h"

void event_join(irc_session_t *, const char *, const char *, const char **, unsigned int);
void event_connect(irc_session_t *, const char *, const char *, const char **, unsigned int);
void event_numeric(irc_session_t *, unsigned int, const char *, const char **, unsigned int);
void event_channel(irc_session_t *, const char *, const char *, const char **, unsigned int);
void event_notice(irc_session_t *, const char *, const char *, const char **, unsigned int );
void event_privmsg(irc_session_t *, const char *, const char *, const char **, unsigned int);
