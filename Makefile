CC = gcc
CCFLAGS = -g

NASM = nasm
NASMFLAGS = -f elf64

NAME = woody-woodpacker
EXEC_DECRYPTER = exec_decrypter
DYN_DECRYPTER = dyn_decrypter

INC_DIR = inc/
SRC_DIR = src/
OBJ_DIR = obj/

INC = $(addprefix $(INC_DIR), woody-woodpacker.h)
PLD = $(addprefix $(SRC_DIR), payload.s)

C_SRC = woody_woodpacker.c mmap_file.c patch_payload.c
ASM_SRC = encrypter.s

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

$(PAYLOAD): dyn_payload exec_payload
	ld $(OBJ_DIR)$(EXEC_DECRYPTER).o -o $(EXEC_DECRYPTER)
	ld $(OBJ_DIR)$(DYN_DECRYPTER).o -o $(DYN_DECRYPTER)

dyn_payload:
	$(NASM) $(NASMFLAGS) $(SRC_DIR)$(DYN_DECRYPTER).s -o $(OBJ_DIR)$(DYN_DECRYPTER).o

exec_payload:
	$(NASM) $(NASMFLAGS) $(SRC_DIR)$(EXEC_DECRYPTER).s -o $(OBJ_DIR)$(EXEC_DECRYPTER).o 	

test:
	resources/./run_test.sh

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME) $(PAYLOAD) $(DYN_DECRYPTER) $(EXEC_DECRYPTER) woody

re: fclean all

.PHONY: all clean fclean re encrypter decrypter sample debug test dyn_payload exec_payload