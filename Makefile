all: tokenizer.c
	gcc tokenizer.c -o tokenizer

clean: tokenizer
	rm tokenizer
