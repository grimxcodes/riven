#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runtime.h"

typedef struct {

    char name[100];

    char value[1000];

    int constant;

} Variable;

typedef struct {

    char name[100];

    char values[100][1000];

} Array;

static Variable variables[1000];

static int variable_count = 0;

static Array arrays[100];

static int array_count = 0;

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

            if (
                variables[i].constant
            ) {

                printf(
                    "Cannot modify firm variable %s\n",
                    name
                );

                exit(1);

            }

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

    variables[
        variable_count
    ].constant = 0;

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

    printf(
        "Unknown variable %s\n",
        name
    );

    exit(1);

}

void runtime_set_constant(
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

            variables[i].constant = 1;

            return;

        }

    }

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
            ) == 0

        ) {

            strcpy(
                arrays[i].values[index],
                value
            );

            return;

        }

    }

    strcpy(
        arrays[array_count].name,
        name
    );

    strcpy(
        arrays[array_count].values[index],
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
            ) == 0

        ) {

            return arrays[i].values[index];

        }

    }

    printf(
        "Unknown array %s\n",
        name
    );

    exit(1);

}

void runtime_stamp(
    const char* text
) {

    printf(
        "%s\n",
        text
    );

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
