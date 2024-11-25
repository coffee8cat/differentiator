#include "diff_tree.h"
#include "tree_dump.h"

int main()
{

    FILE* html_stream = prepare_to_dump();
    if (html_stream == NULL)
        return -1;

    //const char* buff = "{12} + { {ln{5}} * {x}}";
    const char* buff = "{x} ^ {x}";
    node_t* node1 = read_node(&buff, buff + strlen(buff), html_stream);
    tree_dump(node1, html_stream, node1);

    node_t* diff_root1 = diff(node1);
    tree_dump(diff_root1, html_stream, diff_root1);

    optimize(diff_root1, html_stream);
    printf("OPTIMISATION END, diff_root: [%p]\n", diff_root1);

    tree_dump(diff_root1, html_stream, diff_root1);

    write_node(diff_root1);
    /*node_t* root  = new_node(OP, DIV, NULL, NULL);
    root -> left  = new_node(OP, POW, new_node(VAR, 'x', NULL, NULL), new_node(NUM, 2, NULL, NULL));
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
    tree_dtor(diff_root);*/

    return 0;
}
