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

struct node_t
{
    node_type type;
    double value;
    node_t* left;
    node_t* right;
    node_t* parent;
};

//== TREE FUNCTIONS =========================================================================//
node_t* new_node(node_type type, double value, node_t* left, node_t* right);
void fix_parents(node_t* node);
void fix_tree(node_t* node);
int tree_dtor(node_t* node);

//== READING AND DUMPING ====================================================================//

node_t* read_node(const char** curr, const char* end, FILE* html_stream);
enum operations read_func_name(const char** curr);
const char* skip_space(const char* curr);
const char* skip_until(const char* curr, char ch);

//== CALCULATIONS ===========================================================================//
double eval (node_t* node);
node_t* diff(node_t* node, FILE* tex_stream, stack_t* roots_stack, stack_t* subs_stack);

//== OPTIMISATION ===========================================================================//
void optimize(node_t* node, FILE* html_stream);
size_t const_folding(node_t* node);

size_t remove_neutral_elems(node_t** node);
size_t ADD_optimisation(node_t** node);
size_t MUL_optimisation(node_t** node);
size_t SUB_optimisation(node_t** node);
size_t POW_optimisation(node_t** node);

//== SECONDARY FUNCTIONS ====================================================================//
node_t* copy_tree(node_t* root);
bool check_vars(node_t* node);

node_t* GetG();
node_t* GetE();
node_t* GetT();
node_t* GetP();
node_t* GetN();

#endif
