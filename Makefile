NAME = ircserv

SRC_DIR = srcs/
OBJ_DIR = obj/

CC = c++

CFLAGS = -Wall -Werror -Wextra

CSTD = -std=c++98

SRC = srcs/client.cpp srcs/server.cpp srcs/irc.cpp

OBJ = $(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(SRC))

all : $(NAME)

$(NAME)	: $(OBJ_DIR) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(CSTD) -o $(NAME) 

$(OBJ_DIR) :
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	$(CC) $(CFLAGS) $(CSTD) -c $< -o $@

clean :
	rm -rf $(OBJ_DIR)

fclean : clean
	rm -rf $(NAME)

re : fclean all

.PHONY : all clean fclean re