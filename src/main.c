#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "executor.h"

char* read_file(const char* path) {

    FILE* file = fopen(path, "r");

    if (!file) {

        printf("Cannot open file\n");

        exit(1);

    }

    fseek(file, 0, SEEK_END);

    long size = ftell(file);

    rewind(file);

    char* buffer =
        malloc(size + 1);

    fread(buffer, 1, size, file);

    buffer[size] = '\0';

    fclose(file);

    return buffer;

}

int main(int argc, char** argv) {

    if (argc < 2) {

        printf(
            "Usage: rvn file.rv\n"
        );

        return 1;

    }

    char* source =
        read_file(argv[1]);

    init_lexer(source);

    ASTNode* root =
        parse_program();

    execute(root);

    return 0;

}
