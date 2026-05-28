#ifndef LEXER_H
#define LEXER_H

#include "token.h"

void init_lexer(char* source);

Token get_next_token();

Token peek_next_token();

#endif
