#ifndef _LEXICAL_ANALYSIS_H__
#define _LEXICAL_ANALYSIS_H__

#include "diff_tree.h"

enum lexeme_type {OP_L, ID, NUM_L};
struct lexeme_t
{
    lexeme_type type;
    char       label[16];
    double     value;
    lexeme_t*   left;
    lexeme_t*  right;
};

const char* skip_until_spaces(const char* curr);
lexeme_t* string_to_lexems(const char* s);
enum operations get_OP(lexeme_t* lexeme_array, size_t lexeme_num, const char** curr);
void add_label(lexeme_t* lexeme_array, size_t lexeme_num, const char** curr);
#endif
