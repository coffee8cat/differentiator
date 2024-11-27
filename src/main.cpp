#include "diff_tree.h"
#include "tree_dump.h"
#include "tex_dump.h"

int main()
{

    #define DUMP_TO_TEX(node, tex_stream, roots_stack, subs_stack)          \
        write_node(node, tex_stream, roots_stack, subs_stack, 'a', 1);         \
        write_substitutions(tex_stream, roots_stack, subs_stack);           \

    FILE* html_stream = prepare_to_dump();
    if (html_stream == NULL)
        return -1;

    FILE* tex_stream = prepare_TEX_stream();
    if (html_stream == NULL)
        return -1;

    stack_t roots_stack = {};
    stack_t subs_stack  = {};

    stack_init(&roots_stack, 16, sizeof(node_t*));
    stack_init(&subs_stack,  16, sizeof(node_t*));

    //const char* buff = "{12} + { {ln{5}} * {x}}";
    const char* buff = "{{x} / {4}} ^ {x}";
    node_t* node1 = read_node(&buff, buff + strlen(buff), html_stream);
    tree_dump(node1, html_stream, node1);

    node_t* diff_root1 = diff(node1, tex_stream, &roots_stack, &subs_stack);
    printf("%p\n\n", diff_root1);
    tree_dump(diff_root1, html_stream, diff_root1);

    optimize(diff_root1, html_stream);
    printf("OPTIMISATION END, diff_root: [%p]\n", diff_root1);

    tree_dump(diff_root1, html_stream, diff_root1);

    fprintf(tex_stream, "$");
    //DUMP_TO_TEX(diff_root1, tex_stream, &roots_stack, &subs_stack);

    STACK_DUMP(&roots_stack, __func__);
    STACK_DUMP(&subs_stack,  __func__);

    write_node(diff_root1, tex_stream, &roots_stack, &subs_stack, 'a', 1);
    write_substitutions(tex_stream, &roots_stack, &subs_stack);


    fprintf(tex_stream, "$");

    close_TEX_stream(tex_stream);
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
