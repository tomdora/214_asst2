all: tokenizer.c Asst2.c
#	gcc tokenizer.c -o tokenizer
	gcc Asst2.c -Wall -pthread -o detector
	
run: tokenizer.c Asst2.c
#	gcc tokenizer.c -o tokenizer
#	gcc Asst2.c -Wall -pthread -o detector
	gcc Asst2.c -Wall -pthread -o detector
	./detector ./stuff
	
debug: tokenizer.c Asst2.c
	gcc Asst2.c -Wall -g -fsanitize=address,undefined -pthread -o detector
	./detector ./stuff

clean:
	rm -f tokenizer detector *.o
