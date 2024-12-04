obj := main.o fsm.o

CC = clang-18
CFLAGS += -g -Wall -Wextra

all: $(obj)
	cc $(CFLAGS) -o main $(obj)

main.o fsm.o: fsm.h

.PHONY: clean
clean:
	rm -f main $(obj)