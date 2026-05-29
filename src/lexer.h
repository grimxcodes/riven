#ifndef LEXER_H
#define LEXER_H

#include "token.h"

void init_lexer(char* text);

Token get_next_token();

#endif
