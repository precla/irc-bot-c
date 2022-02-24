#pragma once

#include "url_magic.h"
#include "types.h"

void interpret_message(irc *ircs, tokarr msg, user_config *ucfg);
void reply(irc *ircs, char *response);
char *channel_message(tokarr msg);
char *notice_message(tokarr msg, user_config *ucfg);
void private_message(irc *ircs, tokarr msg);
