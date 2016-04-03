mainn: mainn.c
	gcc -o mainn mainn.c -lpthread -lm

clean:
	rm -f mainn *.o
