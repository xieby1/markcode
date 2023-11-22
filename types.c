#include <string.h>
#include "types.h"

bool get_ts_lang_and_pattern_by_ext(
    // input
    const char *ext,
    // output
    TSLanguage **ts, char **lang, char **pattern
) {
    for (int typei = 0; typei < types_len; typei++) {
        const Type *type = &types[typei];
        for (int exti = 0; exti < type->exts_len; exti++) {
            if (!strcmp(ext, type->exts[exti])) {
                *ts = type->ts_func();
                *lang = type->lang;
                *pattern = type->pattern;
                return true;
            }
        }
    }
    return false;
}

bool get_ts_lang_and_pattern_by_filename(
    // input
    const char *filename,
    // output
    TSLanguage **ts, char **lang, char **pattern
) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return false;
    else
        return get_ts_lang_and_pattern_by_ext(dot+1, ts, lang, pattern);
}


