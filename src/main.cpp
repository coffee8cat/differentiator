#include "diff_tree.h"
#include "tree_dump.h"
#include "tex_dump.h"
#include "syntax_analysis.h"
#include "lexical_analysis.h"

int main()
{
    #define DUMP_TO_TEX(node, tex_stream, roots_stack, subs_stack)          \
        fprintf(tex_stream, "\\[");                                         \
        write_node(node, vars_table, tex_stream, roots_stack, subs_stack, 1);           \
        fprintf(tex_stream, "\\]\n");                                       \
        write_substitutions(tex_stream, vars_table, roots_stack, subs_stack);           \

    FILE* html_stream = prepare_to_dump();
    if (html_stream == NULL)
    {
        fprintf(stderr, "!!! HTML STREAM ERROR !!!\n");
        return -1;
    }

    FILE* tex_stream = prepare_TEX_stream();
    if (html_stream == NULL)
    {
        fprintf(stderr, "!!! TEX STREAM ERROR !!!\n");
        return -1;
    }

    variable* vars_table = prepare_vars_table();
    if (!vars_table) { return -1; }

    stack_t roots_stack = {};
    stack_t subs_stack  = {};

    stack_init(&roots_stack, 16, sizeof(node_t*));
    stack_init(&subs_stack,  16, sizeof(node_t*));

    const char* buff = "(0-length+size^0)/(2*(length-2)^2*(curiosity-1)^1)$";

    //const char* buff = "(length^2+2*size)$";
    lexeme_t* cmds = string_to_lexems(buff, vars_table);

    dump_variables_table(vars_table);
    ask_variables_values(vars_table);

    for (size_t i = 0; i < 50; i++)
    {
        printf("%4d: %d\nvalue = %lg(%c)\n", i,
        cmds[i].type,
        cmds[i].value,
        (char)cmds[i].value);
    }

    size_t curr = 0;
    node_t* node = GetG(cmds, &curr, html_stream);
    tree_dump(node, vars_table, html_stream, node);

    fprintf(tex_stream, "\nGentlemen, it's a great pleasure to inform you that today we are differentiating\n");
    DUMP_TO_TEX(node, tex_stream, &roots_stack, &subs_stack);
    printf("evaluation result: %lg\n", evaluate_tree(node, vars_table));


    fprintf(tex_stream, "\nThere we begin...\n");
    node_t* diff_root = differentiate_tree(node, vars_table, 0, tex_stream, &roots_stack, &subs_stack);
    printf("%p\n\n", diff_root);
    tree_dump(diff_root, vars_table, html_stream, diff_root);

    optimize(diff_root, vars_table, html_stream, tex_stream, &roots_stack, &subs_stack);
    printf("OPTIMISATION END, diff_root: [%p]\n", diff_root);

    tree_dump(diff_root, vars_table, html_stream, diff_root);


    fprintf(tex_stream, "\nFinally, after a long battle, we emerged victorious:\n");
    DUMP_TO_TEX(diff_root, tex_stream, &roots_stack, &subs_stack);

    STACK_DUMP(&roots_stack, __func__);
    STACK_DUMP(&subs_stack,  __func__);

    printf("diff evaluate_tree: %lf\n", evaluate_tree(diff_root, vars_table));

    tree_dtor(node);
    tree_dtor(diff_root);

    close_TEX_stream(tex_stream);
    fclose(html_stream);
    return 0;
}
