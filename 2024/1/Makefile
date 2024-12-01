obj := main.o

CFLAGS += -g -Wall -Wextra

all: $(obj)
	cc $(CFLAGS) -o main $(obj)

main.o: main.c

.PHONY: clean
clean:
	rm main $(obj)