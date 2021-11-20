all: part1.exe

part1.exe: partOne.c string_parser.o
	gcc -o part1.exe partOne.c string_parser.o

string_parser.o: string_parser.c string_parser.h
	gcc -c string_parser.c

clean:
	rm -f core *.o part1.exe
