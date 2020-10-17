CFLAGS=-Wall -pedantic -std=c11 -I. -g

all:		test_1 test_2 test_3

list.o:		list.c list.h
					gcc ${CFLAGS} -c list.c

test1.o:	test1.c list.h
					gcc ${CFLAGS} -c test1.c

test2.o:	test2.c list.h
					gcc ${CFLAGS} -c test2.c

test3.o:	test3.c list.h
					gcc ${CFLAGS} -c test3.c

test_1:		test1.o list.o
					gcc ${CFLAGS} test1.o list.o -o test_1

test_2:	        test2.o list.o
					gcc ${CFLAGS} test2.o list.o -o test_2

test_3: 	test3.o list.o
					gcc ${CFLAGS} test3.o list.o -o test_3

clean:
					rm -f *.o test_1 test_2 test_3

