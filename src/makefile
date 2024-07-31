CC		= gcc
CFLAGS	= -Wall -g -pg
#INCLUDE = -I/home/deepaks/Documents/program/InEx/src/headers 

all: program

program: client.o command.o inexData.o customError.o appInfo.o recordFunction.o consoleInput.o
	@echo "\nLinking object files"
	$(CC) $(CFLAGS) client.o command.o inexData.o customError.o appInfo.o recordFunction.o consoleInput.o -o program

client.o: client.c 
	@echo "\nCompiling: client.c"
	$(CC) $(CFLAGS) -c client.c
#	$(CC) $(CFLAGS) $(INCLUDE) -c client.c

command.o: command.c
	@echo "\nCompiling: command.c"
	$(CC) $(CFLAGS) -c command.c
#	$(CC) $(CFLAGS) $(INCLUDE) -c command.c

inexData.o: inexData.c
	@echo "\nCompiling: inexData.c"
	$(CC) $(CFLAGS) -c inexData.c
#	$(CC) $(CFLAGS) $(INCLUDE) -c inexData.c

appInfo.o: appInfo.c 
	@echo "\nCompiling: appInfo.c"
	$(CC) $(CFLAGS) -c appInfo.c
#	$(CC) $(CFLAGS) $(INCLUDE) -c appInfo.c

recordFunction.o: recordFunction.c 
	@echo "\nCompiling: recordFunction.c"
	$(CC) $(CFLAGS) -c recordFunction.c
#	$(CC) $(CFLAGS) $(INCLUDE) -c recordFunction.c

customError.o: customError.c 
	@echo "\nCompiling: customError.c"
	$(CC) $(CFLAGS) -c customError.c
#	$(CC) $(CFLAGS) $(INCLUDE) -c customError.c

consoleInput.o: consoleInput.c 
	@echo "\nCompiling: consoleInput.c"
	$(CC) $(CFLAGS) -c consoleInput.c
#	$(CC) $(CFLAGS) $(INCLUDE) -c consoleInput.c

clean:
	@echo "\nRemoving object and output files!"
	rm -rf *.o program 