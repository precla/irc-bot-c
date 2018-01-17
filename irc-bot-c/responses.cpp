#include "responses.h"

// TITLE_TAGS -> "<title>" and "</title>"
#define TITLE_TAGS 2
// TITLE_TAG_LENGTH -> length of the "<title>" tag
#define TITLE_TAG_LENGTH 7

void init_string_s(string_c *s) {
    s->len = 0;
    s->ptr = (char *)malloc(s->len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

size_t write_response(void *ptr, size_t size, size_t nmemb, string_c *s) {
    size_t new_len = s->len + size*nmemb;
    s->ptr = (char *)realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr + s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size*nmemb;
}

char *check_message_for_url(const char *inputUrl) {
    
    // first check if it's a URL
    // simple pattern that checks if it starts with either 'http://' or 'https://'
    static UChar *httpPattern[] = { (UChar*)"^(http|https):(?:\/\/)(\\S+)" };

    unsigned int numberOfPatterns = (sizeof(httpPattern) / sizeof(char*));

    if (check_regex((UChar *)inputUrl, httpPattern, &numberOfPatterns)) {
        // no URL, abandon ship
        return NULL;
    }

    // it's an URL, continue
    static UChar *domains[] = { (UChar*)"https?:\/\/(www.)?youtu(be|.be)?(.com)?\/(watch\?v=)?(\\S+)",
                                (UChar*)"https?:\/\/(i.imgur|imgur).com\/(\\S+)",
                                (UChar*)"https?:\/\/twitter.com\/\\w*(\/status\/\\d*)?",
                                (UChar*)"https?:\/\/(www.)?imdb.com\/(title\/\\S+)",
                                };

    numberOfPatterns = (sizeof(domains) / sizeof(char*));

    if (check_regex((UChar *)inputUrl, domains, &numberOfPatterns)) {
        // no domain match, abandon boat
        return NULL;
    }

    // numberOfPatterns holds the index of the matched pattern
    unsigned int matchedUrlIndex = numberOfPatterns;

    char *cRegexValue = grab_url_data(inputUrl, &matchedUrlIndex);

    return cRegexValue;
}

int check_regex(UChar *str, static UChar *pattern[], unsigned int *numPatterns) {
    // use 'r' as the onig_new return value, but also as the return value of check_regex()
    int r;
    unsigned char *start, *range, *end;
    regex_t* reg;
    OnigErrorInfo einfo;
    OnigRegion *region;

    OnigEncoding use_encs[] = { ONIG_ENCODING_UTF8 };
    onig_initialize(use_encs, sizeof(use_encs) / sizeof(use_encs[0]));

    for (unsigned int i = 0; i < *numPatterns; i++) {

        UChar* checkPattern = pattern[i];

        r = onig_new(&reg, checkPattern, checkPattern + strlen((char*)checkPattern), ONIG_OPTION_DEFAULT, ONIG_ENCODING_UTF8, ONIG_SYNTAX_DEFAULT, &einfo);

        if (r != ONIG_NORMAL) {
            OnigUChar s[ONIG_MAX_ERROR_MESSAGE_LEN];
            onig_error_code_to_str(s, r, &einfo);
            fprintf(stderr, "ERROR: %s - %s\n", str, s);
            // error, stop the loop and return 1
            r = 1;
            break;
        }

        region = onig_region_new();

        end = str + strlen((char*)str);
        start = str;
        range = end;
        r = onig_search(reg, str, end, start, range, region, ONIG_OPTION_NONE);

        if (r >= 0) {
            fprintf(stderr, "%s - match at %d\n", str, r);
            for (int j = 0; j < region->num_regs; j++) {
                fprintf(stderr, "%d: (%d-%d)\n", j, region->beg[j], region->end[j]);
            }
            // Matched! stop the loop because no need to check for the rest of the patterns
            // set numPatterns to the index of the matched pattern for additional stuff (search: FIND_RATING)
            // return 0 as success
            *numPatterns = i;
            r = 0;
            break;
        }
    }

    onig_region_free(region, 1);
    onig_free(reg);
    onig_end();
    return r;
}

char *grab_url_data(const char *url, unsigned int *matchedUrlIndex) {
    CURL *curl = curl_easy_init();
    char *title = NULL;

    if (curl) {
        CURLcode res;
        string_c response;

        init_string_s(&response);

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
        title = find_title_tag(response.ptr, *matchedUrlIndex);

        // clean up data that won't be needed anymore
        free(response.ptr);
        curl_easy_cleanup(curl);

        if (!title) {
            return NULL;
        }
    }
    return title;
}

char *find_title_tag(char *htmlData, unsigned int matchedUrlIndex) {
    
    static UChar *pattern[] = { (UChar*)"<title>", (UChar*)"<\/title>" };

    unsigned int tt = TITLE_TAGS;

    if (check_regex((UChar *)htmlData, pattern, &tt)) {
        fprintf(stderr, "No title tag found");
        return NULL;
    }

    char *start = strstr(htmlData, "<title>");
    // move by <title> tag length
    start = start + TITLE_TAG_LENGTH;
    char *end = strstr(htmlData, "<\/title>");

    char *title = (char *)calloc(512, sizeof(char));

    strncpy(title, start, (size_t)(end - start) % 256);

    // see 'enum domains_i' to check what index is for what url in 'matchedUrlIndex'
    // FIND_RATING - find rating if the link was a imdb/yt link
    if (matchedUrlIndex == Imdb) {
        // <strong title="7.7 based on 11,037 user ratings"><span itemprop="ratingValue">
        start = strstr(htmlData, "<strong title=\"");
        start = start + strlen("<strong title=\"");
        end = strstr(htmlData, "\"><span itemprop=\"ratingValue\">");

        char *rating = (char *)calloc(256, sizeof(char));
        strncpy(rating, start, (size_t)(end - start) % 256);

        strncat(title, " - ", 3);

        strncat(title, rating, 253);
        free(rating);
    }

    return title;
}
