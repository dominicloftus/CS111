#! /usr/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#	8. wait time per lock (ns)
#
# output:
#	lab2_list-1.png ... operation per second vs threads for different synchronization methods
#	lab2_list-2.png ... wait time per lock and average time per operation vs threads for
#			    mutex synchronization
#	lab2_list-3.png ... successful iterations vs threads for different sync methods
#	lab2_list-4.png ... operations per second vs threads with lists mutex sync
#	lab2_list-5.png ... opteration per second vs threads with lists spinlock sync
#
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

# general plot parameters
set terminal png
set datafile separator ","

# how many threads/iterations we can run without failure (w/o yielding)
set title "List-1: Throughput vs Threads"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput (operations/sec)"
set logscale y 10
set output 'lab2b_1.png'

# grep out non yield, protected, single list output
plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'mutex sync' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'spin sync' with linespoints lc rgb 'green'


set title "List-2: Wait per Mutex Lock and Average Time per Operation vs Threads"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Time(ns)"
set logscale y 10
set output 'lab2b_2.png'
# note that unsuccessful runs should have produced no output
plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
	title 'wait per mutex' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
	title 'time per op' with linespoints lc rgb 'red'
     
set title "List-3: Successful Iterations vs Threads"
set logscale x 2
set xrange [0.75:]
set xlabel "Threads"
set ylabel "Successful Iterations"
set logscale y 2
set output 'lab2b_3.png'
plot \
    "< grep 'list-id-none,[0-9]*,[0-9]*,4' lab2b_list.csv" using ($2):($3) \
	with points lc rgb "red" title "unprotected", \
    "< grep 'list-id-m,[0-9]*,[0-9]*,4' lab2b_list.csv" using ($2):($3) \
        with points lc rgb "green" title "mutex sync", \
    "< grep 'list-id-s,[0-9]*,[0-9]*,4' lab2b_list.csv" using ($2):($3) \
        with points lc rgb "blue" title "spinlock sync"
    

set title "List-4: Throughput vs Threads for Mutex Lock"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput(operations/sec)"
set logscale y 10
set output 'lab2b_4.png'
plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '1 list' with linespoints lc rgb 'blue', \
     "< grep 'list-none-m,[0-9]*,1000,4' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '4 lists' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9]*,1000,8' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '8 lists' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,[0-9]*,1000,16' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '16 lists' with linespoints lc rgb 'yellow'


set title "List-5: Throughput vs Threads for Spin Lock"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput(operations/sec)"
set logscale y 10
set output 'lab2b_5.png'
plot \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '1 list' with linespoints lc rgb 'blue', \
     "< grep 'list-none-s,[0-9]*,1000,4' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '4 lists' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,8' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '8 lists' with linespoints lc rgb 'green', \
     "< grep 'list-none-s,[0-9]*,1000,16' lab2b_list.csv" using ($2):(1000000000/($7)) \
        title '16 lists' with linespoints lc rgb 'yellow'