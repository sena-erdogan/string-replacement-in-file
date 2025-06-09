all:
	gcc -c -g hw1.c -ansi
	gcc -o hw1 hw1.o

clean:
	rm -rf *o hw1
