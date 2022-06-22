fibo.o: fibo.c
	gcc fibo.c -c -o fibo.o -I../include
main.o: main.c
	gcc main.c -c -o main.o -I../include
clean: fibo.o main.o
	rm fibo.o;rm main.o
fibo: fibo.o main.o
	cd .. && make fibo
