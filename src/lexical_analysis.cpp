#include "lexical_analysis.h"

const char* skip_until_spaces(const char* curr)
{
    assert(curr);
    while(isalpha(*curr) || isdigit(*curr))
        curr++;

    return curr;
}

lexeme_t* string_to_lexems(const char* s)
{
    assert(s);
    lexeme_t* lexeme_array = (lexeme_t*)calloc(1000, sizeof(lexeme_t));
    if (!lexeme_array)
    {
        fprintf(stderr, "ERROR: Calloc for lexeme_array failed\n");
        return NULL;
    }

    const char* curr = s;
    const char* end  = strchr(s, '\0');
    printf("--- curr[%p] end[%p]\n", curr, end);
    size_t lexeme_num = 0;

    while (curr < end)
    {
        printf("curr: %c(%d), num = %d\n", *curr, *curr, lexeme_num);
        if (isalpha(*curr) || *curr =='+' || *curr =='-' || *curr =='/' || *curr =='*' || *curr =='^' || *curr == '\\')
        {
            if (get_OP(lexeme_array, lexeme_num, &curr) != UNKNOWN)
            {
                lexeme_num++;
                continue;
            }
            else
            {
                add_label(lexeme_array, lexeme_num, &curr);
                lexeme_num++;
                continue;
            }
        }
        if (*curr == '(')
        {
            lexeme_array[lexeme_num].type = OP_L;
            lexeme_array[lexeme_num].value = '(';
            curr++;
            lexeme_num++;
            continue;
        }
        if (*curr == ')')
        {
            lexeme_array[lexeme_num].type = OP_L;
            lexeme_array[lexeme_num].value = ')';
            curr++;
            lexeme_num++;
            continue;
        }
        if (*curr == '$')
        {
            lexeme_array[lexeme_num].type = OP_L;
            lexeme_array[lexeme_num].value = '$';
            curr++;
            lexeme_num++;
            continue;
        }
        if (isdigit(*curr))
        {
            lexeme_array[lexeme_num].type = NUM_L;
            char* end = NULL;
            lexeme_array[lexeme_num].value = strtod(curr, &end);
            curr = (const char*)end;
            lexeme_num++;
            continue;
        }
    }

    return lexeme_array;
}

enum operations get_OP(lexeme_t* lexeme_array, size_t lexeme_num, const char** curr)
{
    assert(lexeme_array);
    assert(curr);
    assert(*curr);

    #define DEF_OPER(oper, eval_rule, diff, call_name)          \
    if (strncmp(*curr, call_name, strlen(call_name)) == 0)      \
    {                                                           \
        printf("oper: %s\n", call_name);                        \
        *curr = *curr + strlen(call_name);                      \
        lexeme_array[lexeme_num].type  = OP_L;                  \
        lexeme_array[lexeme_num].value = oper;                  \
        strncpy(lexeme_array[lexeme_num].label, call_name, strlen(call_name)); \
        return oper;                                            \
    }                                                           \

    #include "diff_rules_DSL.h"
    #undef DEF_OPER

    return UNKNOWN;
}

void add_label(lexeme_t* lexeme_array, size_t lexeme_num, const char** curr)
{
    assert(lexeme_array);
    assert(curr);
    assert(*curr);

    printf("---ADDING LABEL---\n");
    const char* label_end = skip_until_spaces(*curr);
    strncpy(lexeme_array[lexeme_num].label, *curr, (size_t)(label_end - *curr));
    lexeme_array[lexeme_num].type = ID;
    *curr = label_end;
}
