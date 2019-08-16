#! /usr/bin/gnuplot

set loadpath "plot/palettes/"

set style fill
plot for [i=0:2] "plot/data/phi.txt" using 1:($3==i?$2:1/0):4 with points ls i+4 lc palette


pause -1 "Hit any key to continue"