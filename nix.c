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
                leaf->id = NULL;
                leaf->tree = NULL;
                return false;
            }
        }
    }
}
void leaf_iter_fini(LeafIter *iter) {
    ts_tree_cursor_delete(&iter->cursor);
}

void print_lines_until_byte(FILE *file, uint32_t until_byte) {
    char *line = NULL;
    size_t len = 0;
    ssize_t res;
    for (uint32_t curr_byte = ftell(file); curr_byte < until_byte; curr_byte = ftell(file)) {
        if((res = getline(&line, &len, file)) != -1) {
            printf("%s", line);
        }
    }
    if (line) free(line);
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
    TSNode prev_leaf={0}, curr_leaf={0}, next_leaf={0};
    leaf_iter_init(&iter, root_node);
    leaf_iter_next(&iter, &curr_leaf);
    leaf_iter_next(&iter, &next_leaf);
    rewind(payload.file);
    bool markdown = true;
    while (curr_leaf.id) {
        if (ts_node_symbol(curr_leaf) == TSSymbol_comment &&
            // current comment node does not share lines with prev nor next node
            (!prev_leaf.id || (prev_leaf.id && ts_node_end_point(prev_leaf).row != ts_node_start_point(curr_leaf).row)) &&
            (!next_leaf.id || (next_leaf.id && ts_node_start_point(next_leaf).row != ts_node_end_point(curr_leaf).row))
        ) {
            if (!markdown) {
                printf("```\n\n");
                markdown = true;
            }
        } else {
            uint32_t curr_start_byte = ts_node_start_byte(curr_leaf);
            uint32_t curr_column = ts_node_start_point(curr_leaf).column;
            uint32_t curr_row_start_byte = curr_start_byte - curr_column;
            print_lines_until_byte(payload.file, curr_row_start_byte);

            if (markdown) {
                printf("\n```nix\n");
                markdown = false;
            }
        }
        print_lines_until_byte(payload.file, ts_node_end_byte(curr_leaf));

        prev_leaf = curr_leaf;
        curr_leaf = next_leaf;
        leaf_iter_next(&iter, &next_leaf);
    }
    leaf_iter_fini(&iter);
    if (!markdown)
        printf("```\n");

    ts_tree_delete(tree);
    ts_parser_delete(parser);

    if (payload.line)
        free(payload.line);
    fclose(payload.file);
    return 0;
}
