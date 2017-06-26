run: planes
	./planes

compile: planes.c
	gcc -g -Wall planes.c -o planes

clean:
	rm planes
