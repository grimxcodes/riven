#ifndef RUNTIME_H
#define RUNTIME_H

void runtime_stamp(
    const char* text
);

void runtime_set_variable(
    const char* name,
    const char* value
);

char* runtime_get_variable(
    const char* name
);

char* runtime_input();

#endif
