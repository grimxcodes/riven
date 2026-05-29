#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runtime.h"

typedef struct {

    char name[100];

    char value[1000];

} Variable;

typedef struct {

    char name[100];

    int index;

    char value[1000];

} ArrayItem;

typedef struct {

    char name[100];

} Constant;

static Variable variables[1000];

static int variable_count = 0;

static ArrayItem arrays[1000];

static int array_count = 0;

static Constant constants[1000];

static int constant_count = 0;

void runtime_set_constant(
    const char* name
) {

    strcpy(
        constants[
            constant_count
        ].name,
        name
    );

    constant_count++;

}

int runtime_is_constant(
    const char* name
) {

    for (
        int i = 0;
        i < constant_count;
        i++
    ) {

        if (

            strcmp(
                constants[i].name,
                name
            ) == 0

        ) {

            return 1;

        }

    }

    return 0;

}

void runtime_set_variable(
    const char* name,
    const char* value
) {

    if (
        runtime_is_constant(name)
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

                printf(
                    "Cannot modify firm variable\n"
                );

                exit(1);

            }

        }

    }

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
        variables[
            variable_count
        ].name,
        name
    );

    strcpy(
        variables[
            variable_count
        ].value,
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

void runtime_set_array(
    const char* name,
    int index,
    const char* value
) {

    for (
        int i = 0;
        i < array_count;
        i++
    ) {

        if (

            strcmp(
                arrays[i].name,
                name
            ) == 0 &&

            arrays[i].index ==
            index

        ) {

            strcpy(
                arrays[i].value,
                value
            );

            return;

        }

    }

    strcpy(
        arrays[array_count].name,
        name
    );

    arrays[array_count].index =
        index;

    strcpy(
        arrays[array_count].value,
        value
    );

    array_count++;

}

char* runtime_get_array(
    const char* name,
    int index
) {

    for (
        int i = 0;
        i < array_count;
        i++
    ) {

        if (

            strcmp(
                arrays[i].name,
                name
            ) == 0 &&

            arrays[i].index ==
            index

        ) {

            return arrays[i].value;

        }

    }

    return "0";

}

void runtime_stamp(
    const char* text
) {

    printf("%s\n", text);

}

char* runtime_input() {

    static char buffer[1000];

    fgets(
        buffer,
        sizeof(buffer),
        stdin
    );

    buffer[
        strcspn(buffer, "\n")
    ] = '\0';

    return buffer;

}
