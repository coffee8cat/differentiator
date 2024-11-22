#define LEFT  eval(node -> left)
#define RIGHT eval(node -> right)

#define _NUM(n) new_node(NUM, n, NULL, NULL)
#define _VAR(x) new_node(VAR, x, NULL, NULL)

#define _ADD(a, b) new_node(OP, ADD, a, b)
#define _SUB(a, b) new_node(OP, SUB, a, b)
#define _MUL(a, b) new_node(OP, MUL, a, b)
#define _DIV(a, b) new_node(OP, DIV, a, b)
#define _COS(a)    new_node(OP, COS, a, NULL)
#define _SIN(a)    new_node(OP, SIN, a, NULL)
#define _LOG(a)    new_node(OP, LOG, a, NULL)

#define dL  diff (node -> left)
#define dR  diff (node -> right)
#define cL  copy_tree (node -> left)
#define cR  copy_tree (node -> right)
