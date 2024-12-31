# Variables
CC = gcc
CFLAGS = -g -fPIC
INCLUDES = -I/opt/homebrew/Cellar/libffi/3.4.6/include -I./testlib -I./include
LDFLAGS = -L/opt/homebrew/Cellar/libffi/3.4.6/lib -lffi

# Targets
all: testlib.so main

# Build the shared library
testlib.so: testlib/testlib.c
	$(CC) $(CFLAGS) -shared $(INCLUDES) $(LDFLAGS) -o $@ $<

# Build the main executable
main: src/component.c src/main.c
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $^

# Clean up build artifacts
clean:
	rm -f testlib.so main