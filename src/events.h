#pragma once

#include "irc-bot-c.h"

void interpret_message(irc *ircs, tokarr msg);
void reply(irc *ircs, char *response);
char *channel_message(tokarr msg);
char *notice_message(tokarr msg);
void private_message(irc *ircs, tokarr msg);
