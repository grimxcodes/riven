CC=gcc

SRC=src/main.c \
src/lexer.c \
src/parser.c \
src/runtime.c \
src/ast.c \
src/executor.c \
src/token.c

OUT=rvn

all:
	$(CC) $(SRC) -o $(OUT)

run:
	./rvn examples/riven_demo.rv
