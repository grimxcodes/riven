#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "executor.h"

int main(
    int argc,
    char** argv
) {

    if (argc < 2) {

        printf(
            "Usage: rvn <file.rv>\n"
        );

        return 1;

    }

    FILE* file =
        fopen(argv[1], "r");

    if (!file) {

        printf(
            "Cannot open file\n"
        );

        return 1;

    }

    fseek(
        file,
        0,
        SEEK_END
    );

    long size =
        ftell(file);

    rewind(file);

    char* source =
        malloc(size + 1);

    fread(
        source,
        1,
        size,
        file
    );

    source[size] = '\0';

    fclose(file);

    init_lexer(source);

    ASTNode* root =
        parse_program();

    execute(root);

    free(source);

    return 0;

}
