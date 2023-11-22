#include <stdbool.h>
#include <tree_sitter/api.h>

typedef struct {
    char *lang;
    TSLanguage *(*ts_func)();
#define MAX_EXTS 16
    char *exts[MAX_EXTS];
    char *pattern;
} Type;

TSLanguage *tree_sitter_c();
TSLanguage *tree_sitter_cpp();
TSLanguage *tree_sitter_nix();
static const Type types[] = {
    {"c",       tree_sitter_c,      {"c", "h", NULL},                             "\\s*//+ (?<md>.*)"},
    {"cpp",     tree_sitter_cpp,    {"cc", "cpp", "hh", "hpp", NULL},             "\\s*//+ (?<md>.*)"},
    {"nix",     tree_sitter_nix,    {"nix", NULL},                                "\\s*#+ (?<md>.*)"},
};
#define types_len sizeof(types) / sizeof(Type)

bool get_ts_lang_and_pattern_by_filename(
    // input
    const char *filename,
    // output
    TSLanguage **ts, char **lang, char **pattern
);
