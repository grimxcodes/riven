#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "runtime.h"

typedef struct {

    char name[100];
    char value[100];

} Variable;

Variable variables[100];

int variable_count = 0;

void runtime_stamp(char* text) {

    printf("%s\n", text);

}

void runtime_set_variable(char* name, char* value) {

    strcpy(variables[variable_count].name, name);

    strcpy(variables[variable_count].value, value);

    variable_count++;

}

char* runtime_get_variable(char* name) {

    for (int i = 0; i < variable_count; i++) {

        if (strcmp(variables[i].name, name) == 0) {

            return variables[i].value;

        }

    }

    return "";

}
