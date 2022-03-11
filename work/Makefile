NAME = a.out
SRCS = $(shell find *.cpp)
OBJDIR = ./obj/
OBJS = $(SRCS:%.cpp=$(OBJDIR)%.o)

CXX =  clang++ -pthread -g -O0
CFLAGS = -Wall -Werror -Wextra -std=c++98

# -std=c++98
all: $(OBJDIR) $(NAME)

$(OBJDIR):
	mkdir -p obj

$(OBJDIR)%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

$(NAME): $(OBJS)
	$(CXX) $(CFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -rf $(OBJDIR)
	rm -f out_ft out_std err_ft err_std

fclean: clean
	rm -f $(NAME)

re: fclean all

test:
	$(CXX) $(SRCS) -o serv
	./serv


.PHONY: all fclean clean re test val