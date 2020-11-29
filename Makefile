all: Asst2.c
	gcc Asst2.c -Wall -pthread -o detector
	
run: Asst2.c
	gcc Asst2.c -Wall -pthread -o detector
	./detector ./stuff
	
debug: Asst2.c
	gcc Asst2.c -Wall -g -fsanitize=address,undefined -pthread -o detector
	./detector ./stuff

clean:
	rm -f detector *.o
