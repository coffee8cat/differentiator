#include "syntax_analysis.h"

#define _ADD(a, b) new_node(OP, OP_VALUE(ADD), a,    b)
#define _SUB(a, b) new_node(OP, OP_VALUE(SUB), a,    b)
#define _MUL(a, b) new_node(OP, OP_VALUE(MUL), a,    b)
#define _DIV(a, b) new_node(OP, OP_VALUE(DIV), a,    b)
#define _COS(b)    new_node(OP, OP_VALUE(COS), NULL, b)
#define _SIN(b)    new_node(OP, OP_VALUE(SIN), NULL, b)
#define _LOG(b)    new_node(OP, OP_VALUE(LN),  NULL, b)
#define _POW(a, b) new_node(OP, OP_VALUE(POW), a,    b)

#define OP_VALUE(oper) node_value {.op = (oper)}

#define GRAMMAR_DEBUG_PRINT() \
printf("in %s on %4d: %d\nvalue = %lg\n", __func__, *curr,              \
    lexeme_array[*curr].type,                                           \
    lexeme_array[*curr].value);                                         \

#define SYNTAX_ERROR(expected)                                                                  \
    fprintf(stderr, "\n\n!!! SYNTAX ERROR !!!\n"                                                \
                    "In %s:%d in %s on position %d expected <%d(%c)>, but <%d(%c)> found\n",    \
                    __FILE__, __LINE__, __func__, *curr, expected, expected,                    \
                    lexeme_array[*curr].value, lexeme_array[*curr].value)                       \


node_t* GetG(lexeme_t* lexeme_array, size_t* curr, FILE* html_stream)
{
    assert(lexeme_array);
    assert(curr);
    assert(html_stream);

    GRAMMAR_DEBUG_PRINT();
    node_t* node = GetE(lexeme_array, curr, html_stream);
    GRAMMAR_DEBUG_PRINT();
    if ((char)lexeme_array[*curr].value != '$') { SYNTAX_ERROR('$'); assert(0); }
    (*curr)++;
    return node;
}

node_t* GetE(lexeme_t* lexeme_array, size_t* curr, FILE* html_stream)
{
    assert(lexeme_array);
    assert(curr);
    assert(html_stream);

    GRAMMAR_DEBUG_PRINT();
    node_t* node = GetT(lexeme_array, curr, html_stream);
    GRAMMAR_DEBUG_PRINT();
    while((char)lexeme_array[*curr].value == ADD || (char)lexeme_array[*curr].value == SUB)
    {
        GRAMMAR_DEBUG_PRINT();
        int op = (int)lexeme_array[*curr].value;
        (*curr)++;
        node_t* node2 = GetT(lexeme_array, curr, html_stream);
        if (op == ADD) { node = _ADD(node, node2); }
        else           { node = _SUB(node, node2); }
    }
    GRAMMAR_DEBUG_PRINT();
    return node;
}

node_t* GetT(lexeme_t* lexeme_array, size_t* curr, FILE* html_stream)
{
    assert(lexeme_array);
    assert(curr);
    assert(html_stream);

    GRAMMAR_DEBUG_PRINT();
    node_t* node = GetD(lexeme_array, curr, html_stream);
    GRAMMAR_DEBUG_PRINT();
    while((char)lexeme_array[*curr].value == MUL || (char)lexeme_array[*curr].value == DIV)
    {
        GRAMMAR_DEBUG_PRINT();
        char op = (char)lexeme_array[*curr].value;
        (*curr)++;
        node_t* node2 = GetD(lexeme_array, curr, html_stream);
        if (op == MUL) { node = _MUL(node, node2); }
        else           { node = _DIV(node, node2); }
    }
    GRAMMAR_DEBUG_PRINT();
    return node;
}

node_t* GetD(lexeme_t* lexeme_array, size_t* curr, FILE* html_stream)
{
    assert(lexeme_array);
    assert(curr);
    assert(html_stream);

    GRAMMAR_DEBUG_PRINT();
    node_t* node = GetP(lexeme_array, curr, html_stream);
    GRAMMAR_DEBUG_PRINT();
    while((char)lexeme_array[*curr].value == POW)
    {
        GRAMMAR_DEBUG_PRINT();
        char op = (char)lexeme_array[*curr].value;
        (*curr)++;
        node_t* node2 = GetP(lexeme_array, curr, html_stream);
        node = _POW(node, node2);
    }
    GRAMMAR_DEBUG_PRINT();
    return node;
}

node_t* GetP(lexeme_t* lexeme_array, size_t* curr, FILE* html_stream)
{
    assert(lexeme_array);
    assert(curr);
    assert(html_stream);

    GRAMMAR_DEBUG_PRINT();
    switch(lexeme_array[*curr].type)
    {
        case OP_L:
        {
            if ((char)lexeme_array[*curr].value == '(')
            {
                (*curr)++;
                GRAMMAR_DEBUG_PRINT();
                node_t* node = GetE(lexeme_array, curr, html_stream);
                GRAMMAR_DEBUG_PRINT();
                if ((char)lexeme_array[*curr].value != ')') { assert(0); }
                (*curr)++;
                GRAMMAR_DEBUG_PRINT();
                return node;
            }
            else
            {
                printf("---ADDING OPERATION ---\n");
                operations operation = (operations)lexeme_array[*curr].value;
                (*curr)++;
                return new_node(OP, OP_VALUE(operation), NULL, GetE(lexeme_array, curr, html_stream));
            }
        }
        case ID:    { return _VAR(lexeme_array[(*curr)++].value); }
        case NUM_L: { return _NUM(lexeme_array[(*curr)++].value); }

        default: SYNTAX_ERROR('\0');
                 fprintf(stderr, "ERROR: Invalid lexeme type: %d\n", lexeme_array[*curr].type);
                 assert(0);
                 break;
    }
}

#undef OP_VALUE

#undef _NUM
#undef _VAR

#undef _ADD
#undef _SUB
#undef _MUL
#undef _DIV
#undef _SIN
#undef _COS
#undef _LOG
