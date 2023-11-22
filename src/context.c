#include "context.h"
#include <string.h>
#include <stdlib.h>

void update_context_by_conf(Context *context, char *cf, int cf_len) {
    // strtok may modifies examined string, so copy cf to str
    char *str = malloc(cf_len+1);
    strncpy(str, cf, cf_len);
    str[cf_len] = 0;

    const char *delimiters = ", ";
    for (char *token=strtok(str, delimiters); token; token=strtok(NULL, delimiters)) {
        if      (!strcmp(token, "hide"))
            context->hide = true;
        else if (!strcmp(token, "visible"))
            context->hide = false;
    }

    free(str);
}
