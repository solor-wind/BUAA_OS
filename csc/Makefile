all:./code/fibo.o ./code/main.o ./include/fibo.h
	gcc ./code/main.o ./code/fibo.o -o fibo
./code/fibo.o:
	make -C ./code fibo.o
	#make fibo.o
./code/main.o:
	make -C ./code main.o
	#make main.o
clean:
	rm ./code/*.o
