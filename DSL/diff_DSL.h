#define LEFT  eval(node -> left)
#define RIGHT eval(node -> right)

#define _NUM(n) new_node(NUM, n, NULL, NULL)
#define _VAR(x) new_node(VAR, x, NULL, NULL)

#define _ADD(a, b) new_node(OP, ADD, a,    b)
#define _SUB(a, b) new_node(OP, SUB, a,    b)
#define _MUL(a, b) new_node(OP, MUL, a,    b)
#define _DIV(a, b) new_node(OP, DIV, a,    b)
#define _COS(b)    new_node(OP, COS, NULL, b)
#define _SIN(b)    new_node(OP, SIN, NULL, b)
#define _LOG(b)    new_node(OP, LN, NULL, b)
#define _POW(a, b) new_node(OP, POW, a,    b)

#define dL  diff (node -> left,  tex_stream, roots_stack, subs_stack)
#define dR  diff (node -> right, tex_stream, roots_stack, subs_stack)
#define cL  copy_tree (node -> left)
#define cR  copy_tree (node -> right)
