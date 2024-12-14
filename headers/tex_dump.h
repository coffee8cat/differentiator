#ifndef _LATEX_DIFFERENTIATOR_LOG_H__
#define _LATEX_DIFFERENTIATOR_LOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "diff_tree.h"
#include "my_stack.h"

#define DUMP_TO_TEX(node, tex_stream, roots_stack, subs_stack)                      \
    fprintf(tex_stream, "\\[");                                                     \
    write_node(node, vars_table, tex_stream, roots_stack, subs_stack, 1);           \
    fprintf(tex_stream, "\\]\n");                                                   \
    write_substitutions(tex_stream, vars_table, roots_stack, subs_stack);           \


#define DIFF_INTRO(node, tex_stream, roots_stack, subs_stack)                                                       \
    fprintf(tex_stream, "\nGentlemen, it's a great pleasure to inform you that today we are differentiating\n");    \
    DUMP_TO_TEX(node, tex_stream, &roots_stack, &subs_stack)                                                        \

#define FINAL_ANSWER(diff_root, tex_stream, roots_stack, subs_stack)                    \
    fprintf(tex_stream, "\nFinally, after a long battle, we emerged victorious:\n");    \
    DUMP_TO_TEX(diff_root, tex_stream, &roots_stack, &subs_stack)                       \


#define DIFFERENTIATE(diff_root, vars_table, tex_stream, html_stream, roots_stack, subs_stack)              \
    fprintf(tex_stream, "\nThere we begin...\n");                                                           \
    diff_root = differentiate_tree(node, vars_table, 0, tex_stream, &roots_stack, &subs_stack);             \
    tree_dump(diff_root, vars_table, html_stream, diff_root);                                               \
    optimize(diff_root, vars_table, html_stream, tex_stream, &roots_stack, &subs_stack);                    \
    tree_dump(diff_root, vars_table, html_stream, diff_root)                                                \

struct substitution{
    node_t* root;
    char    label;
    int     buff;
};

FILE* prepare_TEX_stream();
void tex_intro(FILE* fp);

void close_TEX_stream(FILE* fp);
void tex_ending(FILE* fp);

void write_node(node_t* node, variable* vars_table, FILE* tex_stream, stack_t* roots_stack, stack_t* subs_stack, size_t layer);

bool if_add_brackets(node_t* node);
size_t count_nodes(node_t* node);

void add_substitution(node_t* node, FILE* tex_stream, stack_t* roots_stack, stack_t* subs_stack);
void write_substitutions(FILE* tex_stream, variable* vars_table, stack_t* roots_stack, stack_t* subs_stack);

#endif
