NAME		=	webserv

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CC			=	c++ -std=c++98 -DNDEBUG
	CFLAGS		=	-Wall -Werror -Wextra
else
	CC			=	c++ -std=c++98 -g -DDEBUG
	CFLAGS		=	-Wall -Werror -Wextra
endif

HEADER_DIR	=	./include
SRC_DIR		=	./src
OBJ_DIR		=	./obj

HEADER		=	Location.hpp \
				Server.hpp \
				Server_info.hpp \
				User.hpp \
				Location.hpp \
				User.hpp \
				utils.hpp

SRC			=	webserver.cc \
				Nginx.cc \
				User.cc \
				Server.cc \
				utils.cc

OBJ			=	$(addprefix $(OBJ_DIR)/, $(SRC:.cc=.o))

RM_DIR		=	rm -rf
RM_FILE		=	rm -f

#COLORS
C_NO		=	"\033[00m"
C_OK		=	"\033[32m"
C_GOOD		=	"\033[32m"

SUCCESS		=	$(C_GOOD)SUCCESS$(C_NO)
OK			=	$(C_OK)OK$(C_NO)

all			:	$(NAME)

$(OBJ)		: 	| $(OBJ_DIR)	

$(OBJ_DIR)	:
				@mkdir -p $(OBJ_DIR)
			
$(OBJ_DIR)/%.o	:	$(SRC_DIR)/%.cc ${HEADER_DIR}/*.hpp Makefile
					$(CC) $(CFLAG) -c $< -o $@

$(NAME)		:	$(OBJ)
				$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
				@echo "\tCompiling...\t" [ $(NAME) ] $(SUCCESS)

clean		:
				@$(RM_DIR) $(OBJ_DIR)
				@echo "\tCleaning...\t" [ $(OBJ_DIR) ] $(OK)

fclean		:	clean
				@$(RM_FILE) $(NAME)
				@echo "\tDeleting...\t" [ $(NAME) ] $(OK)

re			:	fclean all

.PHONY		:	all, clean, fclean, re, test