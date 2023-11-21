#include <stdio.h>
#include <tree_sitter/api.h>

TSLanguage *tree_sitter_nix();

typedef struct {
    FILE *file;
    char *line;
} Payload;

const char *file_read(void *payload, uint32_t byte_index, TSPoint _, uint32_t *bytes_read) {
    FILE *file = ((Payload *)payload)->file;
    char *line = ((Payload *)payload)->line;
    if (fseek(file, byte_index, SEEK_SET)) {
        perror("file_read");
        abort();
    }

    size_t len = 0;
    ssize_t ret = getline(&line, &len, file);
    if (ret < 0)
        ret = 0;
    *bytes_read = ret;

    return line;
}

typedef enum {
    Dir_Child,
    Dir_Sibling,
    Dir_Parent,
} Direction;
typedef struct {
    Direction direction;
    TSTreeCursor cursor;
} LeafIter;
void leaf_iter_init(LeafIter *iter, TSNode root_node) {
    iter->direction = Dir_Child;
    iter->cursor = ts_tree_cursor_new(root_node);
}
bool leaf_iter_next(LeafIter *iter, TSNode *leaf) {
    while (true) {
        TSNode current_node = ts_tree_cursor_current_node(&iter->cursor);

        if (iter->direction == Dir_Child) {
            // non-leaf node
            if (ts_tree_cursor_goto_first_child(&iter->cursor)) {
                iter->direction = Dir_Child;
            // leaf node
            } else {
                *leaf = current_node;
                iter->direction = Dir_Sibling;
                return true;
            }
        } else if (iter->direction == Dir_Sibling) {
            if (ts_tree_cursor_goto_next_sibling(&iter->cursor)) {
                iter->direction = Dir_Child;
            } else {
                iter->direction = Dir_Parent;
            }
        } else if (iter->direction == Dir_Parent) {
            if (ts_tree_cursor_goto_parent(&iter->cursor)) {
                iter->direction = Dir_Sibling;
            } else {
                return false;
            }
        }
    }
}
void leaf_iter_fini(LeafIter *iter) {
    ts_tree_cursor_delete(&iter->cursor);
}

void print_node(TSNode node, FILE *file) {
    const TSSymbol TSSymbol_comment = ts_language_symbol_for_name(
        tree_sitter_nix(), "comment", 7, true
    );

    if (ts_node_symbol(node) != TSSymbol_comment)
        return;

    TSPoint start_point = ts_node_start_point(node);
    TSPoint end_point = ts_node_end_point(node);
    printf(
        "<%d,%d>~<%d,%d>: ",
        start_point.row, start_point.column,
        end_point.row, end_point.column
    );

    const char *type = ts_node_type(node);
    if (type)
        printf("[%s]: ", type);

    uint32_t start_byte = ts_node_start_byte(node);
    uint32_t end_byte = ts_node_end_byte(node);
    fseek(file, start_byte, SEEK_SET);
    for (int b=start_byte; b<end_byte; b++)
        printf("%c", (char)fgetc(file));
    printf("\n");
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        printf("Usage: %s <nix file>\n", argv[0]);
        return 0;
    }
    char *file_path = argv[1];
    Payload payload = {
        .file = fopen(file_path, "r"),
        .line = NULL
    };
    if (!payload.file) {
        perror("fopen");
        abort();
    }

    TSParser *parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_nix());

    TSInput input = {&payload, file_read, /*TODO: detect file encoding*/TSInputEncodingUTF8};
    TSTree *tree = ts_parser_parse(parser, NULL, input);
    TSNode root_node = ts_tree_root_node(tree);

    const TSSymbol TSSymbol_comment = ts_language_symbol_for_name(
        tree_sitter_nix(), "comment", 7, true
    );
    LeafIter iter;
    TSNode leaf;
    leaf_iter_init(&iter, root_node);
    while (leaf_iter_next(&iter, &leaf)) {
        print_node(leaf, payload.file);
    }
    leaf_iter_fini(&iter);

    ts_tree_delete(tree);
    ts_parser_delete(parser);

    if (payload.line)
        free(payload.line);
    fclose(payload.file);
    return 0;
}
