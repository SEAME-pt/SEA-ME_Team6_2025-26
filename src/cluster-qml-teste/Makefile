#-------------------------------------  COLORS  --------------------------------
RESET			= \033[0m
BLACK 			= \033[1;30m
RED 			= \033[1;31m
GREEN 			= \033[1;32m
YELLOW		 	= \033[1;33m
BLUE			= \033[1;34m
PURPLE 			= \033[1;35m
CYAN 			= \033[1;36m
WHITE 			= \033[1;37m

#----------------------------------- COMMANDS ----------------------------------
CC				= c++
RM 				= rm -rf

#-----------------------------------  FLAGS  -----------------------------------
NPD				= --no-print-directory

#---------------------------------  RULES  --------------------------------------

all:
	./scripts/run.sh

clean:
	rm -rf build

exec:
	./build/team6-cluster

re: clean all

.SILENT:

.PHONY: all clean exec re