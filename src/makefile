CC		= gcc
CFLAGS	= -Wall -g
INCLUDE = -I/home/deepaks/Documents/program/InEx/src

all: final

final: main.o appInterface.o inexData.o customError.o appInfo.o
	@echo "\nLinking object files"
	$(CC) $(CFLAGS) main.o appInterface.o inexData.o customError.o appInfo.o -o final

main.o: main.c 
	@echo "\nCompiling: main.c"
	$(CC) $(CFLAGS) $(INCLUDE) -c main.c

appInterface.o: appInterface.c
	@echo "\nCompiling: appInterface.c"
	$(CC) $(CFLAGS) $(INCLUDE) -c appInterface.c

inexData.o: inexData.c
	@echo "\nCompiling: inexData.c"
	$(CC) $(CFLAGS) $(INCLUDE) -c inexData.c

customError.o: customError.c 
	@echo "\nCompiling: customError.c"
	$(CC) $(CFLAGS) $(INCLUDE) -c customError.c

appInfo.o: appInfo.c 
	@echo "\nCompiling: appInfo.c"
	$(CC) $(CFLAGS) $(INCLUDE) -c appInfo.c

clean:
	@echo "\nRemoving object and output files!"
	rm -rf *.o final 