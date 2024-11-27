obj := main.o

CFLAGS += -g

all: $(obj)
	cc -o main main.o

main.o : main.c

.PHONY: clean
clean:
	rm main $(obj)