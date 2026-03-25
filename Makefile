NAME := irc

### SRCS #######################################################################

PATH_SRCS := srcs

SRCS += main.cpp
SRCS += Client.cpp

vpath %.cpp $(PATH_SRCS)

### OBJS #######################################################################

PATH_OBJS := objs/

OBJS := $(patsubst %.cpp, $(PATH_OBJS)%.o, $(SRCS))

### HEADER #####################################################################

PATH_INCLUDES += includes

### COMPILATION ################################################################

COMPILATION := c++
CPPFLAGS += -Wall
CPPFLAGS += -Wextra
CPPFLAGS += -Werror
CPPFLAGS += -std=c++98

## RULES #######################################################################

all: $(NAME)

$(OBJS): $(PATH_OBJS)%.o: %.cpp
		mkdir -p $(PATH_OBJS)
		$(COMPILATION) $(CPPFLAGS) -c $< -o $@ -I $(PATH_INCLUDES)

$(NAME): $(OBJS)
		$(COMPILATION) $(CPPFLAGS) $(OBJS) -o $(NAME) -I $(PATH_INCLUDES)

clean:
		$(RM) -r $(PATH_OBJS)

fclean: clean
		$(RM) $(NAME)

re: fclean all