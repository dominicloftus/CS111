
.SILENT:

default:
	gcc -Wall -Wextra -g -o lab0 lab0.c

check: default checkAccuracy badFlag noInput badOutput catchSegFault
	echo "Success if no other output"

clean:
	rm -f lab0 lab0-204910863.tar.gz

dist:
	tar -czvf lab0-204910863.tar.gz lab0.c README Makefile backtrace.png breakpoint.png

checkAccuracy:
	-./lab0 --input=test.txt --output=result.txt\
	if [ $$? -ne 0 ] ;\
	then\
	echo "Error: program did not exit normally";\
	fi\
	cmp test.txt result.txt;\
	if[ $$? -ne 0 ] ;\
	then\
		echo "Error: result is not equal to original";\
	fi\
	rm -f result.txt

badFlag:
	-./lab0 --hello;\
	if [ $$? -ne 1 ] ;\
	then\
		echo "Error: did not catch unavailable flag";\
	fi

noInput:
	-./lab0 --input=notafile.txt --output=result.txt;\
	if [ $$? -ne 2 ] ;\
	then\
		echo "Error: did not catch nonexistant file";\
	fi\
	rm -f result.txt\

badOutput:
	-touch badfile.txt;\
	-chmod -w badfile.txt;\
	-./lab0 --input=test.txt --output=badfile.txt;\
	if [ $$? -ne 3 ] ;\
	then\
		echo "Error: did not catch output file without write permission";\
	fi\
	rm -f badfile.txt


catchSegFault:
	-./lab0 --segfault --catch;\
	if [ $$? -ne 4 ] ;\
	then\
		echo "Error: did not catch segmentation fault";\
	fi


