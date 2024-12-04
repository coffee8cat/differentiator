#include "diff_tree.h"
#include "tree_dump.h"
#include "tex_dump.h"

const double Global_x = 2.0;

#define _NUM(n) new_node(NUM, n, NULL, NULL)
#define _VAR(x) new_node(VAR, x, NULL, NULL)

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

node_t* read_node(const char** curr, const char* end, FILE* html_stream)
{
    assert(end);
    assert(curr);
    assert(*curr);
    assert(html_stream);

    if (*curr >= end) { fprintf(stderr, "STRING END REACHED\n"); return NULL; }
    *curr = skip_space(*curr);
    printf("%s\n", *curr);

    //== HANDLE TERMINAL NODES ===================================================//
    if (isdigit(**curr))
    {
        double num = atof(*curr);

        printf("ADDING NUM NODE: %lf\n", num);
        return _NUM(num);
    }

    node_t* first_arg = NULL;
    if (**curr != '{')
    {                                       // here check if it is in var list, add if yes, else skip
        *curr = skip_space(*curr);
        if (**curr == 'x')
        {
            char var = **curr;
            return _VAR('x'); // create var node
        }
    }
    else
    {
        (*curr)++;
        first_arg = read_node(curr, end, html_stream);
        *curr = skip_until(*curr, '}');
        (*curr)++;
    }

    *curr = skip_space(*curr);
    printf("%s\n", *curr);
    enum operations func = read_func_name(curr);
    node_t* op_node = new_node(OP, func, NULL, NULL);

    *curr = skip_space(*curr);
    printf("%s\n", *curr);
    node_t* second_arg = NULL;
    if (**curr == '{')
    {
        (*curr)++;
        second_arg = read_node(curr, end, html_stream);
        *curr = skip_until(*curr, '}');
        (*curr)++;
    }

    op_node -> left  = first_arg;
    op_node -> right = second_arg;

    return op_node;
}

enum operations read_func_name(const char** curr)
{
    assert(curr);
    assert(*curr);

    #define DEF_OPER(oper, eval_rule, diff, call_name)          \
    if (strncmp(*curr, call_name, strlen(call_name)) == 0)      \
    {                                                           \
        printf("oper: %s\n", call_name);                        \
        *curr = *curr + strlen(call_name);                      \
        return oper;                                            \
    }                                                           \

    #include "diff_rules_DSL.h"

    #undef DEF_OPER

    fprintf(stderr, "ERROR: Unknown function: %s", *curr);
    return UNKNOWN;
}

const char* skip_space(const char* curr)
{
    assert(curr);
    printf("skipping space start on %c(%d)[%p] ", *curr, *curr, curr);
    while(isspace(*curr)) { curr++; }
    printf("end on %c(%d)[%p]\n", *curr, *curr, curr);
    return curr;
}

const char* skip_until(const char* curr, char ch)
{
    assert(curr);
    while(*curr != ch) { curr++; }

    return curr;
}

double eval(node_t* node)
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

node_t* diff(node_t* node, FILE* tex_stream, stack_t* roots_stack, stack_t* subs_stack)
{
    assert(node);

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
        node -> value = eval(node);
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
    if (!node) { return 0; }

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

size_t ADD_optimisation(node_t** node)
{
    assert(node);
    assert(*node);

    size_t opt_counter = 0;
    if      (((*node) -> left)  -> type == NUM && ((*node) -> left)  -> value == 0) { REPLACE_WITH(right); }
    else if (((*node) -> right) -> type == NUM && ((*node) -> right) -> value == 0) { REPLACE_WITH(left);  }

    return opt_counter;
}

size_t SUB_optimisation(node_t** node)
{
    assert(node);
    assert(*node);

    size_t opt_counter = 0;
    if (((*node) -> right) -> type == NUM && ((*node) -> right) -> value == 0) { REPLACE_WITH(left); }

    return opt_counter;
}

size_t MUL_optimisation(node_t** node)
{
    assert(node);
    assert(*node);

    size_t opt_counter = 0;
    if (((*node) -> left)  -> type == NUM)
    {
        if      (((*node) -> left) -> value == 0) { REPLACE_WITH(left);  }
        else if (((*node) -> left) -> value == 1) { REPLACE_WITH(right); }

    }
    else if (((*node) -> right) -> type == NUM)
    {
        if      (((*node) -> right) -> value == 0) { REPLACE_WITH(right); }
        else if (((*node) -> right) -> value == 1) { REPLACE_WITH(left);  }
    }

    return opt_counter;
}

size_t POW_optimisation(node_t** node)
{
    assert(node);
    assert(*node);

    size_t opt_counter = 0;
    if (((*node) -> left)  -> type == NUM)
    {
        if (((*node) -> left) -> value == 0 || ((*node) -> left) -> value == 1) { REPLACE_WITH(left); }
    }
    else if (((*node) -> right) -> type == NUM)
    {
        if      (((*node) -> right) -> value == 0)
        {
            ((*node) -> right) -> type  = NUM;
            ((*node) -> right) -> value = 1;
            REPLACE_WITH(right);
        }
        else if (((*node) -> right) -> value == 1) { REPLACE_WITH(left); }
    }

    return opt_counter;
}

#undef REPLACE_WITH

node_t* copy_tree(node_t* root)
{
    if (!root) { return NULL; }

    node_t* node = new_node(root -> type, root -> value, NULL, NULL);
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

int tree_dtor(node_t* node)
{
    if (!node) { return 0; }
    tree_dtor(node -> left);
    tree_dtor(node -> right);
    free(node);

    return 0;
}
