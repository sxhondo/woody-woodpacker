CC = gcc
CCFLAGS = -g

NASM = nasm
NASMFLAGS = -f elf64

NAME = injector
PAYLOAD = payload

INC_DIR = inc/
SRC_DIR = src/
OBJ_DIR = obj/

INC = $(addprefix $(INC_DIR), woody-woodpacker.h)
PLD = $(addprefix $(SRC_DIR), payload.s)

C_SRC = main.c
ASM_SRC = tea_encrypter.s

COBJ = $(C_SRC:%.c=$(OBJ_DIR)%.o)
AOBJ = $(ASM_SRC:%.s=$(OBJ_DIR)%.o)

all: $(NAME) $(PAYLOAD)

$(NAME): $(COBJ) $(AOBJ)
	$(CC) $(CCFLAGS) -no-pie -I $(INC_DIR) $(COBJ) $(AOBJ) -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.c $(INC) Makefile
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CCFLAGS) -I $(INC_DIR) -c $< -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.s $(INC) Makefile
	$(NASM) $(NASMFLAGS) $< -o $@

pload:
	$(NASM) $(NASMFLAGS) $(SRC_DIR)$(PAYLOAD).s -o $(OBJ_DIR)$(PAYLOAD).o 
	ld $(OBJ_DIR)$(PAYLOAD).o -o $(PAYLOAD)

sample:
	$(CC) -no-pie resources/sample.c -o resources/9.3-no-pie-sample64

encrypter: src/encrypt.c src/tea_encrypter.s
	nasm -f elf64 src/tea_encrypter.s -o obj/tea_encrypter.o
	$(CC) -g -no-pie -I $(INC_DIR) src/c_encrypt.c src/open.c obj/tea_encrypter.o -o encrypter

decrypter: src/encrypt.c src/tea_decrypter.s
	nasm -f elf64 src/tea_decrypter.s -o obj/tea_decrypter.o
	$(CC) -g -no-pie -I $(INC_DIR) src/c_encrypt.c src/open.c obj/tea_decrypter.o -o decrypter

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME) $(PAYLOAD)

.PHONY: all clean fclean re encrypter decrypter sample