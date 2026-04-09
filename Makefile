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
               srcs/Executor.cpp \
               srcs/Parsing.cpp \
               srcs/Server.cpp \
               srcs/main.cpp \
               srcs/commands/cmd_pass.cpp \
               srcs/commands/cmd_nick.cpp \
               srcs/commands/cmd_user.cpp \
               srcs/commands/cmd_join.cpp \
			   srcs/commands/cmd_part.cpp \
               srcs/commands/cmd_kick.cpp \
               srcs/commands/cmd_invite.cpp \
               srcs/commands/cmd_mode.cpp \
			   srcs/commands/cmd_ping.cpp \
               srcs/commands/cmd_privmsg.cpp

TEST_SRCS   := test/method_tests/main_test.cpp \
               test/method_tests/test_client.cpp \
               test/method_tests/test_channel.cpp \
               test/method_tests/utils_tests.cpp \
			   test/method_tests/test_validator.cpp \
			   test/method_tests/test_Dispatcher.cpp \
			   test/method_tests/test_parsing.cpp

NAME_BOT := irc_bot
BOT_SRCS    := srcs/bonus/bot.cpp \
               srcs/bonus/main_bot.cpp
BOT_OBJS    := $(patsubst %.cpp, $(PATH_OBJS)%.o, $(BOT_SRCS))

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

bot: $(BOT_OBJS)
	$(COMPILATION) $(CPPFLAGS) $(BOT_OBJS) -o $(NAME_BOT) $(INCLUDES)
	@echo "\n\033[32m[OK] Binaire du bot prêt : ./$(NAME_BOT)\033[0m\n"

test: $(TEST_OBJS) $(TEST_SRV_OBJS)
	$(COMPILATION) $(CPPFLAGS) $^ -o $(TEST_NAME) $(INCLUDES)
	@echo "\n\033[32m[OK] Binaire de test prêt : ./$(TEST_NAME)\033[0m\n"

test_integration: all
	@echo "\033[36m--- Démarrage du serveur IRC en arrière-plan ---\033[0m"
	@./$(NAME) 6667 testpass & echo $$! > server.pid
	@sleep 1
	@echo "\033[36m--- Exécution des tests Python (Custom Runner) ---\033[0m\n"
	@python3 test/commands_tests/run_integration.py ; \
	RESULT=$$?; \
	echo "\n\033[36m--- Arrêt du serveur IRC ---\033[0m" ; \
	kill `cat server.pid` 2>/dev/null || true ; \
	rm -f server.pid ; \
	if [ $$RESULT -ne 0 ]; then \
		echo "\033[31m[ERREUR] Les tests d'intégration ont échoué.\033[0m" ; \
		exit 1 ; \
	else \
		echo "\033[32m[OK] Tous les tests d'intégration sont passés avec succès.\033[0m" ; \
	fi

clean:
	$(RM) -r $(PATH_OBJS)

fclean: clean
	$(RM) $(NAME) $(TEST_NAME) $(NAME_BOT)
	@echo "\033[33m[ Nettoyage complet effectué ]\033[0m"

re: fclean all

-include $(DEPS)

.PHONY: all test clean fclean re