CC = c++

CFLAGS = -Wall -Wextra -Werror -I./includes

SRC_S = srcs/server
HPATH = includes

SRCS		=	main.cpp $(SRC_S)/Server.cpp 

OBJS = $(SRCS:.cpp=.o)

TARGET = webServ

all: $(TARGET)

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) -o $@ $(OBJS)
	
%.o: %.cpp $(HPATH)/Server.hpp
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(TARGET)
    
re: fclean all