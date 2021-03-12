NAME = injector
PAYLOAD = payload
SAMPLE = sample

CC = gcc
CCFLAGS = -g

all: $(NAME) $(PAYLOAD) $(SAMPLE)

$(NAME): main.c
	$(CC) $(CCFLAGS) main.c -o $(NAME)

$(PAYLOAD): payload.s
	nasm -f elf64 -o payload.o payload.s && ld -o payload payload.o

$(SAMPLE): resources/sample.c
	clang -m64 resources/sample.c -o resources/sample64

clean:
	rm -f $(NAME)
	rm -f resources/sample64
	rm -f $(PAYLOAD) $(PAYLOAD).o 