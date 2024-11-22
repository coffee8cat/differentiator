#include "diff_tree.h"
#include "tree_dump.h"

const double Global_x = 2.0;

#define _NUM(n) new_node(NUM, n, NULL, NULL)
#define _VAR(x) new_node(VAR, x, NULL, NULL)
/*
#define _ADD(a, b) new_node(OP, ADD, a, b)
#define _MUL(a, b) new_node(OP, MUL, a, b)

#define dL  diff (node -> left)
#define dR  diff (node -> right)
#define cL  copy_tree (node -> right)
#define cR  copy_tree (node -> right)
*/
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

int read_tree(node_t** node, FILE* stream, FILE* html_stream)
{
    assert(node);
    assert(stream);
    assert(html_stream);

    char ch = 0;
    node_t* temp = NULL;
    fscanf(stream, " %c", &ch);
    switch (ch)
    {
        case '{':
        {
            *node = read_node(stream);
            read_tree(node, stream, html_stream);
            break;
        }
        case 'y':
        {
            skip_until('{', stream);

            if (((*node) -> right = read_node(stream)) == NULL) { fprintf(stderr, "ERROR: Uploading tree failed\n"); return -1;}

            read_tree(&((*node) -> right), stream, html_stream);
        }
        case 'n':
        {
            skip_until('{', stream);
            if (((*node) -> left = read_node(stream)) == NULL) { fprintf(stderr, "ERROR: Uploading tree failed\n"); return -1;}

            read_tree(&((*node) -> left), stream, html_stream);
            break;
        }
        case '}': {return 0;}
        default:{fprintf(stderr, "Unknown option: %c\n", ch); break;}
    }
    return 0;
}

node_t* read_node(FILE* stream)
{
    assert(stream);

    char label[default_str_size] = {};
    fscanf(stream, " \"%[^\"]\"", label);

    return new_node(OP, 0, NULL, NULL);
}

void skip_until(char ch, FILE* stream)
{
    assert(stream);

    char temp = 0;
    while(temp != ch)
        fscanf(stream, "%c", &temp);
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


node_t* diff(node_t* node)
{
    assert(node);

    // !!! not for partial derivatives yet !!!
    if (node -> type == NUM) { return _NUM(0);}
    if (node -> type == VAR) { return _NUM(1);}
    if (node -> type == OP)
    {
        #define DEF_OPER(oper, eval_rule, diff_rule) case oper: return diff_rule;
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
        opt_counter += const_folding(node);
        opt_counter += remove_neutral_elems(&node);

        tree_dump(node, html_stream, node);
    }

}

size_t const_folding(node_t* node)
{
    assert(node);

    size_t opt_counter = 0;

    if (node -> type == OP && !check_vars(node))
    {
        node -> value = eval(node);
        printf("EVALUATION: [%p] <- %lf", node -> value);
        node -> type  = NUM;
        node -> left  = NULL; node -> right = NULL;
        opt_counter++;
    }
    if (node -> left)  { opt_counter += const_folding(node -> left);  }
    if (node -> right) { opt_counter += const_folding(node -> right); }

    return opt_counter;
}

size_t remove_neutral_elems(node_t** node)
{
    assert(node);

    size_t opt_counter = 0;
    if ((*node) -> type == OP)
    {
        switch((int)(*node) -> value)
        {
            case ADD: opt_counter += ADD_optimisation(node); printf("ADD OPTIMISATION\n"); break;
            case SUB: opt_counter += SUB_optimisation(node); printf("SUB OPTIMISATION\n"); break;
            case MUL: opt_counter += MUL_optimisation(node); printf("MUL OPTIMISATION\n"); break;
            //case POW: opt_counter += POW_optimisation(&node); break;
            default: break;
        }
    }

    if ((*node) -> left)  { opt_counter += remove_neutral_elems(&(*node) -> left);  }
    if ((*node) -> right) { opt_counter += remove_neutral_elems(&(*node) -> right); }

    return opt_counter;
}

size_t ADD_optimisation(node_t** node)
{
    assert(node);
    assert(*node);

    size_t opt_counter = 0;
    if      (((*node) -> left)  -> type == NUM && ((*node) -> left)  -> value == 0) { *node = (*node) -> right; opt_counter++; }
    else if (((*node) -> right) -> type == NUM && ((*node) -> right) -> value == 0) { *node = (*node) -> left;  opt_counter++; }

    return opt_counter;
}

size_t SUB_optimisation(node_t** node)
{
    assert(node);
    assert(*node);

    size_t opt_counter = 0;
    if (((*node) -> right) -> type == NUM && ((*node) -> right) -> value == 0) { *node = (*node) -> left;  opt_counter++; }

    return opt_counter;
}

size_t MUL_optimisation(node_t** node)
{
    assert(node);
    assert(*node);

    size_t opt_counter = 0;
    if (((*node) -> left)  -> type == NUM)
    {
        if      (((*node) -> left) -> value == 0) { *node = (*node) -> left;  opt_counter++; }
        else if (((*node) -> left) -> value == 1) { *node = (*node) -> right; opt_counter++; }

    }
    else if (((*node) -> right) -> type == NUM)
    {
        if      (((*node) -> right) -> value == 0) { *node = (*node) -> right; opt_counter++; }
        else if (((*node) -> right) -> value == 1) { *node = (*node) -> left;  opt_counter++; }
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
        if      (((*node) -> left) -> value == 0) { *node = (*node) -> left; opt_counter++; }
        else if (((*node) -> left) -> value == 1) { *node = (*node) -> left; opt_counter++; }

    }
    else if (((*node) -> right) -> type == NUM)
    {
        if      (((*node) -> right) -> value == 0) { *node = _NUM(1);         opt_counter++; }
        else if (((*node) -> right) -> value == 1) { *node = (*node) -> left; opt_counter++; }
    }

    return opt_counter;
}

node_t* copy_tree(node_t* root)
{
    assert(root);
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
