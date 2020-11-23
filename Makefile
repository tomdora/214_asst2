all: tokenizer.c Asst2.c
#	gcc tokenizer.c -o tokenizer
	gcc Asst2.c -o detector
	
run: tokenizer.c Asst2.c
#	gcc tokenizer.c -o tokenizer
	gcc Asst2.c -o detector
	./detector

clean:
	rm -f tokenizer detector *.o
