#include <ctype.h>
#include <errno.h>
#include <regex.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <time.h>

/* libcurl - curl.haxx.se/libcurl/ */
#include <curl/curl.h>

#include "url_magic.h"

/* Max amount of matches with regexec()*/
#define MAX_MATCHED                 1

struct MemoryStruct {
    char    *memory;
    size_t  size;
};

size_t write_response(void *ptr, size_t size, size_t nmemb, char *str) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)str;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        /* out of memory! */ 
        fprintf(stderr, "not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int check_message_for_url(const char *inputText) {
    /* first check if it's a URL
     * simple pattern that checks if it starts with either 'http://' or 'https://'
     */
    const char *httpPattern = "http(s?):[^[:space:]]+";

    if (search_pattern(inputText, httpPattern)){
        fprintf(stderr, "'%s' is not an URL.\nNo match for '%s'\n", inputText, httpPattern);
        return 1;
    }

    fprintf(stderr, "'%s' is an URL. yay\n", inputText);
    return 0;
}

int search_pattern(const char *str, const char *pattern) {
    /*
     * use 'regreturn' as the regcomp() return value,
     * but also as the return value of regexec().
     * no need for two separate variables since they won't
     * be used at the same time.
     */
    int     regreturn;
    regex_t reg;

    regreturn = regcomp(&reg, pattern, REG_EXTENDED | REG_NOSUB);
    if (regreturn){
        /* error while compiling a regular expression.
         * stop here, return with 1 to indicate an error
         */
        fprintf(stderr, "\nError in regcomp.\n");
        return 1;
    }

    regreturn = regexec(&reg, str, 0, NULL, 0);
    if (regreturn == REG_NOMATCH){
        /* no match, exit and free up 'reg' */
        fprintf(stderr, "No match in regexec! pattern: %s\n", pattern);
        regfree(&reg);
        return 1;
    } else if (regreturn){
        char regerr[128];
        regerror(regreturn, &reg, regerr, sizeof(regerr));
        fprintf(stderr, "Regex match failed: %s\n", regerr);
        regfree(&reg);
        return 1;
    }

    fprintf(stderr, "Match in regexec! pattern: %s\n", pattern);
    regfree(&reg);
    return regreturn;
}

int search_special_domains(const char *url, const char *pattern){
    if (search_pattern(url, pattern) != 1)
        return 0;
    
    return 1;
}

char *grab_url_data(const char *url, const short specialDomain) {
    CURL *curl  = curl_easy_init();
    char *title = NULL;

    if (curl) {
        CURLcode res;
        struct MemoryStruct response;

        response.memory = malloc(1);
        if (response.memory == NULL) {
            return NULL;
        }
        response.size = 0;

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:97.0) Gecko/20100101 Firefox/97.0");

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "Failed with curl_easy_perform():\n [%s]\n", curl_easy_strerror(res));
            return NULL;
        }

        fprintf(stderr, "Curl success with - %s\n", url);

        // send the data to find_title_tag() to find the title of the url
        title = find_title_tag(response.memory, specialDomain);

        // clean up data that won't be needed anymore
        free(response.memory);
        curl_easy_cleanup(curl);

        if (!title) {
            return NULL;
        }
    }
    return title;
}

char *find_title_tag(char *htmlData, const short specialDomain) {
    char *start;
    char *end;
    char *title;
    const char *titleTag = "<title>";

    if (search_pattern(htmlData, titleTag)) {
        fprintf(stderr, "No title tag found");
        return NULL;
    }

    start = strstr(htmlData, titleTag);
    /* move by <title> tag length */
    start = start + strlen(titleTag);
    end = strstr(htmlData, "</title>");

    /* Fix if there are whitespace characters at the start of the title */
    while (start < end && isspace(*start)) {
        ++start;
    }

    /*
     * max length of title will be:
     * (MAXLENGTH * 2) + 6
     * in case it checks for yt links
     * all others will be shorter
     */
    title = calloc(MAXLENGTH * 2, sizeof(char));
    strncat(title, "URL: ", 6);
    strncat(title, start, (end - start) % (MAXLENGTH * 2));

    /*
     * see 'enum special_domains' to check what index
     * stands for what url in 'matchedUrlIndex'
     */
    if (specialDomain == YOUTUBE) {
        // get view count
        start = strstr(start, "interactionCount\" content=\"");
        if (start != NULL) {
            start += 27;
            end = strstr(start, "\">");
            if (end) {
                char *views = calloc(64, sizeof(char));
                if (views != NULL) {
                    strcpy(views, " | ");
                    strncat(views, start, (end - start) % 32);
                    strcat(views, " views");
                    strncat(title, views, 64);

                    free(views);
                }
            }
        }

        // get likes count, use previous end to not search trough all of the data
        start = strstr(end, "LIKE\"},\"defaultText\":{\"accessibility\":{\"accessibilityData\":{\"label\":\"");
        if (start != NULL) {
            // move to the end of the above searched string
            start += 69;
            end = strstr(start, " likes");
            if (end){
                char *likes = calloc(MAXLENGTH, sizeof(char));
                if (likes != NULL){
                    strcpy(likes, " | ");
                    strncat(likes, start, (end - start) % MAXLENGTH);
                    strcat(likes, " likes");

                    /*
                    * DISLIKES REMOVED FROM YOUTUBE 2022
                    * start searching from previous 'start' - no need to go trough the whole htmlData
                    * since the 'dislike' data comes after the 'like' data
                    * /
                    start = strstr(start,
                                "DISLIKE\"},\"defaultText\":{\"accessibility\":{\"accessibilityData\":{\"label\":\"");
                    start = start + DISLIKE_LENGTH;
                    end = strstr(start, " ");

                    char *dislikes = calloc(12, sizeof(char));
                    if (dislikes == NULL){
                        free(likes);
                        return title;
                    }
                    strncat(dislikes, start, end - start);
                    strncat(dislikes, " dislikes", 11);

                    strncat(title, " | ", 4);
                    */
                    strncat(title, likes, MAXLENGTH);
                    /*
                    strncat(title, dislikes, YT_RATING_LENGTH);
                    free(dislikes);
                    */

                    free(likes);
                }
            }
        }
    } else if (specialDomain == IMDB) {
        char *rating = calloc(MAXLENGTH, sizeof(char));
        if (rating != NULL){
            start = strstr(htmlData, "ratingCount");

            if (start == NULL) {
                strncat(title, " - no rating", 13);
            } else {
                start = strstr(htmlData, "ratingCount\":");

                if (start) {
                    start += 13;
                    end = strstr(start, ",\"");

                    if (end) {
                        strcpy(rating, " | votes: ");
                        strncat(rating, start, (end - start));
                    }

                    // continue from previous end
                    start = strstr(end, "ratingValue\":") + 13;
                    if (start) {
                        end = strstr(start, "}");

                        if (end){
                            strcat(rating, " | rating: ");
                            strncat(rating, start, (end - start) % 4);
                            strncat(title, rating, MAXLENGTH);
                            strcat(title, "/10");

                            free(rating);
                        }
                    }
                }
            }
        }
    }

    fprintf(stdout, "Title: %s\n", title);
    return title;
}
