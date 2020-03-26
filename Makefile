CC = clang
CFLAGS = -std=c11 -Wall -ggdb
LDFLAGS = -lX11
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = titlefinder

all: $(EXECUTABLE)

%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

.PHONY: clean all
clean:
	rm -f *.o $(EXECUTABLE)
