#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <time.h>
#include <sys/sysinfo.h>

/* libsrsirc - https://github.com/fstd/libsrsirc */
#include <libsrsirc/irc.h>

#include "responses.h"
#include "structs.h"

void interpret_message(tokarr);
/*
void event_numeric(irc_session_t *, unsigned int, const char *, const char **, unsigned int);
void event_channel(irc_session_t *, const char *, const char *, const char **, unsigned int);
void event_notice(irc_session_t *, const char *, const char *, const char **, unsigned int );
void event_privmsg(irc_session_t *, const char *, const char *, const char **, unsigned int);
*/
