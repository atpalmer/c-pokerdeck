OBJECTS=main.o core.o handeval.o
P=a.out

$P: $(OBJECTS)
	$(CC) -o $P $(OBJECTS)

clean:
	rm $(OBJECTS) $P
