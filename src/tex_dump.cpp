#include "tex_dump.h"

FILE* prepare_TEX_stream()
{
    FILE* fp = fopen("data\\tex_log.tex", "w");
    if (fp == NULL)
    {
        fprintf(stderr, "ERROR: Cannot open tex file fot dump\n");
        return NULL;
    }
    tex_intro(fp);

    return fp;
}

void tex_intro(FILE* fp)
{
    assert(fp);

    fprintf(fp,
    "\\documentclass[12pt, letterpaper, twoside]{article}\n"
    "\\usepackage[utf8]{inputenc}\n\n"

    "\\title{The differentiation game}\n"
    "\\author{Novoselov Alexander \\thanks{funded by the 434 team}}\n"
    "\\date{November 2024}\n\n"

    "\\begin{document}\n\n"

    "\\maketitle\n\n");
}

void close_TEX_stream(FILE* fp)
{
    assert(fp);

    tex_ending(fp);
    if (fclose(fp)) {fprintf(stderr, "ERROR: Could not close TEX stream\n");}
}

void tex_ending(FILE* fp)
{
    assert(fp);

    fprintf(fp,
    "\n\n\\end{document}");
}

#define WRITE_NODE(node, vars_table, tex_stream, roots_stack, subs_stack, layer)                                        \
    if ((layer > 4) && (count_nodes(node) > 4)) { add_substitution(node, tex_stream, roots_stack, subs_stack); }        \
    else           { write_node(node, vars_table, tex_stream, roots_stack, subs_stack, layer+1); }                      \

#define DEF_OPER(oper, eval, diff, dump_name, ...) dump_name,
const char* oper_dump_names[] {
    #include "diff_rules_DSL.h"
    "UNKNOWN"
};
#undef DEF_OPER

void write_node(node_t* node, variable* vars_table, FILE* tex_stream, stack_t* roots_stack, stack_t* subs_stack, size_t layer)
{
    assert(tex_stream);
    if (!node) { return; }

    if (node -> type == NUM) { fprintf(tex_stream, "%lg", node -> value.num); }
    if (node -> type == VAR) { fprintf(tex_stream, "%.*s", vars_table[node -> value.var].name_len, vars_table[node -> value.var].name);  }
    if (node -> type == OP)
    {
        if (node -> value.op== DIV)
        {
            fprintf(tex_stream, "\\frac{");
            WRITE_NODE (node -> left,  vars_table, tex_stream, roots_stack, subs_stack, layer+1);
            fprintf(tex_stream, "}{");
            WRITE_NODE (node -> right, vars_table, tex_stream, roots_stack, subs_stack, layer+1);
            fprintf(tex_stream, "}");
        }
        else
        {
            if (node -> left)
            {
                fprintf(tex_stream, "{(");
                WRITE_NODE (node -> left, vars_table, tex_stream, roots_stack, subs_stack, layer+1);
                fprintf(tex_stream, ")}");
            }
            fprintf(tex_stream, "%s", oper_dump_names[node -> value.op]);
            if (node -> right)
            {
                fprintf(tex_stream, "{(");
                WRITE_NODE (node -> right, vars_table, tex_stream, roots_stack, subs_stack, layer+1);
                fprintf(tex_stream, ")}");
            }
        }
    }
}

#undef WRITE_NODE

size_t count_nodes(node_t* node)
{
    if (!node) { return 0;}
    else       { return count_nodes(node -> left) + count_nodes(node -> right) + 1; }
}

void add_substitution(node_t* node, FILE* tex_stream, stack_t* roots_stack, stack_t* subs_stack)
{
    if (!node) { return; }
    assert(tex_stream);
    assert(roots_stack);
    assert(subs_stack);

    static char sub = 'A';
    if (sub > 'Z') { sub = 'A'; }
    fprintf(tex_stream, "%c", sub);
    printf("ADDING SUBSTITUTION: [%p] = %c(%d)\n", node, sub, sub);
    stack_push(roots_stack, &node);
    stack_push(subs_stack,  &sub);
    sub++;
}

void write_substitutions(FILE* tex_stream, variable* vars_table, stack_t* roots_stack, stack_t* subs_stack)
{
    assert(tex_stream);
    assert(roots_stack);
    assert(subs_stack);

    fprintf(tex_stream, "\nWhere ");
    while (roots_stack -> size)
    {
        substitution temp = {};
        stack_pop(roots_stack, &temp.root);
        stack_pop(subs_stack,  &temp.label);

        printf("%c = %p\n", temp.label, temp.root);
        fprintf(tex_stream, "\n\\[%c = ", temp.label);

        write_node(temp.root, vars_table, tex_stream, roots_stack, subs_stack, 1);
        fprintf(tex_stream, "\\]\n");
        printf("sub written succesfully\n");

    }
    fprintf(tex_stream, "\n");
}
