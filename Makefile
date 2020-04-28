main: main.o scheduler.o process.o
	gcc main.o scheduler.o process.o -o main
main.o: main.c Makefile
	gcc main.c -c
scheduler.o: scheduler.c scheduler.h Makefile
	gcc scheduler.c -c
process.o: process.c process.h Makefile
	gcc process.c -c
clean:
	rm -rf *o
run:
	sudo ./main
