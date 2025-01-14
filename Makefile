CC = c++ 

CFLAGS = -Wall -Wextra -Werror -O3 -I./includes -g -fsanitize=address

REQ_FILES = srcs/request/RequestParse.cpp srcs/request/BodyParse.cpp

SRC_S = srcs/server/Server.cpp srcs/ConfigFile/ConServer.cpp srcs/ConfigFile/ParseConfigFile.cpp  
SRC_RES = srcs/response/Response.cpp

HPATH = includes/Server.hpp includes/RequestParse.hpp includes/BodyParse.hpp includes/Utility.hpp

SRCS = main.cpp $(SRC_S) $(SRC_RES) $(REQ_FILES)

OBJS = $(SRCS:.cpp=.o)

TARGET = webServ

all: $(TARGET)

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) -o $@ $(OBJS)
	
%.o: %.cpp $(HPATH)
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm -rf $(OBJS) output.txt

fclean: clean
	rm -rf $(TARGET)
    
re: fclean all