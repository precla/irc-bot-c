#include "responses.h"

/* Max amount of matches with regexec()*/
#define MAX_MATCHED 1

/* TITLE_TAG_LENGTH -> length of the "<title>" tag */
#define TITLE_TAG_LENGTH 7

/* Lenght of text that comes before the like rating */
#define LIKE_LENGTH 42
/* Lenght of text that comes before the dislike rating */
#define DISLIKE_LENGTH 45

size_t write_response(void *ptr, size_t size, size_t nmemb, char *s) {
    size_t new_len = sizeof(s) + (size * nmemb);
    s = (char *)realloc(s, new_len + 1);
    if (s == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s + sizeof(s), ptr, size*nmemb);
    s[new_len] = '\0';

    return size*nmemb;
}

int check_message_for_url(const char *inputText) {
    /* first check if it's a URL
     * simple pattern that checks if it starts with either 'http://' or 'https://'
     */
    const char *httpPattern = "http(s?):[^[:space:]]+";

    if(search_pattern(inputText, httpPattern)){
        fprintf(stderr, "'%s' is not an URL.\nNo match for '%s'\n", inputText, httpPattern);
        return 1;
    }

    fprintf(stderr, "'%s' is an URL. yay\n", inputText);
    return 0;
}

int search_pattern(const char *str, const char *pattern) {
    /* use 'regreturn' as the regcomp() return value,
     * but also as the return value of regexec().
     * no need for two separate variables since they won't
     * be used at the same time.
     */
    int regreturn;
    regex_t reg;

    regreturn = regcomp(&reg, pattern, REG_EXTENDED | REG_NOSUB);
    if(regreturn){
        /* error while compiling a regular expression.
         * stop here, return with 1 to indicate an error
         */
        fprintf(stderr, "\nError in regcomp.\n");
        return 1;
    }

    regreturn = regexec(&reg, str, 0, NULL, 0);
    if (regreturn == REG_NOMATCH){
        /* no match, exit and free up 'reg' */
        fprintf(stderr, "No match in regexec.\n");
        regfree(&reg);
        return 1;
    } else if (regreturn){
        char regerr[128];
        regerror(regreturn, &reg, regerr, sizeof(regerr));
        fprintf(stderr, "Regex match failed: %s\n", regerr);
        regfree(&reg);
        return 1;
    }

    regfree(&reg);
    return regreturn;
}

int search_special_domains(const char *url, const char *pattern){
    if (search_pattern(url, pattern) != -1)
        return 0;
    
    return 1;
}

char *grab_url_data(const char *url, const short specialDomain) {
    CURL *curl = curl_easy_init();
    char *title = NULL;

    if (curl) {
        CURLcode res;
        char *response = calloc(INT_MAX, sizeof(char *));

        // init_string_s(&response);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "Failed with curl_easy_perform():\n [%s]\n", curl_easy_strerror(res));
            return NULL;
        }

        fprintf(stderr, "Curl success with - %s\n", url);

        // send the data to find_title_tag() to find the title of the url
        title = find_title_tag(response, specialDomain);

        // clean up data that won't be needed anymore
        free(response);
        curl_easy_cleanup(curl);

        if (!title) {
            return NULL;
        }
    }
    return title;
}

char *find_title_tag(char *htmlData, const short specialDomain) {
    const char *pattern = "<title>";

    if (search_pattern(htmlData, pattern)) {
        fprintf(stderr, "No title tag found");
        return NULL;
    }

    char *start = strstr(htmlData, "<title>");
    /* move by <title> tag length */
    start = start + TITLE_TAG_LENGTH;
    char *end = strstr(htmlData, "<\/title>");

    /* Fix if there are whitespace characters at the start of the title */
    char *cleanText = start;
    while (cleanText < end && isspace(*cleanText)) {
        ++cleanText;
    }
    start = cleanText;

    char *title = (char *)calloc(512, sizeof(char));

    strncat(title, start, (size_t)(end - start) % 256);

    /* see 'enum special_domains' to check what index 
     * stands for what url in 'matchedUrlIndex'
     */
    if (specialDomain == YOUTUBE) {
        start = strstr(htmlData, "video-extras-sparkbar-likes");
        start = start + LIKE_LENGTH;
        end = strstr(start, "\%");
        char *likes = (char *)calloc(24, sizeof(char));
        strncat(likes, start, (size_t)(end - start) % 24);
        strncat(likes, "\% likes \/", 12);

        /* start searching from previous 'start' - no need to go trough the whole htmlData
         * since the 'dislike' data comes after the 'like' data
         */
        start = strstr(start, "video-extras-sparkbar-dislikes");
        start = start + DISLIKE_LENGTH;
        end = strstr(start, "\%");
        char *dislikes = (char *)calloc(24, sizeof(char));
        strncat(dislikes, start, (size_t)(end - start) % 24);
        strncat(dislikes, "\% dislikes", 12);

        title = strncat(title, " \|", 2);
        title = strncat(title, likes, strlen(likes));
        title = strncat(title, dislikes, strlen(dislikes));

        free(likes);
        free(dislikes);

    } else if (specialDomain == IMDB) {
        start = strstr(htmlData, "<strong title=\"");
        start = start + strlen("<strong title=\"");
        end = strstr(start, "\"><span itemprop=\"ratingValue\">");

        char *rating = (char *)calloc(256, sizeof(char));
        strncat(rating, start, (size_t)(end - start) % 256);

        strncat(title, " - ", 3);

        strncat(title, rating, 253);
        free(rating);
    }

    return title;
}
