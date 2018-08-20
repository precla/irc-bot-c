#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <sys/types.h>
#include <regex.h>

/* libcurl - curl.haxx.se/libcurl/ */
#include <curl/curl.h>

/* libircclient - ulduzsoft.com/libircclient/ */
#include <libircclient/libircclient.h>
#include <libircclient/libirc_rfcnumeric.h>

#include "structs.h"

/* special domains with extendend featuers
 * for Youtube: get likes and dislikes
 * imdb: get rating
 */
enum special_domains {
    YOUTUBE,
    IMDB
};

size_t write_response(void *, size_t, size_t, char *);

/* check if the message is a URL and contains a domain
 * returns NULL if no /url/domain/title match
 * otherwise return the title for the URL
 */
int check_message_for_url(const char *);

/* returns 0 if the regexpression matched,
 * if no match than it returns 1
 */
int search_pattern(const char *, const char *);

/* search for one of the special domains
 * see special_domains for the supported domains
 * takes the regex-pattern for the url and 
 * the url that has been posted in the channel
 * 1st arg: url, 2nd arg: pattern
 * return 0 on success
 */
int search_special_domains(const char *, const char *);

/* get the data from the URL */
char *grab_url_data(const char *, const short);

/* find the <title> tag, extract the title and return it */
char *find_title_tag(char *, const short);
