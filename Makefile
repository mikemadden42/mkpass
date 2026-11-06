PROGRAMS = mkpass

CC = c++
CFLAGS=-Wall -O2 -std=c++17
LIBS=

all: $(PROGRAMS)

mkpass: main.cc
	$(CC) $(CFLAGS) main.cc $(LIBS) -o $@
	strip $@

clean:
	rm -f $(PROGRAMS)
