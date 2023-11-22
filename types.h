#include <stdbool.h>
#include <tree_sitter/api.h>

typedef struct {
    char *lang;
    TSLanguage *(*ts_func)();
    int exts_len;
#define MAX_FILE_EXTS 16
    char *exts[MAX_FILE_EXTS];
    char *pattern;
} Type;

TSLanguage *tree_sitter_c();
TSLanguage *tree_sitter_cpp();
TSLanguage *tree_sitter_nix();
static const Type types[] = {
    {"c",       tree_sitter_c,      2, {"c", "h"},                             "\\s*//+ (.*)"},
    {"cpp",     tree_sitter_cpp,    4, {"cc", "cpp", "hh", "hpp"},             "\\s*//+ (.*)"},
    {"nix",     tree_sitter_nix,    1, {"nix"},                                "\\s*#+ (.*)"},
};
#define types_len sizeof(types) / sizeof(Type)

bool get_ts_lang_and_pattern_by_filename(
    // input
    const char *filename,
    // output
    TSLanguage **ts, char **lang, char **pattern
);
