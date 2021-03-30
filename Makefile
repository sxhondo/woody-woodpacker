CC = gcc
CCFLAGS = -g

NASM = nasm
NASMFLAGS = -f elf64

NAME = woody-woodpacker
EXEC_DECRYPTER = exec_tea_decrypter
DYN_DECRYPTER = dyn_tea_decrypter

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

$(NAME): $(COBJ) $(AOBJ) payload
	$(CC) $(CCFLAGS) -no-pie -I $(INC_DIR) $(COBJ) $(AOBJ) -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.c $(INC) Makefile
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CCFLAGS) -I $(INC_DIR) -c $< -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.s $(INC) Makefile
	$(NASM) $(NASMFLAGS) $< -o $@

payload:
	$(NASM) $(NASMFLAGS) $(SRC_DIR)$(EXEC_DECRYPTER).s -o $(OBJ_DIR)$(EXEC_DECRYPTER).o 
	$(NASM) $(NASMFLAGS) $(SRC_DIR)$(DYN_DECRYPTER).s -o $(OBJ_DIR)$(DYN_DECRYPTER).o
	ld $(OBJ_DIR)$(EXEC_DECRYPTER).o -o $(EXEC_DECRYPTER)
	ld $(OBJ_DIR)$(DYN_DECRYPTER).o -o $(DYN_DECRYPTER)	

sample:
	$(CC) -no-pie resources/sample.c -o resources/9.3-no-pie-sample64

debug: src/c_encrypt.c src/dyn_tea_decrypter.s
	nasm -f elf64 src/dyn_tea_decrypter.s -o obj/dyn_tea_decrypter.o
	$(CC) -g -I $(INC_DIR) src/c_encrypt.c obj/dyn_tea_decrypter.o -o debug

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME) $(PAYLOAD)

.PHONY: all clean fclean re encrypter decrypter sample debug