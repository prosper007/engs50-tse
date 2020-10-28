CFLAGS=-Wall -pedantic -std=c11 -I. -L../lib -g
OFILES=queue.o hash.o webpage.o pageio.o indexio.o
LIBS= -lutils -lcurl

all:		$(OFILES)
				ar cr ../lib/libutils.a $(OFILES)

%.o:		%.c %.h
				gcc $(CFLAGS) -c $<

test1:	test1.o
				gcc ${CFLAGS} test1.o $(LIBS) -o $@

test2:	test2.o
				gcc ${CFLAGS} test2.o $(LIBS) -o $@

clean:
					rm -f test1 test2
