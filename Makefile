PROGRAMS = mkpass

CC = c++
CFLAGS=-Wall -O2 -std=c++17
LIBS=

all: $(PROGRAMS)

.cc: ; $(CC) $(CFLAGS) $@.cc $(LIBS) -o $@
	strip $@

clean:
	rm $(PROGRAMS)
