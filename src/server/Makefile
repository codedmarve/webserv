SRCS		=	main.cpp Servers.cpp DataBase.cpp parsFunc.cpp \
				DomainResolve.cpp HttpRequestParser.cpp RequestBody.cpp \
				RequestHeaders.cpp Utils.cpp RequestLine.cpp

CC_FLAGS		=	-Wall -Wextra -Werror -std=c++98 -g -fsanitize=address
CC				=	c++
SRCS_F			= src/
OBJS_F			= obj/

OBJS		=	$(SRCS:.cpp=.o)
OBJS_P		=	$(addprefix $(OBJS_F), $(OBJS))
NAME		= 	webserv

all:$(NAME)

$(OBJS_F)%.o: $(SRCS_F)%.cpp Makefile
	@mkdir -p $(OBJS_F)
	@echo "Working on: $<"
	@$(CC) $(CC_FLAGS) -O3 -c $< -o $@

$(NAME): $(OBJS_P)
	@$(CC) $(CC_FLAGS) -O3 -o $(NAME) $(OBJS_P)
	@echo "OK"

clean:
	@rm -rf $(OBJS_F)

fclean:	clean
	@rm -rf $(NAME)

re:		fclean all

.PHONY:	all clean fclean re
