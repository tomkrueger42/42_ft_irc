NAME	=	ircserv

CC		=	c++

CFLAGS	+=	-Wall -Werror -Wextra -std=c++98 

SRC_DIR	=	./src/

OBJ_DIR	=	./obj/

INC		=	-Iinc

DEPS	=	$(shell find ./inc -type f -regex ".*\.hpp")

SRC		=	channel_utils.cpp Channel.cpp replies_err.cpp replies_rpl.cpp \
			Server.cpp tom.cpp user_utils.cpp User.cpp utils.cpp

OBJS	=	$(addprefix $(OBJ_DIR), $(SRC:.cpp=.o))

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp $(DEPS)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all
