all: Asst2.c
	gcc Asst2.c -Wall -pthread -lm -o detector
	
run: Asst2.c
	gcc Asst2.c -Wall -pthread -lm -o detector
	./detector .
	
debug: Asst2.c
	gcc Asst2.c -Wall -g -fsanitize=address,undefined -pthread -lm -o detector

clean:
	rm -f detector *.o
