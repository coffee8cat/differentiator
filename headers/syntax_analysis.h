#ifndef _GRAMMAR_READER_H__
#define _GRAMMAR_READER_H__

#include <stdio.h>
#include <stdlib.h>
#include "diff_tree.h"
#include "lexical_analysis.h"
#include "tree_dump.h"

node_t* GetG(lexeme_t* lexeme_array, size_t* curr, FILE* html_stream);
node_t* GetE(lexeme_t* lexeme_array, size_t* curr, FILE* html_stream);
node_t* GetT(lexeme_t* lexeme_array, size_t* curr, FILE* html_stream);
node_t* GetD(lexeme_t* lexeme_array, size_t* curr, FILE* html_stream);
node_t* GetP(lexeme_t* lexeme_array, size_t* curr, FILE* html_stream);

#endif
