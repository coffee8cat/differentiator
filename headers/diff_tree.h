#ifndef _DIIFFERENTIATOR_TREE_H__
#define _DIIFFERENTIATOR_TREE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>

#include "my_stack.h"

const size_t default_str_size = 32;

#define DEF_OPER(oper, ...) oper,
enum operations {
    #include "diff_rules_DSL.h"
    UNKNOWN
};
#undef DEF_OPER

enum node_type {NUM, OP, VAR};

union node_value
{
    double num;
    size_t op;
    size_t var;
};

struct node_t
{
    node_type type;
    node_value value;
    node_t* left;
    node_t* right;
    node_t* parent;
};
//== VARIABLES TABLE ========================================================================//
struct variable
{
    const char* name;
    size_t name_len;
    double value;
};

const size_t max_num_variables = 128;

variable* prepare_vars_table();
size_t add_variable(variable* vars_table, const char* name, size_t name_length);
void ask_variables_values(variable* vars_table);
size_t lookup(variable* vars_table, const char* name, size_t name_length);
void dump_variables_table(variable* vars_table);

//== TREE FUNCTIONS =========================================================================//
node_t* new_node(node_type type, node_value value, node_t* left, node_t* right);
void fix_parents(node_t* node);
void fix_tree(node_t* node);
void tree_dtor(node_t* node, bool is_recursive);

//== CALCULATIONS ===========================================================================//
double evaluate_tree(node_t* node, variable* vars_table);
node_t* differentiate_tree(node_t* node, variable* vars_table, size_t var_num, FILE* tex_stream, stack_t* roots_stack, stack_t* subs_stack);

//== OPTIMISATION ===========================================================================//
void optimize(node_t* node, variable* vars_table, FILE* html_stream, FILE* tex_stream, stack_t* roots_stack, stack_t* subs_stack);
size_t const_folding(node_t* node, variable* vars_table, FILE* tex_stream, stack_t* roots_stack, stack_t* subs_stack);
size_t remove_neutral_elems(node_t** node, variable* vars_table, FILE* tex_stream, stack_t* roots_stack, stack_t* subs_stack);

size_t ADD_optimisation(node_t** node);
size_t MUL_optimisation(node_t** node);
size_t SUB_optimisation(node_t** node);
size_t POW_optimisation(node_t** node);

//== SECONDARY FUNCTIONS ====================================================================//
node_t* copy_tree(node_t* root);
bool check_vars(node_t* node);

#endif
