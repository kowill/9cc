CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
	cc -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
		./9cc -test
		./test.sh

clear:
		rm -f 9cc *.o *~ tmp*