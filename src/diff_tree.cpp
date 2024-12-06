#include "diff_tree.h"
#include "tree_dump.h"
#include "tex_dump.h"

const double Global_x = 2.0;

#define _NUM(n) new_node(NUM, n, NULL, NULL)
#define _VAR(x) new_node(VAR, x, NULL, NULL)

variable* prepare_vars_table()
{
    variable* vars_table = (variable*)calloc(max_num_variables, sizeof(variable));
    if (!vars_table) { fprintf(stderr, "ERROR: Calloc for vars_table failed\n"); }
    return vars_table;
}

size_t add_variable(variable* vars_table, const char* name, size_t name_length)
{
    assert(vars_table);
    assert(name);

    printf("vars_table on %p\n", vars_table);
    size_t position = lookup(vars_table, name, name_length);
    printf("position = %d", position);
    printf("name: %p; starts with %c\n", name, *name);
    printf("name_length = %d\n", name_length);
    if (vars_table[position].name == NULL)
    {
        vars_table[position].name      = name;
        vars_table[position].name_len  = name_length;
    }
    dump_variables_table(vars_table);
    return position;
}

void ask_variables_values(variable* vars_table)
{
    assert(vars_table);

    printf("Enter variables values\n");
    size_t i = 0;
    while (vars_table[i].name && i < max_num_variables)
    {
        printf ("var%d: name: %.*s\n",
                i, vars_table[i].name_len, vars_table[i].name, vars_table[i].name_len, vars_table[i].value);

        scanf("%lg", &vars_table[i].value);
        i++;
    }
    printf("\nAll values have been read succesfully\n");

    dump_variables_table(vars_table);
}

size_t lookup(variable* vars_table, const char* name, size_t name_length)
{
    assert(vars_table);
    assert(name);

    size_t curr = 0;
    while (vars_table[curr].name && curr < max_num_variables)
    {
        if (strncmp(name, vars_table[curr].name, name_length) == 0)
            break;

        curr++;
    }

    return curr;
}

void dump_variables_table(variable* vars_table)
{
    assert(vars_table);

    printf("---VARIABLES TABLE DUMP---\n");
    size_t i = 0;
    printf("var in %p\n", &vars_table[i]);
    while (vars_table[i].name && i < max_num_variables)
    {
        printf ("var%d: name: [%.*s] len = %d value: [%lg]\n",
                i, vars_table[i].name_len, vars_table[i].name, vars_table[i].name_len, vars_table[i].value);
        i++;
        printf("var in %p\n", &vars_table[i]);
    }
    printf("\n--- END ---\n");
}

node_t* new_node(node_type type, double value, node_t* left, node_t* right)
{
    node_t* node = (node_t*)calloc(1, sizeof(node_t));
    if (!node)
    {
        fprintf(stderr, "ERROR: Calloc returned NULL in new_node()");
        return NULL;
    }
    node -> type  = type;
    node -> value = value;
    node -> left  = left;
    node -> right = right;

    return node;
}

void fix_parents(node_t* node)
{
    assert(node);
    if (node -> right) {(node -> right) -> parent = node;}
    if (node -> left)  {(node -> left) ->  parent = node;}
}

void fix_tree(node_t* node)
{
    assert(node);
    fix_parents(node);
    if (node -> right) { fix_parents(node -> right); }
    if (node -> left)  { fix_parents(node -> left);  }
}

void tree_dtor(node_t* node)
{
    if (!node) { return; }
    tree_dtor(node -> left);
    tree_dtor(node -> right);
    free(node);
}

double evaluate_tree(node_t* node)
{
    assert(node);

    if (node -> type == NUM) { return node -> value; }
    if (node -> type == VAR) { return Global_x; }
    if (node -> type == OP)
    {
        #define DEF_OPER(oper, eval_rule, ...) case oper: return eval_rule;
        switch ((int)node -> value)
        {
            #include "diff_rules_DSL.h"
            default: fprintf(stderr, "ERROR: No such operation - [%lf]", node -> value);
                     break;
        }
        #undef DEF_OPER
    }

    return -1;
}

node_t* differentiate_tree(node_t* node, FILE* tex_stream, stack_t* roots_stack, stack_t* subs_stack)
{
    assert(node);
    assert(tex_stream);
    assert(roots_stack);
    assert(subs_stack);

    // !!! not for partial derivatives yet !!!
    if (node -> type == NUM) { return _NUM(0);}
    if (node -> type == VAR) { return _NUM(1);}
    if (node -> type == OP)
    {
        #define DEF_OPER(oper, eval_rule, diff_rule, ...) case oper:            \
        {                                                                       \
            node_t* node_diff = NULL;                                           \
            diff_rule                                                           \
                                                                                \
            fprintf(tex_stream, "\\[\\frac{d}{dx}");                            \
            write_node (node, tex_stream, roots_stack, subs_stack, 1);          \
            fprintf(tex_stream, " = " );                                        \
            write_node (node_diff, tex_stream, roots_stack, subs_stack, 1);     \
            fprintf(tex_stream, "\\]\n\n");                                     \
            write_substitutions(tex_stream, roots_stack, subs_stack);           \
                                                                                \
            return node_diff;                                                   \
        }                                                                       \

        switch ((int)node -> value)
        {
            #include "diff_rules_DSL.h"
            default: fprintf(stderr, "ERROR: Unknown operation for differentiator - [%lf]", node -> value);
                     break;
        }
        #undef DEF_OPER
    }

    return NULL;
}

