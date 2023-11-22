#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <stdio.h>

#include "re.h"

bool mc_match(
    // input
    char *pattern, char *subject, int subject_len,
    // output
    char **matched_str, int *matched_len
) {

    int errorcode;
    PCRE2_SIZE erroffset;
    pcre2_code *re = pcre2_compile(
        (PCRE2_SPTR)pattern,
        PCRE2_ZERO_TERMINATED,
        0,
        &errorcode,
        &erroffset,
        NULL
    );

    if (!re) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
        fprintf(stderr, "PCRE2 compilation failed at offset %d: %s\n", (int)erroffset, buffer);
        return false;
    }

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);
    int rc = pcre2_match(
        re,
        (PCRE2_SPTR)subject,
        subject_len,
        0,
        0,
        match_data,
        NULL
    );

    if (rc < 0) {
        if (rc == PCRE2_ERROR_NOMATCH) {
            /* fprintf(stderr, "No match\n"); */
        } else {
            fprintf(stderr, "Matching error %d\n", rc);
        }
        pcre2_match_data_free(match_data);
        pcre2_code_free(re);
        return false;
    }

    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);

    if (rc >= 2) {
        *matched_str = subject + ovector[2];
        *matched_len = ovector[3] - ovector[2];
        return true;
    } else {
        *matched_str = NULL;
        *matched_len = 0;
        return false;
    }
}
