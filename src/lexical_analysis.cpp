#include "lexical_analysis.h"

const char* skip_until_spaces(const char* curr)
{
    assert(curr);
    while(isalpha(*curr) || isdigit(*curr))
        curr++;

    return curr;
}

lexeme_t* string_to_lexems(const char* s, variable* vars_table)
{
    assert(s);
    lexeme_t* lexeme_array = (lexeme_t*)calloc(1000, sizeof(lexeme_t));
    if (!lexeme_array)
    {
        fprintf(stderr, "ERROR: Calloc for lexeme_array failed\n");
        return NULL;
    }

    const char* curr = s;
    const char* buff_end  = strchr(s, '\0');
    printf("--- curr[%p] end[%p]\n", curr, buff_end);
    size_t lexeme_num = 0;

    while (curr < buff_end)
    {
        printf("curr: %c(%d), num = %d\n", *curr, *curr, lexeme_num);
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
        if (get_OP(lexeme_array, lexeme_num, &curr) != UNKNOWN)
        {
            lexeme_num++;
            continue;
        }
        else
        {
            if (isalpha(*curr))
            {
                add_label(lexeme_array, lexeme_num, &curr, vars_table);
                lexeme_num++;
                printf("lexeme_num: %d", lexeme_num);
                continue;
            }
        }

        fprintf(stderr, "LEXICAL ERROR: No such lexical rule");
        return NULL;
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
        return oper;                                            \
    }                                                           \

    #include "diff_rules_DSL.h"
    #undef DEF_OPER

    return UNKNOWN;
}

void add_label(lexeme_t* lexeme_array, size_t lexeme_num, const char** curr, variable* vars_table)
{
    assert(lexeme_array);
    assert(curr);
    assert(*curr);

    printf("---ADDING LABEL---\n");

    const char* label_end = skip_until_spaces(*curr);
    size_t position = add_variable(vars_table, *curr, (size_t)(label_end - *curr));

    lexeme_array[lexeme_num].type  = ID;
    lexeme_array[lexeme_num].value = position;

    *curr = label_end;
}
