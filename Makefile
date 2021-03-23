NAME = injector
PAYLOAD = payload
SAMPLE = sample

CCFLAGS = -g

INC = inc/
SRCS = src/main.c src/open.c

all: $(NAME) $(PAYLOAD) $(SAMPLE) encrypter

$(NAME): $(SRCS)
	$(CC) $(CCFLAGS) -I $(INC) $(SRCS) -o $@ 

$(PAYLOAD): src/payload.s
	nasm -g -f elf64 -o src/payload.o src/payload.s && ld -g -o payload src/payload.o

$(SAMPLE): resources/sample.c Makefile
	$(CC) -no-pie resources/sample.c -o resources/no-pie-sample64

encrypter: src/encrypt.c src/tea_encrypter.s
	nasm -f elf64 src/tea_encrypter.s
	gcc -g -no-pie -I inc/ src/encrypt.c src/tea_encrypter.o -o encrypter

decrypter: src/encrypt.c src/tea_decrypter.s
	nasm -f elf64 src/tea_decrypter.s
	gcc -g -no-pie -I inc/ src/encrypt.c src/tea_decrypter.o -o decrypter

clean:
	rm -f $(NAME)
	rm -f resources/sample64
	rm -f $(PAYLOAD) $(PAYLOAD).o 