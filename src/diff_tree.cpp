#include "diff_tree.h"

const double Global_x = 2.0;
#define _NUM(n) new_node(NUM, n, NULL, NULL)
#define _VAR(x) new_node(VAR, x, NULL, NULL)

#define _ADD(a, b) new_node(OP, ADD, a, b)
#define _MUL(a, b) new_node(OP, MUL, a, b)

#define dL  diff (node -> left)
#define dR  diff (node -> right)
#define cL  copy_tree (node -> right)
#define cR  copy_tree (node -> right)

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
        switch ((int)node -> value)
        {
            case ADD: return eval(node -> left) + eval(node -> right);
            case MUL: return eval(node -> left) * eval(node -> right);
            default: fprintf(stderr, "ERROR: No such operation - [%lf]", node -> value);
                     break;
        }
    }

    return -1;
}

node_t* diff(node_t* node)
{
    assert(node);

    // !!! not for partial derivatives yet !!!
    if (node -> type == NUM) { return new_node(NUM, 0, NULL, NULL);}
    if (node -> type == VAR) { return new_node(NUM, 1, NULL, NULL);}
    if (node -> type == OP)
    {
        switch ((int)node -> value)
        {
            case ADD: return _ADD (dL, dR);
            case MUL: return _ADD (_MUL (dL, cR), _MUL (cL, dR));
            //case DIV: return
            //case SUB:
            //case SIN:
            //case COS:
            //case POW:
            //case LOG:

            default: fprintf(stderr, "ERROR: Unknown operation for differentiator - [%lf]", node -> value);
                     break;
        }
    }

    return NULL;
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
