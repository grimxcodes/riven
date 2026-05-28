#ifndef RUNTIME_H
#define RUNTIME_H

void runtime_stamp(char* text);

void runtime_set_variable(char* name, char* value);

char* runtime_get_variable(char* name);

#endif
