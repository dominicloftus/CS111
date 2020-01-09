

.SILENT:

default:
	gcc -Wall -Wextra -o lab1a lab1a.c

clean:
	rm -f lab1a lab1a-204910863.tar.gz

dist:
	tar -czvf lab1a-204910863.tar.gz lab1a.c Makefile README
