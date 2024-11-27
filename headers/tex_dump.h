#ifndef _LATEX_DIFFERENTIATOR_LOG_H__
#define _LATEX_DIFFERENTIATOR_LOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "diff_tree.h"
#include "my_stack.h"

FILE* prepare_TEX_stream();
void tex_intro(FILE* fp);

void close_TEX_stream(FILE* fp);
void tex_ending(FILE* fp);

char write_node(node_t* node, FILE* tex_stream, stack_t* roots_stack, stack_t* subs_stack, char sub, size_t layer);
size_t count_nodes(node_t* node);

void add_substitution(node_t* node, stack_t* roots_stack, stack_t* subs_stack, char sub);
void write_substitutions(FILE* tex_stream, stack_t* roots_stack, stack_t* subs_stack);

#endif
