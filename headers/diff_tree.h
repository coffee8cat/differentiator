#ifndef _DIIFFERENTIATOR_TREE_H__
#define _DIIFFERENTIATOR_TREE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

const size_t default_str_size = 32;

#define DEF_OPER(oper, ...) oper,
enum operations {
    #include "diff_rules_DSL.h"
};
#undef DEF_OPER

enum node_type {NUM, OP, VAR};

struct node_t {
    node_type type;
    double value;
    node_t* left;
    node_t* right;
};

node_t* new_node(node_type type, double value, node_t* left, node_t* right);

int read_tree(node_t** node, FILE* stream, FILE* html_stream);
node_t* read_node(FILE* stream);
void skip_until(char ch, FILE* stream);

double eval (node_t* node);
node_t* diff(node_t* node);

void optimize(node_t* node);
size_t const_folding(node_t* node);

node_t* copy_tree(node_t* root);
bool check_vars(node_t* node);

int tree_dtor(node_t* node);

#endif
