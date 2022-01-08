BIN=./bin
SOURCES=src/*.c
CFLAGS=-I./include

all: tophands playround

%: $(SOURCES) prog/%.c
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) -o $(BIN)/$@ $?

clean:
	rm -r $(BIN)
