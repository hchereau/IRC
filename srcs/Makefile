# ==========================================================
#                       PARSING
# ==========================================================

# ================== CONFIGURATION ================== #

NAME			=	Parsing
SRC_DIR			=	
OBJ_DIR			=	obj/
DEP_DIR			=	$(OBJ_DIR)

SRC_FILES		=	main_parsing.cpp Parsing.cpp

SRC				=	$(SRC_FILES)
OBJS			=	$(addprefix $(OBJ_DIR), $(SRC_FILES:.cpp=.o))
DEPS			= 	$(addprefix $(DEP_DIR), $(SRC_FILES:.cpp=.d))

CC		=	c++
CFLAGS	=	-std=c++98 -Wall -Werror -Wextra
OPTIONS	=	-I../includes

# ================== COULEURS ================== #

RESET		= \033[0m
RED			= \033[31m
GREEN		= \033[32m
YELLOW		= \033[33m
BLUE		= \033[34m 
MAGENTA		= \033[35m
CYAN		= \033[36m
GREY		= \033[37m
WHITE		= \033[39m
BOLD		= \033[1m
PURPLE		= \033[2m
ITALIC		= \033[3m
UNDERLINED	= \033[4m
SPARKLE		= \033[5m
REVERSED	= \033[7m
IGREEN		= \033[92m
IBLUE		= \033[94m
ICYAN		= \033[96m

# ================== RÈGLES ================== #

TITLE= "\n$(MAGENTA)$(SPARKLE)=======================================\n$(ICYAN)            PARSING     \n$(MAGENTA)=======================================$(RESET)\n"

TITLE_FLAG = .title_shown

all: $(NAME)

$(NAME):	$(OBJS)
			@echo "$(PURPLE) 🔗 Linking objects..."
			@$ $(CC) $(CFLAGS) $(OBJS) -o $(NAME)
			@echo "$(GREEN)$(BOLD) ✓ Successful compilation of $(NAME)!"


$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
				@mkdir -p $(dir $@)
				@if [ ! -f $(OBJ_DIR)$(TITLE_FLAG) ]; then \
					echo $(TITLE); \
					touch $(OBJ_DIR)$(TITLE_FLAG); \
				fi
				@echo "$(MAGENTA)📄 Compiling $<...."
				@$(CC) $(CFLAGS) $(OPTIONS) -c $< -o $@

clean:
	@echo "$(BLUE)\n✓ Objects and dependances deleted.$(RESET)"
	@$ rm -rf $(OBJ_DIR) $(TITLE_FLAG)

fclean: clean
	@$ rm -rf $(NAME) $(TITLE_FLAG)
	@echo "$(ICYAN)$(BOLD)        ✓ Executable deleted.$(RESET)"

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re