#! /usr/bin/gnuplot

#set style line 1 lc rgb '#0060ad' pt 5   # square
plot "example_phi.txt" using 1:2:4 with points lt palette
pause -1 "Hit any key to continue"