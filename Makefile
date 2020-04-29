main: ./src/fuck.c
	gcc ./src/fuck.c -o main
#main: ./src/main.o ./src/scheduler.o ./src/process.o
#	gcc ./src/main.o ./src/scheduler.o ./src/process.o -o main
#main.o: main.c Makefile
#	gcc ./src/main.c -c
#scheduler.o: scheduler.c scheduler.h Makefile
#	gcc ./src/scheduler.c -c
#process.o: process.c process.h Makefile
#	gcc ./src/process.c -c
#clean:
#	rm ./src/*o -rf
#run:
#	sudo ./src/main
