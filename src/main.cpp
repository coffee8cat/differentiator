#include "diff_tree.h"
#include "tree_dump.h"

int main()
{

    FILE* html_stream = prepare_to_dump();
    if (html_stream == NULL)
        return -1;

    node_t* root  = new_node(OP, DIV, NULL, NULL);
    root -> left  = new_node(OP, MUL, new_node(VAR, 'x', NULL, NULL), new_node(VAR, 'x', NULL, NULL));
    root -> right = new_node(NUM, 50, NULL, NULL);
    tree_dump(root, html_stream, root);

    printf("eval: %lf\n", eval(root));

    node_t* diff_root = diff(root);

    tree_dump(diff_root, html_stream, diff_root);
    printf("diff eval: %lf\n", eval(diff_root));
    optimize(diff_root, html_stream);
    printf("optimized: %p", diff_root);

    tree_dump(diff_root, html_stream, diff_root);

    printf("diff eval: %lf\n", eval(diff_root));
    tree_dtor(root);
    tree_dtor(diff_root);

    return 0;
}
