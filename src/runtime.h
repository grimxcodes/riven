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

void runtime_set_array(
    const char* name,
    int index,
    const char* value
);

char* runtime_get_array(
    const char* name,
    int index
);

#endif
