CC		= gcc
CFLAGS	= -Wall -g -pg
#INCLUDE = -I/home/deepaks/Documents/program/InEx/src/headers 

all: program

program: main.o command.o inexData.o customError.o appInfo.o dataFunction.o
	@echo "\nLinking object files"
	$(CC) $(CFLAGS) main.o command.o inexData.o customError.o appInfo.o dataFunction.o -o program

main.o: main.c 
	@echo "\nCompiling: main.c"
	$(CC) $(CFLAGS) -c main.c
#	$(CC) $(CFLAGS) $(INCLUDE) -c main.c

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

dataFunction.o: dataFunction.c 
	@echo "\nCompiling: dataFunction.c"
	$(CC) $(CFLAGS) -c dataFunction.c
#	$(CC) $(CFLAGS) $(INCLUDE) -c dataFunction.c

customError.o: customError.c 
	@echo "\nCompiling: customError.c"
	$(CC) $(CFLAGS) -c customError.c
#	$(CC) $(CFLAGS) $(INCLUDE) -c customError.c

clean:
	@echo "\nRemoving object and output files!"
	rm -rf *.o program 