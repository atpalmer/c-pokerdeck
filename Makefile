BIN=./bin
SOURCES=src/*.c
CFLAGS=-I./include -O3 -Wall -Wextra -Wpedantic

all: tophands playround

%: $(SOURCES) prog/%.c
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) -o $(BIN)/$@ $?

clean:
	rm -r $(BIN)
