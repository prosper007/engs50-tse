CFLAGS=-Wall -pedantic -std=c11 -I. -g
OFILES=queue.o hash.o webpage.o pageio.o

all:		$(OFILES)
				ar cr ../lib/libutils.a $(OFILES)

%.o:		%.c %.h
				gcc $(CFLAGS) -c $<

test_1:		test1.o hash.o queue.o
					gcc ${CFLAGS} test1.o hash.o queue.o -o $@

test_2:		test2.o queue.o
					gcc ${CFLAGS} test2.o hash.o queue.o -o $@

clean:
					rm -f *.o test_1 test_2

