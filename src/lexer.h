#ifndef LEXER_H
#define LEXER_H

#include "token.h"

Token get_next_token();
void init_lexer(const char* source);

#endif
