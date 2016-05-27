all: main.o modprod.o tablecheck.o encrypt.o
	gcc -g  main.o modprod.o tablecheck.o encrypt.o -o hw5  -lm

main.o: main.c hw5.h
	gcc -c -g -Wall main.c  

modprod.o: modprod.c hw5.h
	gcc -c -g -Wall modprod.c  

tablecheck.o: tablecheck.c hw5.h
	gcc -c -g -Wall tablecheck.c  

encrypt.o: encrypt.c hw5.h
	gcc -c -Wall encrypt.c




clean:
	rm *.o
	rm hw5
