
.SILENT:

default:
	gcc -pthread -Wall -Wextra -g -o lab2_list lab2_list.c SortedList.c

tests: default
	./lab2_list --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2	--iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4	--iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8	--iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1	--iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2	--iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4	--iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8	--iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2b_list.csv

	-./lab2_list --threads=1 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv

	-./lab2_list --threads=1 --iterations=10 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=10 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=10 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=10 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=10 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=20 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=20 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=20 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=20 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=20 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=40 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=40 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=40 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=40 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=40 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=80 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=80 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=80 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=80 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=80 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=10 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=10 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=10 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=10 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=10 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=20 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=20 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=20 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=20 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=20 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=40 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=40 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=40 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=40 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=40 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=80 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=80 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=80 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=80 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=80 --yield=id --sync=s --lists=4 >> lab2b_list.csv

	./lab2_list --threads=1 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv

profile: default
	rm -f raw.gperf profile.out
	LD_PRELOAD=/lib64/libprofiler.so CPUPROFILE=raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	/bin/pprof --text ./lab2_list ./raw.gperf >> profile.out
	/bin/pprof --list=threadFunc ./lab2_list ./raw.gperf >> profile.out
	rm -f raw.gperf

graphs: tests
	gnuplot ./lab2_list.gp

dist: graphs profile
	tar -czvf lab2b-204910863.tar.gz SortedList.h SortedList.c lab2_list.c Makefile lab2b_list.csv profile.out lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png README lab2_list.gp

clean:
	rm -f lab2_list lab2b-204910863.tar.gz
