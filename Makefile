# Makefile
CC = gcc
CFLAGS = -Wall -g
TARGET = riven

$(TARGET): riven.c
	$(CC) $(CFLAGS) -o $(TARGET) riven.c

clean:
	rm -f $(TARGET)
