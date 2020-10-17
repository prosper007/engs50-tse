CFLAGS=-Wall -pedantic -std=c11 -I. -g

all:		test_1 test_2 #test_3

%.o:		%.c %.h
				gcc $(CFLAGS) -c $<

test_1:		test1.o hash.o queue.o
					gcc ${CFLAGS} test1.o hash.o queue.o -o $@

test_2:		test2.o queue.o
					gcc ${CFLAGS} test2.o hash.o queue.o -o $@

#test_3: 	test3.o queue.o
#					gcc ${CFLAGS} test3.o queue.o -o $@

clean:
					rm -f *.o test_1 test_2 #test_3

