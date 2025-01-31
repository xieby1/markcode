//MC # MarkCode
//MC
//MC{hide}
#include <stdbool.h>
#include <tree_sitter/api.h>

typedef struct {
    char *lang;
    TSLanguage *(*ts_func)();
#define MAX_EXTS 16
    char *exts[MAX_EXTS];
    char *pattern;
} Type;

TSLanguage *tree_sitter_bash();
TSLanguage *tree_sitter_c();
TSLanguage *tree_sitter_cpp();
TSLanguage *tree_sitter_nix();
static const Type types[] = {
//MC{visible}
//MC ## Patterns
    {"bash",    tree_sitter_bash,   {"sh", NULL},                                  "\\s*#+MC({(?<cf>[^}]*)})?( ?(?<md>.*\\n))"},
    {"c",       tree_sitter_c,      {"c", "h", NULL},                             "\\s*//+MC({(?<cf>[^}]*)})?( ?(?<md>.*\\n))"},
    {"cpp",     tree_sitter_cpp,    {"cc", "cpp", "hh", "hpp", NULL},             "\\s*//+MC({(?<cf>[^}]*)})?( ?(?<md>.*\\n))"},
    {"nix",     tree_sitter_nix,    {"nix", NULL},                                 "\\s*#+MC({(?<cf>[^}]*)})?( ?(?<md>.*\\n))"},
    {"scala",   tree_sitter_cpp,    {"scala", NULL},                              "\\s*//+MC({(?<cf>[^}]*)})?( ?(?<md>.*\\n))"},
};
#define types_len sizeof(types) / sizeof(Type)

bool get_ts_lang_and_pattern_by_filename(
    // input
    const char *filename,
    // output
    TSLanguage **ts, char **lang, char **pattern
);
