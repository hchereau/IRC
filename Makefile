NAME := irc

### SRCS #######################################################################

PATH_SRCS := srcs

SRCS += Client.cpp
SRCS += Channel.cpp

vpath %.cpp $(PATH_SRCS)

### OBJS & DEPS ################################################################

PATH_OBJS := objs/

OBJS := $(patsubst %.cpp, $(PATH_OBJS)%.o, $(SRCS))
DEPS := $(OBJS:.o=.d)

### HEADER #####################################################################

PATH_INCLUDES := includes

### COMPILATION ################################################################

COMPILATION := c++
CPPFLAGS += -Wall
CPPFLAGS += -Wextra
CPPFLAGS += -Werror
CPPFLAGS += -std=c++98

DEPFLAGS := -MMD -MP 

## RULES #######################################################################

all: $(NAME)

$(OBJS): $(PATH_OBJS)%.o: %.cpp
		mkdir -p $(PATH_OBJS)
		$(COMPILATION) $(CPPFLAGS) $(DEPFLAGS) -c $< -o $@ -I $(PATH_INCLUDES)

$(NAME): $(OBJS)
		$(COMPILATION) $(CPPFLAGS) $(OBJS) -o $(NAME) -I $(PATH_INCLUDES)

clean:
		$(RM) -r $(PATH_OBJS)

fclean: clean
		$(RM) $(NAME)

re: fclean all

-include $(DEPS)