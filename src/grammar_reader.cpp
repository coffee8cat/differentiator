#include "grammar_reader.h"


#define _ADD(a, b) new_node(OP, ADD, a,    b)
#define _SUB(a, b) new_node(OP, SUB, a,    b)
#define _MUL(a, b) new_node(OP, MUL, a,    b)
#define _DIV(a, b) new_node(OP, DIV, a,    b)
#define _COS(b)    new_node(OP, COS, NULL, b)
#define _SIN(b)    new_node(OP, SIN, NULL, b)
#define _LOG(b)    new_node(OP, LOG, NULL, b)
#define _POW(a, b) new_node(OP, POW, a,    b)

#define GRAMMAR_DEBUG_PRINT() printf("in %s on [%d]: %s\n", __func__, p, s+p)

const char* s = "12^(5+7*(3+8))$";
size_t p = 0;

node_t* GetG()
{
    GRAMMAR_DEBUG_PRINT();
    node_t* node = GetE();
    printf("%c\n", s[p]);
    GRAMMAR_DEBUG_PRINT();
    if (s[p] != '$') { assert(0); }
    p++;
    return node;
}

node_t* GetE()
{
    GRAMMAR_DEBUG_PRINT();
    node_t* node = GetT();
    GRAMMAR_DEBUG_PRINT();
    while(s[p] == '+' || s[p] == '-')
    {
        GRAMMAR_DEBUG_PRINT();
        int op = s[p];
        p++;
        node_t* node2 = GetT();
        if (op == '+') { node = _ADD(node, node2); }
        else           { node = _SUB(node, node2); }
    }
    GRAMMAR_DEBUG_PRINT();
    return node;
}

node_t* GetT()
{
    GRAMMAR_DEBUG_PRINT();
    node_t* node = GetD();
    GRAMMAR_DEBUG_PRINT();
    while(s[p] == '*' || s[p] == '/')
    {
        GRAMMAR_DEBUG_PRINT();
        int op = s[p];
        p++;
        node_t* node2 = GetD();
        if (op == '*') { node = _MUL(node, node2); }
        else           { node = _DIV(node, node2); }
    }
    GRAMMAR_DEBUG_PRINT();
    return node;
}

node_t* GetD()
{
    GRAMMAR_DEBUG_PRINT();
    node_t* node = GetP();
    GRAMMAR_DEBUG_PRINT();
    while(s[p] == '^')
    {
        GRAMMAR_DEBUG_PRINT();
        int op = s[p];
        p++;
        node_t* node2 = GetP();
        node = _POW(node, node2);
    }
    GRAMMAR_DEBUG_PRINT();
    return node;
}

node_t* GetN()
{
    int val = 0;
    size_t old_p = p;
    GRAMMAR_DEBUG_PRINT();
    while ('0' <= s[p] && s[p] <= '9')
    {
        GRAMMAR_DEBUG_PRINT();
        val = val * 10 + s[p] - '0';
        p++;
    }
    GRAMMAR_DEBUG_PRINT();
    printf("curr: %s\n", s+p);
    if (old_p == p) { fprintf(stderr, "!!! Syntax Error !!!\n in line: %s\n position %d: %s\n", s, p, s+p); assert(0);}
    return _NUM(val);
}

node_t* GetP()
{
    GRAMMAR_DEBUG_PRINT();
    if (s[p] == '(')
    {
        p++;
        GRAMMAR_DEBUG_PRINT();
        node_t* node = GetE();
        GRAMMAR_DEBUG_PRINT();
        if (s[p] != ')') { assert(0); }
        p++;
        GRAMMAR_DEBUG_PRINT();
        return node;
    }
    else
    {
        GRAMMAR_DEBUG_PRINT();
        return GetN();
    }
}


#undef _NUM
#undef _VAR

#undef _ADD
#undef _SUB
#undef _MUL
#undef _DIV
#undef _SIN
#undef _COS
#undef _LOG
