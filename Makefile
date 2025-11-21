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

# Git operations
push:
	@git add .
	@echo -n "$(BLUE)Enter your commit message for CPP Module 09: $(END)"
	@read commit_message; \
	git commit -m "CPP09: $$commit_message"; \
	git push; \
	echo "$(YELLOW)📤 All CPP Module 09 has been pushed with 'CPP09: $$commit_message'$(END)"

save:
	@git add .
	@git status --porcelain | grep -q . && { \
		echo -n "$(BLUE)Enter your commit message for CPP Module 09: $(END)"; \
		read commit_message; \
		git commit -m "CPP09: $$commit_message"; \
		echo "$(GREEN)💾 Changes saved locally with 'CPP09: $$commit_message'$(END)"; \
	} || echo "$(YELLOW)ℹ️  No changes to commit$(END)"

push-force: save
	@git push
	@echo "$(GREEN)📤 Pushed to remote repository$(END)"%  

.PHONY : all clean fclean re push save push-force