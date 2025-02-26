CC = c++
CFLAGS = -O3 -I./includes -Wall -Wextra -Werror# -g -fsanitize=address

# Colors
RESET = \033[0m
GREEN = \033[32m
YELLOW = \033[33m
CYAN = \033[36m
RED = \033[31m
BOLD = \033[1m

# Sources
REQ_FILES = srcs/request/RequestParse.cpp srcs/request/BodyParse.cpp
SRC_S = srcs/server/Server.cpp srcs/ConfigFile/ConServer.cpp srcs/ConfigFile/ParseConfigFile.cpp
SRC_RES = srcs/response/Response.cpp srcs/response/ResponseUtils.cpp

HPATH = includes/Server.hpp includes/RequestParse.hpp includes/BodyParse.hpp \
        includes/Utility.hpp

SRCS = main.cpp $(SRC_S) $(SRC_RES) $(REQ_FILES)
OBJS = $(SRCS:.cpp=.o)
TARGET = webServ

define BANNER
	@echo "${YELLOW}${BOLD}"
	@echo "         ,-.-.     ,----.               ,-,--.     ,----.                     ,-.-.    "
	@echo ",-..-.-./  \==\ ,-.--\` , \   _..---.  ,-.'-  _\ ,-.--\` , \  .-.,.---.  ,--.-./=/ ,/"
	@echo "|, \=/\=|- |==||==|-  _.-\` .' .'.-. \/==/_ ,_.'|==|-  _.-\` /==/  \`   \/==/, ||=| -|"
	@echo "|- |/ |/ , /==/|==|   \`.-./==/- '=' /\==\  \   |==|   \`.-.|==|-, .=., \==\,  \ / ,|"
	@echo " \, ,     _|==/==/_ ,    /|==|-,   '  \==\ -\ /==/_ ,    /|==|   '='  /\==\ - ' - /"
	@echo " | -  -  , |==|==|    .-' |==|  .=. \ _\==\ ,\|==|    .-' |==|- ,   .'  \==\ ,   |"
	@echo "  \  ,  - /==/|==|_  ,\`-._/==/- '=' ,/==/\/ _ |==|_  ,\`-._|==|_  . ,'.  |==| -  ,/"
	@echo "  |-  /\ /==/ /==/ ,     /==|   -   /\==\ - , /==/ ,     //==/  /\ ,  ) \==\  _ /"
	@echo "  \`--\`  \`--\`  \`--\`-----\`\`\`-._\`.___,'  \`--\`---'\`--\`-----\`\` \`--\`-\`--\`--'   \`--\`--'"
	@echo "                                               by [meserghi + mal-mora + hidriouc]"
	@echo "${RESET}"
endef

all: $(TARGET)
	$(call BANNER)
	@echo "${GREEN}${BOLD}Compilation successful!${RESET}"

$(TARGET): $(OBJS)
	@echo "${CYAN}Compiling $(TARGET)...${RESET}"
	@$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.cpp $(HPATH)
	@echo "${YELLOW}Compiling $<...${RESET}"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(call BANNER)
	@echo "${RED}${BOLD}Cleaning object files...${RESET}"
	@rm -rf $(OBJS)

fclean: clean
	@echo "${RED}${BOLD}Removing $(TARGET)...${RESET}"
	@rm -rf $(TARGET)

re: fclean all