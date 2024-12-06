obj := main.o

CC := clang-18
CFLAGS += -g -Wall -Wextra

all: $(obj)
	$(CC) $(CFLAGS) -o main $(obj)

.PHONY: clean
clean:
	rm -f main $(obj)