#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runtime.h"

typedef struct {

    char name[100];
    char value[100];

} Variable;

static Variable variables[1000];

static int variable_count = 0;

void runtime_set_variable(
    const char* name,
    const char* value
) {

    for (
        int i = 0;
        i < variable_count;
        i++
    ) {

        if (
            strcmp(
                variables[i].name,
                name
            ) == 0
        ) {

            strcpy(
                variables[i].value,
                value
            );

            return;

        }

    }

    strcpy(
        variables[variable_count].name,
        name
    );

    strcpy(
        variables[variable_count].value,
        value
    );

    variable_count++;

}

char* runtime_get_variable(
    const char* name
) {

    for (
        int i = 0;
        i < variable_count;
        i++
    ) {

        if (
            strcmp(
                variables[i].name,
                name
            ) == 0
        ) {

            return variables[i].value;

        }

    }

    return "0";

}

void runtime_stamp(
    const char* text
) {

    printf("%s\n", text);

}
