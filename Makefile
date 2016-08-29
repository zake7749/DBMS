OBJECT = dbms.o FileProcessor.o HashTable.o ItemParser.o

all: $(OBJECT)
	g++ -o cmdsql $^

clean:
	rm dbms.o FileProcessor.o HashTable.o ItemParser.o
