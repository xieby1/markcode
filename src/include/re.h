#include <stdbool.h>

void mc_match(
    // input
    char *pattern, char *subject, int subject_len,
    // output
    char **md, int *md_len, char **cf, int *cf_len
);
