# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hidriouc <hidriouc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/22 13:31:03 by hidriouc          #+#    #+#              #
#    Updated: 2025/01/08 10:24:53 by hidriouc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Compiler and flags
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

# Target executable
TARGET = ConServer

# Source files and object files
SRCS = ConServer.cpp main.cpp ParseConfigFile.cpp
OBJS = $(SRCS:.cpp=.o)

# Header files
HEADERS = conserver.hpp header.hpp

# Default target
all: $(TARGET)

# Build the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) 
	
fclean: clean
	rm -f $(TARGET)

re : clean all
# Phony targets
.PHONY: all clean