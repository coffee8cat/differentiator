#ifndef _LEXICAL_ANALYSIS_H__
#define _LEXICAL_ANALYSIS_H__

#include "diff_tree.h"

enum lexeme_type {OP_L, ID, NUM_L};
struct lexeme_t
{
    lexeme_type type;
    double     value;
    lexeme_t*   left;
    lexeme_t*  right;
};

const char* skip_until_spaces(const char* curr);
lexeme_t* string_to_lexems(const char* s, variable* vars_table);
enum operations get_OP(lexeme_t* lexeme_array, size_t lexeme_num, const char** curr);
void add_label(lexeme_t* lexeme_array, size_t lexeme_num, const char** curr, variable* vars_table);
#endif
