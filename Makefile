CC 					= gcc
CCFLAGS 				= -Wall -Wextra -Werror -g

NASM 					= nasm
NASMFLAGS 			= -f elf64

NAME 					= woody-woodpacker
# PAYLOAD				= exec_decrypter dyn_decrypter

EXEC_DECRYPTER 	= exec_decrypter
DYN_DECRYPTER 		= dyn_decrypter

INC_DIR 				= inc/
SRC_DIR 				= src/
OBJ_DIR 				= obj/

INC 					= $(addprefix $(INC_DIR), woody-woodpacker.h)

C_SRC 				= woody_woodpacker.c mmap_file.c patch_payload.c
ENCR_SRC 			= encrypter.s
DCR_SRC 				= dyn_decrypter.s exec_decrypter.s

COBJ 					= $(C_SRC:%.c=$(OBJ_DIR)%.o)
AOBJ 					= $(ENCR_SRC:%.s=$(OBJ_DIR)%.o)
DCR_OBJ 				= $(DCR_SRC:%.s=$(OBJ_DIR)%.o)

all: $(NAME) $(EXEC_DECRYPTER) $(DYN_DECRYPTER)

$(NAME): $(COBJ) $(AOBJ)
	$(CC) $(CCFLAGS) -no-pie -I $(INC_DIR) $(COBJ) $(AOBJ) -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.c $(INC) Makefile
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CCFLAGS) -I $(INC_DIR) -c $< -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.s $(INC) Makefile
	@mkdir -p $(OBJ_DIR)
	$(NASM) $(NASMFLAGS) $< -o $@

$(EXEC_DECRYPTER): $(OBJ_DIR)$(EXEC_DECRYPTER).o
	ld $(OBJ_DIR)$(EXEC_DECRYPTER).o -o $(EXEC_DECRYPTER)
$(DYN_DECRYPTER): $(OBJ_DIR)$(DYN_DECRYPTER).o
	ld $(OBJ_DIR)$(DYN_DECRYPTER).o -o $(DYN_DECRYPTER)

test:
	test/./run_test.sh

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME) $(PAYLOAD) $(DYN_DECRYPTER) $(EXEC_DECRYPTER) woody

re: fclean all

.PHONY: all clean fclean re debug test