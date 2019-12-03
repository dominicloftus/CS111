
.SILENT:

default:
	gcc -Wall -Wextra -o lab3a lab3a.c

dist: default
	tar -czvf lab3a-204910863.tar.gz lab3a.c Makefile README ext2_fs.h

clean:
	rm -f lab3a lab3a-204910863.tar.gz
