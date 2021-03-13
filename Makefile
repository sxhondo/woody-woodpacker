NAME = injector
PAYLOAD = payload
SAMPLE = sample

CCFLAGS = -g

INC = inc/
SRCS = src/main.c src/open.c

all: $(NAME) $(PAYLOAD) $(SAMPLE)

$(NAME): $(SRCS)
	$(CC) $(CCFLAGS) -I $(INC) $(SRCS) -o $@ 

$(PAYLOAD): payload.s
	nasm -f elf64 -o payload.o payload.s && ld -o payload payload.o

$(SAMPLE): resources/sample.c Makefile
	gcc -g resources/sample.c -o resources/sample64

clean:
	rm -f $(NAME)
	rm -f resources/sample64
	rm -f $(PAYLOAD) $(PAYLOAD).o 