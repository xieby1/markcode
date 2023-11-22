#include <stdbool.h>

bool mc_match(
    // input
    char *pattern, char *subject, int subject_len,
    // output
    char **matched_str, int *matched_len
);
