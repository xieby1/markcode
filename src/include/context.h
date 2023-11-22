#include <stdbool.h>

typedef struct {
    bool hide;
} Context;

void update_context_by_conf(Context *context, char *cf, int cf_len);
