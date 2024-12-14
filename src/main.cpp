#include "diff_tree.h"
#include "tree_dump.h"
#include "tex_dump.h"
#include "syntax_analysis.h"
#include "lexical_analysis.h"

int main()
{
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

    const char* buff = "(\\cos(15*x^3))+((\\sin(12*x+3))^4)$";

    lexeme_t* cmds = string_to_lexems(buff, vars_table);
    ask_variables_values(vars_table);

    size_t curr = 0;
    node_t* node = GetG(cmds, &curr, html_stream);
    tree_dump(node, vars_table, html_stream, node);

    DIFF_INTRO(node, tex_stream, roots_stack, subs_stack);

    node_t* diff_root = NULL;
    DIFFERENTIATE(diff_root, vars_table, tex_stream, html_stream, roots_stack, subs_stack);
    DIFF_INTRO(node, tex_stream, roots_stack, subs_stack);
    FINAL_ANSWER(diff_root, tex_stream, roots_stack, subs_stack);

    printf("evaluation result: %lf\n", evaluate_tree(node, vars_table));
    printf("diff evaluate_tree: %lf\n", evaluate_tree(diff_root, vars_table));

    STACK_DUMP(&roots_stack, __func__);
    STACK_DUMP(&subs_stack,  __func__);


    tree_dtor(node);
    tree_dtor(diff_root);

    close_TEX_stream(tex_stream);
    fclose(html_stream);
    return 0;
}
