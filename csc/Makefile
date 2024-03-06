all:./code/fibo.o ./code/main.o ./include/fibo.h
	gcc ./code/main.o ./code/fibo.o -o fibo
./code/fibo.o:
	make -C ./code
	make fibo.o
./code/main.o:
	make -C ./code
	make main.o
clean:
	rm ./code/*.o
