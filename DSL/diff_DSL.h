#define LEFT  evaluate_tree(node -> left,  vars_table)
#define RIGHT evaluate_tree(node -> right, vars_table)

#define _ADD(a, b) new_node(OP, OP_VALUE(ADD), a,    b)
#define _SUB(a, b) new_node(OP, OP_VALUE(SUB), a,    b)
#define _MUL(a, b) new_node(OP, OP_VALUE(MUL), a,    b)
#define _DIV(a, b) new_node(OP, OP_VALUE(DIV), a,    b)
#define _COS(b)    new_node(OP, OP_VALUE(COS), NULL, b)
#define _SIN(b)    new_node(OP, OP_VALUE(SIN), NULL, b)
#define _LOG(b)    new_node(OP, OP_VALUE(LN),  NULL, b)
#define _POW(a, b) new_node(OP, OP_VALUE(POW), a,    b)

#define OP_VALUE(oper)  node_value {.op = (oper)}
#define NUM_VALUE(n)    node_value {.num = (n)}
#define VAR_VALUE(x)    node_value {.var = (x)}

#define _NUM(n) new_node(NUM, NUM_VALUE(n), NULL, NULL)
#define _VAR(x) new_node(VAR, VAR_VALUE(x), NULL, NULL)

#define dL  differentiate_tree (node -> left,  vars_table, var_num, tex_stream, roots_stack, subs_stack)
#define dR  differentiate_tree (node -> right, vars_table, var_num, tex_stream, roots_stack, subs_stack)
#define cL  copy_tree (node -> left)
#define cR  copy_tree (node -> right)