void optimize(node_t* node, FILE* html_stream)
{
    assert(node);
    assert(html_stream);

    size_t opt_counter = 1;
    while (opt_counter > 0)
    {
        opt_counter = 0;
        printf("CONST FOLDING START\n");
        opt_counter += const_folding(node);
        printf("REMOVING NEUTRAL ELEMS START\n");
        opt_counter += remove_neutral_elems(&node);
        printf("DUMP\n");
        tree_dump(node, html_stream, node);
    }
}

size_t const_folding(node_t* node)
{
    if (!node) { return 0; }

    size_t opt_counter = 0;
    if (node -> left)  { opt_counter += const_folding(node -> left);  }
    if (node -> right) { opt_counter += const_folding(node -> right); }

    if (node -> type == OP && !check_vars(node))
    {
        node -> value = evaluate_tree(node);
        node -> type  = NUM;

        tree_dtor(node -> left);  node -> left  = NULL;
        tree_dtor(node -> right); node -> right = NULL;
        opt_counter++;

        printf("EVALUATION: [%p] <- %lf", node -> value);
    }

    return opt_counter;
}

size_t remove_neutral_elems(node_t** node)
{
    if (!node || !(*node)) { return 0; }

    size_t opt_counter = 0;
    if ((*node) -> left)  { opt_counter += remove_neutral_elems(&(*node) -> left);  }
    if ((*node) -> right) { opt_counter += remove_neutral_elems(&(*node) -> right); }

    if ((*node) -> type == OP)
    {
        switch((int)(*node) -> value)
        {
            case ADD: opt_counter += ADD_optimisation(node); printf("ADD OPTIMISATION\n"); break;
            case SUB: opt_counter += SUB_optimisation(node); printf("SUB OPTIMISATION\n"); break;
            case MUL: opt_counter += MUL_optimisation(node); printf("MUL OPTIMISATION\n"); break;
            case POW: opt_counter += POW_optimisation(node); printf("POW OPTIMISATION\n"); break;
            default: break;
        }
    }

    return opt_counter;
}

#define REPLACE_WITH(son)       \
    node_t* temp = *node;       \
    *node = (*node) -> son;     \
                                \
    temp -> son = NULL;         \
    tree_dtor(temp);            \
    opt_counter++              \

#define IS_NUM(direction, x) (((*node) -> direction) -> type == NUM && ((*node) -> direction) -> value == (x))

size_t ADD_optimisation(node_t** node)
{
    assert(node);
    assert(*node);

    size_t opt_counter = 0;
    if      (IS_NUM(left,  0)) { REPLACE_WITH(right); }
    else if (IS_NUM(right, 1)) { REPLACE_WITH(left);  }

    return opt_counter;
}

size_t SUB_optimisation(node_t** node)
{
    assert(node);
    assert(*node);

    size_t opt_counter = 0;
    if (IS_NUM(right, 0)) { REPLACE_WITH(left); }

    return opt_counter;
}

size_t MUL_optimisation(node_t** node)
{
    assert(node);
    assert(*node);

    size_t opt_counter = 0;
    if      (IS_NUM(left,  0)) { REPLACE_WITH(left);  }
    else if (IS_NUM(left,  1)) { REPLACE_WITH(right); }
    else if (IS_NUM(right, 0)) { REPLACE_WITH(right); }
    else if (IS_NUM(right, 1)) { REPLACE_WITH(left);  }

    return opt_counter;
}

size_t POW_optimisation(node_t** node)
{
    assert(node);
    assert(*node);

    size_t opt_counter = 0;
    if (IS_NUM(left, 0) || IS_NUM(left, 1)) { REPLACE_WITH(left); }
    else if (IS_NUM(right, 0))
    {
        ((*node) -> right) -> type  = NUM;
        ((*node) -> right) -> value = 1;
        REPLACE_WITH(right);
    }
    else if (IS_NUM(right, 1)) { REPLACE_WITH(left); }

    return opt_counter;
}

#undef REPLACE_WITH
#undef IS_NUM

node_t* copy_tree(node_t* root)
{
    if (!root) { return NULL; }

    node_t* node = new_node(root -> type, root -> value, NULL, NULL); // check
    if (root -> left)  { node -> left  = copy_tree(root -> left);  }
    if (root -> right) { node -> right = copy_tree(root -> right); }

    return node;
}

bool check_vars(node_t* node)
{
    if (!node) { return false;}

    if (node -> type == VAR) { return true; }
    else {return (check_vars(node -> left) || check_vars(node -> right)); }
}

