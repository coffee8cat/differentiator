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

#define WRITE_NODE(node, tex_stream, roots_stack, subs_stack, sub, layer)               \
    if (layer > 3)                                                                      \
    {                                                                                   \
        add_substitution(node, roots_stack, subs_stack, sub);                           \
        fprintf(tex_stream, "%c", sub);                                                 \
        sub++;                                                                          \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        sub = write_node(node, tex_stream, roots_stack, subs_stack, sub, layer+1);      \
    }                                                                                   \

#define DEF_OPER(oper, eval, diff, dump_name, ...) dump_name,
const char* oper_dump_names[] {
    #include "diff_rules_DSL.h"
    "UNKNOWN"
};
#undef DEF_OPER

char write_node(node_t* node, FILE* tex_stream, stack_t* roots_stack, stack_t* subs_stack, char sub, size_t layer)
{
    assert(tex_stream);
    if (!node) { return sub; }

    if (node -> type == NUM) { fprintf(tex_stream, "%lf", node -> value);       return sub; }
    if (node -> type == VAR) { fprintf(tex_stream, "%c", (char)node -> value);  return sub; }
    if (node -> type == OP)
    {
        if (node -> value == DIV)
        {
            fprintf(tex_stream, "\\frac{");
            WRITE_NODE (node -> left,  tex_stream, roots_stack, subs_stack, sub, layer+1);
            fprintf(tex_stream, "}{");
            WRITE_NODE (node -> right, tex_stream, roots_stack, subs_stack, sub, layer+1);
            fprintf(tex_stream, "}");
        }
        else
        {
            fprintf(tex_stream, "(");
            WRITE_NODE (node -> left,  tex_stream, roots_stack, subs_stack, sub, layer+1);
            fprintf(tex_stream, "%s", oper_dump_names[(size_t)node -> value]);
            WRITE_NODE (node -> right, tex_stream, roots_stack, subs_stack, sub, layer+1);
            fprintf(tex_stream, ")");
        }
    }

    printf("sub = %c, ferror: %d\n", sub, ferror(tex_stream));
    return sub;
}

#undef WRITE_NODE

size_t count_nodes(node_t* node)
{
    if (!node) { return 0;}
    else       { return count_nodes(node -> left) + count_nodes(node -> right) + 1; }
}

void add_substitution(node_t* node, stack_t* roots_stack, stack_t* subs_stack, char sub)
{
    assert(node);
    assert(roots_stack);
    assert(subs_stack);

    double sub1 = (double)sub;
    printf("ADDING SUBSTITUTION: [%p] = %c(%lf)\n", node, sub, sub1);
    stack_push(roots_stack, &node);
    stack_push(subs_stack,  &sub1);
}

void write_substitutions(FILE* tex_stream, stack_t* roots_stack, stack_t* subs_stack)
{
    assert(tex_stream);
    assert(roots_stack);
    assert(subs_stack);

    fprintf(tex_stream, "\nWhere ");
    for (size_t i = roots_stack -> size; i > 0; i--)
    {
        node_t* root = NULL;
        double sub = 0;
        stack_pop(roots_stack, &root);
        stack_pop(subs_stack,  &sub);

        printf("root = %p\n", root);
        fprintf(tex_stream, "\n%c = ", (char)sub);
        printf("%c[%lf]\n", (char)sub, sub);

        write_node(root, tex_stream, roots_stack, subs_stack, 'a', 1);
        printf("sub written succesfully\n");

    }
    fprintf(tex_stream, "\n");
}
