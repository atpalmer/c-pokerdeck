BIN=./bin
OBJECTS=core.o handeval.o
ALLO=main_tophands.o main_playround.o
ALL=tophands playround

all: $(ALL)

tophands: $(OBJECTS) main_tophands.o
	$(CC) -o $(BIN)/tophands $(OBJECTS) main_tophands.o

playround: $(OBJECTS) main_playround.o
	$(CC) -o $(BIN)/playround $(OBJECTS) main_playround.o

clean:
	rm $(OBJECTS) $(ALLO)
