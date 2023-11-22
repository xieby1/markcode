#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <stdio.h>
#include <string.h>

#include "re.h"

void mc_match(
    // input
    char *pattern, char *subject, int subject_len,
    // output
    char **md, int *md_len, char **cf, int *cf_len
) {
    *md = NULL;
    *md_len = -1;
    *cf = NULL;
    *cf_len = -1;

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
        return;
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
        goto free;
    }

    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);

    // named capture
    uint32_t name_count;
    pcre2_pattern_info(re, PCRE2_INFO_NAMECOUNT, &name_count);
    if (name_count == 0) {
        /* printf("No named substr\n"); */
        goto free;
    } else {
        PCRE2_SPTR name_table;
        pcre2_pattern_info(re, PCRE2_INFO_NAMETABLE, &name_table);
        uint32_t name_entry_size;
        pcre2_pattern_info(re, PCRE2_INFO_NAMEENTRYSIZE, &name_entry_size);

        PCRE2_SPTR tabptr = name_table;
        for (int i = 0; i < name_count; i++) {
            int n = (tabptr[0] << 8) | tabptr[1];
            // TODO: learn how name_table organized
            // The following snippets are based on pcre2 demo
            char *name = (char *)tabptr + 2;
            if (!strcmp("md", name)) {
                *md = subject + ovector[2*n];
                *md_len = ovector[2*n+1] - ovector[2*n];
            } else if (!strcmp("cf", name)) {
                *cf = subject + ovector[2*n];
                *cf_len = ovector[2*n+1] - ovector[2*n];
            }
            tabptr += name_entry_size;
        }
    }

    // ignore cf line
    if (*cf_len > 0) {
        *md = NULL;
        *md_len = -1;
    }

free:
    pcre2_match_data_free(match_data);
    pcre2_code_free(re);
}
