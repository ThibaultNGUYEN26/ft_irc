# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: thibnguy <thibnguy@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/01 19:30:09 by rchbouki          #+#    #+#              #
#    Updated: 2024/05/04 20:06:47 by thibnguy         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	pissouBot
SOURCES		=	.
OBJECTS		=	./bin
INCLUDES	=	.

SRCS		=	pissouBot.cpp \
				pissouBotUtils.cpp \
				main.cpp

OBJS		=	$(addprefix ${OBJECTS}/, $(SRCS:.cpp=.o))

CC			=	c++ -std=c++98
CFLAGS		=	-Wall -Wextra -Werror -g #-fsanitize=address -fsanitize=undefined -pedantic
CINCLUDES	=	-I ${INCLUDES}

BLACK		=	"\033[1;30m"
RED			=	"\033[1;31m"
GREEN		=	"\033[1;32m"
CYAN		=	"\033[1;35m"
BLUE		=	"\033[1;36m"
WHITE		=	"\033[1;37m"
EOC			=	"\033[0;0m"

TOTAL_OBJS := $(words $(OBJS))
COMPILED_OBJS := 0

${OBJECTS}/%.o: ${SOURCES}/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ -c $< $(CINCLUDES)

all: ${NAME}

${NAME}: ${OBJS}
	@echo $(GREEN)"\n● Compilation of ${NAME}...\n"$(EOC)
	@${CC} ${CFLAGS} ${CINCLUDES} -o ${NAME} ${OBJS}

clean:
	@echo ${RED}"● Deletion of binary files (.o)..."$(EOC)
	@rm -rf ${OBJECTS}

fclean: clean
	@echo ${RED}"● Deletion of executables and libraries..."$(EOC)
	@rm -rf ${NAME}

re: fclean all

.PHONY: all clean fclean re
