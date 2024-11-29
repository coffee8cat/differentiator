#include "tree_dump.h"


FILE* prepare_to_dump()
{
    system("make clean_dump\n");
    FILE* fp = fopen("list_dump.html", "w");
    if (fp == NULL)
    {
        fprintf(stderr, "ERROR: Cannot open html file fot dump\n");
        return NULL;
    }
    fprintf(fp, "<pre>\n");

    return fp;
}

int tree_dump(node_t* root, FILE* html_stream, node_t* curr)
{
    static size_t dump_counter = 1;
    char dot_file_name[BUFSIZ] = "";
    char png_file_name[BUFSIZ] = "";
    sprintf(dot_file_name, "dump\\dump%d.dot", dump_counter);
    sprintf(png_file_name, "dump\\dump%d.png", dump_counter);

    FILE* fp = fopen(dot_file_name, "w");
    if (fp == NULL)
    {
        fprintf(stderr, "ERROR: Unable to open file for dump\n");
        return -1;
    }

    make_dot_file(root, fp, curr);
    fclose(fp);

    char command[BUFSIZ] = "";
    sprintf(command, "dot %s -Tpng -o %s\n", dot_file_name, png_file_name);
    system(command);

    dump_to_html(root, png_file_name, html_stream, curr);

    dump_counter++;
    return 0;
}

int dump_to_html(node_t* root, char* png_file_name, FILE* html_stream, node_t* curr)
{
    assert(root);
    assert(png_file_name);

    fprintf(html_stream, "MEMORY PRINT\n");
    printf("\ncurr = %p\n", curr);

    fprintf(html_stream, "\n<img src=%s>\n", png_file_name);

    return 0;
}

int make_dot_file(node_t* root, FILE* fp, node_t* curr)
{
    assert(fp);
    assert(root);

    fprintf(fp, "digraph\n{\n"
                "    rankdir=TP\n\n"
                "    ");

    write_node_info(root, fp, curr);

    fprintf(fp, "}");

    printf("dot file completed\n");
    return 0;
}

#define DEF_OPER(oper, ...) #oper,
const char* operations[] {
    #include "diff_rules_DSL.h"
    "UNKNOWN"
};
#undef DEF_OPER

int write_node_info(node_t* node, FILE* fp, node_t* curr)
{
    assert(fp);

    if (!node) { return 1; }

    const char* color = NULL;
    char* value = (char*)calloc(16, sizeof(char));

    if (node -> type == OP)  { color = "#C51BEC"; strcpy(value, operations[(size_t)node -> value]);}
    if (node -> type == NUM) { color = "#1BECC5"; sprintf(value, "%lf", node -> value);}
    if (node -> type == VAR) { color = "#ECC51B"; sprintf(value, "x");}
    if (node == curr)        { color = "#BF62FC"; }


    fprintf(fp, "    node%p[shape=record,style=\"rounded,filled\",fillcolor=\"%s\",label=\"{ ptr: %p | type: %d | value: %s| { left: %p | right: %p }}\"];\n",
            node, color, node, node -> type, value, node -> left, node -> right);

    if (node -> left)
    {
        fprintf(fp, "    node%p -> node%p[color=\"#0855F0\"]\n", node, node -> left);
        write_node_info(node -> left, fp, curr);
    }
    if (node -> right)
    {
        fprintf(fp, "    node%p -> node%p[color=\"#0855F0\"]\n", node, node -> right);
        write_node_info(node -> right, fp, curr);
    }
    free(value);

    return 0;
}
