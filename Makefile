NAME        := irc
TEST_NAME   := irc_tests

### COMPILATION ################################################################

COMPILATION := c++
CPPFLAGS    := -Wall -Wextra -Werror -std=c++98
DEPFLAGS    := -MMD -MP 
INCLUDES    := -I includes -I test

### PATHS & FILES ##############################################################

PATH_OBJS   := objs/

SRCS        := srcs/main.cpp srcs/Client.cpp srcs/Channel.cpp
OBJS        := $(patsubst %.cpp, $(PATH_OBJS)%.o, $(SRCS))
DEPS        := $(OBJS:.o=.d)

TEST_SRCS   := $(wildcard test/*.cpp)
TEST_SERVER := $(filter-out srcs/main.cpp, $(SRCS))

TEST_OBJS   := $(patsubst %.cpp, $(PATH_OBJS)%.o, $(TEST_SRCS))
TEST_SRV_OBJS := $(patsubst %.cpp, $(PATH_OBJS)%.o, $(TEST_SERVER))
TEST_DEPS   := $(TEST_OBJS:.o=.d)

### RULES ######################################################################

all: $(NAME)

$(PATH_OBJS)%.o: %.cpp
	@mkdir -p $(dir $@)
	$(COMPILATION) $(CPPFLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
	$(COMPILATION) $(CPPFLAGS) $(OBJS) -o $(NAME) $(INCLUDES)

test: $(TEST_OBJS) $(TEST_SRV_OBJS)
	$(COMPILATION) $(CPPFLAGS) $^ -o $(TEST_NAME) $(INCLUDES)
	@echo "\n\033[32m[OK] Binaire de test prêt : ./$(TEST_NAME)\033[0m\n"

clean:
	$(RM) -r $(PATH_OBJS)

fclean: clean
	$(RM) $(NAME) $(TEST_NAME)

re: fclean all

-include $(DEPS) $(TEST_DEPS)

.PHONY: all test clean fclean re