NAME        := irc
TEST_NAME   := irc_tests

### COMPILATION ################################################################

COMPILATION := c++
CPPFLAGS    := -Wall -Wextra -Werror -std=c++98
DEPFLAGS    := -MMD -MP 
INCLUDES    := -I includes -I test

### FILES ######################################################################

PATH_OBJS   := objs/

SRCS        := srcs/Client.cpp \
               srcs/Channel.cpp \
			   srcs/CommandValidator.cpp \
			   srcs/Replies.cpp \
			   srcs/Executor.cpp

TEST_SRCS   := test/main_test.cpp \
               test/test_client.cpp \
               test/test_channel.cpp \
               test/utils_tests.cpp \
			   test/test_validator.cpp

### OBJECTS & DEPS #############################################################

OBJS          := $(patsubst %.cpp, $(PATH_OBJS)%.o, $(SRCS))
TEST_SRV_OBJS := $(filter-out $(PATH_OBJS)srcs/main.o, $(OBJS))
TEST_OBJS     := $(patsubst %.cpp, $(PATH_OBJS)%.o, $(TEST_SRCS))

DEPS          := $(OBJS:.o=.d) $(TEST_OBJS:.o=.d)

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

-include $(DEPS)

.PHONY: all test clean fclean re