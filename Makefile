CC = c++

CFLAGS = -Wall -Wextra -Werror -I./includes

REQ_FILES = srcs/request/RequestParse.cpp

SRC_S = srcs/server

HPATH = includes/Server.hpp includes/RequestParse.hpp

SRCS = main.cpp $(SRC_S)/Server.cpp  $(REQ_FILES)

OBJS = $(SRCS:.cpp=.o)

TARGET = webServ

all: $(TARGET)

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) -o $@ $(OBJS)
	
%.o: %.cpp $(HPATH)
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(TARGET)
    
re: fclean all